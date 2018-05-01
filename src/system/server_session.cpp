//
// Created by Killian on 29/04/18.
//

#include <kboost/kboost.hpp>

#include "server_session.hpp"
#include "tcp_segment.hpp"


namespace coedit {
namespace system {


server_session::server_session(path_type fle_path)
        : fle_editr_(std::move(fle_path), core::newline_format::UNIX)
        , clients_dat_()
        , thrd_()
{
}


void server_session::execute()
{
    thrd_ = std::thread(&server_session::thread_execute, this);
}


void server_session::add_client(const client_data& client_dat)
{
    constexpr const std::size_t data_size =
            sizeof(tcp_segment_data::data) / sizeof(file_editor_type::char_type);
    
    tcp_segment_data tcp_seg{};
    char_type* dat = reinterpret_cast<char_type*>(&tcp_seg.dat.raw[0]);
    std::size_t i = 0;
    bool first_lne = true;
    
    mutx_clients_dat_.lock();
    
    clients_dat_.push_back(client_dat);
    
    for (auto& lne : fle_editr_)
    {
        if (!first_lne)
        {
            tcp_seg.typ = tcp_segment_type::NEWLINE_REQUEST;
            tcp_seg.dat.editr_cmd.lid = lne.get_lid();
            send(client_dat.get_socket(), &tcp_seg, sizeof(tcp_seg), 0);
            memset(&tcp_seg.dat.raw[0], 0, sizeof(tcp_seg.dat.raw));
        }
        
        i = 0;
        for (auto& ch : lne)
        {
            dat[i] = ch;
            ++i;
        }
        
        if (i != 0)
        {
            tcp_seg.typ = tcp_segment_type::INSERSION_REQUEST;
            send(client_dat.get_socket(), &tcp_seg, sizeof(tcp_seg), 0);
            memset(&tcp_seg.dat.raw[0], 0, sizeof(tcp_seg.dat.raw));
        }
        
        first_lne = false;
    }
    
    memset(&tcp_seg.dat.raw[0], 0, sizeof(tcp_seg.dat.raw));
    tcp_seg.typ = tcp_segment_type::INSERSION_REQUEST;
    send(client_dat.get_socket(), &tcp_seg, sizeof(tcp_seg), 0);
    
    mutx_clients_dat_.unlock();
}


bool server_session::is_same_path(const std::filesystem::path& fle_path) const noexcept
{
    return fle_editr_.get_file_path() == fle_path;
}


void server_session::join()
{
    thrd_.join();
}


void server_session::thread_execute()
{
    int max_fd = 0;
    
    while (!clients_dat_.empty())
    {
        FD_ZERO(&fd_socks_);
    
        mutx_clients_dat_.lock();
        for (auto& x : clients_dat_)
        {
            FD_SET(x.get_socket(), &fd_socks_);
            
            if (max_fd < x.get_socket())
            {
                max_fd = x.get_socket();
            }
        }
        mutx_clients_dat_.unlock();
        
        // FIXME(killian.poulaud@etu.upmc.fr): New file descriptors will not be concidered until next select.
        select(max_fd + 1, &fd_socks_ , nullptr, nullptr, nullptr);
        
        mutx_clients_dat_.lock();
        for (auto& x : clients_dat_)
        {
            if (FD_ISSET(x.get_socket(), &fd_socks_))
            {
                manage_request(x);
            }
        }
        mutx_clients_dat_.unlock();
    }
}


void server_session::manage_request(client_data& client_dat)
{
    tcp_segment_data tcp_seg;
    
    recv(client_dat.get_socket(), &tcp_seg, sizeof(tcp_seg), 0);
    
    std::cout << kios::set_light_blue_text << "Managing request from Client : " << client_dat
              << " [" << tcp_seg.dat.editr_cmd.lid << ":"
              << (std::uint32_t)tcp_seg.dat.editr_cmd.cmd << "]"
              << kios::newl;
    
    fle_editr_.handle_command(tcp_seg.dat.editr_cmd.lid,
                              tcp_seg.dat.editr_cmd.loffset,
                              tcp_seg.dat.editr_cmd.cmd);
    
    for (auto& x : clients_dat_)
    {
        send(x.get_socket(), &tcp_seg, sizeof(tcp_seg), 0);
    }
}


}
}

