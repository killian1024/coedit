//
// Created by Killian on 29/04/18.
//

#ifndef COEDIT_SYSTEM_BASIC_SERVER_SESSION_HPP
#define COEDIT_SYSTEM_BASIC_SERVER_SESSION_HPP

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <filesystem>
#include <mutex>
#include <thread>
#include <vector>

#include "../core/basic_file_editor.hpp"
#include "client_data.hpp"


namespace coedit {
namespace system {


class server_session
{
public:
    using char_type = core::file_editor::char_type;
    
    using file_editor_type = core::file_editor;
    
    template<typename T>
    using vector_type = std::vector<T>;
    
    using path_type = std::filesystem::path;
    
    server_session(path_type fle_path);
    
    void execute();
    
    void add_client(const client_data& client_dat);
    
    bool is_same_path(const path_type& fle_path) const noexcept;
    
    void join();
    
private:
    void thread_execute();
    
    void manage_request(client_data& client_dat);

private:
    file_editor_type fle_editr_;
    
    vector_type<client_data> clients_dat_;
    
    fd_set fd_socks_;
    
    std::thread thrd_;
    
    std::mutex mutx_clients_dat_;
};


}
}


#endif
