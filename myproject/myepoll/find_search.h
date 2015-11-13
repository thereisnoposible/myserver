#ifndef FIND_SEARCH_H_INCLUDED
#define FIND_SEARCH_H_INCLUDED
#include <map>
#include <string>
using std::string;
using std::map;
using std::pair;

class MyUser
{
public:
    MyUser(int fd);

    int find_words(const char *);
    const char* del_space(const char*);
    int find_space(const char* words,char* nextwords);
    void callfunc(char* str, char* nextwords);

    void funcbuy(void* vParam = 0);
    void funcsale(void* vParam = 0);
    void funclogin(void* nextwords);
    void funcconnect(void* vParam = 0);
    void funcregist(void* vParam = 0);

    map<string, int> Commd;
    void(MyUser::*ppfunc[64])(void*vParam);
private:
    int infd;
    unsigned int userid;

};





#endif // FIND_SEARCH_H_INCLUDED
