//
// Created by Killian on 10/03/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_IDS_BUFFER_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_IDS_BUFFER_HPP

#include <cstdint>
#include <experimental/filesystem>
#include <fstream>

#include "fundamental_types.hpp"


namespace coedit {
namespace core {


namespace stdfs = std::experimental::filesystem;


template<
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_SIZE
>
class basic_characters_buffer_ids_buffer
{
public:
    basic_characters_buffer_ids_buffer()
            : buf_()
            , cbidsbid_()
    {
    }
    
    basic_characters_buffer_ids_buffer(cbidsbid_t cbidsbid)
            : buf_()
            , cbidsbid_(cbidsbid)
    {
    }
    
    basic_characters_buffer_ids_buffer(const stdfs::path& cbidsb_path)
            : buf_()
            , cbidsbid_()
    {
        std::ifstream ifs;
        
        ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ifs.open(cbidsb_path, std::ios::in | std::ios::binary);
        
        ifs.read((char*)&cbidsbid_, sizeof(cbidsbid_));
        ifs.read((char*)buf_, sizeof(std::uint8_t) * CHARACTERS_BUFFER_IDS_BUFFER_SIZE);
        
        ifs.close();
    }
    
    inline basic_characters_buffer_ids_buffer& set(
            cbidsboffset_t byte_trg,
            std::uint8_t bit_trg
    ) noexcept
    {
        buf_[byte_trg] |= (1 << bit_trg);
        
        return *this;
    }
    
    inline basic_characters_buffer_ids_buffer& erase(
            cbidsboffset_t byte_trg,
            std::uint8_t bit_trg
    ) noexcept
    {
        buf_[byte_trg] &= ~(1 << bit_trg);
        
        return *this;
    }
    
    inline basic_characters_buffer_ids_buffer& clear(cbidsboffset_t byte_trg) noexcept
    {
        buf_[byte_trg] = 0;
        
        return *this;
    }
    
    inline bool is_set(cbidsboffset_t byte_trg, std::uint8_t bit_trg) const noexcept
    {
        return (buf_[byte_trg] & (1 << bit_trg)) != 0;
    }
    
    inline bool is_empty(cbidsboffset_t byte_trg) const noexcept
    {
        return buf_[byte_trg] == 0;
    }
    
    void store(const stdfs::path& cbidsb_path) const
    {
        std::ofstream ofs;
        
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofs.open(cbidsb_path, std::ios::out | std::ios::binary);
        
        ofs.write((const char*)&cbidsbid_, sizeof(cbidsbid_));
        ofs.write((const char*)&buf_, sizeof(std::uint8_t) * CHARACTERS_BUFFER_IDS_BUFFER_SIZE);
        
        ofs.close();
    }
    
    cbid_t get_cbidsbid() const noexcept
    {
        return cbidsbid_;
    }
    
private:
    std::uint8_t buf_[CHARACTERS_BUFFER_IDS_BUFFER_SIZE];
    
    cbidsbid_t cbidsbid_;
};


}
}


#endif
