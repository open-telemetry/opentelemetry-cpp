#include "opentelemetry/ext/http/client/http_client_factory.h"

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>
#include <map>
#include <filesystem>
#include <process.h>

struct Process {
	std::string name;
	std::string bin;
	std::vector<std::string> args;
	std::thread runner;
	std::thread waiter;
	std::string healthCheckURL;
	intptr_t handle{};
	intptr_t exitHandle{};
	int exitCode{};
	volatile bool waitFinished{};
	volatile bool healthy{};
	volatile bool exited{};
};

static const char* cmd_exe = std::getenv("ComSpec");

void waiter(Process* proc)
{
	proc->exitHandle = _cwait(&proc->exitCode, proc->handle, 0);
	proc->waitFinished = true;
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
	proc->handle = _spawnl( _P_NOWAIT, cmd_exe ? cmd_exe : "c:\\windows\\system32\\cmd.exe", "/c", "start", ("\"" + proc->name + "\"").c_str(), "/MIN", "/WAIT", proc->bin.c_str(), cmd.c_str(), nullptr );
	if( proc->handle == -1 )
	{
		proc->exited = true;
		printf("%s: %s failed!\n", proc->name.c_str(), proc->bin.c_str() );	
		return;
	}
	proc->waiter = std::thread(waiter, proc);
	int state{-1};
	while( !proc->waitFinished )
	{
		if( !proc->healthCheckURL.empty() )
		{
			const auto client{ opentelemetry::ext::http::client::HttpClientFactory::CreateSync() };
			const auto r{ client->GetNoSsl( proc->healthCheckURL ) };
			if( r )
			{
				proc->healthy = true;
				if( state != 1 )
					printf("%s: healthy\n", proc->name.c_str());
					state = 1;
			}
			else
			{
				proc->healthy = false;
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
	proc->exited = true;
	printf("%s: %s exited with %d (exitHandle=%zd)\n", proc->name.c_str(), proc->bin.c_str(), proc->exitCode, proc->exitHandle);
}

void demo();

int main(int argc, const char*argv[])
{
	std::vector<std::string> procNames{ "loki", "tempo", "otel", "prom", "graf" };
	std::map<std::string, Process> procs;

	const auto cwd_path{ std::filesystem::current_path() };
	printf("cwd=%s\n", cwd_path.u8string().c_str() );

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
//		printf("key=%s val=%s\n", key.c_str(), val.c_str());
		for( const auto& procName : procNames )
		{
			auto it = procs.try_emplace(procName, Process{procName});
			auto& proc = it.first->second;
			if( key == "--" + procName + "-bin" )
				proc.bin = val;
			else if( key == "--" + procName + "-arg" )
				proc.args.push_back(val);
			else if( key == "--" + procName + "-chk" )
				proc.healthCheckURL = val;
		}
	}

	for( auto it = procs.begin(); it != procs.end(); )
	{
		if( it->second.bin.empty() )
			it = procs.erase(it);
		else
			++it;
	}

	for( auto& it : procs )
	{
		auto& proc = it.second;
		proc.runner = std::thread( runner, &proc );
	}

	bool allHealthyOnce = true;
	for( ;; ) 
	{
		bool allHealthy = true;
		for( const auto& it : procs )
		{
			const auto& proc = it.second;
			if( proc.exited )
				return 1;
			//printf("%s=%d\n\n", proc.bin.c_str(), proc.healthy );
			if( !proc.healthy )
				allHealthy = false;
		}
		if( allHealthy )
		{
			if( allHealthyOnce )
			{
				printf("ALL HEALTHY\n");
				demo();
			}
			allHealthyOnce = false;
		}
 		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	return 0;
}

// ---------------------------

void demo()
{

}
