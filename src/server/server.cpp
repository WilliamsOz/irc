# include "irc.hpp"

bool g_signal = false;

void	Server::signal_handling(int)
{
	g_signal = true;
}

int	server()
{
	// creation et setup du socket
	int socketServer = socket(PF_INET, SOCK_STREAM, 0);
	if (socketServer == -1)
	{
        std::cerr << "Error : Cannot create socket." << std::endl;
		return (1);
	}

	// setup du server (struct)
	sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(6667);

	// link le socketServer et la struct du server
	int retBind = bind(socketServer, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (retBind == -1)
	{
		std::cerr << "Error : Cannot link socket." << std::endl;
		return (1);
	}

	// setup socket pour accepter demandes de connexions
	int retListen = listen(socketServer, 10); // verifier dernier parametre
	if (retListen == -1)
	{
		std::cerr << "Error : Socket listen status failed." << std::endl;
		return (1);
	}

	// creation de l'instance de epoll
	int epollfd = epoll_create1(0);
	if (epollfd == -1)
	{
		std::cerr << "Error : Cannot create instance of epoll." << std::endl;
		return (1);
	}


	// creation du groupe d'event epoll et set socket fd
	epoll_event event;
	event.events = EPOLLIN; // listen event
	event.data.fd = socketServer;

	// ajout du socket au groupe epoll
	int retEpollctl = epoll_ctl(epollfd, EPOLL_CTL_ADD, socketServer, &event);
	if (retEpollctl == -1)
	{
		std::cerr << "Error : Cannot add socket in epoll group." << std::endl;
		return (1);
	}

	epoll_event events[MAX_EVENTS]; // classe client


	epoll_event eventClient;
	eventClient.events = EPOLLIN; // listen event
	signal(SIGINT, Server::signal_handling);
	while (!g_signal) // remplacer true par global
	{
        int numEvents = epoll_wait(epollfd, events, MAX_EVENTS, -1); // attend evenement jusqu'a ce que au moin 1 evenement se produise
        if (numEvents == -1)
		{
            std::cerr << "Error : Unable to wait for events." << std::endl;
            return (1);
        }
        for (int i = 0; i < numEvents; ++i)
		{
            if (events[i].data.fd == socketServer) // nouvelle connexion en attente
			{
                int clientSocket = accept(socketServer, NULL, NULL); // connexion entrante accepté et creation de socket client
																	// changer deuxieme param pour recup info du client et troisieme pour taille de struct
                if (clientSocket == -1)
				{
                    std::cerr << "Error : Unable to accept new client." << std::endl;
                    continue;
                }
                fcntl(clientSocket, F_SETFL, O_NONBLOCK); // change les attributs de clientSocket
                // event.events = EPOLLIN; // | EPOLLET; // EPOLLET : notifie uniquement lorsque etat du socket change
                eventClient.data.fd = clientSocket;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSocket, &eventClient) == -1)
				{
					std::cerr << "Error : Cannot add client socket in epoll group." << std::endl;
                    return (1);
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
                int bytesRead = recv(events[i].data.fd, buffer, sizeof(buffer), 0); // read depuis fd du client
                if (bytesRead <= 0)
				{
                    close(events[i].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &eventClient);
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

    close(socketServer);
    close(epollfd);

	return (0);
}