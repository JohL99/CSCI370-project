#ifndef __DB_HEADER__
#define __DB_HEADER__

#include <string>
#include <occi.h>

#include "ui.h"
#include "user.h"

#define RESPONSE_ERROR "Error: Could not complete the request %s."
#define CONNECTSTRING "database.csci.viu.ca"

using namespace std;
using namespace oracle::occi;

class response {
    public:
        std::string response;
        bool success;
};

class appointment {
    public:
        std::string AptID;
        std::string UserID; // ID of the person booking
        std::string F_UserName; // name of the person being booked
        std::string Time;
        std::string Date;
        std::string Status;
};

void dbConnect(string userName, string password, string connectString, Environment*& env, Connection*& conn);

void dbDisconnect(Environment*& env, Connection*& conn);

bool dbQueryLogin(string username, string password, Connection*& conn, user &usr);

response sendResponse();

bool doesProfessorExist(Connection*& conn, const string& name);

bool isValidTime(const string& timeSlot);

bool isValidDate(const std::string& dateStr);

void createAppointment(Connection*& conn, const appointment& apt);

void scheduleAppointment(Connection*& conn, const user& usr);

void confirmAppointment(Connection*& conn, const string aptID, const user& usr);

void cancelAppointment(Connection*& conn, const user& usr, const string aptID);

string getAppointments(Connection*& conn, const user& usr);


#endif