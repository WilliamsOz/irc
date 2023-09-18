#ifndef COMMANDCLASS_HPP
# define COMMANDCLASS_HPP

#include "irc.hpp"

class Command;

typedef void (Command::*CommandFunctionPointer)(User *, Server *);

class Command
{
	public:
	Command(std::string src);
	~Command();

	std::string					GetCmdName();
	void						ExecCommand(int clientFd, Server *server);
	void						SetUpCommandsContainer();
	// void addCommand(const std::string& commandName, std::function<void(int, User *)> commandFunction);
	// toutes les commandes
	void						JOIN(User *user, Server *server);
	void						PING(User *user, Server *server);
	void						NICK(User *user, Server *server);
	void						USER(User *user, Server *server);
	void						PASS(User *user, Server *server);

	private:
	std::string									_name;
	std::vector<std::string>					_param;
	std::map<std::string, CommandFunctionPointer>	_commands;
};


#endif