//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_CACHE_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_CACHE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer.hpp"
#include "core_exception.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE
>
class basic_characters_buffer_cache
{
public:
    using char_type = TpChar;
    
    using characters_buffer_type = basic_characters_buffer<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using cache_type = kcontain::buffer_cache<
            lid_t,
            characters_buffer_type,
            CHARACTERS_BUFFER_CACHE_SIZE
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
    
    iterator find(cbid_t ky) noexcept
    {
        if (ky == EMPTY)
        {
            return cche_.end();
        }
        
        return cche_.find(ky);
    }
    
    template<typename TpValue_>
    void insert(lid_t ky, TpValue_&& val)
    {
        if (ky == EMPTY)
        {
            throw invalid_cbid_exception();
        }
        
        cche_.insert(ky, std::forward<TpValue_>(val));
    }
    
    characters_buffer_type& get_character_buffer(cbid_t cbid)
    {
        iterator it = find(cbid);
        
        if (!it.end())
        {
            return *it;
        }
        else
        {
            throw invalid_cbid_exception();
        }
    }
    
    cbid_t get_new_cbid()
    {
        static cbid_t old_cbid = EMPTY;
        cbid_t new_cbid = old_cbid;
        iterator it;
        
        do
        {
            ++new_cbid;
            
            if (new_cbid == EMPTY)
            {
                ++new_cbid;
            }
            
            it = find(new_cbid);
            
        } while (new_cbid != old_cbid && !it.end());
        
        if (new_cbid == old_cbid)
        {
            throw length_exception();
        }
    
        old_cbid = new_cbid;
        
        return new_cbid;
    }
    
private:
    cache_type cche_;
};


}
}


#endif
