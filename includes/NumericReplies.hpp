﻿#ifndef __NUMERIC_RPL_HPP__
# define  __NUMERIC_RPL_HPP__

# define user_id(nickname, username, cmd) (":" + nickname + "!~" + username + "@localhost " + cmd + " ")

            /* = = =    TO CONNECT    = = = */
// 001
# define RPL_WELCOME(nickname) (":localhost 001 " + nickname + " :Welcome to the Internet Relay Network " + nickname + "\r\n") // used
// 002
# define RPL_YOURHOST(nickname, version) (":localhost 002 " + nickname + " :Your host is localhost, " + "running version " + version + "\r\n")
// 003
# define RPL_CREATED(nickname, date) (":localhost 003 " + nickname + " :This server was created " + date + "\r\n")
// 004
# define RPL_MYINFO(nickname, version) (":localhost 004 " + nickname + " localhost " + version + "\r\n")

			/* = = =   CAP   = = = */
# define CAP_LS() ("CAP * LS :multi-prefix\r\n")

#define CAP_REQ() ("CAP * ACK multi-prefix\r\n")

            /* = = =    MODE    = = = */
# define SET_CLIENT_MODE(nickname, username, cmd, mode) (user_id(nickname, username, cmd) + nickname + " :+" + mode + "\r\n")

# define UNSET_CLIENT_MODE(nickname, username, cmd, mode) (user_id(nickname, username, cmd) + nickname + " :-" + mode + "\r\n")

# define SET_CHANEL_MODE(nickname, username, cmd, chanel, mode) (user_id(nickname, username, cmd) + chanel + " :+" + mode + "\r\n")

# define UNSET_CHANEL_MODE(nickname, username, cmd, chanel, mode) (user_id(nickname, username, cmd) + chanel + " :-" + mode + "\r\n")

# define SET_OTHER_CLIENT_CHANEL_MODE(nickname, username, cmd, chanel, mode, concerned_client_nickname) ((user_id(nickname, username, cmd) + chanel + " +" + mode  + " " + concerned_client_nickname + "\r\n"))

# define UNSET_OTHER_CLIENT_CHANEL_MODE(nickname, username, cmd, chanel, mode, concerned_client_nickname) ((user_id(nickname, username, cmd) + chanel + " -" + mode  + " " + concerned_client_nickname + "\r\n"))
// 324
# define RPL_CHANNELMODEIS(nickname, chanel, chanel_mods) (":localhost 324 " + nickname + " " + chanel + " +" chanel_mods + "\r\n")
// 329
# define RPL_CREATIONTIME(nickname, chanel, date) (":localhost 329 " + nickname + " " + chanel + " " +date + "\r\n")
// 368
# define RPL_ENDOFBANLIST(nickname, chanel) (":localhost 368 " + nickname + " " + chanel + " :End of channel ban list\r\n")
// 472
# define ERR_UNKNOWNMODE(nickname, mode) (":localhost 472 " + nickname + " " + mode + " :is unknown mode char to me\r\n")
// 474
# define ERR_BANNEDFROMCHAN(client, channel) (":localhost 474 " + client + " " + channel + " :Cannot join channel (+b)\r\n")
// 481
# define ERR_NOPRIVILEGES(nickname) (":localhost 481 " + nickname + " :Permission Denied- You're not an IRC operator\r\n")
// 482
# define ERR_CHANOPRIVSNEED(nickname, chanel) (":localhost 482 " + nickname + " " + chanel + " :You're not chanel operator\r\n")
// 501
# define ERR_UMODEUNKNOWNFLAG(nickname) (":localhost 501 " + nickname + " :Unknown MODE flag\r\n") 

            /* = = =    PASS    = = = */
// 464
# define ERR_PASSWDMISMATCH(nickname) (":localhost 464 " + nickname + " :Password incorrect.\r\n")

            /* = = =    WHO     = = = */
// 311
# define RPL_WHOISUSER(nickname, host_name, username) (":localhost 311 " + nickname + " " + host_name + " ~" + username + " localhost * :" + username + "\r\n") // used
// 312
# define RPL_WHOISSERVER(nickname, host_name) (":localhost 312 " + nickname + " " + host_name + " localhost :<insert catchy phrase here>\r\n")
// 315
# define RPL_ENDOFWHO(nickname, chanel) (":localhost 315 " + nickname + " " + chanel + " :End of /WHO list.\r\n")
// 317
# define RPL_WHOISIDLE(nickname, host_name, create_time) (":localhost 317 " + nickname + " " + host_name + " " + create_time + " :second idle, signon time\r\n")
// 318
# define RPL_ENDOFWHOIS(nickname, host_name) (":localhost 318 " + nickname + " " + host_name + " :End of /WHOIS list.\r\n") // used
// 352
# define RPL_WHOREPLY(nickname, chanel, username, concerned_client_nickname, status, mode) (":localhost 352 " + nickname + " " + chanel + " ~" + username + " localhost localhost " + concerned_client_nickname + " " + status + mode + " :0 " + username + "\r\n")
//352_bis
# define RPL_WHOREPLY_BIS(nickname, chanel, username, concerned_client_nickname, status) (":localhost 352 " + nickname + " " + chanel + " ~" + username + " localhost localhost " + concerned_client_nickname + " " + status +  " :0 " + username + "\r\n")
// 369
# define RPL_ENDOFWHOWAS(nickname, host_name) (":localhost 369 " + nickname + " " + host_name + " :End of WHOWAS\r\n")

            /* = = =    LIST    = = = */
// 321
# define RPL_LISTSTART(nickname) (":localhost 321 " + nickname + " Channel :Users Name\r\n")
// 322
# define RPL_LIST(nickname, chanel, usercount) (":localhost 322 " + nickname + " " + chanel + " " + usercount + " :\r\n")
// 323
# define RPL_LISTEND(nickname) (":localhost 323 " + nickname + " :End of /LIST\r\n")

            /* = = =    NICK    = = = */
// 401
# define ERR_NOSUCHNICK(nickname, host_name) (":localhost 401 " + nickname + " " + host_name + " :No such nick\r\n") // used
// 406
# define ERR_WASNOSUCHNICK(nickname, host_name) (":localhost 406 " + nickname + " " + host_name + " :There was no such nickname\r\n")
// 433
# define ERR_NICKNAMEINUSE(nickname) (":localhost 433 * " + nickname + " :Nickname is already in use\r\n")

            /* = = =    GENERIC ERR     = = = */
// 421
# define ERR_UNKNOWNCOMMAND(nickname, cmd) (":localhost 421 " + nickname + " " + cmd + " :Unknown command\r\n")
// 461
# define ERR_NEEDMOREPARAMS(nickname, cmd) (":localhost 461 " + nickname + " " + cmd + " :Not enough parameters\r\n") // used

            /* = = =    PING / PONG     = = = */
// 513
# define ERR_BADPING(nickname) (":localhost 513 " + nickname + std::string(RED) + "Error: " + std::string(RESET) + "Wrong password !\r\n")

# define RPL_PONG ":localhost PONG localhost :localhost\r\n" // used
// a faire check par oscar

# define CMD_PING(token) ("PING :" + token + "\r\n")

            /* = = =    JOIN     = = = */
# define RPL_JOIN(nickname, chanel) (':' + nickname + " JOIN #" + chanel + "\r\n") // used

# define PART_CHANEL(nickname, username, cmd, chanel) (user_id(nickname, username, cmd) + chanel + "\r\n")

# define SET_CHANEL(nickname, username, cmd, chanel) (user_id(nickname, username, cmd) + ":" + chanel + "\r\n")
// 353
# define RPL_NAMREPLY(nickname, chanel, list_client) (":localhost 353 " + nickname + " = " + chanel + " :" + list_client +"\r\n") // used
// 366
# define RPL_ENDOFNAMES(nickname, chanel) (":localhost 366 " + nickname + " " + chanel + " :End of /NAMES list.\r\n")
// 475
# define ERR_BADCHANNELKEY(nickname, channel) (":localhost 475 " + nickname + " " + channel + " :\r\n") // used
// 403
# define ERR_NOSUCHCHANNEL(nickname, chanel) (":localhost 403 " + nickname + " " + chanel + " :No such channel\r\n")
// 404
# define ERR_CANNOTSENDTOCHAN(nickname, chanel) (":localhost 404 " + nickname + " " + chanel + " :Cannot send to channel\r\n")
// 441
# define ERR_USERNOTINCHANNEL(nickname, concerned_client_nickname, chanel) ((":localhost 441 " + nickname + " " + concerned_client_nickname + " " + chanel + " :They aren't on that channel\r\n")) // used
// 442
# define ERR_NOTONCHANNEL(nickname, chanel) (":localhost 442 " + nickname + " " + chanel + " :You're not on that channel\r\n") // used
// 443
# define ERR_USERONCHANNEL(nickname, channel) (":localhost 443 " + channel + " " + nickname + " :is already on channel\r\n") // used

            /* = = =    PRV MESSAGES     = = = */
# define RPL_PRIVMSG_CHANEL(nickname, username, cmd, chanel, msg) (user_id(nickname, username, cmd) + chanel + " " + msg + "\r\n") // used

# define RPL_PRIVMSG_CLIENT(nickname, username, cmd, concerned_client_nickname, msg) (user_id(nickname, username, cmd) + concerned_client_nickname + " " + msg + "\r\n")

# define ERR_NOTEXTTOSEND(nickname) (nickname + " :No text to send\r\n") // used

            /* = = =    INVITE     = = = */
// 341
# define RPL_INVITING(nickname, target, channel) (":localhost 341 " + nickname + " " + target + " :" + channel + "\r\n") // used

# define INVITE_CLIENT(nickname, username, cmd, concerned_client_nickname, chanel) (user_id(nickname, username, cmd) + concerned_client_nickname + " :" + chanel + "\r\n") // used

# define NOCTICE_CLIENT_INVITE(nickname, chanel) (nickname + " invites you to " + chanel + "\r\n")
// 473
# define ERR_INVITEONLYCHAN(nickname, chanel) (":localhost 473 " +  nickname + " " + chanel + " :Cannot join channel (+i)\r\n")

            /* = = =    KICK     = = = */
#define KICK_CLIENT(nickname, username, cmd, chanel, concerned_client_nickname) ((user_id(nickname, username, cmd)) + chanel + " " + concerned_client_nickname + " :\r\n")

            /* = = =    NOTICE     = = = */
#define NOTICE(nickname, username, cmd, dest, msg) (user_id(nickname, username, cmd) + dest + " " + msg + "\r\n")  

            /* = = =    TOPIC     = = = */
#define SET_TOPIC(nickname, username, cmd, chanel, topic) ((user_id(nickname, username, cmd)) + chanel + " " + topic + "\r\n")
// 331
#define RPL_NOTOPIC(nickname, chanel) (":localhost 331 " + nickname + " " + chanel + " :No topic is set\r\n")
// 332
#define RPL_TOPIC(nickname, chanel, topic) (":localhost 332 " + nickname + " " + chanel + " : " + topic + "\r\n") // used
// 333
#define ROL_TOPICWHOTIME(nickname, chanel, concerned_client_nickname, time ) (":localhost 333 " + nickname +  + " " + chanel + " " concerned_client_nickname + " " + time + "\r\n")

#endif