//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_HPP

#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE
>
class basic_characters_buffer
{
public:
    using char_type = TpChar;
    
    using characters_buffer_type = basic_characters_buffer<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using characters_buffer_cache_type = kcontain::buffer_cache<
            lid_t,
            characters_buffer_type,
            CHARACTERS_BUFFER_CACHE_SIZE
    >;
    
    basic_characters_buffer()
            : buf_()
            , cbid_(EMPTY)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , cur_size_(0)
            , chars_buf_cache_(nullptr)
    {
    }
    
    basic_characters_buffer(
            cbid_t cbid,
            cbid_t prev,
            cbid_t nxt,
            characters_buffer_cache_type* chars_buf_cache
    )
            : buf_()
            , cbid_(cbid)
            , prev_(prev)
            , nxt_(nxt)
            , cur_size_(0)
            , chars_buf_cache_(chars_buf_cache)
    {
    }
    
    void insert_character(char_type ch, cboffset_t cboffset, loffset_t loffset)
    {
        auto offset = cboffset + loffset;
        
        if (offset < cur_size_)
        {
            memcpy((buf_ + offset + 1), (buf_ + offset), (cur_size_ - offset) * sizeof(char_type));
        }
        
        buf_[offset] = ch;
        ++cur_size_;
    }

private:
    char_type buf_[CHARACTERS_BUFFER_CACHE_SIZE];
    
    cbid_t cbid_;
    
    cbid_t prev_;
    
    cbid_t nxt_;
    
    cboffset_t cur_size_;
    
    characters_buffer_cache_type* chars_buf_cache_;
};


}
}


#endif
