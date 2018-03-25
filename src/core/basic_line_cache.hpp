//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINE_CACHE_HPP
#define COEDIT_CORE_BASIC_LINE_CACHE_HPP

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
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        std::size_t LINE_CACHE_SIZE,
        std::size_t LINE_ID_BUFFER_SIZE,
        std::size_t LINE_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_line_cache
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using character_buffer_cache_type = basic_character_buffer_cache<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE
    >;
    
    using line_type = basic_line<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            LINE_CACHE_SIZE,
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    using line_id_buffer_cache_type = basic_id_buffer_cache<
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE
    >;
    
    using line_cache_type = kcontain::static_cache<lid_t, line_type, LINE_CACHE_SIZE>;
    
    using iterator = typename line_cache_type::iterator;
    
    using const_iterator = typename line_cache_type::const_iterator;
    
    basic_line_cache(character_buffer_cache_type* cb_cache, eid_t eid)
            : cb_cache_(cb_cache)
            , l_cache_()
            , lidb_cache_(eid, "lidb")
            , eid_(eid)
            , swap_usd_(false)
    {
    }
    
    ~basic_line_cache() noexcept
    {
        if (swap_usd_)
        {
            std::string rgx_str;
            rgx_str += "^";
            rgx_str += get_line_base_path().filename();
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
        return l_cache_.begin();
    }
    
    inline const_iterator cbegin() const noexcept
    {
        return l_cache_.cbegin();
    }
    
    inline iterator end() noexcept
    {
        return l_cache_.end();
    }
    
    inline const_iterator cend() const noexcept
    {
        return l_cache_.cend();
    }
    
    void lock(const_iterator& it) noexcept
    {
        l_cache_.lock(it);
    }
    
    void lock(cbid_t cbid) noexcept
    {
        l_cache_.lock(cbid);
    }
    
    void unlock(const_iterator& it) noexcept
    {
        l_cache_.unlock(it);
    }
    
    void unlock(cbid_t cbid) noexcept
    {
        l_cache_.unlock(cbid);
    }
    
    iterator find(lid_t lid)
    {
        if (lid == EMPTY)
        {
            return l_cache_.end();
        }
        
        iterator it = l_cache_.find(lid);
        
        if (it.end() && swap_usd_)
        {
            if (try_load_line(lid))
            {
                it = l_cache_.find(lid);
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
    iterator insert(lid_t lid, TpValue_&& val)
    {
        if (lid == EMPTY)
        {
            throw invalid_lid_exception();
        }
        
        if (!l_cache_.is_least_recently_used_free())
        {
            store_line(l_cache_.get_least_recently_used());
        }
    
        lidb_cache_.set(lid);
        return l_cache_.insert(lid, std::forward<TpValue_>(val));
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
    
    line_type& get_line_and_lock(lid_t lid)
    {
        iterator it = find_and_lock(lid);
        
        if (!it.end())
        {
            return *it;
        }
        else
        {
            throw invalid_lid_exception();
        }
    }
    
    void unlock_line(lid_t lid)
    {
        iterator it = find(lid);
        
        if (!it.end())
        {
            unlock(it);
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
        iterator it;
        
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
    stdfs::path get_line_base_path()
    {
        stdfs::path l_path = ".";
        
        l_path.append("coedit-");
        l_path.concat(std::to_string(ksys::get_pid()));
        l_path.concat("-");
        l_path.concat(std::to_string(eid_));
        l_path.concat("-l-");
        
        return l_path;
    }
    
    stdfs::path get_line_path(lid_t lid)
    {
        stdfs::path l_path = get_line_base_path();
        l_path.concat(std::to_string(lid));
        
        return l_path;
    }
    
    void store_line(line_type& l)
    {
        l.store(get_line_path(l.get_lid()));
        swap_usd_ = true;
    }
    
    void load_line(lid_t lid)
    {
        stdfs::path l_path = get_line_path(lid);
        insert(lid, line_type(l_path, cb_cache_, this));
        remove(l_path.c_str());
    }
    
    bool try_load_line(lid_t lid)
    {
        if (lidb_cache_.is_set(lid))
        {
            stdfs::path l_path = get_line_path(lid);
            insert(lid, line_type(l_path, cb_cache_, this));
            remove(l_path.c_str());
            return true;
        }
        else
        {
            return false;
        }
    }
    
private:
    character_buffer_cache_type* cb_cache_;
    
    line_cache_type l_cache_;
    
    line_id_buffer_cache_type lidb_cache_;
    
    eid_t eid_;
    
    bool swap_usd_;
};


}
}


#endif
