# include "irc.hpp"

User::User() : _isOperator(false)
{
	return ;
}

int		User::GetFd()
{
	return (this->_fd);
}

void	User::SetUsername(std::string username)
{
	this->_username = username;
	return ;
}

void	User::SetHostname(std::string hostname)
{
	this->_hostname = hostname;
	return ;
}

void	User::SetServername(std::string servername)
{
	this->_servername = servername;
	return ;
}

void	User::SetRealname(std::string realname)
{
	this->_realname = realname;
}


void		User::SetNickname(std::string nickname)
{
	this->_nickname = nickname;
	return ;
}

void	User::SetFd(int fd)
{
	this->_fd = fd;
	return ;
}

bool	User::GetOperator( void )
{
	return(this->_isOperator);
}

void	User::SetOperator(bool newStatus)
{
	if (newStatus == true && this->_isOperator != newStatus)
		this->_isOperator = newStatus;
	return ;
}

std::string		User::GetNickname()
{
	return (this->_nickname);
}

std::string	User::GetUsername()
{
	return (this->_username);
}

std::string	User::GetHostname()
{
	return (this->_hostname);
}

std::string	User::GetServername()
{
	return (this->_servername);
}

std::string	User::GetRealname()
{
	return (this->_realname);
}

