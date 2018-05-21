//
// Created by Killian on 29/04/18.
//

#include <kboost/kboost.hpp>

#include "server_session.hpp"
#include "tcp_segment.hpp"
#include "system_exception.hpp"


namespace coedit {
namespace system {


server_session::server_session(path_type fle_path)
        : fle_editr_(std::move(fle_path), core::newline_format::UNIX)
        , clients_dat_()
        , thrd_()
        , mutx_clients_dat_()
        , finsh_(false)
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
    
    for (auto& lne : fle_editr_)
    {
        if (!first_lne)
        {
            tcp_seg.typ = tcp_segment_type::NEWLINE_REQUEST;
            tcp_seg.dat.editr_cmd.lid = lne.get_lid();
            if (send(client_dat.get_socket(), &tcp_seg, sizeof(tcp_seg), MSG_NOSIGNAL) == -1)
            {
                shutdown(client_dat.get_socket(), SHUT_RDWR);
                throw bad_send_exception();
            }
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
            if (send(client_dat.get_socket(), &tcp_seg, sizeof(tcp_seg), MSG_NOSIGNAL) == -1)
            {
                shutdown(client_dat.get_socket(), SHUT_RDWR);
                throw bad_send_exception();
            }
            memset(&tcp_seg.dat.raw[0], 0, sizeof(tcp_seg.dat.raw));
        }
        
        first_lne = false;
    }
    
    memset(&tcp_seg.dat.raw[0], 0, sizeof(tcp_seg.dat.raw));
    tcp_seg.typ = tcp_segment_type::INSERSION_REQUEST;
    if (send(client_dat.get_socket(), &tcp_seg, sizeof(tcp_seg), MSG_NOSIGNAL) == -1)
    {
        shutdown(client_dat.get_socket(), SHUT_RDWR);
        throw bad_send_exception();
    }
    
    clients_dat_.push_back(client_dat);
    
    finsh_ = false;
    
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
    int select_res;
    timeval tv{ .tv_sec = 0, .tv_usec = 1000 };
    
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
    
        select_res = select(max_fd + 1, &fd_socks_ , nullptr, nullptr, &tv);
        if (select_res == -1)
        {
            finsh_ = true;
            throw bad_file_descriptor_exception();
        }
        
        if (select_res == 0)
        {
            continue;
        }
        
        mutx_clients_dat_.lock();
    
        for (auto it = clients_dat_.begin(); it != clients_dat_.end(); ++it)
        {
            if (FD_ISSET(it->get_socket(), &fd_socks_))
            {
                it = manage_request(it);
                
                if (it == clients_dat_.end())
                {
                    break;
                }
            }
        }
        mutx_clients_dat_.unlock();
    }
    
    std::cout << kios::set_yellow_text
              << "Session " << fle_editr_.get_file_path() << " closed"
              << kios::newl;
    
    finsh_ = true;
}


server_session::vector_type<client_data>::iterator server_session::manage_request(
        vector_type<client_data>::iterator it_client
)
{
    tcp_segment_data tcp_seg;
    ssize_t recv_res;
    
    recv_res = recv(it_client->get_socket(), &tcp_seg, sizeof(tcp_seg), 0);
    if (recv_res == -1 || recv_res == 0)
    {
        return remove_client(it_client);
    }
    
    std::cout << kios::set_light_blue_text << "Managing request from Client : " << *it_client
              << " [" << tcp_seg.dat.editr_cmd.lid << ":"
              << (std::uint32_t)tcp_seg.dat.editr_cmd.cmd << ":"
              << tcp_seg.dat.editr_cmd.ch << "]"
              << kios::newl;
    
    fle_editr_.handle_command(tcp_seg.dat.editr_cmd.lid,
                              tcp_seg.dat.editr_cmd.loffset,
                              tcp_seg.dat.editr_cmd.cmd,
                              tcp_seg.dat.editr_cmd.ch);
    
    for (auto it = clients_dat_.begin(); it != clients_dat_.end(); ++it)
    {
        if (send(it->get_socket(), &tcp_seg, sizeof(tcp_seg), MSG_NOSIGNAL) == -1)
        {
            if (it_client == it)
            {
                it_client = it = remove_client(it);
            }
            else
            {
                it = remove_client(it);
            }
            
            if (it == clients_dat_.end())
            {
                break;
            }
        }
    }

    return it_client;
}


server_session::vector_type<client_data>::iterator server_session::remove_client(
        vector_type<client_data>::iterator it_client
)
{
    std::cout << kios::set_yellow_text << "Removind Client : " << *it_client << kios::newl;
    shutdown(it_client->get_socket(), SHUT_RDWR);
    close(it_client->get_socket());
    
    return clients_dat_.erase(it_client);
}


}
}
