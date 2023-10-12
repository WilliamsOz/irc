#ifndef COMMANDCLASS_HPP
# define COMMANDCLASS_HPP

class Channel;
class User;
class Server;
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
	std::string					GetMsg();

	// utils
	void						ExecCommand(int clientFd, Server *server);
	void						SetUpCommandsContainer();
	void 						SendToChannel(User *user, Server *server);
	void 						SendToUser(User *user, Server *server);
	void						SendMsgToClient(User *recipient, std::string msg);
	void						SetModeParams(std::vector<std::string> *param);

	void						printWhoIs(User *user, User *target);

	// toutes les commandes
	void						MODE(User *user, Server *server);
	void						JOIN(User *user, Server *server);
	void						PING(User *user, Server *server);
	void						NICK(User *user, Server *server);
	void						USER(User *user, Server *server);
	void						PASS(User *user, Server *server);
	void						CAP(User *user, Server *server);
	void						PRIVMSG(User *user, Server *server);
	void						WHOIS(User *user, Server *server);
	void						INVITE(User *user, Server *server);
	
	private:
	std::string										_name;
	std::vector<std::string>						_param;
	std::stack<std::string>							_modeParams;
	std::map<std::string, CommandFunctionPointer>	_commands;
};

#endif