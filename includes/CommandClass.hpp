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

	//getter
	std::vector<std::string>	GetParameters();
	std::string					GetCmdName();

	// utils
	void						ExecCommand(int clientFd, Server *server);
	void						SetUpCommandsContainer();
	// void 						SendToChannel(User *user, Server *server);
	void 						SendToUser(User *user, Server *server);
	// toutes les commandes
	void						JOIN(User *user, Server *server);
	void						PING(User *user, Server *server);
	void						NICK(User *user, Server *server);
	void						USER(User *user, Server *server);
	void						OPER(User *user, Server *server);
	void						CAP(User *user, Server *server);
	void						PRIVMSG(User *user, Server *server);

	private:
	std::string									_name;
	std::vector<std::string>					_param;
	std::map<std::string, CommandFunctionPointer>	_commands;
};

#endif