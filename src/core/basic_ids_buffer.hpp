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
        std::size_t IDS_BUFFER_CACHE_SIZE,
        std::size_t IDS_BUFFER_SIZE
>
class basic_ids_buffer
{
public:
    basic_ids_buffer()
            : buf_()
            , idsbid_()
    {
    }
    
    basic_ids_buffer(idsbid_t idsbid)
            : buf_()
            , idsbid_(idsbid)
    {
    }
    
    basic_ids_buffer(const stdfs::path& idsb_path)
            : buf_()
            , idsbid_()
    {
        std::ifstream ifs;
        
        ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ifs.open(idsb_path, std::ios::in | std::ios::binary);
        
        ifs.read((char*)&idsbid_, sizeof(idsbid_));
        ifs.read((char*)buf_, sizeof(std::uint8_t) * IDS_BUFFER_SIZE);
        
        ifs.close();
    }
    
    inline basic_ids_buffer& set(idsboffset_t byte_trg, std::uint8_t bit_trg) noexcept
    {
        buf_[byte_trg] |= (1 << bit_trg);
        
        return *this;
    }
    
    inline basic_ids_buffer& erase(idsboffset_t byte_trg, std::uint8_t bit_trg) noexcept
    {
        buf_[byte_trg] &= ~(1 << bit_trg);
        
        return *this;
    }
    
    inline basic_ids_buffer& clear(idsboffset_t byte_trg) noexcept
    {
        buf_[byte_trg] = 0;
        
        return *this;
    }
    
    inline bool is_set(idsboffset_t byte_trg, std::uint8_t bit_trg) const noexcept
    {
        return (buf_[byte_trg] & (1 << bit_trg)) != 0;
    }
    
    inline bool is_empty(idsboffset_t byte_trg) const noexcept
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
    
    void store(const stdfs::path& idsb_path) const
    {
        std::ofstream ofs;
        
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofs.open(idsb_path, std::ios::out | std::ios::binary);
        
        ofs.write((const char*)&idsbid_, sizeof(idsbid_));
        ofs.write((const char*)&buf_, sizeof(std::uint8_t) * IDS_BUFFER_SIZE);
        
        ofs.close();
    }
    
    cbid_t get_idsbid() const noexcept
    {
        return idsbid_;
    }
    
private:
    std::uint8_t buf_[IDS_BUFFER_SIZE];
    
    idsbid_t idsbid_;
};


}
}


#endif
