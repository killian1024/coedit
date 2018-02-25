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
    using characters_buffer_type = basic_characters_buffer<
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
    
    void insert_character(char_type ch, cboffset_t cboffset)
    {
        if (cur_size_ == CHARACTERS_BUFFER_SIZE)
        {
            // New buffer creation.
            cbid_t new_cbid = chars_buf_cache_->get_new_cbid();
            chars_buf_cache_->insert(new_cbid, characters_buffer_type(
                    new_cbid, cbid_, nxt_, chars_buf_cache_));
            if (nxt_ != EMPTY)
            {
                characters_buffer_type& nxt_cb = chars_buf_cache_->get_character_buffer(nxt_);
                nxt_cb.prev_ = new_cbid;
            }
            nxt_ = new_cbid;
            
            // Move the current buffer half data in the new buffer.
            characters_buffer_type& new_cb = chars_buf_cache_->get_character_buffer(new_cbid);
            constexpr auto half_size = CHARACTERS_BUFFER_SIZE / 2;
            memcpy(new_cb.buf_, buf_ + half_size, half_size * sizeof(char_type));
            cur_size_ -= half_size;
            new_cb.cur_size_ = half_size;
        }
    
        // Insert the new character.
        if (cboffset > cur_size_)
        {
            characters_buffer_type& nxt_cb = chars_buf_cache_->get_character_buffer(nxt_);
            nxt_cb.insert_character(ch, cboffset - cur_size_);
        }
        else
        {
            if (cboffset < cur_size_)
            {
                memcpy((buf_ + cboffset + 1), (buf_ + cboffset),
                       (cur_size_ - cboffset) * sizeof(char_type));
            }
    
            buf_[cboffset] = ch;
            ++cur_size_;
        }
    }
    
    void erase_character(cboffset_t cboffset)
    {
        if (cur_size_ > 0)
        {
            if (cboffset >= cur_size_)
            {
                characters_buffer_type& nxt_cb = chars_buf_cache_->get_character_buffer(nxt_);
                nxt_cb.erase_character(cboffset - cur_size_);
            }
            else
            {
                if (cboffset + 1 < cur_size_)
                {
                    memcpy((buf_ + cboffset), (buf_ + cboffset + 1),
                           (cur_size_ - cboffset - 1) * sizeof(char_type));
                }
    
                --cur_size_;
            }
        }
    }
    
    cboffset_t compute_n_chars(cboffset_t cboffset)
    {
        cboffset_t i = 0;
        
        while (cboffset < cur_size_ && buf_[cboffset] != LF && buf_[cboffset] != CR)
        {
            ++i;
            ++cboffset;
        }
        
        if (cboffset == cur_size_ && nxt_ != EMPTY)
        {
            characters_buffer_type& nxt_cb = chars_buf_cache_->get_character_buffer(nxt_);
            return i + nxt_cb.compute_n_chars(0);
        }
        else if (cboffset < cur_size_)
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
        if (i >= cur_size_)
        {
            characters_buffer_type& nxt_cb = chars_buf_cache_->get_character_buffer(nxt_);
            return nxt_cb[i - cur_size_];
        }
        
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
    alignas(CHARACTERS_BUFFER_SIZE * sizeof(char_type)) char_type buf_[CHARACTERS_BUFFER_SIZE];
    
    cbid_t cbid_;
    
    cbid_t prev_;
    
    cbid_t nxt_;
    
    cboffset_t cur_size_;
    
    characters_buffer_cache_type* chars_buf_cache_;
};


}
}


#endif
