#include "include/ui.h"
#include "include/db.h"


// connection string
#define CONNECTSTRING "database.csci.viu.ca"


int main () {

    // quit flag
    bool quit = false;

    std::string userCommand;

    oracle::occi::Environment* env = NULL;
    oracle::occi::Connection* conn = NULL;

    std::string userName;
    std::string password;

    // get user credentials
    userName = getUserName();
    password = readPassword();

    dbConnect(userName, password, CONNECTSTRING, env, conn);
    
    // get user command and interpret it
    while (!quit) {
        getUserCommand(userCommand);
        interpretAndHandleUserCommand(userCommand, quit, conn);
    }

    if (quit) {
        dbDisconnect(env, conn);
    }

    return 0;

}



