#include "opentelemetry/ext/http/client/http_client_factory.h"
//#include "opentelemetry/ext/http/client/http_client.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <process.h>

struct Process {
	std::string name;
	std::string bin;
	std::vector<std::string> args;
	std::thread runner;
	std::thread waiter;
	std::string healthCheckURL;
	intptr_t waitResult{};
	intptr_t handle{};
	intptr_t exitHandle{};
	int exitCode{};
};

static const char* cmd_exe = std::getenv("ComSpec");

void waiter(Process* proc)
{
	proc->exitHandle = _cwait(&proc->exitCode, proc->handle, 0);
}

void runner(Process* proc)
{
	std::string cmd;
	for( const auto& arg : proc->args )
	{
		if( !cmd.empty() )
			cmd.append( " " );
		cmd.append( arg );
	}
	printf("%s: Running %s %s\n", proc->name.c_str(), proc->bin.c_str(), cmd.c_str() );
	proc->handle = _spawnl( _P_NOWAIT, cmd_exe ? cmd_exe : "c:\\windows\\system32\\cmd.exe", "/c", "start", ("\"" + proc->name + "\"").c_str(), "/WAIT", proc->bin.c_str(), cmd.c_str(), nullptr );
	if( proc->handle == -1 )
	{
		printf("%s: %s failed!\n", proc->name.c_str(), proc->bin.c_str() );	
		return;
	}
	proc->waiter = std::thread(waiter, proc);
	int state{-1};
	while( proc->waiter.joinable() )
	{
		if( !proc->healthCheckURL.empty() )
		{
			const auto client{ opentelemetry::ext::http::client::HttpClientFactory::CreateSync() };
			const auto r{ client->GetNoSsl( proc->healthCheckURL ) };
			if( r )
			{
				if( state != 1 )
					printf("%s: healthy\n", proc->name.c_str());
				state = 1;
			}
			else
			{
				if( state != 0 )
					printf("%s: not healthy\n", proc->name.c_str());
				if( state == 1 )
					break;
				state = 0;
			}
		}
 		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
	if( proc->waiter.joinable() )
		proc->waiter.join();
	printf("%s: %s exited with %d (exitHandle=%zd)\n", proc->name.c_str(), proc->bin.c_str(), proc->exitCode, proc->exitHandle);
}

int main(int argc, const char*argv[])
{
	std::vector<Process> procs;

	procs.push_back(Process{"loki"});
	procs.push_back(Process{"tempo"});
	procs.push_back(Process{"prom"});
	procs.push_back(Process{"otel"});
	procs.push_back(Process{"graf"});

	for(int i=1; i<argc; i++ )
	{
		const std::string arg{ argv[i] };
		std::string key, val;
		const auto eqpos{ arg.find_first_of('=') };
		if( eqpos != std::string::npos )
		{
			key = arg.substr(0, eqpos);
			val = arg.substr(eqpos+1);
		}
		if( key.empty() || val.empty() )
		{
			printf("Invalid argument: %s\n", arg.c_str());
			return 1;
		}
		printf("key=%s val=%s\n", key.c_str(), val.c_str());
		for( auto& proc : procs )
		{
			if( key == "--" + proc.name + "-bin" )
				proc.bin = val;
			else if( key == "--" + proc.name + "-arg" )
				proc.args.push_back(val);
			else if( key == "--" + proc.name + "-chk" )
				proc.healthCheckURL = val;
		}
	}

	for( auto& proc : procs )
	{
		if( proc.bin.empty() )
			continue;
		proc.runner = std::thread( runner, &proc );
	}

	for( auto& proc : procs )
	{
		if( proc.runner.joinable() )
			proc.runner.join();
	}

	return 0;
}
