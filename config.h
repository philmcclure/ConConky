#if !defined(__CONFIG_H__)
#define __CONFIG_H__

#include <vector>
#include <string>
#include <map>


class Config
{
public:
    Config();
    ~Config();

    void								reloadConfig();
	std::map<std::string, std::string>	getVars(){return _var2command;}
	std::vector<std::string>			getFormatStrings(){return _formatStr;}
private:
    int                         _createConfigFile();
	std::string					_getHomeDir();

	std::map<std::string, std::string>	_var2command;
	std::vector<std::string>			_formatStr;

};


#endif
