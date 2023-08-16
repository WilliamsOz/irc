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
		std::cerr << "Error : Cannot link socket." << std::endl;
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
	int epollfd = epoll_create(0);
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

	



	return (0);
}