//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINES_CACHE_HPP
#define COEDIT_CORE_BASIC_LINES_CACHE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_line.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<typename TpChar, std::size_t SIZE, typename TpAllocator = std::allocator<int>>
class basic_lines_cache
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using line_type = basic_line<TpChar, TpAllocator>;
    
private:
    kcontain::buffer_cache<lid_t, line_type, SIZE> buf_cache_;
};


}
}


#endif
