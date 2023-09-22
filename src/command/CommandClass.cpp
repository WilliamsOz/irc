﻿#include "irc.hpp"

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

Command::~Command()
{
	return ;
}

void	Command::ExecCommand(int clientFd, Server *server)
{
	if (this->_commands.find(this->_name) != _commands.end())
	{
		(this->*this->_commands[this->_name])(server->GetUserByFd(clientFd), server);
	}
	else
		std::cout << "Unknown command -> " << this->_name << "\n";
}

void	Command::SetUpCommandsContainer()
{
    _commands["PASS"] = &Command::PASS;
    _commands["PING"] = &Command::PING;
    _commands["CAP"] = &Command::CAP;
	_commands["PRIVMSG"] =&Command::PRIVMSG;
    _commands["USER"] = &Command::USER;
	_commands["NICK"] = &Command::NICK;
	_commands["JOIN"] = &Command::JOIN;
	_commands["WHOIS"] = &Command::WHOIS;
}

static void	printWhoIs(int fd, User *user)
{
	std::string message;

	message = "311 : " + user->GetNickname() + " " + user->GetUsername() + " " + user->GetHostname() + " :" + user->GetRealname() + "\r\n"; //realname affiche : en trop
	send(fd, message.c_str(), message.size(), 0);
	message = "End of /WHOIS list : 318\r\n";
	send(fd, message.c_str(), message.size(), 0);
	return ;
}

void	Command::WHOIS(User *user, Server *server)
{
	if (user->GetAuth() == false)
		return ;
	// if (this->_param[0].at(0) == '#') -> whois sur un channel
	// else () -> 403 aucun channel trouver
	for (size_t index = 0; index < this->_param.size() ; index ++)// ->whois sur un user
	{
		int fdToFind = -1;
		fdToFind = server->GetFdByNickName(this->_param[index]);
		if (fdToFind != -1)
			printWhoIs(user->GetFd(), server->GetUserByFd(fdToFind));
		else
		{
			std::string message = ":localhost 401 " + user->GetNickname() + " :No such nick\r\n";
			send(user->GetFd(), message.c_str(), message.size(), 0);
		}
	}
	return ;
}


// /JOIN #moncanal ---------------> creer/rejoins un channel et deviens op si non existant
// /JOIN #moncanal1 +ito ---------------> creer/rejoins channel + applique les modes
// i -> invite only
// t -> topic changeable seulement par operator
void	Command::JOIN(User *user, Server *server)
{
	Channel		*chan;
	std::string	modes;
	size_t		pos;
	size_t		len;
	bool		hasChanStr = false;

	for (unsigned long i = 0; i < this->_param.size(); i++)
	{
		switch (this->_param[i][0])
		{
			case '#':
				if (server->HasChannel(this->_param[i]) == false)
				{
					chan = server->AddChannel(user, this->_param[i]);
					user->JoinChannel(chan);
				}
				else
					server->AddUserToChannel(user, this->_param[i]); // add invite only exception
				hasChanStr = true;
				break;
			case '+':
				if (this->_param[i][1] != '\0' && hasChanStr == true)
				{
					pos = 1;
					len = (this->_param[i].length()) - 1;
					modes = this->_param[i].substr(pos, len);
					chan->SetModes(modes);
				}
				break;
		}
	}
}


// /MODE #moncanal +itk ---------------> applique les modes sur le/les channel :
//																				- +i ---------------> invite only
//																				- +t ---------------> topic changeable seulement par operator
//																				- +k ---------------> definis mdp pour le channel
// /MODE #moncanal +l 10 ---------------> definis le nombre max d'utilisateur pouvant entrer dans le channel
// void	Command::MODE(User *user, Server *server)
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
			user->SetAuth(true);
	}
}
	
void	Command::CAP(User *user, Server *server)
{
	(void)server;

	if (this->GetParameters()[0] == "LS") // liste les capacités disponible pour les clients
	{
		std::string response = "CAP * LS :multi-prefix\r\n";
		send(user->GetFd(), response.c_str(), response.length(), 0);
		// std::cout<<"response : "<< response <<std::endl;
	}
	if (!(this->GetParameters()[0].compare("REQ"))) // demande l'obtention d'une capacité
	{
		std::string response = "CAP * ACK multi-prefix\r\n";
		send(user->GetFd(), response.c_str(), response.length(), 0);
		// std::cout<<"response : "<<response<<std::endl;
    }
	// if (!(this->GetParameters()[0].compare("END"))) // peut-etre ajouter des infos supplementaires?
	// {
		// std::string response = "001 " +(user->GetNickname())+": Bienvenue sur le serveur Irc\r\n";
		// send(user->GetFd(), response.c_str(), response.length(), 0);
	// }
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
	{
		user->SetNickname(this->_param[0]);
		std::string welcomeMessage = RPL_WELCOME(user->GetNickname());
		// std::string welcomeMessage = "001 : Welcome to the IR`C Server! Your connection has been established successfully.\r\n";
		// int bytesSent = send(user->GetFd(), welcomeMessage.c_str(), welcomeMessage.size(), 0);
		int bytesSent = send(user->GetFd(), welcomeMessage.c_str(), welcomeMessage.size(), 0);
		if (bytesSent == -1)
		{
		    std::cerr << "Error sending welcome message." << std::endl;
		    return ;
		}
	}
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

std::string	Command::GetCmdName()
{
	return(this->_name);
}

std::vector<std::string>	Command::GetParameters()
{
	return (this->_param);
}

// void	Command::SendToChannel()
// {
		// verifie que le chan existe
		// // que le user n'est banne du channel
// }