#ifndef __UI_HEADER__
#define __UI_HEADER__

#include <string>
#include <occi.h>

#include "screenStack.h"
#include "db.h"
#include "user.h"

#define PROMPT "COMMAND>>"

#define menuDelimiter "---------------------------------------------------------------------"



void showWelcomeScreen(user &user);
void showLoginScreen(user &user, oracle::occi::Connection*& conn);
void showAppointmentsScreen(user &user, oracle::occi::Connection*& conn);
void showBookingScreen(user &user, oracle::occi::Connection*& conn);
void showHelpScreen();
void confirmAppointmentScreen(const std::string aptID, user& user, oracle::occi::Connection*& conn);
void cancelAppointmentScreen(user &user, oracle::occi::Connection*& conn, const std::string aptID);

void getUserCommand(std::string& userCommand);

void interpretAndHandleUserCommand(std::string command, bool& quitFlag, user& user, ScreenStack& screenstack, screenIDs screenIDs, oracle::occi::Connection*& conn);

std::string readPassword(int dbFlag);

std::string getUserName(int dbFlag);

void handleBackCommand(user& user, ScreenStack& screenstack, screenIDs screenIDs, oracle::occi::Connection*& conn);

#endif