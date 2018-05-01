//
// Created by Killian on 28/04/18.
//

#ifndef COEDIT_SYSTEM_CLIENT_HPP
#define COEDIT_SYSTEM_CLIENT_HPP

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

#include <filesystem>
#include <mutex>
#include <thread>

#include "../core/basic_file_editor.hpp"
#include "../tui/curses_interface.hpp"
#include "system_exception.hpp"


namespace coedit {
namespace system {


class client
{
public:
    using char_type = core::file_editor::char_type;
    
    using file_editor_type = core::file_editor;
    
    using file_editor_command_type = core::file_editor_command;
    
    using interface_type = tui::curses_interface;
    
    using path_type = std::filesystem::path;
    
    client(const char* serv_addr, std::uint16_t port_nbr_, path_type fle_path);
    
    int execute();

private:
    void connect_to_server();
    
    void get_data_from_server();
    
    bool send_command_to_server(file_editor_command_type cmd);
    
    void thread_execute();

private:
    path_type fle_path_;
    
    file_editor_type fle_editr_;
    
    interface_type interf_;
    
    int sock_;
    
    sockaddr_in serv_addr_;
    
    std::thread thrd_;
    
    std::mutex mutx_fle_editr_;
    
    bool execution_finish_;
};


}
}


#endif
