//
// Created by Killian on 06/03/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_IDS_BUFFER_CACHE_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_IDS_BUFFER_CACHE_HPP

#include <array>
#include <fstream>
#include <cstdint>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer_ids_buffer.hpp"
#include "core_exception.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


namespace stdfs = std::experimental::filesystem;


template<
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_SIZE
>
class basic_characters_buffer_ids_buffer_cache
{
public:
    using characters_buffer_ids_buffer_type = basic_characters_buffer_ids_buffer<
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE
    >;
    
    using cache_type = kcontain::static_cache<
            cbidsbid_t,
            characters_buffer_ids_buffer_type,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE
    >;
    
    using iterator = typename cache_type::iterator;
    
    using const_iterator = typename cache_type::const_iterator;
    
    basic_characters_buffer_ids_buffer_cache(eid_t editr_id)
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
    
    iterator find(cbidsbid_t cbidsbid)
    {
        iterator it = cche_.find(cbidsbid);
        
        if (it.end())
        {
            if (try_load_characters_buffer_ids(cbidsbid))
            {
                it = cche_.find(cbidsbid);
            }
        }
        
        return it;
    }
    
    iterator find_and_lock(cbidsbid_t cbidsbid)
    {
        iterator it = cche_.find_and_lock(cbidsbid);
        
        if (it.end())
        {
            if (try_load_characters_buffer_ids(cbidsbid))
            {
                it = cche_.find_and_lock(cbidsbid);
            }
        }
        
        return it;
    }
    
    template<typename TpValue_>
    iterator insert(cbidsbid_t cbidsbid, TpValue_&& val)
    {
        if (!cche_.is_least_recently_used_free())
        {
            store_characters_buffer_ids(cche_.get_least_recently_used_value());
        }
        
        return cche_.insert(cbidsbid, std::forward<TpValue_>(val));
    }
    
    void set(cbid_t cbid)
    {
        if (cbid == EMPTY)
        {
            throw invalid_cbid_exception();
        }
    
        cbidsbid_t cbidsbid = get_buffer_target(cbid);
        iterator it = find(cbidsbid);
        
        if (it.end())
        {
            it = insert(cbidsbid, characters_buffer_ids_buffer_type(cbidsbid));
        }
        
        it->set(get_byte_target(cbid), get_bit_target(cbid));
    }
    
    bool is_set(cbid_t cbid)
    {
        if (cbid == EMPTY)
        {
            throw invalid_cbid_exception();
        }
        
        cbidsbid_t cbidsbid = get_buffer_target(cbid);
        iterator it = find(cbidsbid);
        
        return it.end() ? false : it->is_set(get_byte_target(cbid), get_bit_target(cbid));
    }

private:
    inline cbidsbid_t get_buffer_target(cbid_t cbid)
    {
        return cbid / (CHARACTERS_BUFFER_IDS_BUFFER_SIZE * 8);
    }
    
    inline cbidsboffset_t get_byte_target(cbid_t cbid)
    {
        return static_cast<std::uint32_t>((cbid / 8) % CHARACTERS_BUFFER_IDS_BUFFER_SIZE);
    }
    
    inline std::uint8_t get_bit_target(cbid_t cbid)
    {
        return static_cast<std::uint8_t>(cbid % 8);
    }
    
    stdfs::path get_characters_buffer_ids_path(cbidsbid_t cbidsbid)
    {
        stdfs::path cbidsb_path = ".";
        
        cbidsb_path.append("coedit-");
        cbidsb_path.concat(std::to_string(ksys::get_pid()));
        cbidsb_path.concat("-");
        cbidsb_path.concat(std::to_string(editr_id_));
        cbidsb_path.concat("-cbidsb-");
        cbidsb_path.concat(std::to_string(cbidsbid));
        
        return cbidsb_path;
    }
    
    void store_characters_buffer_ids(characters_buffer_ids_buffer_type& cbidsb)
    {
        cbidsb.store(get_characters_buffer_ids_path(cbidsb.get_cbidsbid()));
    }
    
    void load_characters_buffer_ids(cbidsbid_t cbidsbid)
    {
        stdfs::path cbidsb_path = get_characters_buffer_ids_path(cbidsbid);
        insert(cbidsbid, characters_buffer_ids_buffer_type(cbidsb_path));
        remove(cbidsb_path.c_str());
    }
    
    bool try_load_characters_buffer_ids(cbidsbid_t cbidsbid)
    {
        stdfs::path cbidsb_path = get_characters_buffer_ids_path(cbidsbid);
        
        if (stdfs::exists(cbidsb_path))
        {
            insert(cbidsbid, characters_buffer_ids_buffer_type(cbidsb_path));
            remove(cbidsb_path.c_str());
            
            return true;
        }
        else
        {
            return false;
        }
    }

private:
    cache_type cche_;
    
    eid_t editr_id_;
};


}
}


#endif
