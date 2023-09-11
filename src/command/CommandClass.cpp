# include "irc.hpp"

Command::Command(std::string src)
{
	size_t	pos = 0;
	bool	first = true;

	if (src.empty())
	{
		_CmdName = "";
		return ;
	}
	while (pos != std::string::npos)
	{
		pos = src.find(' ');
		if (first == true)
		{
			_CmdName = src.substr(0, pos);
			first = false;
		}
		else
		{
			std::cout << src.substr(0, pos) << std::endl;
			_Parameters.push_back(src.substr(0, pos));
		}
		src = src.erase(0, pos + 1);
	}
}

Command::~Command()
{
}

std::string	Command::GetCmdName()
{
	return(this->_CmdName);
}

std::vector<std::string>	Command::GetParameters()
{
	return (this->_Parameters);
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
