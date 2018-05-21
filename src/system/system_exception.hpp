//
// Created by Killian on 01/02/18.
//

#ifndef COEDIT_SYSTEM_SYSTEM_EXCEPTION_HPP
#define COEDIT_SYSTEM_SYSTEM_EXCEPTION_HPP

#include <exception>


namespace coedit {
namespace system {


struct system_exception : public std::exception
{
    const char* what() const noexcept override
    {
        return "system exception";
    }
};


struct impossible_to_open_socket_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Impossible to open socket";
    }
};


struct bad_server_address_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Bad server address";
    }
};


struct impossible_to_connect_to_server_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Impossible to connect to server";
    }
};


struct impossible_to_accept_client_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Impossible to accept client";
    }
};


struct bad_server_request_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Bad server request";
    }
};


struct bad_send_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Bad send";
    }
};


struct bad_receive_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Bad receive";
    }
};


struct invalid_path_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Invalid path";
    }
};


struct bad_file_descriptor_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "Bad file descriptor";
    }
};


}
}


#endif
