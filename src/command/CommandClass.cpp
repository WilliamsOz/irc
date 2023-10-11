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
		std::cout << _name << std::endl;
		(this->*this->_commands[this->_name])(server->GetUserByFd(clientFd), server);
	}
	else
		std::cout << "Unknown command -> [" << this->_name << "]" << "\n";
}

void	Command::SetUpCommandsContainer()
{
    _commands["PASS"] = &Command::PASS;
    _commands["MODE"] = &Command::MODE;
    _commands["PING"] = &Command::PING;
    _commands["CAP"] = &Command::CAP;
	_commands["PRIVMSG"] =&Command::PRIVMSG;
    _commands["USER"] = &Command::USER;
	_commands["NICK"] = &Command::NICK;
	_commands["JOIN"] = &Command::JOIN;
	_commands["WHOIS"] = &Command::WHOIS;
	_commands["INVITE"] = &Command::INVITE;

}

// seulement les membres du channel sont autorise a invite sinon ERR_NOTONCHANNEL
// si channel est invite-only, server doit replies ERR_CHANOPRIVSNEEDED si user pas operateur
// si user deja dans channel, reject commande avec replies ERR_USERONCHANNEL
// replies RPL_INVITING si commande accepte
void	Command::INVITE(User *user, Server *server)
{
	Channel *chan = NULL;
	User	*target = NULL;
	std::string tmp;

	if (this->_param.size() < 2)
		return ;
	tmp.assign(this->_param[1]);
	if (this->_param[1].find('#') != std::string::npos) // si contient un '#', supprimer
		this->_param[1].erase(0, 1);
	if (server->HasChannel(this->_param[1]) == true) // channel exist
	{
		chan = server->GetChannelByName(this->_param[1]);
		target = server->GetUserByFd(server->GetFdByNickName(this->_param[0]));
		if (chan->HasUser(user) == false) 															// le user est pas membre du channel
			SendMsgToClient(user, ERR_NOTONCHANNEL(user->GetNickname(), tmp));
		else if (chan->IsOper(user) == false && chan->GetModes().find('i') != std::string::npos)	// user pas op et channel en invite only
			SendMsgToClient(user, ERR_CHANOPRIVSNEED(user->GetNickname(), tmp));
		else if (chan->HasUser(target))																// user target est deja dans channel
			SendMsgToClient(user, ERR_USERONCHANNEL(target->GetNickname(), this->_param[1]));
		else																						// inviter le client
		{
			SendMsgToClient(user, RPL_INVITING(user->GetNickname(), target->GetNickname(), chan->GetName()));
			SendMsgToClient(target, INVITE_CLIENT(user->GetNickname(), user->GetUsername(), "Invite", target->GetNickname(), chan->GetName()));
			chan->AddUserToInviteList(target);
		}
	}
	else
		SendMsgToClient(user, ERR_NOSUCHCHANNEL(user->GetNickname(), this->_param[1]));				// channel inexistant
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

// potentiellement ajouter des return apres chaque reply error pour eviter l'application de mode sur un mauvais join
// /join #chanelexistant +it ----> regler ce probleme
void	Command::JOIN(User *user, Server *server)
{
	Channel		*chan;
	std::string	modes;
	std::string replies;
	size_t		pos;
	size_t		len;
	bool		newChanCreated = false;

	for (unsigned long i = 0; i < this->_param.size(); i++)
	{
		switch (this->_param[i][0])
		{
			case '#':
				this->_param[i].erase(0, 1); // enlever le '#' du nom du channel
				if (server->HasChannel(this->_param[i]) == false) // channel inexistant
				{
					chan = server->AddChannel(this->_param[i]); // ajouter channel dans classe server
					user->JoinChannel(chan); // ajout du channel dans vector de classe user
					chan->AddUser(user); // ajout du user dans vector de classe channel
					chan->AddOper(user); // ajout du user dans vector operator de classe channel
					SendMsgToClient(user, RPL_JOIN(user->GetNickname(), chan->GetName()));
					if (chan->GetTopic().empty() == false)
						SendMsgToClient(user, RPL_TOPIC(user->GetNickname(), chan->GetName(), chan->GetTopic()));
					std::string userNickname = "@" + user->GetNickname();
					SendMsgToClient(user, RPL_NAMREPLY(userNickname, chan->GetName(), chan->GetClientList()));
					SendMsgToClient(user, RPL_ENDOFNAMES(user->GetNickname(), chan->GetName()));
					newChanCreated = true;
				}
				else // channel existant
				{
					chan = server->GetChannelByName(this->_param[i]);
					if (this->_param.size() > 1) // si a 2e arg
					{ 								// fusionner les deux if et voir si il y a un segfault
						if (this->_param[i + 1][0] != '+') // si 2e arg pas un mode
						{
							if (server->IsPassCorrect(this->_param[i], this->_param[i + 1]) == true
								&& chan->IsUserInvited(user) == true) // password correct et user invited
							{
								chan = server->AddUserToChannel(user, this->_param[i]); // ajouter user a map de channel dans classe server
								SendMsgToClient(user, RPL_JOIN(user->GetNickname(), chan->GetName()));
								if (chan->GetTopic().empty() == false)
									SendMsgToClient(user, RPL_TOPIC(user->GetNickname(), chan->GetName(), chan->GetTopic()));
								SendMsgToClient(user, RPL_NAMREPLY(user->GetNickname(), chan->GetName(), chan->GetClientList()));
								SendMsgToClient(user, RPL_ENDOFNAMES(user->GetNickname(), chan->GetName()));
							}
							else if (server->IsPassCorrect(this->_param[i], this->_param[i + 1]) == false) // password incorrect
							{
								SendMsgToClient(user, ERR_BADCHANNELKEY(user->GetNickname(), this->_param[i]));
								return ;
							}
							else // user n'est pas invite
							{
								SendMsgToClient(user, ERR_INVITEONLYCHAN(user->GetNickname(), chan->GetName()));
								return ;
							}
						}
					}
					else // pas de 2e arg ou bien c'est un mode
					{
						if (server->HasPass(this->_param[i]) == false
							&& chan->IsUserInvited(user) == true) // si le channel n'a pas de mdp et que l'user est invited
						{
							chan = server->AddUserToChannel(user, this->_param[i]); // ajouter user a map de channel dans classe server
							user->JoinChannel(chan);
							chan->AddUser(user);
							SendMsgToClient(user, RPL_JOIN(user->GetNickname(), chan->GetName()));
							if (chan->GetTopic().empty() == false)
								SendMsgToClient(user, RPL_TOPIC(user->GetNickname(), chan->GetName(), chan->GetTopic()));
							SendMsgToClient(user, RPL_NAMREPLY(user->GetNickname(), chan->GetName(), chan->GetClientList()));
							SendMsgToClient(user, RPL_ENDOFNAMES(user->GetNickname(), chan->GetName()));
						}
						else if (server->HasPass(this->_param[i]) == true) // channel a un password
						{
							SendMsgToClient(user, ERR_BADCHANNELKEY(user->GetNickname(), this->_param[i]));
							return ;
						}
						else // user n'est pas invited
						{
							SendMsgToClient(user, ERR_INVITEONLYCHAN(user->GetNickname(), chan->GetName()));
							return ;
						}
					}
				}
				break ;
			case '+':
				if (this->_param[i].size() > 1 && newChanCreated == true) // si ne contient pas seulement '+'
				{
					pos = 1;
					len = (this->_param[i].length()) - 1;
					modes = this->_param[i].substr(pos, len);
					// chan->SetModes(modes);
					// std::cout << chan->GetModes() << std::endl;
				}
				break;
		}
	}
}

void	Command::SetModeParams(std::vector<std::string> *param)
{
	int i = 2; // les parametres de mode sont stocker partir de cet indice (ex: #channel(0) +ok-v(1) mp(2))
	int j = param->size() - 1;

	_modeParams.push(""); // on push une valeur par defaut au cas ou il n'y a pas d'arg
	while (j >= i)
	{
		_modeParams.push((*param)[j]);
		j--; // on rempli a l'envers car c une stack
	}
}

void	Command::MODE(User *user, Server *server)
{
	Channel	*channel = server->GetChannelByName(_param[0].erase(0, 1)); //on supprime le hashtag

	SetModeParams(&_param);
	if (_param[0][0] != '#') // on ne gere que les mode de channel pas ceux des clients
		return;
	if (!channel)
	{	
		SendMsgToClient(user, ERR_NOSUCHCHANNEL(user->GetNickname(), _param[0]));
		return;
	}
	if (!channel->IsOper(user))
	{	
		SendMsgToClient(user, ERR_CHANOPRIVSNEED(user->GetNickname(), _param[0]));
		return;
	}
	int i = 0; // index des flag de mode

	if (_param[1][i] == '+')
		while (_param[1][i] != '-' && _param[1][i])
		{
			i++; // on passe le '+'
			channel->SetModes(_param[1][i], &_modeParams, server, this, user); // on incremente j quand on a utilisé un param
		}
	if (_param[1][i] == '-')
		while (_param[1][i] != '-')
		{
			i++;
			channel->UnsetModes(_param[1][i++], &_modeParams, server, this, user);
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
		{
			user->SetAuth(true);
		}
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
		if (user->GetNickname().empty() == false)
			SendMsgToClient(user, RPL_WELCOME(user->GetNickname()));
	}
	return ;
}

void	Command::PING(User *user, Server *server)
{
	(void)server;

	if (user->GetAuth())
		SendMsgToClient(user, RPL_PONG);
	return ;
}

std::string	Command::GetMsg()
{
	std::string	msg;
	size_t 		i = 1;

	_param[i].erase(0, 1); // on supp les ":"
	while (i < _param.size())
	{
		msg += _param[i];
		msg += " ";
		i++;
	}
	return (msg);
}

void	Command::SendToUser(User *user, Server *server)
{
	User	*recipient = server->GetUserByNickname(_param[0]);

	if (recipient) // si le user appartient bien au server
	{
		SendMsgToClient(recipient, RPL_PRIVMSG_CLIENT(user->GetNickname(), this->GetMsg()));
	}
	else // le user est inconnu
		SendMsgToClient(user, ERR_NOSUCHNICK(user->GetNickname(), user->GetHostname()));
}

void	Command::SendToChannel(User *user, Server *server)
{
	this->_param[0].erase(0, 1); // on retire le '#' devant le nom du channel
	Channel	*recipient = server->GetChannelByName(this->_param[0]);

	if (server->HasChannel(this->_param[0]) == false) // check si le chan existe
	{
		SendMsgToClient(user, ERR_NOSUCHCHANNEL(user->GetNickname(), _param[0]));
		return ;	
	}
	if (!recipient->HasUser(user) && recipient->IsUserInvited(user) == false) // check si le user appartient bien au channel
	{
		SendMsgToClient(user, ERR_CANNOTSENDTOCHAN(this->_param[0], recipient->GetName()));
		return ;
	}
	if (this->_param[1] == "") // check si le msg n'est pas vide
	{
		SendMsgToClient(user, ERR_NOTEXTTOSEND(this->_param[0]));
		return ;
	}
	std::vector<User *>::iterator	it = recipient->GetUsers().begin();
	std::vector<User *>::iterator	ite = recipient->GetUsers().end();

	while (it != ite)
	{
		std::cout << (*it)->GetNickname() << std::endl;
		SendMsgToClient(*it, RPL_PRIVMSG_CHANNEL(user->GetNickname(), user->GetUsername(), _name, recipient->GetName(), _param[1]));
		it++;
	}
}

void	Command::PRIVMSG(User *user, Server *server)
{
	if (_param.size() < 2)
	{
		SendMsgToClient(user, ERR_NEEDMOREPARAMS(user->GetNickname(), this->_name));
		return;
	}
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
	int 		len = msg.size();


	if ((send(recipient->GetFd(), msg.c_str(), len, 0 )) != len)
		return ;
		// throw std::invalid_argument("send");
	// if ret send() == -1 -> throw error 
}