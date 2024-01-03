#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <thread>
#include <process.h>

struct Process {
	std::string name;
	std::string bin;
	std::vector<std::string> args;
	std::thread runner;
};

static const char* cmd_exe = std::getenv("ComSpec");

void runner(const Process* proc)
{
	std::string cmd;
	for( const auto& arg : proc->args )
	{
		if( !cmd.empty() )
			cmd.append( " " );
		cmd.append( arg );
	}
	printf("%s: Running %s %s\n", proc->name.c_str(), proc->bin.c_str(), cmd.c_str() );
	const auto r{ _spawnl( _P_WAIT, cmd_exe ? cmd_exe : "c:\\windows\\system32\\cmd.exe", "/c", "start", ("\"" + proc->name + "\"").c_str(), "/WAIT", proc->bin.c_str(), cmd.c_str(), nullptr ) };
	printf("%s: %s exited with %d\n", proc->name.c_str(), proc->bin.c_str(), r);
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
