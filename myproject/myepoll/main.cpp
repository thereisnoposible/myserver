#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include <string.h>
#include "find_search.h"
#include <vector>
#include <map>
#include "connectdb.h"
#define MAXEVENTS 64

using std::vector;
using std::map;
using std::pair;

map<int,int> mapfdList;
MyUser* cUser[1024] = {NULL};

static int create_and_bind(char *port)
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;
    int s, sfd;

    memset (&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    s = getaddrinfo (NULL, port, &hints, &result);
    if(s != 0)
    {
        fprintf (stderr, "getaddrinfo: %s\n",gai_strerror(s));
        return -1;
    }

    for(rp = result; rp != NULL; rp = rp->ai_next)
    {
        sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        s = bind (sfd, rp->ai_addr, rp->ai_addrlen);

        if (s == 0)
        {
            break;
        }
        close (sfd);
    }

    if (rp == NULL)
    {
        fprintf (stderr, "could not bind\n");
        return -1;
    }

    freeaddrinfo (result);

    return sfd;
}

static int make_socket_non_blocking (int sfd)
{
    int flags, s;

    flags = fcntl (sfd, F_GETFL, 0);
    if (flags == -1)
    {
        perror ("fcntl\n");
        return -1;
    }

    flags |= O_NONBLOCK;
    s = fcntl(sfd, F_SETFL, flags);
    if(s == -1)
    {
        perror("fcntl\n");
        return -1;
    }
    return 0;
}

void* wantknow(void*)
{
    char a[10]={0};
    while(1)
    {
        int z = 0;
        memset(a, 0, sizeof(a));
        std::cin>>a;
        map<int ,int >::iterator i_iter;
        for(i_iter = mapfdList.begin(); i_iter != mapfdList.end(); i_iter++)
            z = send(i_iter->first,a,strlen(a),0);
        std::cout<<"send "<<z<<" bytes\n";
    }

    return 0;
}

int main(int argc, char *argv[])
{
    Initdb();
    int sfd, s;
    int efd;
    struct epoll_event event;
    struct epoll_event *events;
    pthread_t pid;
    pthread_create(&pid, NULL, wantknow, NULL);
    char port[10] ="88888";

    sfd = create_and_bind(port);
    if(sfd == -1)
        abort();

    s = make_socket_non_blocking(sfd);
    if(s == -1)
        abort();

    s = listen(sfd, SOMAXCONN);
    if(s == -1)
    {
        perror("listen");
        abort();
    }

    efd =epoll_create1(0);
    if(efd == -1)
    {
        perror ("epoll_create\n");
        abort();
    }

    event.data.fd = sfd;
    event.events = EPOLLIN | EPOLLET;
    s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
    if (s == -1)
    {
        perror ("epoll_ctl\n");
        abort();
    }

    events = (epoll_event*)calloc (MAXEVENTS, sizeof(event));

    while(1)
    {
        int n, i;

        std::cout<<"waitting..."<<std::endl;
        n = epoll_wait (efd, events, MAXEVENTS, -1);

        for( i=0; i < n; i++)
        {
            std::cout<<"search events["<<i<<"]\n";
            if( (events[i].events & EPOLLERR) ||
                 (events[i].events & EPOLLHUP) ||
                 (!events[i].events & EPOLLIN))
            {
                fprintf(stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
            }

            else if( sfd == events[i].data.fd)
            {
                std::cout<<"find events["<<i<<"]\n";
                while (1)
                {
                    struct sockaddr in_addr;
                    socklen_t in_len;
                    int infd;
                    char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                    in_len = sizeof( in_addr );
                    infd = accept( sfd, &in_addr, &in_len );
                    if( infd == -1)
                    {
                        if(( errno ==EAGAIN) ||
                            ( errno ==EWOULDBLOCK))
                        {
                            break;
                        }
                        else
                        {
                            perror("accept\n");
                            break;
                        }
                    }

                    s = getnameinfo( &in_addr, in_len, hbuf, sizeof(hbuf), sbuf, sizeof(sbuf), NI_NUMERICHOST | NI_NUMERICSERV);
                    if( s == 0)
                    {
                        printf("Accepted connection on descriptor %d (host=%s, port=%s\n)",infd, hbuf, sbuf );

                    }

                    s = make_socket_non_blocking(infd);
                    if( s == -1)
                        abort();

                    event.data.fd = infd;
                    event.events = EPOLLIN |EPOLLET;
                    s = epoll_ctl(efd, EPOLL_CTL_ADD ,infd, &event);
                    if( s == -1 )
                    {
                        perror("epoll_ctl");
                        abort();
                    }


                    mapfdList.insert(pair<int,int>(infd,infd));
                    cUser[infd] = new MyUser(infd);
                }
                continue;
            }
            else
            {
                int done = 0;
                std::cout<<"in the loop events["<<i<<"]\n";
                while(1)
                {
                    ssize_t count1;
                    char buf[512];
                    memset(buf, 0, sizeof(buf));

 //                   count1 = read(events[i].data.fd, buf, sizeof(buf));
                    count1 =recv(events[i].data.fd, buf, sizeof(buf), 0);
                    if(count1 > 0)
                    {
                        cUser[events[i].data.fd]->find_words(buf);
                        std::cout<<"find_words back\n";
 //                       int iSuccessed = find_words(buf);

                    }
                    if( count1 == -1 )
                    {
                        if(errno != EAGAIN)
                        {
                            perror("read");
                            done = 1;
                        }
                        break;
                    }
                    else if(count1 == 0)
                    {
                        done = 1;
                        break;
                    }

                    //std::cout<<buf<<std::endl;
                }

                if(done)
                {
                    printf("Closed connection on descriptor %d\n",events[i].data.fd);

                    delete cUser[events[i].data.fd];
                    cUser[events[i].data.fd] = NULL;

                    mapfdList.erase(events[i].data.fd);

                    close(events[i].data.fd);
                }
            }
        }
    }
    free(events);
    close (sfd);
    return EXIT_SUCCESS;
}
