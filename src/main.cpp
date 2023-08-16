#include "irc.hpp"


int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Erreur lors de la création du socket." << std::endl;
        return 1;
    }

    sockaddr_in serverAddress;
    std::memset(&serverAddress, 0, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(12345);

    if (bind(serverSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        std::cerr << "Erreur lors de la liaison du socket." << std::endl;
        return 1;
    }

    if (listen(serverSocket, 10) == -1) {
        std::cerr << "Erreur lors de la mise en écoute du socket." << std::endl;
        return 1;
    }

    int epollfd = epoll_create1(0);
    if (epollfd == -1) {
        std::cerr << "Erreur lors de la création de l'instance epoll." << std::endl;
        return 1;
    }

    epoll_event event;
    event.events = EPOLLIN; // Événement de lecture
    event.data.fd = serverSocket;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverSocket, &event) == -1) {
        std::cerr << "Erreur lors de l'ajout du socket au groupe epoll." << std::endl;
        return 1;
    }

    epoll_event events[MAX_EVENTS];

    while (true) {
        int numEvents = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (numEvents == -1) {
            std::cerr << "Erreur lors de l'attente des événements." << std::endl;
            return 1;
        }

        for (int i = 0; i < numEvents; ++i) {
            if (events[i].data.fd == serverSocket) {
                int clientSocket = accept(serverSocket, NULL, NULL);
                if (clientSocket == -1) {
                    std::cerr << "Erreur lors de l'acceptation de la connexion client." << std::endl;
                    continue;
                }
                fcntl(clientSocket, F_SETFL, O_NONBLOCK); // Met le socket en mode non bloquant
                event.events = EPOLLIN | EPOLLET; // Événement de lecture, mode Edge Triggered
                event.data.fd = clientSocket;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientSocket, &event) == -1) {
                    std::cerr << "Erreur lors de l'ajout du socket client au groupe epoll." << std::endl;
                    return 1;
                }
                std::cout << "Nouveau client connecté." << std::endl;
            } else {
                char buffer[1024];
                int bytesRead = recv(events[i].data.fd, buffer, sizeof(buffer), 0);
                if (bytesRead <= 0) {
                    close(events[i].data.fd);
                    epoll_ctl(epollfd, EPOLL_CTL_DEL, events[i].data.fd, &event);
                    std::cout << "Client déconnecté." << std::endl;
                } else {
                    buffer[bytesRead] = '\0';
                    std::cout << "Message du client : " << buffer << std::endl;
                }
            }
        }
    }

    close(serverSocket);
    close(epollfd);

    return 0;
}