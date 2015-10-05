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

#include "config.h"

#include <fstream>
#include <wordexp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>


Config::Config()
{
	reloadConfig();
}

Config::~Config()
{

}

void    Config::reloadConfig()
{
	std::string path = _getHomeDir();
	std::string path_total = path + std::string(".conconky/config");

	if (access(path_total.c_str(), F_OK) != 0)
	{
		_createConfigFile();
	}

	// At this point config exists in any case
	std::vector<std::string> lines;

	std::string line;
	std::ifstream infile(path_total);
	while (std::getline(infile, line))
	{
		lines.push_back(line);
	}


	// Cleaning up comments and voids
	for (auto i = lines.begin(); i != lines.end(); )
	{
		if ((*i)[0] == '#' || (*i)[0] == ' ')
		{
			i = lines.erase(i);
		}
		else
		{
			++i;
		}
	}

	// Found and cutting out format strings;
	for (auto i = lines.begin(); i != lines.end(); ++i)
	{
		if ((*i)[0] == '~')
		{
			// from here begins format strings
			i = lines.erase(i);
			while (i != lines.end())
			{
				_formatStr.push_back(*i);
				i = lines.erase(i);
			}
			break;
		}
	}

	// In lines array left only var->command pairs;
	if (lines.size() % 2 != 0)
	{
		exit(-1);
	}

	for (auto i = lines.begin(); i != lines.end();)
	{
		std::string var = *i;
		++i;
		std::string command = *i;
		++i;
		_var2command[var] = command;
	}

	lines.clear();

}



std::string Config::_getHomeDir()
{
	std::string path;

	wordexp_t exp_result;
	wordexp("~/", &exp_result, 0);
	path = std::string(exp_result.we_wordv[0]);
	wordfree(&exp_result);

	return path;
}

int Config::_createConfigFile()
{
	FILE * config;
	char default_config[] =
			"#ConConky config\n"
			"uname\n"
			"uname\n"
			"name\n"
			"whoami\n"
			"~\n"
			"Uname = ~uname~ !\n"
			"My name is ~name~ !\n"
			;

	std::string path = _getHomeDir();
	std::string path_dir = path + std::string(".conconky");
	std::string total_dir = path_dir + std::string("/config");

	// check if ".conconky" dir is exist
	if (access(path_dir.c_str(), F_OK) != 0)
	{
		// we need to create dir
		mkdir(path_dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	}


	config = fopen(total_dir.c_str(), "w");
	if (config == NULL)
	{
		printf("Error: %d (%s)\n", errno, strerror(errno));
		return errno;
	}

	fwrite(default_config, 1, strlen(default_config), config);
	fclose(config);

	return 0;
}


