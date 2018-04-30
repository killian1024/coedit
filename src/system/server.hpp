//
// Created by Killian on 28/04/18.
//

#ifndef COEDIT_SYSTEM_SERVER_HPP
#define COEDIT_SYSTEM_SERVER_HPP

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <cstring>
#include <filesystem>

#include "../core/basic_file_editor.hpp"
#include "client_data.hpp"
#include "server_session.hpp"
#include "system_exception.hpp"


namespace coedit {
namespace system {


class server
{
public:
    template<typename T>
    using vector_type = std::vector<T>;
    
    using path_type = std::filesystem::path;
    
    server(std::uint16_t port_nbr);
    
    ~server();
    
    int execute();

private:
    void add_client();
    
    bool server_session_exists(const path_type& fle_path) const noexcept;
    
    server_session& get_server_session(const path_type& fle_path);
    
private:
    int master_sock_;
    
    sockaddr_in server_addr_;
    
    std::uint16_t port_nbr_;
    
    vector_type<server_session*> sessions_;
    
    bool execution_finish_;
};


}
}


#endif
