# include "irc.hpp"

User::User()
{
	return ;
}

int		User::GetFd()
{
	return (this->_fd);
}

void	User::SetUsername(std::string username)
{
	this->_userName = username;
	return ;
}

void	User::SetHostname(std::string hostname)
{
	this->_hostName = hostname;
	return ;
}

void	User::SetServername(std::string servername)
{
	this->_serverName = servername;
	return ;
}

void	User::SetRealname(std::string realname)
{
	this->_realName = realname;
	return ;
}

void	User::SetFd(int fd)
{
	this->_fd = fd;
	return ;
}