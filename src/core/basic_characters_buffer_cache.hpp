//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_CACHE_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_CACHE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<typename TpChar, std::size_t SIZE>
class basic_characters_buffer_cache
{
public:
    using char_type = TpChar;
    
    using characters_buffer_type = basic_characters_buffer<TpChar, SIZE>;
    
private:
    kcontain::buffer_cache<lid_t, characters_buffer_type, SIZE> buf_cache_;
};


}
}


#endif
