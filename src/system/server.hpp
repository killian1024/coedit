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
#include <experimental/filesystem>

#include "../core/basic_file_editor.hpp"
#include "server_session.hpp"
#include "client_data.hpp"
#include "system_exception.hpp"


namespace coedit {
namespace system {


namespace stdfs = std::experimental::filesystem;


class server
{
public:
    template<typename T>
    using vector_type = std::vector<T>;
    
    server(std::uint16_t port_nbr);
    
    ~server();
    
    int execute();

private:
    void add_client();
    
    bool server_session_exists(const stdfs::path& fle_path) const noexcept;
    
    server_session& get_server_session(const stdfs::path& fle_path);
    
private:
    bool execution_finish_;
    
    std::uint16_t port_nbr_;
    
    int master_sock_;
    
    sockaddr_in server_addr_;
    
    vector_type<server_session*> sessions_;
};


}
}


#endif
