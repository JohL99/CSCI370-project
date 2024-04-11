#include "ui.h"
#include "db.h"
#include "user.h"
#include "commands.h"
#include "screenStack.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <termios.h>
#include <unistd.h>
#include <occi.h>
#include <cstdio>
#include <cstdlib>

using namespace std;
using namespace oracle::occi;


/**
 * @brief Gets the user command from the console.
 * 
 * @param userCommand The string to store the user command.
 */
void getUserCommand(string& userCommand) {
    cout << PROMPT;
    getline(cin, userCommand);
}


/**
 * @brief Displays the given response on the console.
 * 
 * @param response The response to be displayed.
 */
response getResponseObject(string res) {
    response resp;
    resp.response = res;
    resp.success = true;
    return resp;
}


/**
 * @brief Interprets and handles the user command.
 * 
 * This function takes a command string and a quit flag as input. It tokenizes the command string
 * and interprets the main command. Based on the main command, it performs the corresponding action
 * and updates the quit flag if necessary.
 * 
 * @param command The user command to interpret and handle.
 * @param quitFlag A boolean flag indicating whether the application should quit.
 */
void interpretAndHandleUserCommand(string command, bool& quitFlag, user &user, ScreenStack &screenstack, screenIDs screenIDs, Connection*& conn) {
    stringstream commStream(command);
    vector<string> tokens;
    string token;

    string username;
    string topic;


    while (commStream >> token) {
        tokens.push_back(token);
    }

    if (tokens.empty()) {
        // error reading input
        cout << "Error reading input." << endl;
        return;
    }

    map <string, int> inputs;
        // general commands
        inputs[quit] = 0;
        inputs[help] = 1;
        inputs[back] = 2;

        // welcome screen commands
        inputs[viewAppointments] = 3;
        inputs[bookAppointment] = 4;
        inputs[confirm] = 5;
        inputs[cancelAppointment] = 6;

        

    string mainCommand = tokens[0];

    map<string, int>::iterator it = inputs.find(mainCommand);

    if (it != inputs.end()) {
        
        // The command exists in the map
        switch (it->second) {

            case 0:
                // Handle QUIT command
                cout << "Quitting  application..." << endl;
                cout << menuDelimiter << endl << endl;
                quitFlag = true;
                break;

            case 1:
                // Handle help command
                showHelpScreen();
                // Dont push the screen onto the stack
                //screenstack.push(screenIDs.helpID);
                break;

            case 2:
                // Handle back command
                handleBackCommand(user ,screenstack, screenIDs, conn);
                break;

            case 3:
                // Handle view command
                showAppointmentsScreen(user, conn);
                screenstack.push(screenIDs.viewAptsID);
                break;
            
            case 4:
                // Handle book command
                showBookingScreen(user, conn);
                // Dont push the screen onto the stack
                //screenstack.push(screenIDs.bookAptID);
                break;

            case 5:
                // Handle confirm command
                if (tokens.size() != 2) {
                    cout << "Invalid command: confirm <ID>" << endl;
                    break;
                } 

                confirmAppointmentScreen(tokens[1], user, conn);
                break;

            case 6:
                // Handle cancel command
                if (tokens.size() != 2) {
                    cout << "Invalid command: cancel <ID>" << endl;
                    break;
                } 

                cancelAppointmentScreen(user, conn, tokens[1]);
                break;
            
            default:
                // The command doesn't exist in the map
                showHelpScreen();
                //screenstack.push(screenIDs.helpID);
                break;

        }

    } else {
        // The command doesn't exist in the map
        cout << "Invalid command: " << mainCommand << endl;
        showHelpScreen();
    }
}


/**
 * @brief Reads a password from the user without displaying it on the screen.
 * @note lifted from examplePrepared.cpp
 * 
 * @return The password entered by the user.
 */
string readPassword(int dbFlag) {
    if (dbFlag == 1) {
        cout << "Enter your oracle password>> ";
    } else {
        cout << "Enter your password>> ";
    }

    struct termios settings;
    tcgetattr( STDIN_FILENO, &settings );
    settings.c_lflag =  (settings.c_lflag & ~(ECHO));
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );

    string password = "";
    getline(cin, password);

    settings.c_lflag = (settings.c_lflag |   ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );
    cout << endl;
    return password;
}


/**
 * @brief Function to get the user's name.
 * 
 * This function prompts the user to enter their name and returns it as a string.
 * 
 * @return The user's name as a string.
 */
string getUserName(int dbFlag) {
    if (dbFlag == 1) {
        cout << "Enter your oracle username>> ";
    } else {
        cout << "Enter your username>> ";
    }

    string userName;
    getline(cin, userName);
    return userName;
}


void showWelcomeScreen(user &user, Connection*& conn) {

    if (!user.isLoggedIn) {
        cout << endl << "Invalid username or password." << endl;
        cout << "Quitting..." << endl;
        exit(1);
    } else {
        cout << "welcome " << user.name << " what would you like to do?" << endl;
        cout << "view - view appointments" << endl;
        cout << "book - book an appointment" << endl;
        cout << "back - go back" << endl;
        cout << "help - show help" << endl;
        cout << "quit - exit the application" << endl << endl;
    }
    
}


void showLoginScreen(user &user, Connection*& conn) {
    string username;
    string password;

    username = getUserName(0);
    password = readPassword(0);

    if (username == "quit" || password == "quit") {
        cout << "Quitting application..." << endl;
        return;
    }
    

    cout << "Logging in..." << endl << endl;

    //user.isLoggedIn = dbQueryLogin(username, password, conn, user);

    // stub testing
    user.isLoggedIn = true;
    user.IdNum = username;

    // get user name from db
    user.name = "John Doe";
    
}


void showAppointmentsScreen(user &user, Connection*& conn) {

    string response = "";

    //response = getAppointments(conn, user);

    response = "test data";
    cout << menuDelimiter << endl << endl;

    cout << "Current appointments:" << endl;

    if (response == "") {
        cout << "no appointments found." << endl;
    } else if (response == "ERR"){
        char err[100] = "";
        sprintf(err, RESPONSE_ERROR, "view appointments");
        cout << err << endl;
    } else {
        cout << response << endl;
    }

    cout << "confirm <ID> - confirm appointment" << endl;
    cout << "cancel <ID> - cancel appointment" << endl;
    cout << "back - go back" << endl;
    cout << "help - show help" << endl;
    cout << "quit - exit the application" << endl << endl;

}

void confirmAppointmentScreen(const string aptID, user& user, Connection*& conn) {

    cout << "Confirming appointment " << aptID << "..." << endl;

    // send confirmation to db
    // if success
    //confirmAppointment(conn, aptID, user);

}

void cancelAppointmentScreen(user &user, Connection*& conn, const string aptID) {

    cout << "Cancelling appointment " << aptID << "..." << endl;

    // send cancellation to db
    //cancelAppointment(conn, user, aptID);
    // if success

    cout << "Appointment cancelled successfully." << endl;

    cout << "Appointment ID: " << aptID << endl << endl;

}


void showBookingScreen(user &user, Connection*& conn) {

    cout << menuDelimiter << endl << endl;

    appointment apt;

    string name;
    string time;
    string date;

    apt.UserID = user.IdNum;

    cout << "Who would you like to book an appointment with or type 'CANCEL' to cancel>> ";
    getline(cin, name);

    while (name != "CANCEL" && !doesProfessorExist(conn, name)) {
        cout << "Professor with name " << name << " does not exist. Please enter a valid name or type 'CANCEL' to cancel>> ";
        getline(cin, name);
    }

    if (name == "CANCEL") {
        return;
    } else {
        apt.F_UserName = name;
    }

    cout << "Enter the date of the appointment (YYYY-MM-DD) >>";
    getline(cin, date);

    while (date != "CANCEL" && !isValidTime(date)) {
        cout << "Invalid date. Please enter a valid date in the form (YYYY-MM-DD) and today or later or type 'CANCEL' to cancel>> ";
        getline(cin, date);
    }

    if (apt.Date == "CANCEL") {
        return;
    } else {
        apt.Date = date;
    }

    cout << "Enter the time of the appointment (HH:MM) or CANCEL to cancel>> ";
    getline(cin, time);

    while (time != "CANCEL" && !isValidTime(time)) {
        getline(cin, time);
    }

    if (time == "CANCEL") {
        return;
    } else {
        apt.Time = time;
    }

    apt.Status = "pending";

    cout << "Creating booking..." << endl;

    createAppointment(conn, apt);

    cout << "Booking created successfully, status pending awaiting confirmation." << endl;

    cout << "Booker:  " << user.name << "\t" << " Bookee: " << apt.F_UserName << "\t" << " Date: " 
         << apt.Date << "\t" << " Time: " << apt.Time << "\t" << " Status: " << apt.Status << endl << endl;

    cout << menuDelimiter << endl << endl;
}

void showHelpScreen() { 

    cout << menuDelimiter << endl << endl;

    cout << "Usage:" << endl;
    cout << "view - view appointments" << endl;
    cout << "book - book an appointment" << endl;
    cout << "back - go back" << endl;
    cout << "help - show help" << endl;
    cout << "quit - exit the application" << endl << endl;

}

void handleBackCommand(user &user, ScreenStack &screenstack, screenIDs screenIDs, Connection*& conn) {

    cout << menuDelimiter << endl << endl;
    screenstack.pop();

    int topScreen = screenstack.peek();

    switch (topScreen) {
        case 1:
            screenstack.push(screenIDs.welcomeID);
            showWelcomeScreen(user, conn);
            break;
        
        case 2:
            screenstack.push(screenIDs.viewAptsID);
            showAppointmentsScreen(user, conn);
            break;
        
        case 3:
            // Do nothing in this case
            //screenstack.push(screenIDs.bookAptID);
            //showBookingScreen(user);
            break;
        
        case 4:
            //screenstack.push(screenIDs.changeAptID);
            //showChangesScreen(user);
            break;
        
        case 5:
            // Do nothing in this case
            //screenstack.push(screenIDs.helpID);
            //showHelpScreen();
            break;
        
        default:
            screenstack.push(screenIDs.welcomeID);
            showWelcomeScreen(user, conn);
            break;
    }

}