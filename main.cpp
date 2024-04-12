#include "ui.h"
#include "db.h"
#include "screenStack.h"

#include <iostream>



int main () {

    user user;
    screenIDs screenIDs;

    // quit flag
    bool quit = false;

    std::string userCommand;

    oracle::occi::Environment* env = NULL;
    oracle::occi::Connection* conn = NULL;

    // get database credentials
    std::string dbUserName = getUserName(1);
    std::string dbPassword = readPassword(1);

    // initialise the screen stack
    ScreenStack screenStack;
    screenStack.push(screenIDs.stackBottom);


    dbConnect(dbUserName, dbPassword, CONNECTSTRING, env, conn);

    if (env == NULL || conn == NULL) {
        // error connecting to database
        return 1;
    }

    createDatabaseTables(conn);
    //insertDataFromCSV(conn);
    insertDummyUsers(conn);
    insertDummyAppointments(conn);


    // show login screen
    showLoginScreen(user, conn);

    showWelcomeScreen(user);
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



