#include "irc.hpp"

void	Command::ExecCommand(int clientFd, Server *server)
{
	if (this->_commands.find(this->_name) != _commands.end())
	{
		(this->*this->_commands[this->_name])(server->GetUserByFd(clientFd), server);
	}
	else
		std::cout << "Unknown command -> " << this->_name << "\n";
}

Command::Command(std::string src)
{
	size_t	pos = 0;
	bool	first = true;

	if (src.empty())
	{
		_name = "";
		return ;
	}
	while (pos != std::string::npos)
	{
		pos = src.find(' ');
		if (first == true)
		{
			_name = src.substr(0, pos);
			first = false;
		}
		else
			_param.push_back(src.substr(0, pos));
		src = src.erase(0, pos + 1);
	}
	this->SetUpCommandsContainer();
}

void	Command::SetUpCommandsContainer()
{
	// _commands["JOIN"] = &Command::JOIN;
    _commands["PASS"] = &Command::PASS;
    _commands["PING"] = &Command::PING;
    _commands["NICK"] = &Command::NICK;
    _commands["USER"] = &Command::USER;
	//etc
}

// void	Command::JOIN(int clientFd, User *user)
// {

// }

void	Command::PASS(User *user, Server *server)
{
	if (user->GetAuth() == false)
	{
		if (this->_param[0] != server->GetServerPassword())
		{
			// std::map<int, User*>::iterator it = (server->GetUsers()).find(user->GetFd());
			std::string message = "464 : Password incorrect.\r\n";
			send(user->GetFd(), message.c_str(), message.size(), 0);
            epoll_ctl(server->GetEpollFd(), EPOLL_CTL_DEL, user->GetFd(), server->GetClientEvent());
			// (server->GetUsers()).erase(it); // -> segfault, a mettre dans destructeur
			close(user->GetFd());
		}
		else
		{
			std::string welcomeMessage = "001 : Welcome to the IR`C Server! Your connection has been established successfully.\r\n";
			int bytesSent = send(user->GetFd(), welcomeMessage.c_str(), welcomeMessage.size(), 0);
			if (bytesSent == -1)
			{
			    std::cerr << "Error sending welcome message." << std::endl;
			    return ;
			}
			user->SetAuth(true);
		}
	}
}

void	Command::USER(User *user, Server *server)
{
	(void)server;

	if (user->GetAuth() == true)
	{
		user->SetUsername(this->_param[0]);
		user->SetHostname(this->_param[1]);
		user->SetServername(this->_param[2]);
		user->SetRealname(this->_param[3]);
	}
	return ;
}

void	Command::NICK(User *user, Server *server)
{
	(void)server;
	if (user->GetAuth() == true)
		user->SetNickname(this->_param[0]);
	return ;
}

void	Command::PING(User *user, Server *server)
{
	(void)server;

	if (user->GetAuth())
	{
		std::cout << this->_name << std::endl;
		std::string pongMessage = "PONG :" + this->_name + "\r\n";
		send(user->GetFd(), pongMessage.c_str(), pongMessage.size(), 0);
	}
	return ;
}

Command::~Command()
{
}

std::string	Command::GetCmdName()
{
	return(this->_name);
}
