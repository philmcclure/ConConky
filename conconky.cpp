/*
Copyright 2012-2013 Alexander Gulchenko aka A.Stahl

ConConky is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ConConky is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/


#include <signal.h>
#include <getopt.h>
#include "parsers.h"
#include "system.h"
#include "config.h"



#define     SWITCH_SIGNAL               SIGTTOU
#define		TIMEOUT						1

static bool		flag_daemon = false;	//  d
static bool		flag_output = true;		//  if we want to show

/// COMMON FUNCTIONS ////////////////////////////////////////////////////

void usage()
{
    printf("ConConky.\n");
    printf("-d -- daemonize?;\n");
    printf("-h -- help(this output);\n");
    printf("-? -- help(this output);\n");
}


void parse_params(int argc, char **argv)
{
    int c;
    opterr = 0;

    while ((c = getopt (argc, argv, "dh?")) != -1)
    {
        switch (c)
        {
            case 'd':
            {
                 flag_daemon = true;
            }break;
            case 'h':
            case '?':
            {
                 usage();
                 exit(1);
            }break;
        }
    }
}

void output_flag_handler(int i)
{
    if (i != SWITCH_SIGNAL) return;

	if (flag_output == true)
	{
		flag_output = false;
	}
	else
	{
		flag_output = true	;
	}
}

void set_signal_handler()
{
    struct sigaction sa;
    sa.sa_handler = output_flag_handler;
    sigaction(SWITCH_SIGNAL, &sa, 0);
}

void daemonize()
{
    pid_t pid, sid;

    // Fork off the parent process
    pid = fork();
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // Create a new SID for the child process
    sid = setsid();
    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Close out the standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}


///////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
	parse_params(argc, argv);
	set_signal_handler();

	if (flag_daemon)
    {
        daemonize();
    }

	System sys;
   	Config cnf;
	std::map<std::string, std::string>	var2com = cnf.getVars();
	std::vector<std::string>			format = cnf.getFormatStrings();

	Parsers parsers(var2com, format);


	std::vector<std::string> lines;

	while (true)
	{
		if (flag_output == false)
		{
			sleep(TIMEOUT);
			continue;
		}

		lines.clear();
		lines = parsers.getLines();

		for (auto i = lines.begin(); i != lines.end(); ++i)
		{
			sys.push_line(*i);
		}

		sys.output_lines();

		sleep(TIMEOUT);
	}

}
