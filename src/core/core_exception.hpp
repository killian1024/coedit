//
// Created by Killian on 01/02/18.
//

#ifndef COEDIT_CORE_CORE_EXCEPTION_HPP
#define COEDIT_CORE_CORE_EXCEPTION_HPP

#include <exception>


namespace coedit {
namespace core {


class core_exception : public std::exception
{
    const char* what() const noexcept override
    {
        return "core exception";
    }
};


class length_exception : public core_exception
{
    const char* what() const noexcept override
    {
        return "length exception";
    }
};


class bad_allocation_exception : public core_exception
{
    const char* what() const noexcept override
    {
        return "bad allocation exception";
    }
};


class invalid_lid_exception : public core_exception
{
    const char* what() const noexcept override
    {
        return "invalid lid exception";
    }
};


class invalid_cbid_exception : public core_exception
{
    const char* what() const noexcept override
    {
        return "invalid cbid exception";
    }
};


class invalid_operation_exception : public core_exception
{
    const char* what() const noexcept override
    {
        return "invalid operation exception";
    }
};


class characte_buffer_overflow_exception : public core_exception
{
    const char* what() const noexcept override
    {
        return "character buffer overflow exception";
    }
};


class line_overflow_exception : public core_exception
{
    const char* what() const noexcept override
    {
        return "line overflow exception";
    }
};


}
}


#endif
