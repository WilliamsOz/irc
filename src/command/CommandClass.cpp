# include "irc.hpp"

void	Command::ExecCommand(int clientFd, User *user)
{
	if (this->_commands.find(this->_name) != _commands.end())
		this->_commands[this->_name](clientFd, user);
	else
		std::cout << "Unknown command\n";
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
		{
			std::cout << src.substr(0, pos) << std::endl;
			_param.push_back(src.substr(0, pos));
		}
		src = src.erase(0, pos + 1);
	}
	this->SetUpCommandsContainer();
}

void Command::addCommand(const std::string& commandName, std::function<void(int, User)> commandFunction)
{
        _commands[commandName] = commandFunction;
}

void	Command::SetUpCommandsContainer()
{
	addCommand("JOIN",  [this](int clientFd, User user) { JOIN(clientFd, user); });
	addCommand("PING",  [this](int clientFd, User user) { PING(clientFd, user); });
}

void	Command::JOIN(int clientFd, User user)
{

}

void	Command::PING(int clientFd, User user)
{
	(void)user;

	std::cout << this->_name << std::endl;
	std::string pongMessage = "PONG :" + this->_name + "\r\n";
	send(clientFd, pongMessage.c_str(), pongMessage.size(), 0);
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
