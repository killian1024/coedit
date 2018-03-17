//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINES_CACHE_HPP
#define COEDIT_CORE_BASIC_LINES_CACHE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_character_buffer_cache.hpp"
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
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_SIZE,
        typename TpAllocator
>
class basic_lines_cache
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using characters_buffer_cache_type = basic_character_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE
    >;
    
    using line_ids_buffer_cache_type = basic_ids_buffer_cache<
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE
    >;
    
    using line_type = basic_line<
            TpChar,
            LINES_CACHE_SIZE,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE,
            TpAllocator
    >;
    
    using cache_type = kcontain::static_cache<lid_t, line_type, LINES_CACHE_SIZE>;
    
    using iterator = typename cache_type::iterator;
    
    using const_iterator = typename cache_type::const_iterator;
    
    basic_lines_cache(eid_t editr_id)
            : cche_()
            , editr_id_(editr_id)
            , swap_usd_(false)
    {
    }
    
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
    
    void lock(const_iterator it) noexcept
    {
        cche_.lock(it);
    }
    
    void unlock(const_iterator it) noexcept
    {
        cche_.unlock(it);
    }
    
    iterator find(lid_t ky) noexcept
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
            throw invalid_lid_exception();
        }
        
        cche_.insert(ky, std::forward<TpValue_>(val));
    }
    
    line_type& get_line(lid_t lid)
    {
        iterator it = find(lid);
    
        if (!it.end())
        {
            return *it;
        }
        else
        {
            throw invalid_lid_exception();
        }
    }
    
    lid_t get_new_lid()
    {
        static lid_t old_lid = EMPTY;
        lid_t new_lid = old_lid;
        iterator it = end();
        
        do
        {
            ++new_lid;
            
            if (new_lid == EMPTY)
            {
                ++new_lid;
            }
            
            it = find(new_lid);
            
        } while (new_lid != old_lid && !it.end());
        
        if (new_lid == old_lid)
        {
            throw length_exception();
        }
        
        old_lid = new_lid;
        
        return new_lid;
    }
    
private:
    cache_type cche_;
    
    characters_buffer_ids_buffer_cache_type characters_buffer_ids_buffer_cche_;
    
    eid_t editr_id_;
    
    bool swap_usd_;
};


}
}


#endif
