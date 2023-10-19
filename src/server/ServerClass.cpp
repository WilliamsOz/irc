# include "irc.hpp"

void	Server::SignalHandler(int)
{
	g_signal = true;
}

Server::Server(int port, const char *password): _port(port), _password(password)
{
	return ;
}

int	Server::GetEpollFd()
{
	return (this->_epollfd);
}

epoll_event*	Server::GetClientEvent()
{
	return (&(this->_clientEvent));
}

User*	Server::GetUserByFd(int fd)
{
	User *userFound;
	userFound = this->_users.find(fd)->second;
	return userFound;
}

std::string	Server::GetServerPassword( void )
{
	return(this->_password);
}

int		Server::GetFdByNickName(std::string nickName) 
{
	for (std::map<int, User *>::iterator it =_users.begin(); it != _users.end(); it++)
	{
		if (it->second->GetNickname() == nickName)
			return (it->second->GetFd());
	}
	return (-1);
}

User	*Server::GetUserByNickname(std::string nickName) 
{
	for (std::map<int, User *>::iterator it =_users.begin(); it != _users.end(); it++)
	{
		if (it->second->GetNickname() == nickName)
			return (it->second);
	}
	return (NULL);
}

std::map<int, User *>&	Server::GetUsers()
{
	return (this->_users);
}

bool	Server::IsPassCorrect(std::string channel, std::string password)
{
	std::map<std::string, Channel *>::iterator it;
	it = this->_channels.find(channel);

	if (it->second->GetPassword() == password)
		return (true);
	else
		return (false);
}

bool	Server::HasPass(std::string channel)
{
	std::map<std::string, Channel *>::iterator it;
	it = this->_channels.find(channel);

	if (it->second->GetPassword().empty() == false)
		return (true);
	else
		return (false);
}

bool	Server::HasChannel(std::string name)
{
	std::map<std::string, Channel *>::iterator it;
	it = this->_channels.find(name);

	if (it->first == name)
		return (true);
	else
		return (false);
}

Channel	*Server::GetChannelByName(std::string name)
{
	std::map<std::string, Channel *>::iterator it;

	it = this->_channels.find(name);

	if (it != this->_channels.end())
		return (it->second);
	else
		return (NULL);
}

Channel*	Server::AddChannel(std::string name)
{
	Channel *newChannel = new Channel(name);
	this->_channels[name] = newChannel;
	
	return (newChannel);
}

void	Server::AddUserToChannel(User *user, std::string channel)
{
	Channel *chan;

	std::map<std::string, Channel *>::iterator it;
	it = this->_channels.find(channel);
	chan = it->second;

	chan->AddUser(user);
	return ;
}

void	Server::AddUser()
{
	User *newUser = new User();
	sockaddr_in   addr_client; // struct qui contient addresse ip et port du client notamment
	socklen_t     addr_size = sizeof(addr_client);

	newUser->SetFd(accept(this->_socketServer, reinterpret_cast<sockaddr*>(&addr_client), &addr_size));
	if (newUser->GetFd() == -1)
	{
        std::cerr << "Error : Unable to accept new client." << std::endl;
		return ;
	}
	this->_clientEvent.data.fd = newUser->GetFd();
	this->_clientEvent.events = EPOLLIN;
	fcntl(newUser->GetFd(), F_SETFL, O_NONBLOCK);
	int retEpollCtl = epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, newUser->GetFd(), &this->_clientEvent);
	if (retEpollCtl == -1)
	{
		std::cerr << "Error : Cannot add client socket in epoll group." << std::endl;
        return ;
	}
	_users[newUser->GetFd()] = newUser;
	return ;
}

std::string Server::HandlePackets(std::string& currentCmd, const std::string& packet)
{
    // Ajouter le paquet à la commande en cours
    if (packet != "")
		currentCmd += packet;

    // Vérifier si la commande se termine
    size_t pos = currentCmd.find("\r\n");
    if (pos != std::string::npos) {
        // La commande se termine ici, donc renvoyer la commande complète
        std::string command = currentCmd.substr(0, pos);
        // Retirer la commande de la chaîne
        currentCmd = currentCmd.substr(pos + 2);
        return command;
    }

    // Si la commande n'est pas terminée, renvoyer une chaîne vide
    return "";
}

void	Server::LaunchServer()
{
	int optionVal = 1;	
	
	// creation et setup du socket
	this->_socketServer = socket(PF_INET, SOCK_STREAM, 0);
	if (this->_socketServer == -1)
	{
        std::cerr << "Error :\tCannot create socket." << std::endl;
		return ;
	}
	// setup du socket pour etre reutilisable apres un redemarrage rapide
	int retSock = setsockopt(this->_socketServer, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optionVal, sizeof(optionVal));
	if (retSock == -1)
	{
		std::cerr << "Error :\tCannot create socket." << std::endl;
		return ;
	}
	std::cout << "Socket server successfully created." << std::endl;

	// setup du server (struct)
    std::memset(&this->_serverAddress, 0, sizeof(this->_serverAddress));
    this->_serverAddress.sin_family = AF_INET;
    this->_serverAddress.sin_addr.s_addr = INADDR_ANY;
    this->_serverAddress.sin_port = htons(static_cast<uint16_t>(this->_port));

	// link le socketServer et la struct du server
	int retBind = bind(this->_socketServer, (struct sockaddr*)&this->_serverAddress, sizeof(this->_serverAddress));
	if (retBind == -1)
	{
		std::cerr << "Error :\tCannot link socket." << std::endl;
		close(this->_socketServer);
		return ;
	}

	// setup socket pour accepter demandes de connexions
	int retListen = listen(this->_socketServer, 10); // verifier dernier parametre
	if (retListen == -1)
	{
		std::cerr << "Error :\tSocket listen status failed." << std::endl;
		close(this->_socketServer);
		return ;
	}

	// creation de l'instance de epoll
	this->_epollfd = epoll_create1(0);
	if (this->_epollfd == -1)
	{
		std::cerr << "Error :\tCannot create instance of epoll." << std::endl;
		close(this->_socketServer);
		return ;
	}

	// creation du groupe d'event epoll et set socket fd
	this->_serverEvent.events = EPOLLIN; // listen event
	this->_serverEvent.data.fd = this->_socketServer;

	// ajout du socket au groupe epoll
	int retEpollctl = epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, this->_socketServer, &this->_serverEvent);
	if (retEpollctl == -1)
	{
		std::cerr << "Error : Cannot add socket in epoll group." << std::endl;
		close(this->_socketServer);
		close(this->_epollfd);
		return ;
	}

	signal(SIGINT, Server::SignalHandler);

	this->_clientEvent.events = EPOLLIN;
	int numEvents;
	std::string	currentCmd;
	while (!g_signal)
	{
        numEvents = epoll_wait(this->_epollfd, this->_events, 1, -1); // traite evenement 1 par 1
        if (numEvents == -1)
		{
            std::cerr << "Error : Unable to wait for events." << std::endl;
			// ajouter close des fd ouverts
            return ;
        }
        for (int i = 0; i < numEvents; i++)
		{
            if (this->_events[i].data.fd == this->_socketServer)
			{
				this->AddUser();
            }
			else
			{
                char packet[1024];
                int bytesRead = recv(this->_events[i].data.fd, packet, sizeof(packet), 0);
				if (bytesRead <= 0) // fermer proprement tout les fd + revoir epoll_ctl 3e argument
				{
					// supprimer user du container
                    close(this->_events[i].data.fd);
                    epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, this->_events[i].data.fd, &this->_clientEvent);
                    std::cout << "Client disconnected." << std::endl;
                }
				else
				{
                    packet[bytesRead] = '\0';
					std::string input = "";
					while (1)
					{
						if (input.empty())
							input = HandlePackets(currentCmd, packet);
						else
							input = HandlePackets(currentCmd, "");
						packet[0] = '\0';
						if (!input.empty())
						{
							Command cmd(input);
							cmd.ExecCommand(this->_events[i].data.fd, this);
						}
						if (currentCmd.empty())
							break ;
					}
                }
            }
        }
    }

    close(this->_socketServer);
    close(this->_epollfd);

	return ;
}
