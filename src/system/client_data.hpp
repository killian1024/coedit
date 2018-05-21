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

#include <cstring>
#include <iostream>


namespace coedit {
namespace system {


class client_data
{
public:
    client_data() = default;
    
    client_data(int sock, const sockaddr_in& addr)
            : sock_(sock)
            , addr_(addr)
    {
    }
    
    inline int get_socket() const noexcept
    {
        return sock_;
    }
    
    inline const sockaddr_in& get_address() const noexcept
    {
        return addr_;
    }
    
    bool operator ==(const client_data& rhs) const
    {
        return sock_ == rhs.sock_;
    }
    
    bool operator !=(const client_data& rhs) const
    {
        return !(rhs == *this);
    }

private:
    int sock_;
    
    sockaddr_in addr_;
};


std::ostream& operator <<(std::ostream& os, const client_data& client_dat);


}
}


#endif
