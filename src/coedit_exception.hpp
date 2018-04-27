//
// Created by Killian on 27/04/18.
//

#ifndef COEDIT_COEDIT_EXCEPTION_HPP
#define COEDIT_COEDIT_EXCEPTION_HPP

#include <exception>


namespace coedit {


class coedit_exception : public std::exception
{
    const char* what() const noexcept override
    {
        return "coedit exception";
    }
};


}


#endif
