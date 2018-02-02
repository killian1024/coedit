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
    
    using characters_buffer_cache_type = kcontain::buffer_cache<
            lid_t,
            characters_buffer_type,
            CHARACTERS_BUFFER_CACHE_SIZE
    >;
    
    using iterator = typename characters_buffer_cache_type::iterator;
    
    using const_iterator = typename characters_buffer_cache_type::const_iterator;
    
    inline iterator begin() noexcept
    {
        return chars_buf_cache_.begin();
    }
    
    inline const_iterator cbegin() const noexcept
    {
        return chars_buf_cache_.cbegin();
    }
    
    inline iterator end() noexcept
    {
        return chars_buf_cache_.end();
    }
    
    inline const_iterator cend() const noexcept
    {
        return chars_buf_cache_.cend();
    }
    
    void load_characters_buffer(cbid_t* cbid, cboffset_t* cboffset)
    {
        if (*cbid == EMPTY)
        {
            *cbid = get_new_cbid();
            *cboffset = 0;
            insert_in_cache(*cbid, characters_buffer_type(*cbid, EMPTY, EMPTY, &chars_buf_cache_));
        }
    }
    
    void insert_character(char_type ch, cbid_t cbid, cboffset_t cboffset, loffset_t loffset)
    {
        iterator it = find_in_cache(cbid);
        
        if (!it.end())
        {
            it->insert_character(ch, cboffset, loffset);
        }
        else
        {
            throw invalid_cbid_exception();
        }
    }

private:
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
            
            it = find_in_cache(new_cbid);
            
        } while (new_cbid != old_cbid && !it.end());
        
        if (new_cbid == old_cbid)
        {
            throw length_exception();
        }
    
        old_cbid = new_cbid;
        
        return new_cbid;
    }
    
    iterator find_in_cache(cbid_t ky) noexcept
    {
        if (ky == EMPTY)
        {
            return chars_buf_cache_.end();
        }
        
        return chars_buf_cache_.find(ky);
    }
    
    template<typename TpValue_>
    void insert_in_cache(lid_t ky, TpValue_&& val)
    {
        if (ky == EMPTY)
        {
            throw invalid_cbid_exception();
        }
        
        chars_buf_cache_.insert(ky, std::forward<TpValue_>(val));
    }
    
private:
    characters_buffer_cache_type chars_buf_cache_;
};


}
}


#endif
