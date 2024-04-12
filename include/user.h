#ifndef __USER_HEADER__
#define __USER_HEADER__

/**
 * @file user.h
 * @brief This file contains the definition of the user struct and the userTypes enum.
 */

/**
 * @enum userTypes
 * @brief An enumeration representing different types of users.
 * 
 * The userTypes enum is used to differentiate between different types of users, such as admin, professor, and student.
 */

enum userTypes {ADMIN, PROFESSOR, STUDENT};

/**
 * @struct user
 * @brief A struct representing a user.
 * 
 * The user struct contains information about a user, such as their ID number, name, email, login status, and user type.
 */
struct user {
    std::string IdNum; /**< The ID number of the user. */
    std::string name; /**< The name of the user. */
    std::string email; /**< The email address of the user. */
    bool isLoggedIn; /**< A flag indicating whether the user is currently logged in. */
    userTypes type; /**< The type of the user. */

    /**
     * @brief Default constructor for the user struct.
     * 
     * Initializes the isLoggedIn member variable to false.
     */
    user() : isLoggedIn(false) {}
}; 

#endif
