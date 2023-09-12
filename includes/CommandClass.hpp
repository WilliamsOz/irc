#ifndef COMMANDCLASS_HPP

#define COMMANDCLASS_HPP

#include "irc.hpp"

class Command
{
	private:
	std::string									_name;
	std::vector<std::string>					_param;
	std::map<std::string, std::function<void(int, User *)>>		_commands;
	
	public:
	std::string					GetCmdName();
	std::vector<std::string>	GetParameters();
	void						ExecCommand(int clientFd, User *user);
	void						SetUpCommandsContainer();
	void addCommand(const std::string& commandName, std::function<void(int, User)> commandFunction);
	// toutes les commandes
	void						JOIN(int clientFd, User *user);
	void						PING(int clientFd, User *user);
	
	Command(std::string src);
	~Command();
};

#endif