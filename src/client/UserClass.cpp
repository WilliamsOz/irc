# include "irc.hpp"

User::User()
{
	return ;
}

int		User::GetFd()
{
	return (this->_fd);
}

std::string		User::GetNickName()
{
	return (this->_nickname);
}

void	User::SetFd(int fd)
{
	this->_fd = fd;
	return ;
}