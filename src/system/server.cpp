//
// Created by Killian on 29/04/18.
//

#include "server.hpp"


namespace coedit {
namespace system {


server::server(std::uint16_t port_nbr)
        : execution_finish_(false)
        , port_nbr_(port_nbr)
        , master_sock_()
        , server_addr_()
{
    memset(&server_addr_, 0, sizeof(server_addr_));
    server_addr_.sin_family = AF_INET;
    server_addr_.sin_addr.s_addr = INADDR_ANY;
    server_addr_.sin_port = htons(port_nbr_);
    bind(master_sock_, (sockaddr*)&server_addr_, sizeof(server_addr_));
    listen(master_sock_, 5);
}


server::~server()
{
    for (auto& x : sessions_)
    {
        delete x;
    }
}


int server::execute()
{
    while (!execution_finish_)
    {
        add_client();
    }
    
    for (auto& x : sessions_)
    {
        x->join();
    }
    
    return 0;
}


void server::add_client()
{
    stdfs::path fle_path("./file.txt");
    int client_sock;
    sockaddr_in client_addr;
    socklen_t sock_len;
    
    sock_len = sizeof(client_addr);
    client_sock = accept(master_sock_, (struct sockaddr *)&client_addr, &sock_len);
    
    if (!server_session_exists(fle_path))
    {
        sessions_.push_back(new server_session(fle_path));
        sessions_.back()->add_client(client_data(client_sock, client_addr));
        sessions_.back()->execute();
    }
    else
    {
        server_session& serv_session = get_server_session(fle_path);
        serv_session.add_client(client_data(client_sock, client_addr));
    }
}


bool server::server_session_exists(const stdfs::path& fle_path) const noexcept
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


server_session& server::get_server_session(const stdfs::path& fle_path)
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
