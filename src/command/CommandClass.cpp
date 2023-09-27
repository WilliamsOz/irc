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
						replies = RPL_JOIN(chan->GetName());
						send(user->GetFd(), replies.c_str(), replies.size(), 0);
						if (chan->GetTopic().empty() == false)
						{
							replies = RPL_TOPIC(user->GetNickname(), chan->GetName(), chan->GetTopic());
							send(user->GetFd(), replies.c_str(), replies.size(), 0);
						}
						replies = RPL_NAMREPLY(user->GetNickname(), chan->GetName(), chan->GetClientList()); //
						// std::cout << replies << std::endl;
						send(user->GetFd(), replies.c_str(), replies.size(), 0);
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
		server->SendMsgToClient(user, RPL_PRIVMSG_CLIENT(user->GetNickname(), user->GetUsername(), "PRIVMSG", this->_param[0], this->_param[1]));
	else // le user est inconnu
		server->SendMsgToClient(user, ERR_NOSUCHNICK(user->GetNickname(), this->_param[0]));
}

void	Command::SendToChannel(User *user, Server *server)
{
	Channel	*recipient = server->GetChannelByName(this->_param[0]);

	if (server->HasChannel(this->_param[0]) == false)
	{
		server->SendMsgToClient(user, ERR_NOSUCHNICK(user->GetNickname(), this->_param[0]));
		return ;	
	}
	if (!recipient->HasUser(user))// && HasVoiceRight(recipient, user)
	{
		server->SendMsgToClient(user, ERR_CANNOTSENDTOCHAN(this->_param[0], recipient->GetName()));
		return ;
	}
	if (recipient->IsInBlackList(user) == true)
		return ; // fail en silence
	// check que le client a les droits d'envoyer un msg -> mode moderate activé et si le client a les privileges de parler
	if (this->_param[1] == "") // check que le msg n'est pas vide
	{
		server->SendMsgToClient(user, ERR_NOTEXTTOSEND(this->_param[0]));
		return ;
	}
	std::vector<User *>::iterator	it = recipient->GetUsers().begin();
	std::vector<User *>::iterator	ite = recipient->GetUsers().end();

	// std::vector<User *>& users = recipient->GetUsers();  // Get a reference to the vector of User pointers

	// std::vector<User *>::iterator it = users.begin();  // Access begin() on the vector object
	// std::vector<User *>::iterator ite = users.end();   // Access end() on the vector object

	while (it != ite)
	{
		server->SendMsgToClient(user,RPL_PRIVMSG_CHANEL(user->GetNickname(), user->GetUsername(), "PRIVMSG", recipient->GetName(), this->_param[1]));
		it++;
	}
}

void	Command::PRIVMSG(User *user, Server *server)
{
	if (_param.size() < 2)
		server->SendMsgToClient(user, ERR_NEEDMOREPARAMS(user->GetNickname(), this->_name));
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
