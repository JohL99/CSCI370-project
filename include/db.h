#ifndef __DB_HEADER__
#define __DB_HEADER__

#include <string>
#include <occi.h>

#include "ui.h"
#include "user.h"

#define RESPONSE_ERROR "Error: Could not complete the request %s."
#define CONNECTSTRING "database.csci.viu.ca"

#define APPOINTMENT_CSV "data/Appointment.csv"
#define USERS_CSV "data/Users.csv"

using namespace std;
using namespace oracle::occi;


class appointment {
    public:
        std::string AptID;
        std::string UserID; // ID of the person booking
        std::string F_UserName; // name of the person being booked
        std::string Time;
        std::string Date;
        std::string Status;
        std::string Length;
};

void dbConnect(string userName, string password, string connectString, Environment*& env, Connection*& conn);

void dbDisconnect(Environment*& env, Connection*& conn);

bool dbQueryLogin(string username, string password, Connection*& conn, user &usr);

string getUserName(Connection*& conn, const string& userID);

bool doesProfessorExist(Connection*& conn, const string& name);

bool isValidTime(Connection*& conn, const string& timeSlot, const string& bookee);

bool isValidDate(const std::string& dateStr);

bool isValidLength(const std::string& str);

void createAppointment(Connection*& conn, const appointment& apt);

void confirmAppointment(Connection*& conn, const string aptID, const user& usr);

void cancelAppointment(Connection*& conn, const user& usr, const string aptID);

string getAppointments(Connection*& conn, const user& usr);

void insertDataFromCSV(Connection*& conn);

void createDatabaseTables(Connection*& conn);

void dropDatabaseObjects(Connection*& conn);

void insertDummyUsers(Connection*& conn);

void insertDummyAppointments(Connection*& conn);


#endif