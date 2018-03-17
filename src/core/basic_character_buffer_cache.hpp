//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTER_BUFFER_CACHE_HPP
#define COEDIT_CORE_BASIC_CHARACTER_BUFFER_CACHE_HPP

#include <experimental/filesystem>
#include <memory>
#include <regex>

#include <kboost/kboost.hpp>

#include "basic_character_buffer.hpp"
#include "basic_ids_buffer_cache.hpp"
#include "core_exception.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


namespace stdfs = std::experimental::filesystem;


template<
        typename TpChar,
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE
>
class basic_character_buffer_cache
{
public:
    using char_type = TpChar;
    
    using character_buffer_type = basic_character_buffer<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE
    >;
    
    using character_buffer_cache_type = kcontain::static_cache<
            cbid_t,
            character_buffer_type,
            CHARACTER_BUFFER_CACHE_SIZE
    >;
    
    using character_buffer_id_buffer_cache_type = basic_ids_buffer_cache<
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE
    >;
    
    using iterator = typename character_buffer_cache_type::iterator;
    
    using const_iterator = typename character_buffer_cache_type::const_iterator;
    
    basic_character_buffer_cache(eid_t editr_id)
            : character_buffer_cche_()
            , character_buffer_id_buffer_cche_(editr_id, "cbidsb")
            , editr_id_(editr_id)
            , swap_usd_(false)
    {
    }
    
    ~basic_character_buffer_cache() noexcept
    {
        if (swap_usd_)
        {
            std::string rgx_str;
            rgx_str += "^";
            rgx_str += get_character_buffer_base_path().filename();
            rgx_str += "[0-9]+$";
            std::regex rgx(rgx_str);
            
            for (auto& x : stdfs::directory_iterator("."))
            {
                if (stdfs::is_regular_file(x))
                {
                    if (std::regex_match(x.path().filename().c_str(), rgx))
                    {
                        remove(x.path().c_str());
                    }
                }
            }
        }
    }
    
    inline iterator begin() noexcept
    {
        return character_buffer_cche_.begin();
    }
    
    inline const_iterator cbegin() const noexcept
    {
        return character_buffer_cche_.cbegin();
    }
    
    inline iterator end() noexcept
    {
        return character_buffer_cche_.end();
    }
    
    inline const_iterator cend() const noexcept
    {
        return character_buffer_cche_.cend();
    }
    
    void lock(const_iterator& it) noexcept
    {
        character_buffer_cche_.lock(it);
    }
    
    void lock(cbid_t cbid) noexcept
    {
        character_buffer_cche_.lock(cbid);
    }
    
    void unlock(const_iterator& it) noexcept
    {
        character_buffer_cche_.unlock(it);
    }
    
    void unlock(cbid_t cbid) noexcept
    {
        character_buffer_cche_.unlock(cbid);
    }
    
    iterator find(cbid_t cbid)
    {
        if (cbid == EMPTY)
        {
            return character_buffer_cche_.end();
        }
        
        iterator it = character_buffer_cche_.find(cbid);
        
        if (it.end() && swap_usd_)
        {
            if (try_load_character_buffer(cbid))
            {
                it = character_buffer_cche_.find(cbid);
            }
        }
        
        return it;
    }
    
    iterator find_and_lock(cbid_t cbid)
    {
        iterator it = find(cbid);
        lock(it);
    
        return it;
    }
    
    template<typename TpValue_>
    iterator insert(cbid_t cbid, TpValue_&& val)
    {
        if (cbid == EMPTY)
        {
            throw invalid_cbid_exception();
        }
        
        if (!character_buffer_cche_.is_least_recently_used_free())
        {
            store_character_buffer(character_buffer_cche_.get_least_recently_used());
        }
        
        character_buffer_id_buffer_cche_.set(cbid);
        return character_buffer_cche_.insert(cbid, std::forward<TpValue_>(val));
    }
    
    character_buffer_type& get_character_buffer(cbid_t cbid)
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
    
    character_buffer_type& get_character_buffer_and_lock(cbid_t cbid)
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
    stdfs::path get_character_buffer_base_path()
    {
        stdfs::path cb_path = ".";
        
        cb_path.append("coedit-");
        cb_path.concat(std::to_string(ksys::get_pid()));
        cb_path.concat("-");
        cb_path.concat(std::to_string(editr_id_));
        cb_path.concat("-cb-");
        
        return cb_path;
    }
    
    stdfs::path get_character_buffer_path(cbid_t cbid)
    {
        stdfs::path cb_path = get_character_buffer_base_path();
        cb_path.concat(std::to_string(cbid));
        
        return cb_path;
    }
    
    void store_character_buffer(character_buffer_type& cb)
    {
        cb.store(get_character_buffer_path(cb.get_cbid()));
        swap_usd_ = true;
    }
    
    void load_character_buffer(cbid_t cbid)
    {
        stdfs::path cb_path = get_character_buffer_path(cbid);
        insert(cbid, character_buffer_type(cb_path, this));
        remove(cb_path.c_str());
    }
    
    bool try_load_character_buffer(cbid_t cbid)
    {
        if (character_buffer_id_buffer_cche_.is_set(cbid))
        {
            stdfs::path cb_path = get_character_buffer_path(cbid);
            insert(cbid, character_buffer_type(cb_path, this));
            remove(cb_path.c_str());
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    character_buffer_cache_type character_buffer_cche_;
    
    character_buffer_id_buffer_cache_type character_buffer_id_buffer_cche_;
    
    eid_t editr_id_;
    
    bool swap_usd_;
};


}
}


#endif
