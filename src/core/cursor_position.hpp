//
// Created by Killian on 08/02/18.
//

#ifndef COEDIT_CORE_CURSOR_POSITION_HPP
#define COEDIT_CORE_CURSOR_POSITION_HPP

#include "fundamental_types.hpp"


namespace coedit {
namespace core {


struct cursor_position
{
    inline bool operator ==(const cursor_position& rhs) const noexcept
    {
        return loffset == rhs.loffset &&
               coffset == rhs.coffset;
    }
    
    inline bool operator !=(const cursor_position& rhs) const noexcept
    {
        return !(rhs == *this);
    }
    
    loffset_t loffset;
    coffset_t coffset;
};


}
}


#endif
