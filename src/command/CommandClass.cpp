# include "../../includes/irc.hpp"

Command::Command(std::string src)
{
	size_t	pos = 0;
	bool	first = true;

	if (src.empty())
	{
		_cmd_name = "";
		return ;
	}
	while (pos != std::string::npos)
	{
		pos = src.find(' ');
		if (first == true)
		{
			_cmd_name = src.substr(0, pos);
			first = false;
			std::cout << "in cmd name -> " << _cmd_name << std::endl;
		}
		else
			_parameters.push_back(src.substr(0, pos));
		src = src.erase(0, pos + 1);
	}
}

Command::~Command()
{
	// truc à free ?
	// substr?
}

std::string	Command::GetCmdName()
{
	return(this->_cmd_name);
}

std::vector<std::string>	Command::GetParameters()
{
	return (this->_parameters);
}

int main(int ac, char **av)
{
	Command		cmd(av[1]);

	std::cout << cmd.GetCmdName() << std::endl;
	for (std::vector<std::string>::iterator it = cmd.GetParameters().begin(); it != cmd.GetParameters().end(); it++)
		std::cout  << *it << std::endl;
	return (0);
}