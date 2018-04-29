//
// Created by Killian on 01/02/18.
//

#ifndef COEDIT_SYSTEM_SYSTEM_EXCEPTION_HPP
#define COEDIT_SYSTEM_SYSTEM_EXCEPTION_HPP

#include <exception>


namespace coedit {
namespace system {


class system_exception : public std::exception
{
    const char* what() const noexcept override
    {
        return "system exception";
    }
};


class invalid_path_exception : public system_exception
{
    const char* what() const noexcept override
    {
        return "invalid path exception";
    }
};


}
}


#endif
