#ifndef CFDLIST_H_INCLUDED
#define CFDLIST_H_INCLUDED
class CFdList
{
public:
    CFdList(int fd);
    ~CFdList(){};
    void AddMember(int fd);
    void DeleteMember(int fd);
private:
    CFdList* FirstMember;
    CFdList* NextMember;
    CFdList* LastMember;
    CFdList* PrevMember;
    int infd;

};


#endif // CFDLIST_H_INCLUDED
