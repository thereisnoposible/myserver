#include "connectdb.h"
#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>
#include <string>

mongo::DBClientConnection conn;

void Initdb()
{

    conn.connect("localhost");

}

bool searchuser(string username, string password)
{
    mongo::BSONObjBuilder query;
    query.append("username", username);
    query.append("password", password);

    mongo::BSONObj obj;
    obj = conn.findOne("haoyun.haoyun", query.obj());

    if(obj.isEmpty())
        return false;
    return true;
}
bool searchuser(string username)
{
    mongo::BSONObjBuilder query;
    query.append("username", username);

    mongo::BSONObj obj;
    obj = conn.findOne("haoyun.haoyun", query.obj());

    if(obj.isEmpty())
        return false;
    return true;
}


bool registuser(string username, string password)
{
    if(searchuser(username))
        return false;

    mongo::BSONObjBuilder query;
    query.append("username", username);
    query.append("password", password);

    conn.insert("haoyun.haoyun", query.obj());
    return true;
}
