//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINE_CACHE_HPP
#define COEDIT_CORE_BASIC_LINE_CACHE_HPP

#include <experimental/filesystem>
#include <memory>
#include <regex>

#include <kboost/kboost.hpp>

#include "basic_character_buffer_cache.hpp"
#include "basic_line.hpp"
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
        std::size_t LINE_CACHE_SIZE,
        std::size_t LINE_ID_BUFFER_SIZE,
        std::size_t LINE_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_line;


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
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            TpAllocator
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
            , lne_cache_()
            , lidb_cache_(eid, "lidb")
            , eid_(eid)
            , swap_usd_(false)
            , old_lid_(EMPTY)
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
    
    basic_line_cache& operator =(const basic_line_cache& rhs)
    {
        if (this != &rhs)
        {
            cb_cache_ = rhs.cb_cache_;
            lidb_cache_ = rhs.lidb_cache_;
            eid_ = rhs.eid_;
            swap_usd_ = rhs.swap_usd_;
            old_lid_ = rhs.old_lid_;
        }
    
        return *this;
    }
    
    basic_line_cache& operator =(basic_line_cache&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(cb_cache_, rhs.cb_cache_);
            lidb_cache_ = std::move(rhs.lidb_cache_);
            std::swap(eid_, rhs.eid_);
            std::swap(swap_usd_, rhs.swap_usd_);
            std::swap(old_lid_, rhs.old_lid_);
        }
    
        return *this;
    }
    
    inline iterator begin() noexcept
    {
        return lne_cache_.begin();
    }
    
    inline const_iterator cbegin() const noexcept
    {
        return lne_cache_.cbegin();
    }
    
    inline iterator end() noexcept
    {
        return lne_cache_.end();
    }
    
    inline const_iterator cend() const noexcept
    {
        return lne_cache_.cend();
    }
    
    void lock(const_iterator& it) noexcept
    {
        lne_cache_.lock(it);
    }
    
    void lock(cbid_t cbid) noexcept
    {
        lne_cache_.lock(cbid);
    }
    
    void unlock(const_iterator& it) noexcept
    {
        lne_cache_.unlock(it);
    }
    
    void unlock(cbid_t cbid) noexcept
    {
        lne_cache_.unlock(cbid);
    }
    
    iterator find(lid_t lid)
    {
        if (lid == EMPTY)
        {
            return lne_cache_.end();
        }
        
        iterator it = lne_cache_.find(lid);
        
        if (it.end() && swap_usd_)
        {
            if (try_load_line(lid))
            {
                it = lne_cache_.find(lid);
            }
        }
        
        return it;
    }
    
    iterator find_and_lock(lid_t lid)
    {
        iterator it = find(lid);
        
        if (!it.end())
        {
            lock(it);
        }
        
        return it;
    }
    
    iterator insert_first_line()
    {
        lid_t new_lid = get_new_lid();
        
        return insert_line_in_cache(new_lid, line_type(new_lid, cb_cache_, this));
    }
    
    iterator insert_line_after(lid_t lid, loffset_t loffset, newline_format newl_format)
    {
        lid_t new_lid = get_new_lid();
        line_type& current_lne = get_line(lid);
        iterator it_newline;
    
        switch (newl_format)
        {
            case newline_format::UNIX:
                current_lne.insert_character(LF, loffset);
                break;
        
            case newline_format::MAC:
                current_lne.insert_character(CR, loffset);
                break;
        
            case newline_format::WINDOWS:
                current_lne.insert_character(CR, loffset);
                ++loffset;
                current_lne.insert_character(LF, loffset);
                break;
        }
        
        it_newline = insert_line_in_cache(new_lid, line_type(new_lid, lid, cb_cache_, this));
        
        return it_newline;
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

private:
    // todo : Improve this method, the more lid are near better it will be.
    // todo : D'onst use find(), use something that doesn't load anything.
    lid_t get_new_lid()
    {
        lid_t new_lid = old_lid_;
        iterator it;
        
        do
        {
            ++new_lid;
            
            if (new_lid == EMPTY)
            {
                ++new_lid;
            }
            
            it = find(new_lid);
            
        } while (new_lid != old_lid_ && !it.end());
        
        if (new_lid == old_lid_)
        {
            throw length_exception();
        }
    
        old_lid_ = new_lid;
        
        return new_lid;
    }
    
    template<typename TpValue_>
    iterator insert_line_in_cache(lid_t lid, TpValue_&& val)
    {
        if (lid == EMPTY)
        {
            throw invalid_lid_exception();
        }
        
        if (!lne_cache_.is_least_recently_used_free())
        {
            store_line(lne_cache_.get_least_recently_used());
        }
        
        lidb_cache_.set(lid);
        
        return lne_cache_.insert(lid, std::forward<TpValue_>(val));
    }
    
    stdfs::path get_line_base_path()
    {
        stdfs::path lne_path = ".";
        
        lne_path.append("coedit-");
        lne_path.concat(std::to_string(ksys::get_pid()));
        lne_path.concat("-");
        lne_path.concat(std::to_string(eid_));
        lne_path.concat("-l-");
        
        return lne_path;
    }
    
    stdfs::path get_line_path(lid_t lid)
    {
        stdfs::path lne_path = get_line_base_path();
        lne_path.concat(std::to_string(lid));
        
        return lne_path;
    }
    
    void store_line(line_type& lne)
    {
        lne.store(get_line_path(lne.get_lid()));
        swap_usd_ = true;
    }
    
    void load_line(lid_t lid)
    {
        stdfs::path l_path = get_line_path(lid);
        insert_line_in_cache(lid, line_type(l_path, cb_cache_, this));
        remove(l_path.c_str());
    }
    
    bool try_load_line(lid_t lid)
    {
        if (lidb_cache_.is_set(lid))
        {
            stdfs::path l_path = get_line_path(lid);
            insert_line_in_cache(lid, line_type(l_path, cb_cache_, this));
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
    
    line_cache_type lne_cache_;
    
    line_id_buffer_cache_type lidb_cache_;
    
    eid_t eid_;
    
    bool swap_usd_;
    
    lid_t old_lid_;
};


}
}


#endif
