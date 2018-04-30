//
// Created by Killian on 30/04/18.
//

#include "client_data.hpp"


namespace coedit {
namespace system {


std::ostream& operator <<(std::ostream& os, const client_data& client_dat)
{
    char host_name[128] = {0};
    
    if (getnameinfo((sockaddr*)&client_dat.get_address(), sizeof(client_dat.get_address()),
                    host_name, sizeof(host_name), nullptr, 0, 0) != 0)
    {
        strcpy(host_name, "??????");
    }
    
    os << "<IP = " << inet_ntoa(client_dat.get_address().sin_addr)
       << ", PORT = " << ntohs(client_dat.get_address().sin_port)
       << ", COMPUTER = " << host_name
       << ">";
    
    return os;
}


}
}
