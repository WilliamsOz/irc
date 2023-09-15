#include "irc.hpp"

// Command::~Command()
// {
// 	return ;
// }

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
    _commands["PING"] = &Command::PING;
    _commands["NICK"] = &Command::NICK;
    _commands["USER"] = &Command::USER;
	//etc
}

// void	Command::JOIN(int clientFd, User *user)
// {

// }

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

void	Command::USER(User *user, Server *server)
{
	(void)server;

	user->SetUsername(this->_param[0]);
	user->SetHostname(this->_param[1]);
	user->SetServername(this->_param[2]);
	user->SetRealname(this->_param[3]);
}

void	Command::NICK(User *user, Server *server)
{
	(void)server;
	user->SetNickname(this->_param[0]);
	return ;
}

void	Command::PING(User *user, Server *server)
{
	(void)server;

	std::cout << this->_name << std::endl;
	std::string pongMessage = "PONG :" + this->_name + "\r\n";
	send(user->GetFd(), pongMessage.c_str(), pongMessage.size(), 0);
}

// void	Command::NOTICE(int clientFd, User *user, Server *server)
// {
	//envoyer des notif entre utilisateur && || canaux
	//notice == privmsg, sauf que aucune reponse auto ou erreurs
	//ne doit etre envoyer en reponse a notice

	// (void)clientFd;

	// return ;
// }


Command::~Command()
{
}

std::string	Command::GetCmdName()
{
	return(this->_name);
}
