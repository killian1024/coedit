//
// Created by Killian on 10/03/18.
//

#ifndef COEDIT_CORE_BASIC_ID_BUFFER_HPP
#define COEDIT_CORE_BASIC_ID_BUFFER_HPP

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <cstdint>
#include <cstdlib>
#include <experimental/filesystem>
#include <fstream>

#include "core_exception.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


namespace stdfs = std::experimental::filesystem;


template<
        std::size_t ID_BUFFER_SIZE,
        std::size_t ID_BUFFER_CACHE_SIZE
>
class basic_id_buffer
{
public:
    basic_id_buffer()
            : buf_(nullptr)
            , idbid_(EMPTY)
    {
    }
    
    basic_id_buffer(idbid_t idbid)
            : buf_()
            , idbid_(idbid)
    {
        allocate_memory();
    }
    
    basic_id_buffer(const stdfs::path& idb_path)
            : buf_()
            , idbid_()
    {
        std::ifstream ifs;
    
        allocate_memory();
        ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ifs.open(idb_path, std::ios::in | std::ios::binary);
        
        ifs.read((char*)&idbid_, sizeof(idbid_));
        ifs.read((char*)buf_, sizeof(std::uint8_t) * ID_BUFFER_SIZE);
        
        ifs.close();
    }
    
    basic_id_buffer(const basic_id_buffer& rhs) = delete;
    
    basic_id_buffer(basic_id_buffer&& rhs) = delete;
    
    ~basic_id_buffer()
    {
        if (buf_ != nullptr)
        {
            free(buf_);
            buf_ = nullptr;
        }
    }
    
    basic_id_buffer& operator =(const basic_id_buffer& rhs)
    {
        if (this != &rhs)
        {
            if (buf_ == nullptr)
            {
                allocate_memory();
            }
            
            memcpy(buf_, rhs.buf_, ID_BUFFER_SIZE);
            idbid_ = rhs.idbid_;
        }
        
        return *this;
    }
    
    basic_id_buffer& operator =(basic_id_buffer&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(buf_, rhs.buf_);
            std::swap(idbid_, rhs.idbid_);
        }
        
        return *this;
    }
    
    inline basic_id_buffer& set(idboffset_t byte_trg, std::uint8_t bit_trg) noexcept
    {
        buf_[byte_trg] |= (1 << bit_trg);
        
        return *this;
    }
    
    inline basic_id_buffer& erase(idboffset_t byte_trg, std::uint8_t bit_trg) noexcept
    {
        buf_[byte_trg] &= ~(1 << bit_trg);
        
        return *this;
    }
    
    inline basic_id_buffer& clear(idboffset_t byte_trg) noexcept
    {
        buf_[byte_trg] = 0;
        
        return *this;
    }
    
    inline bool is_set(idboffset_t byte_trg, std::uint8_t bit_trg) const noexcept
    {
        return (buf_[byte_trg] & (1 << bit_trg)) != 0;
    }
    
    inline bool is_empty(idboffset_t byte_trg) const noexcept
    {
        return buf_[byte_trg] == 0;
    }
    
    inline bool is_empty() const noexcept
    {
        for (size_t i = 0; i < ID_BUFFER_SIZE; ++i)
        {
            if (buf_[i] != 0)
            {
                return false;
            }
        }
        
        return true;
    }
    
    void store(const stdfs::path& idb_path) const
    {
        std::ofstream ofs;
        
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofs.open(idb_path, std::ios::out | std::ios::binary);
        
        ofs.write((const char*)&idbid_, sizeof(idbid_));
        ofs.write((const char*)&buf_, sizeof(std::uint8_t) * ID_BUFFER_SIZE);
        
        ofs.close();
    }
    
    cbid_t get_idbid() const noexcept
    {
        return idbid_;
    }

private:
    // todo : Implement an align allocator and give it to this class.
    void allocate_memory()
    {
        std::size_t page_sze = ID_BUFFER_SIZE;
        
        if (page_sze < sizeof(void*))
        {
            page_sze = sizeof(void*);
        }
        
        if (posix_memalign((void**)&buf_, page_sze, page_sze) != 0)
        {
            throw bad_allocation_exception();
        }
        
        memset(buf_, 0, page_sze);
    }
    
private:
    std::uint8_t* buf_;
    
    idbid_t idbid_;
};


}
}


#endif
