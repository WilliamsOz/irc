# include "irc.hpp"

User::User() : _isOperator(false)
{
	return ;
}

int		User::GetFd()
{
	return (this->_fd);
}

std::string		User::GetNickname()
{
	return (this->_nickname);
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