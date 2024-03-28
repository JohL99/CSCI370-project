#ifndef __DB_HEADER__
#define __DB_HEADER__

#include <string>
#include <occi.h>



void dbConnect(std::string userName, std::string password, std::string connectString, oracle::occi::Environment*& env, oracle::occi::Connection*& conn);

void dbDisconnect(oracle::occi::Environment*& env, oracle::occi::Connection*& conn);

/* void dbQuerySubscribe(std::string username, std::string topic, oracle::occi::Connection*& conn, std::string& response);

void dbQueryUnsubscribe(std::string username, std::string topic, oracle::occi::Connection*& conn, std::string& response);


void selectAllFromUsers(oracle::occi::Connection*& conn);

void selectAllFromActiveSubscriptions(oracle::occi::Connection*& conn);

void selectAllFromSubscribeHistory(oracle::occi::Connection*& conn); */


#endif