/*
 * "Parsers" class gets map with variables and corresponding commands,
 * runs these commands and fill format strings;
 * Also it adds some new variables wich are easier to resolve in code;
 */


#if !defined(__PARSERS_H__)
#define __PARSERS_H__

#include <time.h>
#include <map>
#include <string>
#include <vector>
#include <fstream>

class Parsers
{
public:
	Parsers(std::map<std::string, std::string> var2command, std::vector<std::string>	formatStr);
	~Parsers();

	std::vector<std::string>    getLines();


private:
	int				_timeHour();
	int				_timeMin();
	int				_timeSec();


	std::string		_runCommand(std::string command);
	int				_runCommands();
	std::string		_replace(std::string text, std::string s, std::string d);

	std::map<std::string, std::string>	_var2command;
	std::map<std::string, std::string>	_var2result;
	std::vector<std::string>			_formatStr;

};

#endif
