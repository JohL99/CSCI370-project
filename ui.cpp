#include "include/ui.h"
#include "include/db.h"

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <termios.h>
#include <unistd.h>
#include <occi.h>

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
void showResponse(string response) {
    cout << response << endl;
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
void interpretAndHandleUserCommand(string command, bool& quitFlag, Connection*& conn) {
    stringstream commStream(command);
    vector<string> tokens;
    string token;

    string response;
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
    inputs[QUIT] = 0;
    inputs[SUBSCRIBE] = 1;
    inputs[UNSUBSCRIBE] = 2;

    string mainCommand = tokens[0];

    map<string, int>::iterator it = inputs.find(mainCommand);

    if (it != inputs.end()) {
        
        // The command exists in the map
        switch (it->second) {

            case 0:
                // Handle QUIT command
                cout << "Quitting  application..." << endl;
                quitFlag = true;
                break;

            case 1:
                // Handle SUBSCRIBE command
                if (tokens.size() < 3) {
                    cout << "Invalid command, not enough arguments" << endl << 
                        "Usage: subscribe <username> <topic>" << endl;
                    break;
                }

                username = tokens[1];
                topic = command.substr(command.find(username) + username.size());

                dbQuerySubscribe(username, topic, conn, response);
                showResponse(response);
                break;

            case 2:
                // Handle UNSUBSCRIBE command
                if (tokens.size() < 3) {
                    cout << "Invalid command, not enough arguments" << endl << 
                        "Usage: subscribe <username> <topic>" << endl;
                    break;
                }

                username = tokens[1];
                topic = command.substr(command.find(username) + username.size());

                dbQueryUnsubscribe(username, topic, conn, response);
                showResponse(response);
                break;

        }

    } else {
        // The command doesn't exist in the map
        cout << "Invalid command: " << mainCommand << endl;
    }
}


/**
 * @brief Reads a password from the user without displaying it on the screen.
 * @note lifted from examplePrepared.cpp
 * 
 * @return The password entered by the user.
 */
string readPassword() {
    struct termios settings;
    tcgetattr( STDIN_FILENO, &settings );
    settings.c_lflag =  (settings.c_lflag & ~(ECHO));
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );

    string password = "";
    cout << PPASSWORD;
    getline(cin, password);

    settings.c_lflag = (settings.c_lflag |   ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &settings );
    return password;
}


/**
 * @brief Function to get the user's name.
 * 
 * This function prompts the user to enter their name and returns it as a string.
 * 
 * @return The user's name as a string.
 */
string getUserName() {
    string userName;
    cout << PLOGIN;
    getline(cin, userName);
    return userName;
}
