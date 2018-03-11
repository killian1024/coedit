//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_FUNDAMENTAL_TYPES_HPP
#define COEDIT_CORE_FUNDAMENTAL_TYPES_HPP

#include <cstdint>

#define EMPTY (~0ull)
#define LF (0xA)
#define CR (0xD)


namespace coedit {
namespace core {


using eid_t = std::uint64_t;
using lid_t = std::uint64_t;
using loffset_t = std::uint64_t;
using coffset_t = std::uint64_t;
using cbid_t = std::uint64_t;
using cboffset_t = std::uint64_t;
using cbidsbid_t = std::uint64_t;
using cbidsboffset_t = std::uint64_t;


}
}


#endif
