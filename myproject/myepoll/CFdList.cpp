#include "cFdList.h"
#include <iostream>
#include <string.h>
CFdList::CFdList(int fd)
{
    infd = fd;
    FirstMember = this;
    LastMember = this;
    PrevMember =NULL;
    NextMember =NULL;
}

void CFdList::AddMember(int fd)
{
    if( infd == 0 )
    {
        infd = fd;
        return;
    }
    CFdList* cCopy = new CFdList( fd );
    cCopy->PrevMember = LastMember;
    cCopy->FirstMember = FirstMember;
    this = cCopy;
}

void CFdList::DeleteMember(int fd)
{
    if(FirstMember->infd == fd)
    {
        delete FirstMember;
        FirstMember = FirstMember->NextMember;
    }
    CFdList * cCopy = FirstMember;
    while(cCopy->infd != fd && cCopy->NextMember != NULL)
    {
        cCopy++;
    }
    if(cCopy->infd == fd && cCopy->NextMember == NULL)
    {
        cCopy->PrevMember->NextMember = cCopy->NextMember;
        delete cCopy;
    }
}
