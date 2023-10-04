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
		std::cout << "Unknown command -> [" << this->_name << "]" << "\n";
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

void	Command::printWhoIs(User *user)
{
	SendMsgToClient(user, RPL_WHOISUSER(user->GetNickname(), user->GetHostname(), user->GetUsername()));
	SendMsgToClient(user, RPL_ENDOFWHOIS(user->GetNickname(), user->GetHostname()));
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
			printWhoIs(server->GetUserByFd(fdToFind));
		else
			SendMsgToClient(user, ERR_NOSUCHNICK(user->GetNickname(), user->GetHostname()));
	}
	return ;
}

// gerer le cas ou le channel est sur invitation seulement
void	Command::JOIN(User *user, Server *server)
{
	Channel		*chan;
	std::string	modes;
	std::string replies;
	size_t		pos;
	size_t		len;
	bool		hasChanStr = false;

	for (unsigned long i = 0; i < this->_param.size(); i++)
	{
		switch (this->_param[i][0])
		{
			case '#':
				this->_param[i].erase(0, 1); // enlever le '#' du nom du channel
				if (server->HasChannel(this->_param[i]) == false) // channel inexistant
				{
					chan = server->AddChannel(this->_param[i]); // ajouter channel dans classe server
					if (chan->HasUser(user) == false) // si user n'est pas deja dans ce channel
					{
						user->JoinChannel(chan); // ajout du channel dans vector de classe user
						chan->AddUser(user); // ajout du user dans vector de classe channel
						chan->AddOper(user); // ajout du user dans vector operator de classe channel
						server->SendMsgToClient(user, RPL_JOIN(user->GetNickname(), chan->GetName()));
						if (chan->GetTopic().empty() == false)
							server->SendMsgToClient(user, RPL_TOPIC(user->GetNickname(), chan->GetName(), chan->GetTopic()));
						std::string userNickname = "@" + user->GetNickname();
						server->SendMsgToClient(user, RPL_NAMREPLY(userNickname, chan->GetName(), chan->GetClientList()));
						server->SendMsgToClient(user, RPL_ENDOFNAMES(user->GetNickname(), chan->GetName()));
					}
				}
				else // channel existant
				{
					if (this->_param.size() > 1) // si a 2e arg
					{ 								// fusionner les deux if et voir si il y a un segfault
						if (this->_param[i + 1][0] != '+') // si 2e arg pas un mode
						{
							if (server->IsPassCorrect(this->_param[i], this->_param[i + 1]) == true) // password correct
							{
								chan = server->AddUserToChannel(user, this->_param[i]); // ajouter user a map de channel dans classe server
								server->SendMsgToClient(user, RPL_JOIN(user->GetNickname(), chan->GetName()));
								if (chan->GetTopic().empty() == false)
									server->SendMsgToClient(user, RPL_TOPIC(user->GetNickname(), chan->GetName(), chan->GetTopic()));
								server->SendMsgToClient(user, RPL_NAMREPLY(user->GetNickname(), chan->GetName(), chan->GetClientList()));
								server->SendMsgToClient(user, RPL_ENDOFNAMES(user->GetNickname(), chan->GetName()));
							}
							else // password incorrect
								server->SendMsgToClient(user, ERR_BADCHANNELKEY(user->GetNickname(), this->_param[i]));
						}
					}
					else // pas de 2e arg ou bien c'est un mode
					{
						if (server->HasPass(this->_param[i]) == false)
						{
							chan = server->AddUserToChannel(user, this->_param[i]); // ajouter user a map de channel dans classe server
							server->SendMsgToClient(user, RPL_JOIN(user->GetNickname(), chan->GetName()));
							if (chan->GetTopic().empty() == false)
								server->SendMsgToClient(user, RPL_TOPIC(user->GetNickname(), chan->GetName(), chan->GetTopic()));
							server->SendMsgToClient(user, RPL_NAMREPLY(user->GetNickname(), chan->GetName(), chan->GetClientList()));
							server->SendMsgToClient(user, RPL_ENDOFNAMES(user->GetNickname(), chan->GetName()));
						}
						else // mdp needed
							server->SendMsgToClient(user, ERR_BADCHANNELKEY(user->GetNickname(), this->_param[i]));
					}
				}
				hasChanStr = true;
				break;
			case '+':
				if (this->_param[i][1] != '\0' && hasChanStr == true // si ne contient pas seulement '+'
					&& server->HasChannel(chan->GetName()) == false) // si case '#' est execute && channel n'existe pas dans classe server
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

void	Command::PASS(User *user, Server *server)
{
	if (user->GetAuth() == false)
	{
		if (this->_param[0] != server->GetServerPassword())
		{
			// std::map<int, User*>::iterator it = (server->GetUsers()).find(user->GetFd());
			SendMsgToClient(user, ERR_PASSWDMISMATCH(user->GetNickname()));
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
		SendMsgToClient(user, CAP_LS());
	if (!(this->GetParameters()[0].compare("REQ"))) // demande l'obtention d'une capacité
		SendMsgToClient(user, CAP_REQ());
}

void	Command::USER(User *user, Server *server)
{
	(void)server;

	if (user->GetAuth() == true)
	{
		user->SetUsername(this->_param[0]);
		user->SetHostname(this->_param[1]);
		user->SetServername(this->_param[2]);
		user->SetRealname(&_param);
	}
	return ;
}

void	Command::NICK(User *user, Server *server)
{
	(void)server;

	if (user->GetAuth() == true)
	{
		user->SetNickname(this->_param[0], server);
		SendMsgToClient(user, RPL_WELCOME(user->GetNickname()));
	}
	return ;
}

void	Command::PING(User *user, Server *server)
{
	(void)server;

	if (user->GetAuth())
	{
		std::cout << this->_name << std::endl;
		SendMsgToClient(user, RPL_PONG);
	}
	return ;
}

void	Command::SendToUser(User *user, Server *server)
{
	int	recipientFd = server->GetFdByNickName(_param[0]);

	if (recipientFd != -1) // si le user appartient bien au server
		SendMsgToClient(user, RPL_PRIVMSG_CLIENT(user->GetNickname(), user->GetUsername(), "PRIVMSG", _param[0], _param[1]));
	else // le user est inconnu
		SendMsgToClient(user, ERR_NOSUCHNICK(user->GetNickname(), user->GetHostname()));
}

void	Command::SendToChannel(User *user, Server *server)
{
	Channel	*recipient = server->GetChannelByName(this->_param[0]);

	if (server->HasChannel(this->_param[0]) == false)
	{
		SendMsgToClient(user, ERR_NOSUCHNICK(user->GetNickname(), user->GetHostname()));
		return ;	
	}
	if (!recipient->HasUser(user))
	{
		SendMsgToClient(user, ERR_CANNOTSENDTOCHAN(this->_param[0], recipient->GetName()));
		return ;
	}
	if (this->_param[1] == "") // check que le msg n'est pas vide
	{
		SendMsgToClient(user, ERR_NOTEXTTOSEND(this->_param[0]));
		return ;
	}
	std::vector<User *>::iterator	it = recipient->GetUsers().begin();
	std::vector<User *>::iterator	ite = recipient->GetUsers().end();

	while (it != ite)
	{
		SendMsgToClient(user, RPL_PRIVMSG_CHANEL(user->GetNickname(), user->GetUsername(), "PRIVMSG", recipient->GetName(), this->_param[1]));
		it++;
	}
}

void	Command::PRIVMSG(User *user, Server *server)
{
	if (_param.size() < 2)
		SendMsgToClient(user, ERR_NEEDMOREPARAMS(user->GetNickname(), this->_name));
	if (this->GetParameters()[0][0] == '#')
		this->SendToChannel(user, server);
	else
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

void        Command::SendMsgToClient(User* recipient, std::string msg)
{
	int			bytes_sent;
	int 		len = msg.size();

	if (len > 4096) // == taille maximum des msg sur internet
		// throw an error
	if ((bytes_sent = send(recipient->GetFd(), msg.c_str(), len, 0 )) != len)
		return ;
		// throw std::invalid_argument("send");
	// if ret send() == -1 -> throw error 
}