/*
 * Software created by:
 * Michael Champagne
 * Raven (ID: 22885233 | 512388)

 * Licensed with GNU lpgl v3.0
 * Found in gnu-v3.txt or available online here
 * http://www.gnu.org/licenses/lgpl.txt
 */

#include "baseClient.h"

baseClient::baseClient(string username, string password, string botAdmin, string adminName, string botName, string cmdAdmin, string cmdUser)
{
    //Login Details | No more compile time username and password
	this->username 	= username;
	this->password 	= password;

	this->botAdmin 	= botAdmin;     //the ID of the user who owns the bot
	this->botName	= botName;	    //Set the bots name to give it personality
	this->adminName	= adminName;    //used for printing the admins name in messages

	//these set the way you call commands
	//ie #leave or /help
	this->cmdAdmin 	= cmdAdmin;
	this->cmdBase	= cmdUser;

    //Feature is currently depreciated
    //A way to save the group number for special bot commands
	//controlGroup      = "[Group ID]"; //palringo
	//controlGroupName  = "[Group ID]"; //palringo

	//various "system" variables
	canTalk     = true;     //if the bot is muted or not
	adminOnline = false;    //if admin is online or not
	startTime   = time(NULL);//sets the time the bot initializes
}

//triggered when a message is received
void baseClient::recv_message(string group, string user, string message)
{
    this->parse_commands(group, user, engine.splitStr(message, ' '));
}

//triggered when a PM is received
void baseClient::recv_pm(string name, string user, string message)
{
	if(user != botAdmin)
	{
		string buffer = "";
		buffer.append("====== PM Received ======\r\n");
		buffer.append("From: ");
		buffer.append(name);
		buffer.append("\r\n");
		buffer.append(message);
		buffer.append("\r\n=======================\r\n");

		this->send_pm(botAdmin, buffer);
		buffer = "";
		buffer.append("#msg ");
		buffer.append(user);
		buffer.append(" ");
		this->send_pm(botAdmin, buffer);
	}
	else
	{
		this->parse_pm(name, user, engine.splitStr(message,' '));
	}
}

//triggered when anyone joins or leaves a group
void baseClient::group_update()
{
    //TODO: fully furnish, doesnt work at all except for the fact it gets triggered
    engine.pl("palClient-> someone joined or left a group");
}

//triggered when anyone commits an admin action
void baseClient::group_admin(string group, string admin, string user, string action)
{
    //TODO: works, just need more details coded

    string op       = "1";
    string mod      = "2";
    string silence  = "8";
    string reset    = "0";
    string kick     = "16";
    string ban      = "4";

    if(action == op)
        this->send_message(group, "Congrats!\r\nJust dont power abuse");
    else if(action == mod)
        this->send_message(group, "Aww thats cute\r\nYou think you have power");
    else if(action == silence)
        this->send_message(group, "I love duct tape!");
    else if(action == reset)
        this->send_message(group, "Yay they reset you :3");
    else if(action == kick)
        this->send_message(group, "Get told");
    else if(action == ban)
        this->send_message(group, "Here comes the buthurt");
}

//ive left in some of the basic bot features im writting for my personal bot to give you a feel of how i do things
void baseClient::parse_commands(string group, string user, vector<string> data)
{
	int		blocks		= data.size();
	string	cmd			= data[0];

	if(user == botAdmin && adminOnline == false)
	{
		//sets the bot admins status to being back online
		adminOnline = true;
		adminMessage = "";
		this->send_message(group, adminName+" is back online!");
	}

	if(cmd == cmdAdmin+"check")
	{
		if(user == botAdmin)
		{
			if(canTalk == true)
                this->send_message(group, "I am online!");
			else
			{
				//if shes been muted this will run
				canTalk = true;
				this->send_message(group, "I've been muted but im here ;_;");
				canTalk = false;
			}
		}
	}
	else if(cmd == cmdAdmin+"leave")
	{
		if(user == botAdmin)
		{
			this->group_part(group);
		}
	}
	else if(cmd == cmdAdmin+"join")
	{
		if(user == botAdmin)
		{
			if(blocks == 2)
			{
				//just sends group name
				this->group_join(data[1]);
			}
			else if(blocks >= 3)
			{
				//sends group name only
				this->group_join( this->messagePatcher(data) );
			}
			else
			{
				this->send_message(group, "#join <group name>");
			}
		}
	}
	else if(cmd == cmdAdmin+"msg")
	{
		if(user == botAdmin)
		{
			if(blocks > 2)
			{
				//generate message
				string buff = this->messagePatcher(data, " ", 2);

				//send the buffer
				this->send_pm(data[1], buff);
				this->send_message(group, "Message Sent!");
			}
			else
			{
				this->send_message(group, cmdAdmin+"msg <user ID> <message>");
			}
		}
	}
	else if(cmd == cmdAdmin+"away")
	{
		/* Allows the bot admin to go afk, and the bot will know your not online, and leaves a message showing your not online */
		if(user == botAdmin)
		{
			if(blocks > 1)
			{
				adminMessage	= this->messagePatcher(data, " ");
				adminOnline		= false;
				this->send_message(group, "Bye " + adminName + "!\r\nAnd don't worry I saved the message");
			}
			else
			{
			    adminOnline		= false;
				this->send_message(group, "Bye "+adminName+"!");
			}
		}
	}
	else if(cmd == cmdAdmin+"mute")
	{
	    if(user == botAdmin)
        {
            if(canTalk == true)
            {
                this->send_message(group, "Muted myself");
                canTalk = false;
            }
            else
            {
                canTalk = true;
                this->send_message(group, "I can talk again!");
            }
        }
	}
	else if(cmd == cmdAdmin+"help")
    {
        if(user == botAdmin)
        {
             this->send_message(group, 	"Admin Help\r\n"+
                                        cmdAdmin+"check\r\n"+
                                        cmdAdmin+"leave\r\n"+
                                        cmdAdmin+"join <group name>\r\n"+
                                        cmdAdmin+"msg <user id> <message>\r\n"+
                                        cmdAdmin+"away <message>\r\n"+
                                        cmdAdmin+"mute (also unmute)"
                                        );
        }
    }
	else if(cmd == cmdBase+"admin")
    {
        string output = "The bot owner is "+adminName+" Userid: "+botAdmin;
        //this->send_message(group, "The bot owner is "+adminName+" Userid: "+botAdmin);

        if(adminOnline)
            output += "\r\nAnd they are online";
        else
        {
             output += "\r\nAnd they are offline";

             if(adminMessage != "")
                output += "\r\nBut they left a message!\r\n"+adminMessage;
        }

        this->send_message(group, output);
    }
	else if(cmd == cmdBase+"help")
	{
		this->send_message(group, 	"User Help\r\n"+
                                    cmdBase+"google <query>\r\n"+
									cmdBase+"youtube <query>\r\n"+
									cmdBase+"credits\r\n"+
									cmdBase+"admin\r\n"+
									cmdBase+"uptime\r\n"+
									cmdBase+"website");
	}
	else if(cmd == cmdBase+"credits")
	{
		this->send_message(group, 	"Ravenclaw bot created by Raven [furry]\r\n\r\n"
									"I can thank so many people on pal for this\r\n"
									"Mainly ross, sniper, and nom\r\n"
									"And my friend city fox for a binary file reader\r\n"
									"the rest know who you are\r\n");
	}
	else if(cmd == cmdBase+"website")
	{
		this->send_message(group, "This bot is open source under GPLv3"
                                    "\r\nTalk to "+adminName+" about getting access to the repo\r\n"
                                    "https://bitbucket.org/BlackRaven/ravenclaw/");
	}
	else if(cmd == cmdBase+"google")
	{
		if(blocks > 1)
		{
			string search = "Here you go\r\nhttp://www.google.com/search?q=";
			search.append(this->messagePatcher(data, "+"));
			this->send_message(group, search);
		}
		else
		{
			this->send_message(group, "Sorry thats not how you use this command\r\n"+cmdBase+"google <search query>");
		}
	}
	else if(cmd == cmdBase+"youtube")
	{
		if(blocks > 1)
		{
			string search = "Videos are fun!\r\nhttp://www.youtube.com/results?search_query=";
			search.append(this->messagePatcher(data, "+"));
			this->send_message(group, search);
		}
		else
		{
			this->send_message(group, "Sorry hun thats not how you use this command\r\n"+cmdBase+"youtube <search query>");
		}
	}
	else if(cmd == cmdBase+"uptime")
    {
        double uptime = difftime(time(NULL), startTime);

        char buffer[256];
        int days    = uptime / 86400;
        int hours   = (uptime - 86400 * days) / 3600;
        int minutes = ((uptime - 86400 * days) - (3600 * hours))/60;
        int seconds = ((uptime - 86400 * days) - (3600 * hours))- 60 * minutes;

        //double days = (double)uptime/86400;
        snprintf(buffer, sizeof(buffer), "Uptime: %id %ih %im %is", days, hours, minutes, seconds);

        this->send_message(group, buffer);
    }
	else if(cmd == botName) /* "talk to the bot" */
	{
		if(blocks >= 2)
		{
			string aiCommand = this->messagePatcher(data, " ");

			//command phrases
			//such basic AI haha
			if(aiCommand == "is funny")
			{
				if(user == botAdmin)
				{
					this->send_message(group, "Of course I am");
				}
				else
				{
					this->send_message(group, "im not just funny im amazing :3");
				}
			}
			else if(aiCommand == "is cute")
			{
				if(user == botAdmin)
				{
					this->send_message(group, "Thank you "+botAdmin+" :3");
				}
				else
				{
					this->send_message(group, "i know im cute");
					this->send_message(group, "im flattered\r\nbut seriously\r\nim just some computer code");
					this->send_message(group, "#android-problems");
				}
			}
			else if(aiCommand == "is ugly")
			{
				if(user == botAdmin)
				{
					this->send_message(group, "Stop being a bitch "+botAdmin);
				}
				else
				{
					this->send_message(group, "/me cries");
					this->send_message(group, "You're a bitch >:C");
				}
			}
		}
		else
		{
			this->send_message(group, "Hello :3");
		}
	}

	else /* filters */
	{
		if(blocks >= 1) /* phrase filters */
		{
			//generates our phrase with our magical messagePatcher <3
			string phrase = this->messagePatcher(data, " ", 0);

			if(phrase == "bye "+botName)
			{
				this->send_message(group, "see ya soon :D");
			}
			else if(phrase == "bye guys")
			{
				this->send_message(group, "/me waves goodbye");
			}
			else /* word filters, triggers if the word is anywhere in the users post */
			{
				//word trigger
				//remember to break at the end of each if
				//for the love of god dont forget
				for(int i=0; i<blocks; i++)
				{
					//Hash Tags, triggers if someone uses a hashtag
					if(i==(blocks-1) && engine.str2ch(data[i])[0] == '#')
					{
						string hash = data[i];
						if(hash == "#poniproblems")
						{
							this->send_message(group, "Fuck you, and fuck your ponies");
							this->admin_silence(group, user);
						}
						else if(hash == "#yolo")
                        {
                            this->send_message(group, "NIGGA\r\nYou only live fuckin once\r\nSo stop getting fucking shitfaced every damn day because YOU HAVE A LIFE TO LIVE NIGGA");
                        }
						else if(hash == "#swag")
						{
							this->send_message(group, "Man check you out, fuckin swaggin so hard, the term swag itself comes from how swaggin you are.");
						}

						break;
					}
					else if(data[i] == "u" || data[i] == "c" || data[i] == "r" || data[i] == "y" || data[i] == "o")
					{
						this->send_message(group, "Use words not letters\r\nDon't be lazy!");
						//this->admin_silence(group, user);
						break;
					}
					else if(data[i] == "ass" || data[i] == "bitch" || data[i] == "fuck" || data[i] == "shit" || data[i] == "damn" || data[i] == "hell" || data[i] == "cunt")
					{
						this->send_message(group, "Potty Mouth!!!");
						//this->admin_silence(group, user);
						break;
					}
					else if(data[i] == "cod" || (i<blocks-2 && data[i] == "call" && data[i+1] == "of" && data[i+2] == "duty"))
					{
						/* can also detect phrases anywhere in the post */
						if(user != botAdmin)
						{
							this->send_message(group, 	"it would be awesome if you didnt talk about that shitty game\r\n"
														"It has the most inaccurate presntations of firearms second only to doom\r\n"
														"Brink has more accurate weapons than COD\r\n"
														"The ak74 isnt a fucking SMG");
							break;
						}
					}
				}
			}
		}
	}
}

//seperate command parser for pms
void baseClient::parse_pm(string name, string user, vector<string> data)
{
	int		blocks		= data.size();
	string	cmd			= data[0];

	if(cmd == cmdAdmin+"msg")
    {
		if(user == botAdmin)
		{
			if(blocks > 2)
			{
				//generate message
				string buff = this->messagePatcher(data, " ", 2);

				//send the buffer
				this->send_pm(data[1], buff);
			}
			else
			{
				this->send_pm(user, "#msg <user ID> <message>");
			}
		}
    }
    if(cmd == cmdAdmin+"uptime")
    {
        double uptime = difftime(time(NULL), startTime);

        char buffer[256];
        int days    = uptime / 86400;
        int hours   = (uptime - 86400 * days) / 3600;
        int minutes = ((uptime - 86400 * days) - (3600 * hours))/60;
        int seconds = ((uptime - 86400 * days) - (3600 * hours))- 60 * minutes;

        //double days = (double)uptime/86400;
        snprintf(buffer, sizeof(buffer), "Uptime: %id %ih %im %is", days, hours, minutes, seconds);

        this->send_pm(user, buffer);
    }
    if(cmd == cmdAdmin+"leave")
    {
        if(user == botAdmin)
		{
			if(blocks == 2)
			{
				//just sends group name
				this->group_part(data[1]);
			}
			else
			{
				this->send_pm(user, cmdAdmin+"leave <group id>");
			}
		}
    }
    if(cmd == cmdAdmin+"join")
    {
        if(user == botAdmin)
		{
			if(blocks == 2)
			{
				//just sends group name
				this->group_join(data[1]);
			}
			else if(blocks >= 3)
			{
				//sends group name only
				this->group_join( this->messagePatcher(data) );
			}
			else
			{
				this->send_pm(user, cmdAdmin+"join <group name>");
			}
		}
    }
    if(cmd == cmdAdmin+"help")
    {
        if(user == botAdmin)
		{
		    this->send_pm(user, 	"Admin Help\r\n"+
                                        cmdAdmin+"msg <user id> <message>\r\n"+
                                        cmdAdmin+"leave <group id>\r\n"+
                                        cmdAdmin+"join <group name>\r\n"+
                                        cmdAdmin+"uptime\r\n"
                                        );
		}
    }
}

////////////////////////////////////////////////////////////
/* Everything Below This Segmet should remain un touched */
//////////////////////////////////////////////////////////
//base group functions
void 	baseClient::group_join(string groupName, string password)	{ palGroup->group_join(groupName, password); }
void	baseClient::group_part(string groupID)						{ palGroup->group_part(groupID); }

//base message handlers
void 	baseClient::send_message(string group, string message)		{ if(canTalk) palMesg->send_message(group, message); }
void 	baseClient::send_pm(string id, string message) 				{ palMesg->send_pm(id, message); }

//base admin functions
void	baseClient::admin_admin  (string groupID, string userID)	{ palMesg->admin_admin(groupID, userID); }
void	baseClient::admin_mod    (string groupID, string userID)	{ palMesg->admin_mod(groupID, userID); }
void	baseClient::admin_silence(string groupID, string userID)	{ palMesg->admin_silence(groupID, userID); }
void	baseClient::admin_reset  (string groupID, string userID)	{ palMesg->admin_reset(groupID, userID); }
void	baseClient::admin_kick   (string groupID, string userID)	{ palMesg->admin_kick(groupID, userID); }
void	baseClient::admin_ban    (string groupID, string userID)	{ palMesg->admin_ban(groupID, userID); }

//sets pointers
void	baseClient::set_palMesg(palringoMessage *mesg)				{ palMesg = mesg; }
void	baseClient::set_palGroup(palringoGroup *group)				{ palGroup = group; }

//public functions to get variables
string 	baseClient::get_Username() 									{ return this->username; }
string 	baseClient::get_Password() 									{ return this->password; }

//various usefull functions
string baseClient::messagePatcher(vector<string> message, string patch, int start)
{
	string output = "";
	int	   blocks = message.size();

	if(blocks > start+1)
	{
		for(int i=start; i<blocks; i++)
		{
			output.append(message[i]);
			if(i != blocks-1)
				output.append(patch);
		}
	}
	else if(blocks == 2)
	{
		output.append(message[start]);
	}
	else if(blocks == start+1)
    {
        output.append(message[start]);
    }
	else
	{
		output.append("null");
	}

	return output;
}

