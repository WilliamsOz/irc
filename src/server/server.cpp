# include "irc.hpp"
# include "server.hpp"

int	server()
{
	// creation et setup du socket
	int socketfd = socket(PF_INET, SOCK_STREAM, 0);
	if (socketfd == -1)
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

	// link le socketfd et la struct du server
	int retBind = bind(socketfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress));
	if (retBind == -1)
	{
		// std::cerr << "Error : Cannot link socket." << std::endl;
		perror("Error binding socket");
		return (1);
	}

	// setup socket pour accepter demandes de connexions
	int retListen = listen(socketfd, 10); // verifier dernier parametre
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
	event.data.fd = socketfd;

	// ajout du socket au groupe epoll
	int retEpollctl = epoll_ctl(epollfd, EPOLL_CTL_ADD, socketfd, &event);
	if (retEpollctl == -1)
	{
		std::cerr << "Error : Cannot add socket in epoll group." << std::endl;
		return (1);
	}

	epoll_event events[MAX_EVENTS];

	while (true)
	{
        int numEvents = epoll_wait(epollfd, events, MAX_EVENTS, -1); // attend evenement jusqu'a ce que au moin 1 evenement se produise
        if (numEvents == -1)
		{
            std::cerr << "Error : Unable to wait for events." << std::endl;
            return (1);
        }
        for (int i = 0; i < numEvents; i++)
		{
            if (events[i].data.fd == socketfd) // nouvelle connexion en attente
			{
                int clientSocket = accept(socketfd, NULL, NULL); // connexion entrante accepté et creation de socket client
																	// changer deuxieme param pour recup info du client et troisieme pour taille de struct
                if (clientSocket == -1)
				{
                    std::cerr << "Error : Unable to accept new client." << std::endl;
                    continue;
                }
                fcntl(clientSocket, F_SETFL, O_NONBLOCK); // change les attributs de clientSocket
                event.events = EPOLLIN | EPOLLET; // EPOLLET : notifie uniquement lorsque etat du socket change
                event.data.fd = clientSocket;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSocket, &event) == -1)
				{
					std::cerr << "Error : Cannot add client socket in epoll group." << std::endl;
                    return (1);
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
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &event);
                    std::cout << "Client disconnected." << std::endl;
                }
				else
				{
                    buffer[bytesRead] = '\0';
					if (strstr(buffer, "/join") != NULL)
						std::cout << "Commande join recus" << std::endl;
                }
            }
        }
    }

    close(socketfd);
    close(epollfd);

	return (0);
}