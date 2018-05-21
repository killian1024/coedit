//
// Created by Killian on 29/04/18.
//

#include <signal.h>

#include <kboost/kboost.hpp>

#include "server.hpp"
#include "tcp_segment.hpp"


namespace coedit {
namespace system {


server::server(std::uint16_t port_nbr)
        : master_sock_(-1)
        , server_addr_()
        , port_nbr_(port_nbr)
        , thrd_()
        , mutx_sessions_()
        , execution_finish_(false)
{
    if ((master_sock_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
    {
        throw impossible_to_open_socket_exception();
    }
    memset(&server_addr_, 0, sizeof(server_addr_));
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = INADDR_ANY;
    server_addr_.sin_port = htons(port_nbr_);
    if (bind(master_sock_, (sockaddr*)&server_addr_, sizeof(server_addr_)) == -1)
    {
        throw impossible_to_open_socket_exception();
    }
    
    if (listen(master_sock_, 5) == -1)
    {
        throw impossible_to_open_socket_exception();
    }
}


server::~server()
{
    for (auto& x : sessions_)
    {
        delete x;
    }
    if (master_sock_ != -1)
    {
        close(master_sock_);
        master_sock_ = -1;
    }
}


int server::execute()
{
    std::cout << kios::set_light_purple_text << "Starting coedit server" << kios::newl;
    
    thrd_ = std::thread(&server::remove_sessions, this);
    
    while (!execution_finish_)
    {
        try
        {
            add_client();
        }
        catch (const coedit::system::system_exception& e)
        {
            std::cerr << kios::set_light_red_text << e.what() << kios::newl;
        }
    }
    
    for (auto& x : sessions_)
    {
        x->join();
    }
    
    thrd_.join();
    
    return 0;
}


void server::add_client()
{
    int client_sock;
    sockaddr_in client_addr;
    socklen_t sock_len;
    tcp_segment_data tcp_seg;
    path_type fle_path;
    client_data client_dat;
    
    std::cout << kios::set_light_purple_text << "Waiting connection" << kios::newl;
    sock_len = sizeof(client_addr);
    if ((client_sock = accept(master_sock_, (sockaddr*)&client_addr, &sock_len)) == -1)
    {
        throw impossible_to_accept_client_exception();
    }
    client_dat = client_data(client_sock, client_addr);
    
    if (recv(client_sock, &tcp_seg, sizeof(tcp_seg), 0) == -1)
    {
        shutdown(client_sock, SHUT_RDWR);
        throw bad_receive_exception();
    }
    fle_path = (const char*)tcp_seg.dat.raw;
    tcp_seg.typ = tcp_segment_type::SESSION_REPLY;
    if (!fle_path.is_absolute() || !std::filesystem::exists(fle_path))
    {
        tcp_seg.dat.ok = false;
        if (send(client_sock, &tcp_seg, sizeof(tcp_seg), MSG_NOSIGNAL) == -1)
        {
            shutdown(client_sock, SHUT_RDWR);
            throw bad_send_exception();
        }
        std::cout << kios::set_light_red_text << "Client : " << client_dat << " Rejected"
                  << kios::newl;
        return;
    }
    tcp_seg.dat.ok = true;
    if (send(client_sock, &tcp_seg, sizeof(tcp_seg), MSG_NOSIGNAL) == -1)
    {
        shutdown(client_sock, SHUT_RDWR);
        throw bad_send_exception();
    }
    std::cout << kios::set_light_green_text << "Client : " << client_dat << " Accepted"
              << kios::newl;
    
    // Add to the session the new client.
    mutx_sessions_.lock();
    if (!server_session_exists(fle_path))
    {
        sessions_.push_back(new server_session(fle_path));
        std::cout << kios::set_light_green_text << "Adding new session in: "
                  << fle_path << kios::newl;
        
        sessions_.back()->add_client(client_dat);
        sessions_.back()->execute();
    }
    else
    {
        server_session& serv_session = get_server_session(fle_path);
        serv_session.add_client(client_dat);
    }
    mutx_sessions_.unlock();
    
    std::cout << kios::set_light_green_text << "Adding new client to: "
              << fle_path << kios::newl;
}


void server::remove_sessions()
{
    while (!execution_finish_)
    {
        mutx_sessions_.lock();
        
        for (auto it = sessions_.begin(); it != sessions_.end(); ++it)
        {
            if ((*it)->is_finish())
            {
                (*it)->join();
                delete *it;
                it = sessions_.erase(it);
                
                std::cout << kios::set_yellow_text
                          << "Session closed [ok]"
                          << kios::newl;
            
                if (it == sessions_.end())
                {
                    break;
                }
            }
        }
        
        mutx_sessions_.unlock();
        
        ksys::nanosleep(0, 100);
    }
}


bool server::server_session_exists(const path_type& fle_path) const noexcept
{
    for (auto& x : sessions_)
    {
        if (x->is_same_path(fle_path))
        {
            return true;
        }
    }
    
    return false;
}


server_session& server::get_server_session(const path_type& fle_path)
{
    for (auto& x : sessions_)
    {
        if (x->is_same_path(fle_path))
        {
            return *x;
        }
    }
    
    throw invalid_path_exception();
}


}
}
