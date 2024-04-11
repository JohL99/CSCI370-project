#include "ui.h"
#include "db.h"
#include "screenStack.h"

#include <iostream>


// connection string
#define CONNECTSTRING "database.csci.viu.ca"


int main () {

    user user;
    screenIDs screenIDs;

    // quit flag
    bool quit = false;

    std::string userCommand;

    oracle::occi::Environment* env = NULL;
    oracle::occi::Connection* conn = NULL;

    std::string userName;
    std::string password;

    // get database credentials
    std::string dbUserName = getUserName(1);
    std::string dbPassword = readPassword(1);

    // initialise the screen stack
    ScreenStack screenStack;
    screenStack.push(screenIDs.stackBottom);

    /* user.isLoggedIn = false;
    user.IdNum = "123456789";
    user.name = "John Doe"; */

    

    // stub testing
    /* user.isLoggedIn = true;
    user.IdNum = userName; */


    dbConnect(dbUserName, dbPassword, CONNECTSTRING, env, conn);

    if (env == NULL || conn == NULL) {
        return 1;
    }

    // show login screen
    showLoginScreen(user, conn);

    showWelcomeScreen(user, conn);
    screenStack.push(screenIDs.welcomeID);

    // get user command and interpret it
    while (!quit) {
        getUserCommand(userCommand);
        interpretAndHandleUserCommand(userCommand, quit, user, screenStack, screenIDs, conn);
    }

    if (quit) {
        dbDisconnect(env, conn);
    }

    return 0;

}



