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


namespace stdfs = std::experimental::filesystem;


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
            cbid_t,
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
    
    iterator find(cbid_t ky)
    {
        if (ky == EMPTY)
        {
            return cche_.end();
        }
        
        iterator it = cche_.find(ky);
        
        if (it.end())
        {
            load_cb(ky);
            it = cche_.find(ky);
        }
        
        return it;
    }
    
    iterator find_and_lock(cbid_t ky)
    {
        if (ky == EMPTY)
        {
            return cche_.end();
        }
        
        iterator it = cche_.find_and_lock(ky);
        
        if (it.end())
        {
            load_cb(ky);
            it = cche_.find_and_lock(ky);
        }
        
        return it;
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
            store_cb(cche_.get_least_recently_used_value());
        }
        
        cche_.insert(ky, std::forward<TpValue_>(val));
    }
    
    characters_buffer_type& get_cb(cbid_t cbid)
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
    
    characters_buffer_type& get_cb_and_lock(cbid_t cbid)
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
    
    void unlock_cb(cbid_t cbid)
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
    stdfs::path get_cb_path(cbid_t cbid)
    {
        //stdfs::path tmp_path = ksys::get_tmp_path();
        stdfs::path cb_path = ".";
        
        cb_path.append("coedit-");
        cb_path.concat(std::to_string(ksys::get_pid()));
        cb_path.concat("-");
        cb_path.concat(std::to_string(editr_id_));
        cb_path.concat("-");
        cb_path.concat(std::to_string(cbid));
        
        return cb_path;
    }
    
    void store_cb(characters_buffer_type& cb)
    {
        cb.store_buffer(get_cb_path(cb.get_cbid()));
    }
    
    void load_cb(cbid_t ky)
    {
        // todo : implement bit-set for available cbid's
        try
        {
            stdfs::path cb_path = get_cb_path(ky);
            insert(ky, characters_buffer_type(cb_path, this));
            remove(cb_path.c_str());
        }
        catch (...)
        {
        }
    }

private:
    cache_type cche_;
    
    eid_t editr_id_;
};


}
}


#endif
