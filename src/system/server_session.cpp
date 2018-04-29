//
// Created by Killian on 29/04/18.
//

#include <kboost/kboost.hpp>

#include "server_session.hpp"


namespace coedit {
namespace system {


server_session::server_session(stdfs::path fle_path)
        : fle_path_(fle_path)
        , file_editr_(std::move(fle_path), core::newline_format::UNIX)
        , clients_dat_()
        , thd_()
{
}


void server_session::execute()
{
    thd_ = std::thread(&server_session::execute_in_thread, this);
}


void server_session::add_client(const client_data& client_dat)
{
    mutx_clients_dat_.lock();
    clients_dat_.push_back(client_dat);
    mutx_clients_dat_.unlock();
}


bool server_session::is_same_path(const stdfs::path& fle_path) const noexcept
{
    return fle_path_ == fle_path;
}


void server_session::join()
{
    thd_.join();
}


void server_session::execute_in_thread()
{
    int max_fd = 0;
    
    while (!clients_dat_.empty())
    {
        FD_ZERO(&fd_socks_);
    
        mutx_clients_dat_.lock();
        for (auto& x : clients_dat_)
        {
            FD_SET(x.get_client_socket(), &fd_socks_);
            
            if (max_fd < x.get_client_socket())
            {
                max_fd = x.get_client_socket();
            }
        }
        mutx_clients_dat_.unlock();
        
        select(max_fd + 1, &fd_socks_ , nullptr, nullptr, nullptr);
    
        mutx_clients_dat_.lock();
        for (auto& x : clients_dat_)
        {
            if (FD_ISSET(x.get_client_socket(), &fd_socks_))
            {
                manage_request(x);
            }
        }
        mutx_clients_dat_.unlock();
    }
}


void server_session::manage_request(client_data& client_dat)
{
    std::cout << kios::set_blue_text << "Managing request from client" << kios::newl;
    
    // HERE : request/response d'insersion
    
    /* Request :
     *      type { REQUESTE, RESPONSE }
     *      lid_t
     *      loffset_t
     *      file_editor_command { NEWLINE, BACKSPACE, INSERT }
     */
}


}
}

