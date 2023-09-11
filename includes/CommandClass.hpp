#ifndef COMMANDCLASS_HPP

#define COMMANDCLASS_HPP

#include "irc.hpp"

class Command
{
private:
	std::string					_CmdName;
	std::vector<std::string>	_Parameters;
	//exec_cmd()
public:
	std::string	GetCmdName();
	std::vector<std::string>	GetParameters();
	Command(std::string src);
	~Command();
};




#endif