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

#include "parsers.h"

Parsers::Parsers(std::map<std::string, std::string> var2command, std::vector<std::string>	formatStr)
	: _var2command(var2command)
	, _formatStr(formatStr)
{

}

Parsers::~Parsers()
{

}


std::vector<std::string>	Parsers::getLines()
{
	// fill _var2result array
	_runCommands();
	// now we can fill format strings
	std::vector<std::string> result;

	for (auto j = _formatStr.begin(); j != _formatStr.end(); ++j)
	{
		std::string temp_line = *j;

		for (auto i = _var2result.begin(); i != _var2result.end(); ++i)
		{
			std::string var = "~"+(*i).first+"~";
			std::string result = (*i).second;
			temp_line = _replace(temp_line, var, result);
		}

		result.push_back(temp_line);

	}

	return result;

}

std::string	Parsers::_runCommand(std::string command)
{
	FILE * f = popen( command.c_str(), "r" );

	if ( f == 0 )
	{
		return std::string();
	}

	std::string result;

	const int BUFSIZE = 10000;
	char buf[BUFSIZE];
	while(fgets(buf, BUFSIZE,  f ))
	{
		result = std::string(buf);
	}

	pclose(f);

	return result;
}

int	Parsers::_runCommands()
{
	_var2result.clear();
	// This function runs on _var2command and fills _var2result;
	for (auto i = _var2command.begin(); i != _var2command.end(); ++i)
	{
		std::string var = (*i).first;
		std::string command = (*i).second;
		std::string result = _runCommand(command);
		result = _replace(result, "\n", "");
		_var2result[var] = result;
	}
	return 0;
}

std::string Parsers::_replace(std::string text, std::string s, std::string d)
{
	for(size_t index=0; index=text.find(s, index), index!=std::string::npos;)
	{
		text.replace(index, s.length(), d);
		index+=d.length();
	}
	return text;
}





