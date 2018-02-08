//
// Created by Killian on 08/02/18.
//

#ifndef COEDIT_CORE_CURSOR_HPP
#define COEDIT_CORE_CURSOR_HPP

#include "fundamental_types.hpp"


namespace coedit {
namespace core {


struct cursor
{
    loffset_t loffset;
    coffset_t coffset;
};


}
}


#endif
