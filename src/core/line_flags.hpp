//
// Created by Killian on 02/02/18.
//

#ifndef COEDIT_LINE_FLAGS_HPP
#define COEDIT_LINE_FLAGS_HPP

#include <cstdint>


namespace coedit {
namespace core {


enum class line_flags : std::uint8_t
{
    CHARACTER_BUFFER_ASSOCIATED = 0x1,
    ASSOCIATED_WITH_ORIG_FILE = 0x2,
    ASSOCIATED_WITH_SWAP_FILE = 0x4,
};


}
}


#endif
