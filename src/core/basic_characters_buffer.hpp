//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_HPP

#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<typename TpChar, std::size_t SIZE>
class basic_characters_buffer
{
public:
    using char_type = TpChar;
    
private:
    char_type buf_[SIZE];
};


}
}


#endif
