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
		{
			// check la longueur du message ?
			_param.push_back(src.substr(0, pos));
		}
		src = src.erase(0, pos + 1);
	}
	this->SetUpCommandsContainer();
}

void	Command::SetUpCommandsContainer()
{
    _commands["PASS"] = &Command::PASS;
    _commands["PING"] = &Command::PING;
    _commands["CAP"] = &Command::CAP;
	_commands["PRIVMSG"] =&Command::PRIVMSG;
	// _commands["JOIN"] = &Command::JOIN;
    // _commands["USER"] = &Command::USER;
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
			// supprimer user du container (class)channel et (class)server
			close(user->GetFd());
    	    epoll_ctl(server->GetEpollFd(), EPOLL_CTL_DEL, user->GetFd(), server->GetClientEvent());
			std::cout << user->GetNickname() << " wrong password" << std::endl;
		}
		else
		{
			user->SetAuth(true);
			std::cout << user->GetNickname() << " good password" << std::endl;
		}
	}
}

// void	Command::CAP(int clientFd, User *user)
// {
// 	if (this->GetParameters()[0] == "LS") // liste les capacités disponible pour les clients
// 	{
// 		std::string response = "CAP * LS :multi-prefix\r\n";
// 		send(user->GetFd(), response.c_str(), response.length(), 0);
// 		std::cout<<"response : "<<response<<std::endl;
// 	}
// 	if (!(this->GetParameters()[0].compare("REQ"))) // demande l'obtemtion d'une capacité
// 	{
// 		std::string response = "CAP * ACK multi-prefix\r\n";
// 		send(user->GetFd(), response.c_str(), response.length(), 0);
// 		std::cout<<"response : "<<response<<std::endl;
//     }
// 	if (!(this->GetParameters()[0].compare("END"))) // peut-etre ajouter des infos supplementaires
// 	{
// 		std::string response = "001 " +(user->GetNickName())+": Bienvenue sur le serveur Irc\r\n";
// 		send(user->GetFd(), response.c_str(), response.length(), 0);
// 	}
// }

void	Command::CAP(User *user, Server *server)
{
	(void)server;

	if (this->GetParameters()[0] == "LS") // liste les capacités disponible pour les clients
	{
		std::string response = "CAP * LS :multi-prefix\r\n";
		send(user->GetFd(), response.c_str(), response.length(), 0);
		std::cout<<"response : "<<response<<std::endl;
	}
	if (!(this->GetParameters()[0].compare("REQ"))) // demande l'obtention d'une capacité
	{
		std::string response = "CAP * ACK multi-prefix\r\n";
		send(user->GetFd(), response.c_str(), response.length(), 0);
		std::cout<<"response : "<<response<<std::endl;
    }
	if (!(this->GetParameters()[0].compare("END"))) // peut-etre ajouter des infos supplementaires?
	{
		std::string response = "001 " +(user->GetNickname())+": Bienvenue sur le serveur Irc\r\n";
		send(user->GetFd(), response.c_str(), response.length(), 0);
	}
}

void	Command::PING(User *user, Server *server)
{
	(void)server;

	std::cout << this->_name << std::endl;
	std::string pongMessage = "PONG :" + this->_name + "\r\n";
	send(user->GetFd(), pongMessage.c_str(), pongMessage.size(), 0);
}


// void	Command::SendToChannel()
// {
		// verifie que le chan existe
		// // que le user n'est banne du channel
// }

void	Command::SendToUser(User *user, Server *server)
{
	int	recipientFd = server->GetFdByNickName(_param[0]);

	if (recipientFd != -1) // si le user appartient bien au server
	{
		std::string	response = ":" + user->GetNickname() + " " + _param[1] + "\r\n";
		send(recipientFd, response.c_str(), response.length(), 0);
	}
	else // le user est inconnu
	{
		std::string	response = ":localhost 401" + user->GetNickname() + " : No such NickName\r\n";
		// si ca fail -> message d'erreur  ERR_CANNOTSENDTOCHAN (404)
		send(user->GetFd(), response.c_str(), response.length(), 0);
	}
}

void	Command::PRIVMSG(User *user, Server *server)
{
	// if (_param.size() < 2)
	// 	// SendMessagetoClient(user, ERR_NEEDMOREPARAMS(user->GetNickName(), this->_name));
	// if (this->GetParameters()[0][0] == '#')
	// 	this->SendToChannel(user, server);
	this->SendToUser(user, server);
}

Command::~Command()
{
}

std::string	Command::GetCmdName()
{
	return(this->_name);
}

std::vector<std::string>	Command::GetParameters()
{
	return (this->_param);
}
