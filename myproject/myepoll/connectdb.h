#ifndef CONNECTDB_H_INCLUDED
#define CONNECTDB_H_INCLUDED

#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>
#include <string>
using namespace mongo;

extern DBClientConnection conn;

void Initdb();
bool searchuser(string username, string password);
bool searchuser(string username);
bool registuser(string username, string password);


#endif // CONNECTDB_H_INCLUDED
