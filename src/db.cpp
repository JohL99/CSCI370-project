#include "db.h"
#include "ui.h"
#include "user.h"

#include <string>
#include <occi.h>
#include <iostream>
#include <ctime>
#include <sstream>

using namespace std;
using namespace oracle::occi;


/**
 * @brief Establishes a connection to the database using the provided credentials and connection string.
 * 
 * @param userName The username for the database connection.
 * @param password The password for the database connection.
 * @param connectString The connection string for the database.
 * @param env A pointer to the Environment object.
 * @param conn A pointer to the Connection object.
 */
void dbConnect(string userName, string password, string connectString, Environment*& env, Connection*& conn) {
    
    try {
        env = Environment::createEnvironment(Environment::DEFAULT);
        conn = env->createConnection(userName, password, connectString);
        cout << "Successfully connected to the database." << endl;

    } catch (SQLException& ex) {
        cout << "Failed to connect to the database. Error: " << ex.what();
        cout << "Quitting..." << endl;
    }
}


/**
 * @brief Disconnects from the database.
 * 
 * @param env A pointer to the database environment.
 * @param conn A pointer to the database connection.
 */
void dbDisconnect(Environment*& env, Connection*& conn) {

    if (conn) {
        env->terminateConnection(conn);
    }

    if (env) {
        Environment::terminateEnvironment(env);
    }

    cout << "Successfully disconnected from the database." << endl;

}

bool dbQueryLogin(string username, string password, Connection*& conn, user &usr) {
    try {
        Statement* stmt = conn->createStatement();
        stmt->setSQL("SELECT * FROM Students WHERE TRIM(StudentID) = :1 AND TRIM(Password) = :2");
        stmt->setString(1, username);
        stmt->setString(2, password);
        ResultSet* rs = stmt->executeQuery();

        if (rs->next()) {
            usr.isLoggedIn = true;
            usr.IdNum = rs->getString(1);
            usr.name = rs->getString(2) + " " + rs->getString(3); // Combine first name and last name
            usr.email = rs->getString(4); // Get email from the result set
            usr.type = STUDENT;
            delete rs;
            delete stmt;
            return true;
        } else {
            delete rs;
            delete stmt;

            // If not found in Students table, check Professors table
            stmt = conn->createStatement();
            stmt->setSQL("SELECT * FROM Professors WHERE TRIM(professorID) = :1 AND TRIM(Password) = :2");
            stmt->setString(1, username);
            stmt->setString(2, password);
            rs = stmt->executeQuery();

            if (rs->next()) {
                usr.isLoggedIn = true;
                usr.IdNum = rs->getString(1);
                usr.name = rs->getString(2) + " " + rs->getString(3); // Combine first name and last name
                usr.email = rs->getString(4); // Get email from the result set
                usr.type = PROFESSOR;
                delete rs;
                delete stmt;
                return true;
            } else {
                delete rs;
                delete stmt;
                return false; // Neither student nor professor found
            }
        }

    } catch (SQLException& ex) {
        cout << "Database error: " << ex.what() << endl;
        return false;
    }
}



response sendResponse() {
    response r;
    r.response = "Success";
    r.success = true;
    return r;
}

bool doesProfessorExist(Connection*& conn, const string& name) {
/*     try {
        // Splitting the name into first and last names if it contains a space
        size_t spacePos = name.find(' ');
        string firstName;
        string lastName;

        if (spacePos != string::npos) {
            firstName = name.substr(0, spacePos);
            lastName = name.substr(spacePos + 1);
        } else {
            // If no space is found, assume the whole name is either the first or last name
            firstName = name;
            lastName = name;
        }

        Statement* stmtExist = conn->createStatement();
        string sqlQueryExist = "SELECT COUNT(*) FROM Users WHERE FirstName = :1 OR LastName = :2";
        stmtExist->setSQL(sqlQueryExist);
        stmtExist->setString(1, firstName);
        stmtExist->setString(2, lastName);
        ResultSet* rsExist = stmtExist->executeQuery();

        int countExist = 0;
        if (rsExist->next()) {
            countExist = rsExist->getInt(1);
        }

        delete rsExist;
        delete stmtExist;

        return countExist > 0;
    } catch (SQLException& ex) {
        cerr << "Database error: " << ex.what() << endl;
        return false; // Return false on error
    } */
}





bool isValidTime(const string& timeSlot) {
/*     // Check if the time slot has the correct format (HH:MM)
    if (timeSlot.size() != 5 || timeSlot[2] != ':') {
        cout << "please enter a time in the format HH:MM\n";
        return false;
    }

    // Extract hours and minutes from the time slot string
    int hours = stoi(timeSlot.substr(0, 2));
    int minutes = stoi(timeSlot.substr(3, 2));

    // Check if hours are between 09 and 17
    if (hours < 9 || hours > 17) {
        cout << "please enter a time between 09:00 and 17:00\n"
        return false;
    }

    // Check if minutes are one of "00", "30", "45"
    if (minutes != 0 && minutes != 30 && minutes != 45) {
        cout << "please enter a time with minutes as 00, 15, 30, or 45\n"
        return false;
    }

    // Time slot is valid
    return true; */
}


bool isValidDate(const std::string& dateStr) {
/*     // Get current date
    std::time_t currentTime = std::time(nullptr);
    std::tm* localTime = std::localtime(&currentTime);
    int currentYear = localTime->tm_year + 1900;
    int currentMonth = localTime->tm_mon + 1;
    int currentDay = localTime->tm_mday;

    // Parse the input date
    int year, month, day;
    if (sscanf(dateStr.c_str(), "%d-%d-%d", &year, &month, &day) != 3) {
        std::cerr << "Invalid date format. Please use YYYY-MM-DD." << std::endl;
        return false;
    }

    // Check if the year, month, and day are valid
    if (year < currentYear || (year == currentYear && month < currentMonth) ||
        (year == currentYear && month == currentMonth && day < currentDay)) {
        std::cerr << "Date must be today or later." << std::endl;
        return false;
    }

    // Check if the month and day are valid
    if (month < 1 || month > 12 || day < 1 || day > 31) {
        std::cerr << "Invalid month or day." << std::endl;
        return false;
    }

    // Check if the day is valid for the given month
    bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
    int maxDaysInMonth = (month == 2 && isLeapYear) ? 29 : daysInMonth[month - 1];
    if (day > maxDaysInMonth) {
        std::cerr << "Invalid day for the given month." << std::endl;
        return false;
    }

    // Date is valid
    return true; */
}



// Function to create a new appointment
void createAppointment(Connection*& conn, const appointment& apt) {
/*     try {
        Statement* stmt = conn->createStatement();
        stmt->setSQL("INSERT INTO Appointment (StudentID, ProfessorID, TimeSlot, AppointmentDate, Status) VALUES (:1, :2, TO_TIMESTAMP(:3, 'HH24:MI'), TO_DATE(:4, 'YYYY-MM-DD'), 'Pending')");
        stmt->setString(1, apt.UserID);
        stmt->setString(2, apt.F_UserName);
        stmt->setString(3, apt.Time);
        stmt->setString(4, apt.Date);
        stmt->executeUpdate();
        conn->commit();
        delete stmt;
        cout << "Appointment created successfully!" << endl;
    } catch (SQLException& ex) {
        cout << "Database error: " << ex.what() << endl;
    } */
}


void scheduleAppointment(Connection*& conn, const user& usr) {
/*     if (getUserType(usr) == STUDENT) {
        string professorName;
        string timeSlot;
        string appointmentDate;
        
        // Get professor's name and time slot
        cout << "Enter professor's name: ";
        getline(cin, professorName);
        cout << "Enter time slot (HH:MM): ";
        getline(cin, timeSlot);
        cout << "Enter appointment date (YYYY/MM/DD): ";
        getline(cin, appointmentDate);
        
        // Check if the professor exists and is available at the given time slot
        // You need to implement this function based on your database schema
        if (isProfessorAvailable(conn, professorName, timeSlot, appointmentDate)) {
            // Create a new appointment
            createAppointment(conn, usr.IdNum, professorName, timeSlot, appointmentDate);
        } else {
            cout << "The professor is not available at the given time slot." << endl;
            // You may prompt the user to choose another time slot or take other actions
        }
    } else if (getUserType(usr) == PROFESSOR) {
        // Handle scheduling logic for professors
    } else {
        cout << "Invalid user type!" << endl;
    } */
}

void confirmAppointment(Connection*& conn, const string aptID, const user& usr) {
/*     try {
        Statement* stmt = conn->createStatement();
        stmt->setSQL("UPDATE Appointment SET Status = 'Confirmed' WHERE bookerID = :1 AND AppointmentID = :2");
        stmt->setString(1, usr.IdNum);
        stmt->setString(2, aptID);
        int numRowsUpdated = stmt->executeUpdate();
        conn->commit();
        delete stmt;

        if (numRowsUpdated == 0) {
            cout << "No appointment found with ID: " << aptID << " for user: " << usr.IdNum << endl;
        } else {
            cout << "Appointment confirmed successfully!" << endl;
        }
    } catch (SQLException& ex) {
        cout << "Database error: " << ex.what() << endl;
    } */
}



void cancelAppointment(Connection*& conn, const user& usr, const string aptID) {
/*     try {
        Statement* stmt = conn->createStatement();
        stmt->setSQL("UPDATE Appointment SET Status = 'Cancelled' WHERE AppointmentID = :1 AND StudentID = :2");
        stmt->setString(1, appointmentID);
        stmt->setString(2, usr.IdNum);
        stmt->executeUpdate();
        conn->commit();
        delete stmt;
        cout << "Appointment cancelled successfully!" << endl;
    } catch (SQLException& ex) {
        cout << "Database error: " << ex.what() << endl;
    }
 */
}


string getAppointments(Connection*& conn, const user& usr) {
/*     try {
        Statement* stmt = conn->createStatement();
        string sqlQuery = "SELECT * FROM Appointment WHERE bookerID = :1";
        stmt->setSQL(sqlQuery);
        stmt->setString(1, usr.IdNum);
        ResultSet* rs = stmt->executeQuery();

        string result = "";

        while (rs->next()) {
            result += "Appointment ID: " + to_string(rs->getInt(1)) + "\t";
            result += "With: " + rs->getString(3) + "\t";
            result += "Date: " + rs->getString(4) + "\t";
            result += "Time: " + rs->getString(5) + "\t";
            result += "Status: " + rs->getString(6) + "\t";
            result += "length: " + rs->getString(7) + "\n";

            // Add other appointment details as needed
            result += "\n";
        }

        delete rs;
        delete stmt;

        return result;
    } catch (SQLException& ex) {
        cerr << "Database error: " << ex.what() << endl;
        return "ERR"; // Return empty string on error
    } */
}


/* bool confirmAppointment(appointment apt) {
    return true;

} */

/**
 * @brief Subscribes a user to a channel in the database.
 *
 * @param username The username of the user to subscribe.
 * @param topic The name of the channel to subscribe to.
 * @param conn A pointer to the database connection.
 * @param response A reference to a string to store the response message.
 */
/* void dbQuerySubscribe(string username, string topic, Connection*& conn, string& response) {
    try {

        // Trim leading and trailing whitespace from username
        username.erase(0, username.find_first_not_of(' ')); // leading
        username.erase(username.find_last_not_of(' ') + 1); // trailing

        try {
            topic.erase(0, topic.find_first_not_of(' ')); // leading
            topic.erase(topic.find_last_not_of(' ') + 1); // trailing
        } catch(exception e) {
            // do nothing
        }


        // Create a Statement object
        Statement* stmt = conn->createStatement();

        // Check if the user is active
        stmt->setSQL("SELECT status FROM Users WHERE TRIM(username) = :1");
        stmt->setString(1, username);
        ResultSet* rs = stmt->executeQuery();

        if (!rs->next()) {
            response = username + " does not exist.";
            stmt->closeResultSet(rs);
            conn->terminateStatement(stmt);
            return;
        } else if (rs->getString(1) != "active") {
            cout << rs->getString(1) << endl;
            response = username + " is not an active user.";
            stmt->closeResultSet(rs);
            conn->terminateStatement(stmt);
            return;
        }

        stmt->closeResultSet(rs);

        // Check if the channel exists
        stmt->setSQL("SELECT * FROM Channels WHERE channelName = :1");
        stmt->setString(1, topic);
        rs = stmt->executeQuery();

        if (!rs->next()) {
            response = "Channel '" + topic + "' does not exist.";
            stmt->closeResultSet(rs);
            conn->terminateStatement(stmt);
            return;
        }

        stmt->closeResultSet(rs);

        // Check if the user is already subscribed to the channel
        stmt->setSQL("SELECT * FROM ActiveSubscriptions WHERE username = :1 AND channel = :2");
        stmt->setString(1, username);
        stmt->setString(2, topic);
        rs = stmt->executeQuery();

        if (rs->next()) {
            response = username + " is already subscribed to " + topic + ".";
            stmt->closeResultSet(rs);
            conn->terminateStatement(stmt);
            return;
        }

        stmt->closeResultSet(rs);

        // Subscribe the user to the channel
        time_t t = time(0);
        tm* now = localtime(&t);
        ostringstream oss;
        oss << (now->tm_year + 1900) << "-" << (now->tm_mon + 1) << "-" << now->tm_mday;
        string subscribeTime = oss.str();
        stmt->setSQL("INSERT INTO ActiveSubscriptions (username, channel, subscribeTime) VALUES (:1, :2, TO_DATE(:3, 'YYYY-MM-DD'))");
        stmt->setString(1, username);
        stmt->setString(2, topic);
        stmt->setString(3, subscribeTime);
        stmt->executeUpdate();

        response = username + " successfully subscribed to " + topic + ".";

        // Clean up
        conn->terminateStatement(stmt);

    } catch (SQLException& ex) {
        response = "Database error: " + string(ex.what());
    }
} */


/**
 * @brief Unsubscribes a user from a topic in the database.
 *
 * @param username The username of the user to unsubscribe.
 * @param topic The topic to unsubscribe from.
 * @param conn A pointer to the database connection.
 * @param response A reference to a string to store the response message.
 */
/* void dbQueryUnsubscribe(string username, string topic, Connection*& conn, string& response) {
    try {

        // Trim leading and trailing whitespace from username
        username.erase(0, username.find_first_not_of(' ')); // leading
        username.erase(username.find_last_not_of(' ') + 1); // trailing

        try {
            topic.erase(0, topic.find_first_not_of(' ')); // leading
            topic.erase(topic.find_last_not_of(' ') + 1); // trailing
        } catch(exception e) {
            // do nothing 
        }

        // Create a Statement object
        Statement* stmt = conn->createStatement();

        // Check if the user is subscribed to the channel
        stmt->setSQL("SELECT * FROM ActiveSubscriptions WHERE TRIM(username) = :1 AND TRIM(channel) = :2");
        stmt->setString(1, username);
        stmt->setString(2, topic);
        ResultSet* rs = stmt->executeQuery();

        if (!rs->next()) {
            response = username + " is not subscribed to " + topic + ".";
            stmt->closeResultSet(rs);
            conn->terminateStatement(stmt);
            return;
        }

        stmt->closeResultSet(rs);

        // Get the current date/time
        time_t t = time(0);
        tm* now = localtime(&t);
        std::ostringstream oss;
        oss << (now->tm_year + 1900) << "-" << (now->tm_mon + 1) << "-" << now->tm_mday;
        string unsubscribeTime = oss.str();

        // Copy the subscription to the SubscribeHistory table
        stmt->setSQL("INSERT INTO SubscribeHistory (username, channel, subscribeTime, unsubscribeTime) SELECT username, channel, subscribeTime, TO_DATE(:1, 'YYYY-MM-DD') FROM ActiveSubscriptions WHERE TRIM(username) = :2 AND TRIM(channel) = :3");
        stmt->setString(1, unsubscribeTime);
        stmt->setString(2, username);
        stmt->setString(3, topic);
        stmt->executeUpdate();

        // Delete the subscription from the ActiveSubscriptions table
        stmt->setSQL("DELETE FROM ActiveSubscriptions WHERE TRIM(username) = :1 AND TRIM(channel) = :2");
        stmt->setString(1, username);
        stmt->setString(2, topic);
        stmt->executeUpdate();

        response = username + " successfully unsubscribed from " + topic + ".";

        // Clean up
        conn->terminateStatement(stmt);

    } catch (SQLException& ex) {
        response = "Database error: " + string(ex.what());
    }
} */

// =============================================================================================================================================================================================
// used for debugging

/* void selectAllFromUsers(Connection*& conn) {
    try {
        // Create a Statement object
        Statement* stmt = conn->createStatement();

        // Execute the query
        stmt->setSQL("SELECT * FROM Users where username = 'John'");
        ResultSet* rs = stmt->executeQuery();

        // Print the results
        while (rs->next()) {
            std::cout << "Username: " << rs->getString(1) << ", Status: " << rs->getString(5) << std::endl;
        }

        // Clean up
        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);

    } catch (SQLException& ex) {
        std::cout << "Database error: " << ex.what() << std::endl;
    }
}

void selectAllFromActiveSubscriptions(Connection*& conn) {
    try {
        // Create a Statement object
        Statement* stmt = conn->createStatement();

        // Execute the query
        stmt->setSQL("SELECT * FROM ActiveSubscriptions");
        ResultSet* rs = stmt->executeQuery();

        // Print the results
        while (rs->next()) {
            std::cout << "Username: " << rs->getString(1) << ", Channel Name: " << rs->getString(2) << ", Subscribe Time: " << rs->getString(3) << std::endl;
        }

        // Clean up
        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);

    } catch (SQLException& ex) {
        std::cout << "Database error: " << ex.what() << std::endl;
    }
}

void selectAllFromSubscribeHistory(Connection*& conn) {
    try {
        // Create a Statement object
        Statement* stmt = conn->createStatement();

        // Execute the query
        stmt->setSQL("SELECT * FROM SubscribeHistory");
        ResultSet* rs = stmt->executeQuery();

        // Print the results
        while (rs->next()) {
            std::cout << "Username: " << rs->getString(1) << ", Channel: " << rs->getString(2) 
                      << ", Subscribe Time: " << rs->getString(3) << ", Unsubscribe Time: " << rs->getString(4) << std::endl;
        }

        // Clean up
        stmt->closeResultSet(rs);
        conn->terminateStatement(stmt);

    } catch (SQLException& ex) {
        std::cout << "Database error: " << ex.what() << std::endl;
    }
} */

// =============================================================================================================================================================================================