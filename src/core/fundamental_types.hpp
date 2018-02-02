//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_FUNDAMENTAL_TYPES_HPP
#define COEDIT_CORE_FUNDAMENTAL_TYPES_HPP

#include <cstdint>

#define EMPTY (~0u)


namespace coedit {
namespace core {


using lid_t = std::uint64_t;
using loffset_t = std::uint64_t;
using cbid_t = std::uint64_t;
using cboffset_t = std::uint64_t;


}
}


#endif
