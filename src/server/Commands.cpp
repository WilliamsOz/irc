#include "irc.hpp"


// void	privmsg(Command command, std::string parameter, std::string user)
// {
// 	if (parameter == "#") // alors c un msg pour un channel
// 		sendToChannel(command);
// 	else 
// 	{
// 		if (_users.find(user) != _users.end()) // le user est connu
// 		{
// 			std::string	response = ":" + user.getNickName() + " " + command.getNumericReply() + " " + command.getMsg() + "\r\n";
// 			send(...);
// 			std::cout << "response =>" << response << std::endl;
// 		}
// 		else // le user est inconnu
// 		{
// 			std::string	response = ":localhost 401" + user.getNickName() + " : No such NickName\r\n";
// 			send(...);
// 			std::cout << "response =>" << response << std::endl;
// 		}
// 	}
// }