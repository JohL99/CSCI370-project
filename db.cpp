#include "include/db.h"

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
        cout << "Failed to connect to the database. Error: " << ex.what() << endl;
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
 * @brief Subscribes a user to a channel in the database.
 *
 * @param username The username of the user to subscribe.
 * @param topic The name of the channel to subscribe to.
 * @param conn A pointer to the database connection.
 * @param response A reference to a string to store the response message.
 */
void dbQuerySubscribe(string username, string topic, Connection*& conn, string& response) {
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
}


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