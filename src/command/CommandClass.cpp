# include "../../includes/irc.hpp"

std::string	Command::GetCmdName()
{
	return(this->_cmd_name);
}

std::string Command::GetParameter(int i)
{
	std::cout << "in get paramter " << this->_parameters[i] << std::endl;
	return (this->_parameters[i]);
	// tester avec un chiffre aui depasse
}

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

int main(int ac, char **av)
{
	Command		cmd(av[1]);

	std::cout << cmd.getCmdName() << std::endl;
	for (int i = 0; cmd.getParameter(i) != ""; i++)
	{
		std::cout << "for i = " << i << " " << cmd.getParameter(i) << std::endl;

	}
	return (0);
}