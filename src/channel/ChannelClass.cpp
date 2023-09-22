# include "irc.hpp"

Channel::Channel() : _modes("none")
{
	return ;
}

Channel::Channel(const std::string name) : _name(name), _modes("none")
{
	return ;
}

Channel::~Channel()
{
	return ;
}

std::string	Channel::GetName()
{
	return (this->_name);
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

void	Channel::SetModes(std::string modes)
{
	this->_modes = modes;
	return ;
}

std::string	Channel::GetModes()
{
	return (this->_modes);
}
