#ifndef __USER_HEADER__
#define __USER_HEADER__


// stretch goal to have different fucntionality for different user types and admin users who can view/change all appointments
enum userTypes {ADMIN, PROFESSOR, STUDENT};

struct user {
    std::string IdNum;
    std::string name;
    std::string email;
    bool isLoggedIn;
    userTypes type;

    // Constructor
    user() : isLoggedIn(false) {} // Initialize isLoggedIn in the constructor
}; 

// Function to check if the user is a student or professor
/* userTypes getUserType(const user& usr) {
    return usr.type;
} */

#endif