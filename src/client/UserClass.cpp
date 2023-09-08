# include "irc.hpp"

User::User()
{
	return ;
}

int		User::GetFd()
{
	return (this->_fd);
}

void	User::SetFd(int fd)
{
	this->_fd = fd;
	return ;
}