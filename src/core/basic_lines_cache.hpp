//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINES_CACHE_HPP
#define COEDIT_CORE_BASIC_LINES_CACHE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer_cache.hpp"
#include "basic_line.hpp"
#include "core_exception.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t LINES_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE,
        typename TpAllocator
>
class basic_lines_cache
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using characters_buffer_cache_type = basic_characters_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using line_type = basic_line<TpChar, CHARACTERS_BUFFER_CACHE_SIZE, CHARACTERS_BUFFER_SIZE>;
    
    using lines_cache_type = kcontain::buffer_cache<lid_t, line_type, LINES_CACHE_SIZE>;
    
    using iterator = typename lines_cache_type::iterator;
    
    using const_iterator = typename lines_cache_type::const_iterator;
    
    basic_lines_cache()
            : lnes_cache_()
            , chars_buf_cache_()
    {
    }
    
    inline iterator begin() noexcept
    {
        return lnes_cache_.begin();
    }
    
    inline const_iterator cbegin() const noexcept
    {
        return lnes_cache_.cbegin();
    }
    
    inline iterator end() noexcept
    {
        return lnes_cache_.end();
    }
    
    inline const_iterator cend() const noexcept
    {
        return lnes_cache_.cend();
    }
    
    void load_line(lid_t* lid)
    {
        if (*lid == EMPTY)
        {
            *lid = get_new_lid();
            insert_in_cache(*lid, line_type(*lid, EMPTY, EMPTY, EMPTY, 0, &chars_buf_cache_));
        }
    }
    
    void insert_character(char_type ch, lid_t lid, loffset_t loffset)
    {
        iterator it = find_in_cache(lid);
        
        if (!it.end())
        {
            it->insert_character(ch, loffset);
        }
        else
        {
            throw invalid_lid_exception();
        }
    }

private:
    lid_t get_new_lid()
    {
        static lid_t old_lid = EMPTY;
        lid_t new_lid = old_lid;
        iterator it;
    
        do
        {
            ++new_lid;
            
            if (new_lid == EMPTY)
            {
                ++new_lid;
            }
            
            it = find_in_cache(new_lid);
        
        } while (new_lid != old_lid && !it.end());
    
        if (new_lid == old_lid)
        {
            throw length_exception();
        }
        
        old_lid = new_lid;
        
        return new_lid;
    }
    
    iterator find_in_cache(lid_t ky) noexcept
    {
        if (ky == EMPTY)
        {
            return lnes_cache_.end();
        }
        
        return lnes_cache_.find(ky);
    }
    
    template<typename TpValue_>
    void insert_in_cache(lid_t ky, TpValue_&& val)
    {
        if (ky == EMPTY)
        {
            throw invalid_lid_exception();
        }
        
        lnes_cache_.insert(ky, std::forward<TpValue_>(val));
    }
    
private:
    lines_cache_type lnes_cache_;
    
    characters_buffer_cache_type chars_buf_cache_;
};


}
}


#endif
