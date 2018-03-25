//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_FUNDAMENTAL_TYPES_HPP
#define COEDIT_CORE_FUNDAMENTAL_TYPES_HPP

#include <cstdint>


namespace coedit {
namespace core {


enum
{
    EMPTY = (~0ull),
    LF = (0xA),
    CR = (0xD),
};


using eid_t = std::uint64_t;
using lid_t = std::uint64_t;
using loffset_t = std::uint64_t;
using coffset_t = std::uint64_t;
using cbid_t = std::uint64_t;
using cboffset_t = std::uint64_t;
using idbid_t = std::uint64_t;
using idboffset_t = std::uint64_t;


}
}


#endif
