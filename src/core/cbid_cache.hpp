//
// Created by Killian on 06/03/18.
//

#ifndef COEDIT_CORE_CBID_CACHE_HPP
#define COEDIT_CORE_CBID_CACHE_HPP

#include <array>
#include <cstdint>

#include <kboost/kboost.hpp>

#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<std::size_t CBID_BUFFER_CACHE_SIZE, std::size_t CBID_BUFFER_SIZE>
class cbid_cache
{
public:
    using buffer_type = std::array<std::uint8_t, CBID_BUFFER_SIZE>;
    
    using cache_type = kcontain::static_cache<
            std::size_t,
            buffer_type,
            CBID_BUFFER_CACHE_SIZE
    >;
    
    using iterator = typename cache_type::iterator;
    
    using const_iterator = typename cache_type::const_iterator;
    
    inline iterator begin() noexcept
    {
        return cche_.begin();
    }
    
    inline const_iterator cbegin() const noexcept
    {
        return cche_.cbegin();
    }
    
    inline iterator end() noexcept
    {
        return cche_.end();
    }
    
    inline const_iterator cend() const noexcept
    {
        return cche_.cend();
    }
    
    void insert(cbid_t cbid)
    {
        std::size_t buffer_trg = cbid / CBID_BUFFER_SIZE;
        std::size_t byte_trg = (cbid / 8) % CBID_BUFFER_SIZE;
        std::uint8_t bit_trg = cbid % 8;
        
        // HERE : continue
    }

private:
    cache_type cche_;
};


}
}


#endif
