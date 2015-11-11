#include "find_search.h"
#include "string.h"
#include <iostream>
#include <sys/socket.h>
#include <vector>
#include <mongo/client/dbclient.h>
#include <mongo/bson/bson.h>

void split(std::vector<string> &arr,string src, char* instead);

int MyUser::find_words(const char *words)//返回-1 表示空字符
{
    std::cout<<"in find_words,words:"<<words<<std::endl;
    int iReturn = -1;
    const char* cBuffer = del_space( words );
    if( cBuffer[0] == '\r' && cBuffer[1] == '\n')
        return iReturn;
    char nextwords[64] = {0};
    iReturn = find_space( cBuffer, nextwords);
    char str[12] = {0};
    if(iReturn == 0)
    {
        strncpy(str, cBuffer, strlen(cBuffer));
    }
    else
    {
       strncpy(str, cBuffer, iReturn-1);
    }
    callfunc(str, nextwords);
    return iReturn;
}

const char* MyUser::del_space(const char* words)
{
    const char* cpPoint = words;
    for(unsigned int i = 0; i< strlen( words); ++i)
    {
        if(cpPoint[0] == ' ')
            cpPoint++;
    }
    return cpPoint;
}

int MyUser::find_space(const char* words, char* nextwords)
{
    int iNum = 0;
    const char* cpPoint = words;
    for(unsigned int i = 0 ; i< strlen(words); ++i)
    {
        iNum++;
        if( cpPoint[i] == ' ')
            {
                const char *p = (words+iNum);
                p = del_space(p);
                strncpy(nextwords, p, strlen(p));
                return iNum;
            }
    }
    return 0;
}

MyUser::MyUser(int fd)
{
    infd = fd;
    Commd.insert(pair<string ,int>("buy\r\n",0));
    Commd.insert(pair<string, int>("sale\r\n",1));
    Commd.insert(pair<string ,int>("buy",0));
    Commd.insert(pair<string, int>("sale",1));
    Commd.insert(pair<string, int>("login\r\n",2));
    Commd.insert(pair<string, int>("login",2));
    Commd.insert(pair<string, int>("connect",3));
    Commd.insert(pair<string, int>("connect\r\n",3));
    std::cout<<"in Initial\n";

    ppfunc[0] = &MyUser::funcbuy;
    ppfunc[1] = &MyUser::funcsale;
    ppfunc[2] = &MyUser::funclogin;
    ppfunc[3] = &MyUser::funcconnect;
}

void MyUser::callfunc(char* str, char* nextwords)
{
    std::cout<<"in callfunc\n";
    map<string, int>::iterator it;
    it = Commd.find(str);
    std::cout<<"size:"<<Commd.size()<<std::endl;
    if(it != Commd.end())
    {
        void (MyUser::*pfunc)(void*) = ppfunc[it->second];
        (this->*pfunc)(nextwords);
    }
    else
    {
        std::cout<<"could not find word\n";
    }
}

void MyUser::funcbuy(void* nextwords)
{
    char *p = (char*)nextwords;
    std::cout<<"this is buy,"<<p<<std::endl;
}

void MyUser::funcsale(void* nextwords)
{
    char *p = (char*)nextwords;
    std::cout<<"this is sale,"<<p<<std::endl;
}

void MyUser::funclogin(void* nextwords)
{
    std::cout<<"in funclogin\n"<<std::endl;
    char *p = (char*)nextwords;
    if(*p == NULL)
       {
           send(infd,"please input username",sizeof("please input username"),0);
       }
}

void MyUser::funcconnect( void* vParam )
{
    string str = static_cast<char*>(vParam);
    std::cout<<"in funcconnect: str:"<<str<<std::endl;

    std::vector<string>(strarray);
    split(strarray, str, "\n");
    std::cout<<"strarray.size:"<<strarray.size()<<std::endl;


}

void split(std::vector<string> &arr,string src, char* instead)
{
    int out = 1;
    int pos = 0;
    do
    {
        out = src.find(instead, pos);
        if(out != -1)
        {
            string new1 = src.substr(pos,out-pos);

            if(new1.length())
                {
                arr.push_back(new1);
                std::cout<<"out != -1 push_back:"<<new1<<"new1.length"<<new1.length()<<std::endl;
                }
            pos = out;
            pos += strlen(instead);
        }
        else
        {
            string new1 =src.substr(pos);
            if(new1.length())
            {
                arr.push_back(new1);
                std::cout<<"out == -1 push_back:"<<new1<<"new1.length"<<new1.length()<<std::endl;
            }

        }

    }while(out != -1);
}
