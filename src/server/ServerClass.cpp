# include "irc.hpp"

void	Server::signal_handler(int)
{
	g_signal = true;
}

Server::Server(int port, char *password): _port(port), _password(password)
{
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
	// memset(this->_serverEvent, 0, sizeof(this->_serverEvent)); // ne compile pas 

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
	while (!g_signal) // remplacer true par global
	{
        numEvents = epoll_wait(this->_epollfd, this->_events, MAX_EVENTS, -1); // attend evenement jusqu'a ce que au moin 1 evenement se produise
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
                int clientSocket = accept(this->_socketServer, NULL, NULL); // connexion entrante accepté et creation de socket client
																	// changer deuxieme param pour recup info du client et troisieme pour taille de struct
                if (clientSocket == -1)
				{
                    std::cerr << "Error : Unable to accept new client." << std::endl;
                    continue;
                }
                fcntl(clientSocket, F_SETFL, O_NONBLOCK); // change les attributs de clientSocket
                // event.events = EPOLLIN; // | EPOLLET; // EPOLLET : notifie uniquement lorsque etat du socket change
                this->_clientEvent.data.fd = clientSocket;
                if (epoll_ctl(this->_epollfd, EPOLL_CTL_ADD, clientSocket, &this->_clientEvent) == -1)
				{
					std::cerr << "Error : Cannot add client socket in epoll group." << std::endl;
                    return ;
                }
				std::string welcomeMessage = "001 YourNickname :Welcome to the IRC Server! Your connection has been established successfully.\r\n";
				int bytesSent = send(clientSocket, welcomeMessage.c_str(), welcomeMessage.size(), 0);
				if (bytesSent == -1)
				{
				    std::cerr << "Error sending welcome message." << std::endl;
				    // Gérer l'erreur selon vos besoins.
				}
                std::cout << "New client connected." << std::endl;
            }
			else // client deja connecte qui envoi des données
			{
                char buffer[1024];
                int bytesRead = recv(this->_events[i].data.fd, buffer, sizeof(buffer), 0); // read depuis fd du client
                // fonction createUser(this->_events[i].data.fd, ...)
				if (bytesRead <= 0)
				{
                    close(this->_events[i].data.fd);
                    epoll_ctl(this->_epollfd, EPOLL_CTL_DEL, this->_events[i].data.fd, &this->_clientEvent);
                    std::cout << "Client disconnected." <<std::endl;
                }
				else 
				{
                    buffer[bytesRead] = '\0';
					std::cout << buffer << std::endl;
                }
            }
        }
    }

	// devra etre supprimer lorsque les destructeurs seront codé
    close(this->_socketServer);
    close(this->_epollfd);

	return ;
}
