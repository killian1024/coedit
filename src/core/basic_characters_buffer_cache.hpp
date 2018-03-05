//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_CACHE_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_CACHE_HPP

#include <experimental/filesystem>
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
    
    using cache_type = kcontain::static_cache<
            lid_t,
            characters_buffer_type,
            CHARACTERS_BUFFER_CACHE_SIZE
    >;
    
    using iterator = typename cache_type::iterator;
    
    using const_iterator = typename cache_type::const_iterator;
    
    basic_characters_buffer_cache(eid_t editr_id)
            : cche_()
            , editr_id_(editr_id)
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
    
    iterator find(cbid_t ky) noexcept
    {
        if (ky == EMPTY)
        {
            return cche_.end();
        }
        
        return cche_.find(ky);
    }
    
    iterator find_and_lock(cbid_t ky) noexcept
    {
        if (ky == EMPTY)
        {
            return cche_.end();
        }
        
        return cche_.find_and_lock(ky);
    }
    
    template<typename TpValue_>
    void insert(lid_t ky, TpValue_&& val)
    {
        if (ky == EMPTY)
        {
            throw invalid_cbid_exception();
        }
        
        if (!cche_.is_least_recently_used_free())
        {
            store_character_buffer(cche_.get_least_recently_used_value());
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
    
    characters_buffer_type& get_character_buffer_and_lock(cbid_t cbid)
    {
        iterator it = find_and_lock(cbid);
        
        if (!it.end())
        {
            return *it;
        }
        else
        {
            throw invalid_cbid_exception();
        }
    }
    
    void unlock_character_buffer(cbid_t cbid)
    {
        iterator it = find(cbid);
        
        if (!it.end())
        {
            unlock(it);
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
        iterator it = end();
        
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
    void store_character_buffer(characters_buffer_type& cb)
    {
        namespace stdfs = std::experimental::filesystem;
        
        stdfs::path tmp_path = ksys::get_tmp_path();
        
        tmp_path.append("coedit-");
        tmp_path.concat(std::to_string(ksys::get_pid()));
        tmp_path.concat("-");
        tmp_path.concat(std::to_string(editr_id_));
        tmp_path.concat("-");
        tmp_path.concat(std::to_string(cb.get_cbid()));
        
        
    }

private:
    cache_type cche_{};
    
    eid_t editr_id_{};
};


}
}


#endif
