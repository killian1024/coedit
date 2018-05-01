//
// Created by Killian on 29/04/18.
//

#include "client.hpp"
#include "tcp_segment.hpp"


namespace coedit {
namespace system {


client::client(const char* serv_addr, std::uint16_t port_nbr_, path_type fle_path)
        : fle_path_(std::move(fle_path))
        , fle_editr_(path_type(), core::newline_format::UNIX)
        , interf_(&fle_editr_)
        , sock_()
        , serv_addr_()
        , thrd_()
        , mutx_fle_editr_()
        , execution_finish_(false)
{
    hostent* hostnt;
    
    sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    hostnt = gethostbyname(serv_addr);
    memset(&serv_addr_, 0, sizeof(serv_addr_));
    serv_addr_.sin_family = AF_INET;
    memcpy(&serv_addr_.sin_addr, hostnt->h_addr_list[0], static_cast<size_t>(hostnt->h_length));
    serv_addr_.sin_port = htons(port_nbr_);
}


int client::execute()
{
    file_editor_command_type cmd;
    
    connect_to_server();
    get_data_from_server();
    
    interf_.init();
    interf_.print();
    
    thrd_ = std::thread(&client::thread_execute, this);
    
    while (!execution_finish_)
    {
        if ((cmd = interf_.get_command()) != file_editor_command_type::NIL)
        {
            if (cmd == file_editor_command_type::EXIT)
            {
                execution_finish_ = true;
            }
            else if (!send_command_to_server(cmd))
            {
                mutx_fle_editr_.lock();
                fle_editr_.handle_command(cmd);
                interf_.print();
                mutx_fle_editr_.unlock();
            }
        }
    }
    
    interf_.free();
    
    return 0;
}


void client::connect_to_server()
{
    tcp_segment_data tcp_seg;
    const std::string& sfle_path = fle_path_.native();
    
    std::cout << kios::set_light_purple_text << "Connecting to server...";
    if (connect(sock_, (sockaddr*)&serv_addr_, sizeof(serv_addr_)) == -1)
    {
        std::cout << kios::set_light_red_text << "[ERR]" << kios::newl;
        throw impossible_to_connect_to_server_exception();
    }
    std::cout << kios::set_light_green_text << "[OK]" << kios::newl << std::flush;
    
    std::cout << kios::set_light_purple_text << "Sending request to server...";
    tcp_seg.typ = tcp_segment_type::SESSION_REQUEST;
    memcpy(&tcp_seg.dat.raw[0], sfle_path.c_str(), sfle_path.size());
    send(sock_, &tcp_seg, sizeof(tcp_seg), 0);
    recv(sock_, &tcp_seg, sizeof(tcp_seg), 0);
    if (!tcp_seg.dat.ok)
    {
        std::cout << kios::set_light_red_text << "[ERR]" << kios::newl;
        throw bad_server_request_exception();
    }
    std::cout << kios::set_light_green_text << "[OK]" << kios::newl << std::flush;
}


void client::get_data_from_server()
{
    constexpr const std::size_t data_size =
            sizeof(tcp_segment_data::data) / sizeof(file_editor_type::char_type);
    
    tcp_segment_data tcp_seg{};
    char_type* dat = reinterpret_cast<char_type*>(&tcp_seg.dat.raw[0]);
    bool insersion_end = false;
    
    std::cout << kios::set_light_purple_text << "Receiving data from server...";
    
    do
    {
        recv(sock_, &tcp_seg, sizeof(tcp_seg), 0);
        
        for (std::size_t i = 0; i < data_size; ++i)
        {
            if (dat[i] != 0)
            {
                fle_editr_.insert_character(dat[i]);
            }
            else
            {
                insersion_end = true;
                break;
            }
        }
        
    } while (!insersion_end);
    
    std::cout << kios::set_light_green_text << "[OK]" << kios::newl << std::flush;
}


bool client::send_command_to_server(file_editor_command_type cmd)
{
    tcp_segment_data tcp_seg;
    
    tcp_seg.typ = tcp_segment_type::INSERSION_REQUEST;
    tcp_seg.dat.editr_cmd.lid = fle_editr_.get_current_lid();
    tcp_seg.dat.editr_cmd.loffset = fle_editr_.get_cursor_position().loffset;
    tcp_seg.dat.editr_cmd.cmd = cmd;
    
    if (cmd > file_editor_command_type::MAX ||
        cmd == file_editor_command_type::NEWLINE ||
        cmd == file_editor_command_type::BACKSPACE)
    {
        send(sock_, &tcp_seg, sizeof(tcp_seg), 0);
        return true;
    }
    
    return false;
}


void client::thread_execute()
{
    tcp_segment_data tcp_seg;
    
    while (!execution_finish_)
    {
        recv(sock_, &tcp_seg, sizeof(tcp_seg), 0);
    
        if (tcp_seg.dat.editr_cmd.cmd > file_editor_command_type::MAX ||
            tcp_seg.dat.editr_cmd.cmd == file_editor_command_type::NEWLINE ||
            tcp_seg.dat.editr_cmd.cmd == file_editor_command_type::BACKSPACE)
        {
            mutx_fle_editr_.lock();
            fle_editr_.handle_command(tcp_seg.dat.editr_cmd.lid,
                                      tcp_seg.dat.editr_cmd.loffset,
                                      tcp_seg.dat.editr_cmd.cmd);
            interf_.print();
            mutx_fle_editr_.unlock();
        }
    }
}


}
}
