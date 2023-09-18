# include "irc.hpp"

void	Server::signal_handler(int)
{
	g_signal = true;
}

Server::Server(int port, char *password): _port(port), _password(password)
{
	return ;
}

int	Server::GetEpollFd()
{
	return (this->_epollfd);
}

epoll_event	Server::GetClientEvent()
{
	return (this->_clientEvent);
}

User*	Server::GetUserByFd(int fd)
{
	User *userFound;
	userFound = this->_users.find(fd)->second;
	return userFound;
}

int		Server::GetFdByNickName(std::string nickName) 
{
	for (std::map<int, User *>::iterator it =_users.begin(); it != _users.end(); it++)
	{
		if (it->second->GetNickName() == nickName)
			return (it->second->GetFd());
	}
	return (-1);
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
	std::string welcomeMessage = "001 YourNickname :Welcome to the IRC Server! Your connection has been established successfully.\r\n";
	// remplacer YourNickname par le pseudo de l'utilisateur
	int bytesSent = send(newUser->GetFd(), welcomeMessage.c_str(), welcomeMessage.size(), 0);
	if (bytesSent == -1)
	{
	    std::cerr << "Error sending welcome message." << std::endl;
	    return ;
	}
	_users.insert(std::make_pair(newUser->GetFd(), newUser));
    std::cout << "New client connected." << std::endl;
	return ;
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

	signal(SIGINT, Server::signal_handler);

	this->_clientEvent.events = EPOLLIN; // listen event
	int numEvents;
	while (!g_signal)
	{
        // numEvents = epoll_wait(this->_epollfd, this->_events, MAX_EVENTS, -1); // attend evenement jusqu'a ce que au moin 1 evenement se produise
        numEvents = epoll_wait(this->_epollfd, this->_events, 1, -1); // traite evenement 1 par 1
        if (numEvents == -1)
		{
            std::cerr << "Error : Unable to wait for events." << std::endl;
			// ajouter close des fd ouverts
            return ;
        }
        for (int i = 0; i < numEvents; i++)
		{
            if (this->_events[i].data.fd == this->_socketServer) // nouvelle connexion en attente
			{
				this->AddUser();
            }
			else // client deja connecte qui envoi des données
			{
                char buffer[1024];
                int bytesRead = recv(this->_events[i].data.fd, buffer, sizeof(buffer), 0); // read depuis fd du client
				if (bytesRead <= 0) // fermer proprement tout les fd + revoir epoll_ctl 3e argument
				{
					// supprimer user du container
                    close(this->_events[i].data.fd);
                    epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, this->_events[i].data.fd, &this->_clientEvent);
                    std::cout << "Client disconnected." << std::endl;
                }
				else // interpreter ici les input du client ->nessrine
				{
                    buffer[bytesRead] = '\0';
					std::string input = buffer;
					// this->ParseInput(input, this->_events[i].data.fd); // amodifier
					// boucle while pour recuperer toutes les infos du nouvel user qui se connecte
					size_t pos = 0;
					while ((pos = input.find('\n')) != std::string::npos)
					{
						Command cmd(input.substr(0, pos - 1));
						cmd.ExecCommand(this->_events[i].data.fd, this);
						input.erase(0, pos + 1);
					}
                }
            }
        }
    }

	// devra etre supprimer lorsque les destructeurs seront codé
    close(this->_socketServer);
    close(this->_epollfd);

	return ;
}
