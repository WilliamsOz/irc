#include "irc.hpp"

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
    _commands["PASS"] = &Command::MODE;
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

// en cas de success pour join -> envoyer 
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
				if (server->HasChannel(this->_param[i]) == false)
				{
					this->_param[i].erase(0, 1);
					chan = server->AddChannel(this->_param[i]); // utiliser constructor parametrique ?
					if (chan->HasUser(user) == false)
					{
						user->JoinChannel(chan);
						chan->AddUser(user);
						chan->AddOper(user);
						SendMsgToClient(user, RPL_JOIN(chan->GetName()));
						if (chan->GetTopic().empty() == false)
							SendMsgToClient(user, RPL_TOPIC(user->GetNickname(), chan->GetName(), chan->GetTopic()));
						SendMsgToClient(user, RPL_NAMREPLY(user->GetNickname(), chan->GetName(), chan->GetClientList()));
					}
				}
				else
					server->AddUserToChannel(user, this->_param[i]);
				hasChanStr = true;
				break;
			case '+':
				if (this->_param[i][1] != '\0' && hasChanStr == true
					&& server->HasChannel(chan->GetName()) == false)
				{
					pos = 1;
					len = (this->_param[i].length()) - 1;
					modes = this->_param[i].substr(pos, len);
					// chan->SetModes(modes); // a modifier
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

void	Command::MODE(User *user, Server *server)
{
	Channel	*channel = server->GetChannelByName(_param[0]);

	if (_param[0][0] != '#' || !channel)
	{	
		SendMsgToClient(user, ERR_NOSUCHCHANNEL(user->GetNickname(), _param[0]));
		return;
	}
	if (!channel->IsOper(user))
	{	
		SendMsgToClient(user, ERR_CHANOPRIVSNEED(user->GetNickname(), _param[0]));
		return;
	}
	int i = 0;
	int	j = 2; // index des arguments des modes

	if (_param[1][i] == '+')
		while (_param[1][i] != '-' && _param[1][i])
			channel->SetModes(_param[1][i++], _param[j], &j, server);
	// if (_param[1][i] == '-')
	// 	while (_param[1][i++] != '-')
	// 		channel->UnsetModes(_param[1][i++], &_param, &j);
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