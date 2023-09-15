# include "irc.hpp"

void	Command::ExecCommand(int clientFd, Server *server)
{
	if (this->_commands.find(this->_name) != _commands.end())
	{
		(this->*this->_commands[this->_name])(server->GetUserByFd(clientFd), server);
	}
	else
		std::cout << "Unknown command -> " << this->_name << this->_param[0] << "\n";
}

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

void	Command::SetUpCommandsContainer()
{
	// _commands["JOIN"] = &Command::JOIN;
    _commands["PING"] = &Command::PING;
    _commands["OPER"] = &Command::OPER;
    // _commands["USER"] = &Command::USER;
	//etc
}

// void	Command::JOIN(int clientFd, User *user)
// {

// }

// void	Command::CAP(int clientFd, User *user)
// {
// 	if (this->GetParameters()[0] == "LS") // liste les capacités disponible pour les clients
// 	{
// 		std::string response = "CAP * LS :multi-prefix\r\n";
// 		send(user->GetFd(), response.c_str(), response.length(), 0);
// 		std::cout<<"response : "<<response<<std::endl;
// 	}
// 	if (!(this->GetParameters()[0].compare("REQ"))) // demande l'obtemtion d'une capacité
// 	{
// 		std::string response = "CAP * ACK multi-prefix\r\n";
// 		send(user->GetFd(), response.c_str(), response.length(), 0);
// 		std::cout<<"response : "<<response<<std::endl;
//     }
// 	if (!(this->GetParameters()[0].compare("END"))) // peut-etre ajouter des infos supplementaires
// 	{
// 		std::string response = "001 " +(user->GetNickName())+": Bienvenue sur le serveur Irc\r\n";
// 		send(user->GetFd(), response.c_str(), response.length(), 0);
// 	}
// }

void	Command::PING(User *user, Server *server)
{
	(void)server;

	std::cout << this->_name << std::endl;
	std::string pongMessage = "PONG :" + this->_name + "\r\n";
	send(user->GetFd(), pongMessage.c_str(), pongMessage.size(), 0);
}

void	Command::OPER(User *user, Server *server)
{
	std::string	success_msg = "381 :You are now an IRC operator\r\n";
	if (this->GetParameters()[1] == server->GetServerPassword())
	{
		if (user->GetOperator() == true)
			std::cout << "xdddddddddddd" << std::endl;
		else
		{
			user->SetOperator(true);
			send(user->GetFd(), success_msg.c_str(), success_msg.size(), 0);
		}
	}
	else
		std::cout << "Server password is incorrect, please try again." << std::endl;
	return ;
}

Command::~Command()
{
}

std::string	Command::GetCmdName()
{
	return(this->_name);
}

std::vector<std::string>	Command::GetParameters()
{
	return (this->_param);
}

// int main(int ac, char **av)
// {
//     Command cmd(av[1]);

//     std::cout << cmd.GetCmdName() << std::endl;

//     std::vector<std::string> parameters = cmd.GetParameters(); // Stocker la valeur dans une variable

//     for (std::vector<std::string>::iterator it = parameters.begin(); it != parameters.end(); it++)
//         std::cout << "->" << *it << std::endl;

//     return (0);
// }
