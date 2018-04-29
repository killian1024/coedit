//
// Created by Killian on 29/04/18.
//

#ifndef COEDIT_SYSTEM_CLIENT_DATA_HPP
#define COEDIT_SYSTEM_CLIENT_DATA_HPP

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <fcntl.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


namespace coedit {
namespace system {


class client_data
{
public:
    client_data(int client_sock, const sockaddr_in& client_addr)
            : client_sock_(client_sock)
            , client_addr_(client_addr)
    {
    }
    
    inline int get_client_socket() const noexcept
    {
        return client_sock_;
    }
    
private:
    int client_sock_;
    
    sockaddr_in client_addr_;
};


}
}


#endif
