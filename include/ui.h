#ifndef __UI_HEADER__
#define __UI_HEADER__

#include <string>
#include <occi.h>

#define PROMPT "COMMAND>>"
#define PLOGIN "USERNAME>>"
#define PPASSWORD "PASSWORD>>"

#define QUIT "quit"
#define SUBSCRIBE "subscribe"
#define UNSUBSCRIBE "unsubscribe"

enum userTypes {ADMIN, PROFESSOR, STUDENT};

struct user {
    std::string IdNum;
    std::string name;
    std::string email;
    bool isLoggedIn;
    userTypes type;
}; 


void getUserCommand(std::string& userCommand);
// Displays a command line prompt as follows:
//      COMMAND>>
// Lets user to type command and reads the user command from the prompt.


void showResponse(std::string response); 
// Displays the response to the user command.

void interpretAndHandleUserCommand(std::string command, bool& quitFlag, oracle::occi::Connection*& conn);

std::string readPassword();

std::string getUserName();

#endif