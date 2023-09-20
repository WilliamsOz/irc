# include "irc.hpp"

Channel::Channel(const std::string name) : _name(name), _mode("none")
{
	return ;
}

Channel::~Channel()
{
	return ;
}

void	Channel::AddUser(User *toAdd)
{
	this->_users.push_back(toAdd);
	return ;
}

void	Channel::AddOper(User *toAdd)
{
	this->_opers.push_back(toAdd);
	return ;
}
