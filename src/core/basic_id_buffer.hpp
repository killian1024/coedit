//
// Created by Killian on 10/03/18.
//

#ifndef COEDIT_CORE_BASIC_ID_BUFFER_HPP
#define COEDIT_CORE_BASIC_ID_BUFFER_HPP

#include <cstdint>
#include <experimental/filesystem>
#include <fstream>

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
            : buf_()
            , idbid_()
    {
    }
    
    basic_id_buffer(idbid_t idbid)
            : buf_()
            , idbid_(idbid)
    {
    }
    
    basic_id_buffer(const stdfs::path& idb_path)
            : buf_()
            , idbid_()
    {
        std::ifstream ifs;
        
        ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ifs.open(idb_path, std::ios::in | std::ios::binary);
        
        ifs.read((char*)&idbid_, sizeof(idbid_));
        ifs.read((char*)buf_, sizeof(std::uint8_t) * ID_BUFFER_SIZE);
        
        ifs.close();
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
        for (auto& x : buf_)
        {
            if (x != 0)
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
    std::uint8_t buf_[ID_BUFFER_SIZE];
    
    idbid_t idbid_;
};


}
}


#endif
