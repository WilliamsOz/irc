#include "irc.hpp"

// /!
// Remember to handle the case where the params string does not contain a space, which means 
// that the PRIVMSG command is missing required parameters. In this case,
//  you should return an appropriate error message to the user.

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

// void handle_privmsg(std::string params, irc_reply_data* hostd, IRC* irc_conn)
// {
//     std::string target;
//     std::string message;

//     std::size_t pos = params.find(" ");
//     if (pos != std::string::npos)
//     {
//         target = params.substr(0, pos);
//         message = params.substr(pos + 1);
//     }
//     else
//     {
//         // Handle error: PRIVMSG requires a target and a message
//         return;
//     }

//     // Check if the target is a valid channel or user
//     // If target is a channel, send the message to all users in the channel
//     // If target is a user, send the message to the user
//     // Here you would use your data structures that keep track of users and channels
    
//     // Example code for sending the message to the user
//     irc_input_send_user_message(target, message, hostd, irc_conn);
// }

// void irc_input_send_user_message(std::string target, std::string message, irc_reply_data* hostd, IRC* irc_conn)
// {
//     // Check if the target is a valid user
//     // If not, handle the error

//     // Example code for sending the message to the user
//     std::string command = "PRIVMSG " + target + " :" + message;
//     irc_conn->send_raw(command);
// }
