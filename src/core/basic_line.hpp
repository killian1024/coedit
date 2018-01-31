//
// Created by Killian on 20/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINE_HPP
#define COEDIT_CORE_BASIC_LINE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<typename TpChar, typename TpAllocator = std::allocator<int>>
class basic_line
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    template<typename T>
    using list_type = kcontain::circular_doubly_linked_list<T, TpAllocator>;
    
    basic_line()
            : lid_(0)
            , prev_(0)
            , nxt_(0)
            , bid_(0)
            , bid_offset_(0)
    {
    }

private:
    lid_t lid_;
    lid_t prev_;
    lid_t nxt_;
    cbid_t bid_;
    cbid_offset_t bid_offset_;
};


using line = basic_line<char16_t>;


}
}


#endif
