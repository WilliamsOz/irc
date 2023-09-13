#ifndef COMMANDCLASS_HPP
# define COMMANDCLASS_HPP

# include "irc.hpp"

class Command
{
	private:
		std::string 				_name;
		std::vector<std::string>	_param;
		
	public:
		Command();
		~Command();
		
		void	USER(User user);
		std::string	GetParam(int which);
};

#endif