#ifndef COMMANDCLASS_HPP

#define COMMANDCLASS_HPP

#include "irc.hpp"

class Command;

typedef void (Command::*CommandFunctionPointer)(int, User *, Server *);

class Command
{
	private:
	std::string									_name;
	std::vector<std::string>					_param;
	std::map<std::string, CommandFunctionPointer>	_commands;

	public:
	std::string					GetCmdName();
	std::vector<std::string>	GetParameters();
	void						ExecCommand(int clientFd, User *user, Server *server);
	void						SetUpCommandsContainer();
	// utils
	void 						SendToChannel(Server *server);
	void 						SendToUser(Server *server);
	// toutes les commandes
	void						JOIN(int clientFd, User *user, Server *server);
	void						PING(int clientFd, User *user, Server *server);
	void						OPER(int clientFd, User *user, Server *server);
	void						CAP(int clientFd, User *user, Server *server);
	void						PRIVMSG(int clientFd, User *user, Server *server);
	Command(std::string src);
	~Command();
};


#endif