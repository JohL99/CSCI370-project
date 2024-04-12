#include "db.h"
#include "ui.h"
#include "user.h"

#include <string>
#include <occi.h>
#include <iostream>
#include <ctime>
#include <sstream>
#include <fstream>
#include <cstring>
#include <vector>
#include <cstdlib>
#include <cstdio>
#include <algorithm>


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


/**
 * Executes a database query to check if the provided username and password match a user in the database.
 * If a match is found, the user's information is retrieved and stored in the 'usr' object.
 * 
 * @param username The username to check.
 * @param password The password to check.
 * @param conn A pointer to the database connection.
 * @param usr A reference to the user object where the retrieved user information will be stored.
 * @return True if the username and password match a user in the database, false otherwise.
 */
bool dbQueryLogin(string username, string password, Connection*& conn, user &usr) {

    try {
        Statement* stmt = conn->createStatement();
        stmt->setSQL("SELECT * FROM Users370 WHERE TRIM(userID) = :1 AND TRIM(Password) = :2");
        stmt->setString(1, username);
        stmt->setString(2, password);
        ResultSet* rs = stmt->executeQuery();

        if (rs->next()) {
            usr.isLoggedIn = true;
            usr.IdNum = rs->getString(1);
            usr.name = rs->getString(2) + " " + rs->getString(3); // Combine first name and last name
            usr.email = rs->getString(4); // Get email from the result set
            usr.type = rs->getString(6) == "STUDENT" ? STUDENT : PROFESSOR; // Determine user type based on UserType column
            delete rs;
            delete stmt;
            return true;
        } else {
            delete rs;
            delete stmt;
            return false; // User not found
        }

    } catch (SQLException& ex) {
        cout << "Database error: " << ex.what() << endl;
        return false;
    }
}


/**
 * @brief Retrieves the full name of a user based on their user ID.
 * 
 * This function executes a SQL query to retrieve the first name and last name of a user
 * from the Users370 table in the database. It concatenates the first name and last name
 * to form the full name of the user and returns it as a string.
 * 
 * @param conn A pointer to the database connection object.
 * @param userID The user ID of the user whose name is to be retrieved.
 * @return The full name of the user as a string. If an error occurs during the database
 *         operation, the function returns "Unknown".
 * @throws SQLException If an error occurs while executing the SQL query.
 */
string getUserName(Connection*& conn, const string& userID) {
    try {
        Statement* stmt = conn->createStatement();
        string sqlQuery = "SELECT FirstName, LastName FROM Users370 WHERE userID = :1";
        stmt->setSQL(sqlQuery);
        stmt->setString(1, userID);
        ResultSet* rs = stmt->executeQuery();

        string userName = "";

        if (rs->next()) {
            userName = rs->getString(1) + " " + rs->getString(2);
        }

        delete rs;
        delete stmt;

        return userName;
    } catch (SQLException& ex) {
        cerr << "Database error: " << ex.what() << endl;
        return "Unknown"; // Return "Unknown" on error
    }
}


/**
 * Checks if a professor with the given name exists in the database.
 * 
 * @param conn A pointer to the database connection.
 * @param name The name of the professor to check.
 * @return True if the professor exists, false otherwise.
 */
bool doesProfessorExist(Connection*& conn, const string& name) {
    try {
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
        string sqlQueryExist = "SELECT COUNT(*) FROM Users370 WHERE FirstName = :1 OR LastName = :2";
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
    }
}



/**
 * Checks if a given time slot is valid.
 * 
 * @param timeSlot The time slot to be checked in the format HH:MM.
 * @return True if the time slot is valid, false otherwise.
 */
bool isValidTime(Connection*& conn, const string& timeSlot, const string& bookeeName) {
    // Check if the time slot has the correct format (HH:MM)
    if (timeSlot.size() != 5 || timeSlot[2] != ':') {
        cout << "Please enter a time in the format HH:MM\n";
        return false;
    }

    // Extract hours and minutes from the time slot string
    int hours = atoi(timeSlot.substr(0, 2).c_str());
    int minutes = atoi(timeSlot.substr(3, 2).c_str());

    // Check if hours are between 09 and 17
    if (hours < 9 || hours > 17) {
        cout << "Please enter a time between 09:00 and 17:00\n";
        return false;
    }

    // Check if minutes are one of "00", "30", "45"
    if (minutes != 0 && minutes != 30 && minutes != 45) {
        cout << "Please enter a time with minutes as 00, 30, or 45\n";
        return false;
    }

    try {
        // Splitting the name into first and last names if it contains a space
        size_t spacePos = bookeeName.find(' ');
        string firstName;
        string lastName;

        if (spacePos != string::npos) {
            firstName = bookeeName.substr(0, spacePos);
            lastName = bookeeName.substr(spacePos + 1);
        } else {
            // If no space is found, assume the whole name is either the first or last name
            firstName = bookeeName;
            lastName = bookeeName;
        }

        // Query Users370 table to get the ID based on first or last name
        Statement* stmtID = conn->createStatement();
        string sqlQueryID = "SELECT UserID FROM Users370 WHERE FirstName = :1 OR LastName = :2";
        stmtID->setSQL(sqlQueryID);
        stmtID->setString(1, firstName);
        stmtID->setString(2, lastName);
        ResultSet* rsID = stmtID->executeQuery();

        string bookeeID;
        if (rsID->next()) {
            bookeeID = rsID->getString(1);
        } else {
            cout << "User with name '" << bookeeName << "' not found.\n";
            delete rsID;
            delete stmtID;
            return false;
        }
        delete rsID;
        delete stmtID;

        // Check for existing appointments
        Statement* stmtAppt = conn->createStatement();
        string sqlQueryAppt = "SELECT COUNT(*) FROM Appointment370 WHERE bookeeID = :1 AND TimeSlot = TO_TIMESTAMP(:2, 'HH24:MI')";
        stmtAppt->setSQL(sqlQueryAppt);
        stmtAppt->setString(1, bookeeID);
        stmtAppt->setString(2, timeSlot);
        ResultSet* rsAppt = stmtAppt->executeQuery();

        int countAppt = 0;
        if (rsAppt->next()) {
            countAppt = rsAppt->getInt(1);
        }

        delete rsAppt;
        delete stmtAppt;

        if (countAppt > 0) {
            cout << "Appointment already exists for the specified user at this time.\n";
            return false;
        }
    } catch (SQLException& ex) {
        cerr << "Database error: " << ex.what() << endl;
        return false; // Return false on error
    }

    // Time slot is valid
    return true;
}




/**
 * Checks if a given date string is valid.
 * 
 * @param dateStr The date string to be validated in the format "YYYY-MM-DD".
 * @return True if the date is valid, false otherwise.
 */
bool isValidDate(const std::string& dateStr) {
    // Get current date
    std::time_t currentTime = std::time(NULL);
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
    int maxDaysInMonth = (month == 2 && isLeapYear) ? 29 : ((month == 2) ? 28 : ((month == 4 || month == 6 || month == 9 || month == 11) ? 30 : 31));
    if (day > maxDaysInMonth) {
        std::cerr << "Invalid day for the given month." << std::endl;
        return false;
    }

    // Date is valid
    return true;
}


/**
 * Checks if a given string has a valid length.
 * used to check for appointment length
 * 
 * @param str The string to be checked.
 * @return True if the string has a valid length, false otherwise.
 */
bool isValidLength(const std::string& str) {
    std::string validValues[] = {"15", "30", "45", "60"};
    size_t size = sizeof(validValues) / sizeof(validValues[0]);
    return std::find(validValues, validValues + size, str) != validValues + size;
}




// Function to create a new appointment
/**
 * Creates an appointment in the database.
 *
 * @param conn A pointer to the database connection.
 * @param apt The appointment object containing the appointment details.
 */
void createAppointment(Connection*& conn, const appointment& apt) {
    try {
        // Splitting the name into first and last names if it contains a space
        size_t spacePos = apt.F_UserName.find(' ');
        string firstName;
        string lastName;

        if (spacePos != string::npos) {
            firstName = apt.F_UserName.substr(0, spacePos);
            lastName = apt.F_UserName.substr(spacePos + 1);
        } else {
            // If no space is found, assume the whole name is either the first or last name
            firstName = apt.F_UserName;
            lastName = apt.F_UserName;
        }

        // Check if the professor exists based on first name
        Statement* stmtExist = conn->createStatement();
        string sqlQueryExist = "SELECT userID FROM Users370 WHERE FirstName = :1 OR LastName = :2";
        stmtExist->setSQL(sqlQueryExist);
        stmtExist->setString(1, firstName);
        stmtExist->setString(2, lastName);
        ResultSet* rsExist = stmtExist->executeQuery();

        string bookeeID;
        if (rsExist->next()) {
            bookeeID = rsExist->getString(1);
        } else {
            cout << "Bookee not found." << endl;
            delete rsExist;
            delete stmtExist;
            return;
        }

        delete rsExist;
        delete stmtExist;

        // Insert appointment into Appointment370 table
        Statement* stmt = conn->createStatement();
        stmt->setSQL("INSERT INTO Appointment370 (bookerID, bookeeID, TimeSlot, AppointmentDate, Status, Length) VALUES (:1, :2, TO_TIMESTAMP(:3, 'HH24:MI'), TO_DATE(:4, 'YYYY-MM-DD'), :5, :6)");
        stmt->setString(1, apt.UserID); // Using apt.UserID directly for bookerID
        stmt->setString(2, bookeeID);
        stmt->setString(3, apt.Time);
        stmt->setString(4, apt.Date);
        stmt->setString(5, apt.Status);
        stmt->setString(6, apt.Length);
        stmt->executeUpdate();
        conn->commit();
        delete stmt;
        cout << "Booking created successfully." << endl;
    } catch (SQLException& ex) {
        cout << "Database error: " << ex.what() << endl;
    }
}


/**
 * @brief Confirms an appointment for a given user.
 * 
 * This function updates the status of an appointment to 'Confirmed' in the Appointment370 table
 * in the database for the specified user and appointment ID.
 * 
 * @param conn A pointer to the database connection.
 * @param aptID The ID of the appointment to confirm.
 * @param usr The user object representing the user confirming the appointment.
 */
void confirmAppointment(Connection*& conn, const string aptID, const user& usr) {
    try {
        Statement* stmt = conn->createStatement();
        stmt->setSQL("UPDATE Appointment370 SET Status = 'Confirmed' WHERE bookeeID = :1 AND AppointmentID = :2");
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
    }
}



/**
 * @brief Cancels an appointment for a given user.
 * 
 * This function cancels an appointment in the database for a specified user. It deletes the appointment
 * from the Appointment370 table where the AppointmentID matches the provided aptID and the bookeeID or
 * bookerID matches the user's IdNum.
 * 
 * @param conn A pointer to the database connection.
 * @param usr The user for whom the appointment is being cancelled.
 * @param aptID The ID of the appointment to be cancelled.
 */
void cancelAppointment(Connection*& conn, const user& usr, const string aptID) {
    try {
        Statement* stmt = conn->createStatement();
        stmt->setSQL("DELETE FROM Appointment370 WHERE AppointmentID = :1 AND bookeeID = :2 OR bookerID = :2");
        stmt->setString(1, aptID);
        stmt->setString(2, usr.IdNum);
        stmt->executeUpdate();
        conn->commit();
        delete stmt;
        cout << "Appointment cancelled successfully!" << endl;
    } catch (SQLException& ex) {
        cout << "Database error: " << ex.what() << endl;
    }
}


/**
 * @brief shows all appointments for the logged in user.
 * 
 * 
 * @param conn A pointer to the database connection.
 * @param usr The user for whom the appointment is being cancelled.
 * @param aptID The ID of the appointment to be cancelled.
 */
string getAppointments(Connection*& conn, const user& usr) {
    try {
        Statement* stmt = conn->createStatement();
        string sqlQuery = "SELECT * FROM Appointment370 WHERE bookerID = :1 OR bookeeID = :1";
        stmt->setSQL(sqlQuery);
        stmt->setString(1, usr.IdNum);
        ResultSet* rs = stmt->executeQuery();

        string result = "";

        while (rs->next()) {
            std::ostringstream oss;
            oss << rs->getInt(1); // Convert int to string
            result += "Appointment ID: " + oss.str() + "\t";

            // Get the details of the booker
            string bookerID = rs->getString(2);

            if (bookerID == usr.IdNum) {
                // Get the details of the bookee
                string bookeeID = rs->getString(3);
                string bookeeName = getUserName(conn, bookeeID);
                result += "With: " + bookeeName + "\t";
            } else {
                string bookerName = getUserName(conn, bookerID);
                result += "With: " + bookerName + "\t";
            }

            result += "Time: " + rs->getString(4) + "\t";
            result += "Date: " + rs->getString(5) + "\t";
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
    }
}


/**
 * Drops database objects including trigger, sequence, and tables.
 * 
 * @param conn A pointer to the Connection object representing the database connection.
 */
void dropDatabaseObjects(Connection*& conn) {
    try {
        Statement* stmt = conn->createStatement();

        // Drop trigger
        try {
            stmt->setSQL("DROP TRIGGER appointment_id_trigger");
            stmt->executeUpdate();
            cout << "Appointment ID trigger dropped successfully." << endl;
        } catch (SQLException& ex) {
            // Do nothing if the trigger does not exist
        }

        // Drop sequence
        try {
            stmt->setSQL("DROP SEQUENCE appointment_id_seq");
            stmt->executeUpdate();
            cout << "Appointment sequence dropped successfully." << endl;
        } catch (SQLException& ex) {
            // Do nothing if the sequence does not exist
        }

        // Drop tables
        try {
            stmt->setSQL("DROP TABLE Appointment370");
            stmt->executeUpdate();
            cout << "Appointment table dropped successfully." << endl;

            stmt->setSQL("DROP TABLE Users370");
            stmt->executeUpdate();
            cout << "Users table dropped successfully." << endl;

            conn->commit(); // Commit changes
        } catch (SQLException& ex) {
            // Do nothing if the tables do not exist
        }

        delete stmt;
    } catch (SQLException& ex) {
        cerr << "Database error: " << ex.what() << endl;
    }
}


/**
 * Creates the necessary database tables, sequence and trigger for the application.
 * 
 * @param conn A pointer to the database connection object.
 */
void createDatabaseTables(Connection*& conn) {
    try {
        Statement* stmt = conn->createStatement();

        // Drop existing tables if they exist
        dropDatabaseObjects(conn);
        

        // Create Users table
        stmt->setSQL("CREATE TABLE Users370( \
                userID CHAR(9) PRIMARY KEY, \
                FirstName VARCHAR(65), \
                LastName VARCHAR(65), \
                Email VARCHAR(75), \
                Password VARCHAR(60), \
                UserType VARCHAR(20) DEFAULT 'STUDENT' \
                )");

        stmt->executeUpdate();
        cout << "Users table created successfully." << endl;

        // Create Appointment table
        stmt->setSQL("CREATE TABLE Appointment370 ( \
                    AppointmentID NUMBER PRIMARY KEY, \
                    bookerID CHAR(9), \
                    bookeeID CHAR(9), \
                    TimeSlot TIMESTAMP, \
                    AppointmentDate DATE, \
                    Status VARCHAR2(10), \
                    Length CHAR(2), \
                    CONSTRAINT fk_booker FOREIGN KEY (bookerID) REFERENCES Users370(userID), \
                    CONSTRAINT fk_bookee FOREIGN KEY (bookeeID) REFERENCES Users370(userID) \
                )");

        stmt->executeUpdate();

        // Create sequence for AppointmentID
        stmt->setSQL("CREATE SEQUENCE appointment_id_seq START WITH 1 INCREMENT BY 1 NOCACHE NOCYCLE");
        stmt->executeUpdate();
        //cout << "Appointment sequence created successfully." << endl;

        // Create trigger to auto-increment AppointmentID
        stmt->setSQL("CREATE OR REPLACE TRIGGER appointment_id_trigger \
                      BEFORE INSERT ON Appointment370 \
                      FOR EACH ROW \
                      BEGIN \
                        SELECT appointment_id_seq.nextval INTO :new.AppointmentID FROM dual; \
                      END;");
        stmt->executeUpdate();
        //cout << "Appointment ID trigger created successfully." << endl;

        cout << "Appointment table created successfully." << endl;

        conn->commit(); // Commit changes

        delete stmt;
    } catch (SQLException& ex) {
        cerr << "Database error: " << ex.what() << endl;
    }
}



void insertDataFromCSV(Connection*& conn) {
    // Insert data from Appointment.csv
    ifstream appointmentFile(APPOINTMENT_CSV);
    if (!appointmentFile.is_open()) {
        cout << "Error: Unable to open " << APPOINTMENT_CSV << endl;
        return;
    }

    string line;
    while (getline(appointmentFile, line)) {
        stringstream ss(line);
        string aptID, bookerID, bookeeID, timeSlot, status, length;

        getline(ss, aptID, ',');
        getline(ss, bookerID, ',');
        getline(ss, bookeeID, ',');
        getline(ss, timeSlot, ',');
        getline(ss, status, ',');
        getline(ss, length, ',');

        Statement* stmt = conn->createStatement();
        stmt->setSQL("INSERT INTO Appointment370 (bookerID, bookeeID, TimeSlot, Status, Length) VALUES (:1, :2, TO_TIMESTAMP(:3, 'HH24:MI'), :4, :5)");
        stmt->setString(1, bookerID);
        stmt->setString(2, bookeeID);
        stmt->setString(3, timeSlot);
        stmt->setString(4, status);
        stmt->setString(5, length);
        stmt->executeUpdate();
        delete stmt;
    }

    appointmentFile.close();

    // Insert data from Users.csv
    ifstream usersFile(USERS_CSV);
    if (!usersFile.is_open()) {
        cout << "Error: Unable to open " << USERS_CSV << endl;
        return;
    }

    while (getline(usersFile, line)) {
        stringstream ss(line);
        string userID, firstName, lastName, email, password, userType;

        getline(ss, userID, ',');
        getline(ss, firstName, ',');
        getline(ss, lastName, ',');
        getline(ss, email, ',');
        getline(ss, password, ',');
        getline(ss, userType, ',');

        Statement* stmt = conn->createStatement();
        stmt->setSQL("INSERT INTO Users370 (userID, FirstName, LastName, Email, Password, UserType) VALUES (:1, :2, :3, :4, :5, :6)");
        stmt->setString(1, userID);
        stmt->setString(2, firstName);
        stmt->setString(3, lastName);
        stmt->setString(4, email);
        stmt->setString(5, password);
        stmt->setString(6, userType);
        stmt->executeUpdate();
        delete stmt;
    }

    usersFile.close();

    // Commit the changes
    conn->commit();

    cout << "Data inserted successfully from CSV files." << endl;
}


/**
 * Inserts dummy users into the Users370 table.
 * 
 * @param conn A pointer to the Connection object representing the database connection.
 */
void insertDummyUsers(Connection*& conn) {
    try {
        Statement* stmt = conn->createStatement();

        // Insert first dummy user
        stmt->setSQL("INSERT INTO Users370 (userID, FirstName, LastName, Email, Password, UserType) VALUES ('123456789', 'John', 'Doe', 'john.doe@viu.ca', 'password123', 'STUDENT')");
        stmt->executeUpdate();
        cout << "First dummy user inserted successfully." << endl;

        // Insert second dummy user
        stmt->setSQL("INSERT INTO Users370 (userID, FirstName, LastName, Email, Password, UserType) VALUES ('987654321', 'Jane', 'Smith', 'jane.smith@viu.ca', 'password456', 'PROFESSOR')");
        stmt->executeUpdate();
        cout << "Second dummy user inserted successfully." << endl;

        conn->commit(); // Commit changes

        delete stmt;
    } catch (SQLException& ex) {
        cerr << "Database error: " << ex.what() << endl;
    }
}


/**
 * Inserts dummy appointments into the database.
 * 
 * @param conn A pointer to the database connection.
 */
void insertDummyAppointments(Connection*& conn) {
    try {
        // Dummy data for appointments
        std::string bookee[] = {"123456789", "123456789", "987654321"};
        std::string booker[] = {"987654321", "987654321", "123456789"};
        std::string timeSlot[] = {"12:00", "14:30", "16:45"};
        std::string date[] = {"2024-02-15", "2024-05-22", "2024-03-10"};
        std::string status[] = {"Confirmed", "Pending", "Confirmed"};
        std::string length[] = {"15", "30", "45"};

        // Insert dummy appointments
        for (int i = 0; i < 3; i++) {   
            Statement* stmt = conn->createStatement();
            stmt->setSQL("INSERT INTO Appointment370 (bookerID, bookeeID, TimeSlot, AppointmentDate, Status, Length) VALUES (:1, :2, TO_TIMESTAMP(:3, 'YYYY-MM-DD HH24:MI:SS'), TO_DATE(:4, 'YYYY-MM-DD'), :5, :6)");
            stmt->setString(1, booker[i]); // Booker ID
            stmt->setString(2, bookee[i]); // Bookee ID
            stmt->setString(3, date[i] + " " + timeSlot[i] + ":00"); // TimeSlot (combining date and time)
            stmt->setString(4, date[i]); // AppointmentDate
            stmt->setString(5, status[i]); // Status
            stmt->setString(6, length[i]); // Length
            stmt->executeUpdate();
            delete stmt;
        }

        // Commit the changes
        conn->commit();

        std::cout << "Dummy appointments inserted successfully." << std::endl;
    } catch (SQLException& ex) {
        std::cerr << "Database error: " << ex.what() << std::endl;
    }
}