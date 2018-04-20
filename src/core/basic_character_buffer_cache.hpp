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
#include "basic_id_buffer_cache.hpp"
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
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_character_buffer;


template<
        typename TpChar,
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_character_buffer_cache
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using character_buffer_type = basic_character_buffer<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    using character_buffer_cache_type = kcontain::static_cache<
            cbid_t,
            character_buffer_type,
            CHARACTER_BUFFER_CACHE_SIZE
    >;
    
    using character_buffer_id_buffer_cache_type = basic_id_buffer_cache<
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE
    >;
    
    using iterator = typename character_buffer_cache_type::iterator;
    
    using const_iterator = typename character_buffer_cache_type::const_iterator;
    
    basic_character_buffer_cache(eid_t editr_id)
            : cb_cache_()
            , cbidb_cache_(editr_id, "cbidb")
            , eid_(editr_id)
            , swap_usd_(false)
            , old_cbid_(EMPTY)
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
    
    basic_character_buffer_cache& operator =(const basic_character_buffer_cache& rhs)
    {
        if (this != &rhs)
        {
            eid_ = rhs.eid_;
            swap_usd_ = rhs.swap_usd_;
            old_cbid_ = rhs.old_cbid_;
        }
    
        return *this;
    }
    
    basic_character_buffer_cache& operator =(basic_character_buffer_cache&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(eid_, rhs.eid_);
            std::swap(swap_usd_, rhs.swap_usd_);
            std::swap(old_cbid_, rhs.old_cbid_);
        }
    
        return *this;
    }
    
    inline iterator begin() noexcept
    {
        return cb_cache_.begin();
    }
    
    inline const_iterator cbegin() const noexcept
    {
        return cb_cache_.cbegin();
    }
    
    inline iterator end() noexcept
    {
        return cb_cache_.end();
    }
    
    inline const_iterator cend() const noexcept
    {
        return cb_cache_.cend();
    }
    
    void lock(const_iterator& it) noexcept
    {
        cb_cache_.lock(it);
    }
    
    void lock(cbid_t cbid) noexcept
    {
        cb_cache_.lock(cbid);
    }
    
    void unlock(const_iterator& it) noexcept
    {
        cb_cache_.unlock(it);
    }
    
    void unlock(cbid_t cbid) noexcept
    {
        cb_cache_.unlock(cbid);
    }
    
    iterator find(cbid_t cbid)
    {
        if (cbid == EMPTY)
        {
            return cb_cache_.end();
        }
        
        iterator it = cb_cache_.find(cbid);
        
        if (it.end() && swap_usd_)
        {
            if (try_load_character_buffer(cbid))
            {
                it = cb_cache_.find(cbid);
            }
        }
        
        return it;
    }
    
    iterator find_and_lock(cbid_t cbid)
    {
        iterator it = find(cbid);
        
        if (!it.end())
        {
            lock(it);
        }
    
        return it;
    }
    
    iterator insert_first_character_buffer()
    {
        cbid_t new_cbid = get_new_cbid();
        
        return insert_character_buffer_in_cache(
                new_cbid, character_buffer_type(new_cbid, this));
    }
    
    iterator insert_character_buffer_after(cbid_t prev)
    {
        cbid_t new_cbid = get_new_cbid();
        
        return insert_character_buffer_in_cache(
                new_cbid, character_buffer_type(new_cbid, prev, this));
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
    
private:
    cbid_t get_new_cbid()
    {
        cbid_t new_cbid = old_cbid_;
        iterator it;
        
        do
        {
            ++new_cbid;
            
            if (new_cbid == EMPTY)
            {
                ++new_cbid;
            }
            
            it = find(new_cbid);
            
        } while (new_cbid != old_cbid_ && !it.end());
        
        if (new_cbid == old_cbid_)
        {
            throw length_exception();
        }
    
        old_cbid_ = new_cbid;
        
        return new_cbid;
    }
    
    template<typename TpValue_>
    iterator insert_character_buffer_in_cache(cbid_t cbid, TpValue_&& val)
    {
        if (cbid == EMPTY)
        {
            throw invalid_cbid_exception();
        }
        
        if (!cb_cache_.is_least_recently_used_free())
        {
            store_character_buffer(cb_cache_.get_least_recently_used());
        }
        
        cbidb_cache_.set(cbid);
        
        return cb_cache_.insert(cbid, std::forward<TpValue_>(val));
    }
    
    stdfs::path get_character_buffer_base_path()
    {
        stdfs::path cb_path = ".";
        
        cb_path.append("coedit-");
        cb_path.concat(std::to_string(ksys::get_pid()));
        cb_path.concat("-");
        cb_path.concat(std::to_string(eid_));
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
        insert_character_buffer_in_cache(cbid, character_buffer_type(cb_path, this));
        remove(cb_path.c_str());
    }
    
    bool try_load_character_buffer(cbid_t cbid)
    {
        if (cbidb_cache_.is_set(cbid))
        {
            stdfs::path cb_path = get_character_buffer_path(cbid);
            insert_character_buffer_in_cache(cbid, character_buffer_type(cb_path, this));
            remove(cb_path.c_str());
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    character_buffer_cache_type cb_cache_;
    
    character_buffer_id_buffer_cache_type cbidb_cache_;
    
    eid_t eid_;
    
    bool swap_usd_;
    
    cbid_t old_cbid_;
};


}
}


#endif
