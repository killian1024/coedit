//
// Created by Killian on 06/03/18.
//

#ifndef COEDIT_CORE_BASIC_ID_BUFFER_CACHE_HPP
#define COEDIT_CORE_BASIC_ID_BUFFER_CACHE_HPP

#include <array>
#include <cstdint>
#include <fstream>
#include <regex>

#include <kboost/kboost.hpp>

#include "basic_id_buffer.hpp"
#include "core_exception.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


namespace stdfs = std::experimental::filesystem;


template<
        std::size_t ID_BUFFER_SIZE,
        std::size_t ID_BUFFER_CACHE_SIZE
>
class basic_id_buffer_cache
{
public:
    using id_buffer_type = basic_id_buffer<
            ID_BUFFER_SIZE,
            ID_BUFFER_CACHE_SIZE
    >;
    
    using cache_type = kcontain::static_cache<
            idbid_t,
            id_buffer_type,
            ID_BUFFER_CACHE_SIZE
    >;
    
    using iterator = typename cache_type::iterator;
    
    using const_iterator = typename cache_type::const_iterator;
    
    basic_id_buffer_cache(eid_t eid, std::string file_id)
            : cche_()
            , eid_(eid)
            , file_id_(std::move(file_id))
            , swap_usd_(false)
    {
    }
    
    ~basic_id_buffer_cache() noexcept
    {
        if (swap_usd_)
        {
            std::string rgx_str;
            rgx_str += "^";
            rgx_str += get_id_buffer_base_path().filename();
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
    
    void lock(const_iterator& it) noexcept
    {
        cche_.lock(it);
    }
    
    void lock(idbid_t idbid)
    {
        cche_.lock(idbid);
    }
    
    void unlock(const_iterator& it) noexcept
    {
        cche_.unlock(it);
    }
    
    void unlock(idbid_t idbid)
    {
        cche_.unlock(idbid);
    }
    
    iterator find(idbid_t idbid)
    {
        iterator it = cche_.find(idbid);
        
        if (it.end() && swap_usd_)
        {
            if (try_load_id_buffer(idbid))
            {
                it = cche_.find(idbid);
            }
        }
        
        return it;
    }
    
    iterator find_and_lock(idbid_t idbid)
    {
        iterator it = find(idbid);
        lock(it);
    
        return it;
    }
    
    template<typename TpValue_>
    iterator insert(idbid_t idbid, TpValue_&& val)
    {
        if (!cche_.is_least_recently_used_free() &&
            !cche_.get_least_recently_used().is_empty())
        {
            store_id_buffer(cche_.get_least_recently_used());
        }
        
        return cche_.insert(idbid, std::forward<TpValue_>(val));
    }
    
    void set(std::size_t id)
    {
        if (id == EMPTY)
        {
            throw invalid_cbid_exception();
        }
        
        idbid_t idbid = get_buffer_target(id);
        iterator it = find(idbid);
        
        if (it.end())
        {
            it = insert(idbid, id_buffer_type(idbid));
        }
        
        it->set(get_byte_target(id), get_bit_target(id));
    }
    
    bool is_set(std::size_t id)
    {
        if (id == EMPTY)
        {
            throw invalid_cbid_exception();
        }
        
        idbid_t cbidsbid = get_buffer_target(id);
        iterator it = find(cbidsbid);
        
        return it.end() ? false : it->is_set(get_byte_target(id), get_bit_target(id));
    }

private:
    inline idbid_t get_buffer_target(std::size_t id)
    {
        return id / (ID_BUFFER_SIZE * 8);
    }
    
    inline idboffset_t get_byte_target(std::size_t id)
    {
        return static_cast<std::uint32_t>((id / 8) % ID_BUFFER_SIZE);
    }
    
    inline std::uint8_t get_bit_target(std::size_t id)
    {
        return static_cast<std::uint8_t>(id % 8);
    }
    
    stdfs::path get_id_buffer_base_path()
    {
        stdfs::path idb_path = ".";
        
        idb_path.append("coedit-");
        idb_path.concat(std::to_string(ksys::get_pid()));
        idb_path.concat("-");
        idb_path.concat(std::to_string(eid_));
        idb_path.concat("-");
        idb_path.concat(file_id_);
        idb_path.concat("-");
        
        return idb_path;
    }
    
    stdfs::path get_id_buffer_path(idbid_t idbid)
    {
        stdfs::path idb_path = get_id_buffer_base_path();
        
        idb_path.concat(std::to_string(idbid));
        
        return idb_path;
    }
    
    void store_id_buffer(id_buffer_type& idb)
    {
        idb.store(get_id_buffer_path(idb.get_idbid()));
        swap_usd_ = true;
    }
    
    void load_id_buffer(idbid_t idbid)
    {
        stdfs::path idb_path = get_id_buffer_path(idbid);
        
        insert(idbid, id_buffer_type(idb_path));
        remove(idb_path.c_str());
    }
    
    bool try_load_id_buffer(idbid_t idbid)
    {
        stdfs::path idb_path = get_id_buffer_path(idbid);
        
        if (stdfs::exists(idb_path))
        {
            insert(idbid, id_buffer_type(idb_path));
            remove(idb_path.c_str());
            
            return true;
        }
        else
        {
            if (cche_.get_least_recently_used().is_empty())
            {
                insert(idbid, id_buffer_type(idbid));
            }
            
            return false;
        }
    }

private:
    cache_type cche_;
    
    eid_t eid_;
    
    std::string file_id_;
    
    bool swap_usd_;
};


}
}


#endif
