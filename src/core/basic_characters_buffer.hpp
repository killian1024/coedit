//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_HPP

#include "basic_characters_buffer_cache.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE
>
class basic_characters_buffer_cache;


template<
        typename TpChar,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE
>
class basic_characters_buffer
{
public:
    using self_type = basic_characters_buffer<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using char_type = TpChar;
    
    using characters_buffer_cache_type = basic_characters_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
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
    
    void erase_character(cboffset_t cboffset, loffset_t loffset)
    {
        auto offset = cboffset + loffset;
    
        if (cur_size_ > 0)
        {
            if (offset + 1 < cur_size_)
            {
                memcpy((buf_ + offset), (buf_ + offset + 1),
                       (cur_size_ - offset - 1) * sizeof(char_type));
            }
            
            --cur_size_;
        }
    }
    
    cboffset_t compute_n_chars(cboffset_t cboffset)
    {
        cboffset_t i = 0;
        
        while (buf_[cboffset] != LF && buf_[cboffset] != CR && cboffset < cur_size_)
        {
            ++i;
            ++cboffset;
        }
        
        if (cboffset < cur_size_)
        {
            ++i;
            if (buf_[cboffset] == CR && buf_[cboffset + 1] == LF)
            {
                ++i;
            }
        }
    
        return i;
    }
    
    char_type& operator [](std::size_t i) noexcept
    {
        return buf_[i];
    }
    
    cbid_t get_cbid() const
    {
        return cbid_;
    }
    
    void set_cbid(cbid_t cbid)
    {
        cbid_ = cbid;
    }
    
    cbid_t get_prev() const
    {
        return prev_;
    }
    
    void set_prev(cbid_t prev)
    {
        prev_ = prev;
    }
    
    cbid_t get_nxt() const
    {
        return nxt_;
    }
    
    void set_nxt(cbid_t nxt)
    {
        nxt_ = nxt;
    }
    
    cboffset_t get_cur_size() const
    {
        return cur_size_;
    }
    
    void set_cur_size(cboffset_t cur_size)
    {
        cur_size_ = cur_size;
    }
    
    characters_buffer_cache_type* get_chars_buf_cache() const
    {
        return chars_buf_cache_;
    }
    
    void set_chars_buf_cache(characters_buffer_cache_type* chars_buf_cache)
    {
        chars_buf_cache_ = chars_buf_cache;
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
