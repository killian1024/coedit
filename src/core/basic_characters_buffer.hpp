//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTERS_BUFFER_HPP
#define COEDIT_CORE_BASIC_CHARACTERS_BUFFER_HPP

#include <experimental/filesystem>
#include <fstream>
#include <memory>

#include "basic_characters_buffer_cache.hpp"
#include "core_exception.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


namespace stdfs = std::experimental::filesystem;


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
    
    using cboffset_type = cboffset_t;
    
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
            , size_(0)
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
            , size_(0)
            , chars_buf_cache_(chars_buf_cache)
    {
    }
    
    basic_characters_buffer(
            const stdfs::path& cb_path,
            characters_buffer_cache_type* chars_buf_cache
    )
            : chars_buf_cache_(chars_buf_cache)
    {
        std::ifstream ifs;
    
        ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ifs.open(cb_path, std::ios::in | std::ios::binary);
    
        ifs.read((char*)&cbid_, sizeof(cbid_));
        ifs.read((char*)&prev_, sizeof(prev_));
        ifs.read((char*)&nxt_, sizeof(nxt_));
        ifs.read((char*)&size_, sizeof(size_));
        ifs.read((char*)buf_, sizeof(char_type) * size_);
    
        ifs.close();
    }
    
    basic_characters_buffer& operator =(const basic_characters_buffer& rhs)
    {
        if (this != &rhs)
        {
            memcpy(buf_, rhs.buf_, rhs.size_ * sizeof(char_type));
            cbid_ = rhs.cbid_;
            prev_ = rhs.prev_;
            nxt_ = rhs.nxt_;
            size_ = rhs.size_;
            chars_buf_cache_ = rhs.chars_buf_cache_;
        }
        
        return *this;
    }
    
    basic_characters_buffer& operator =(basic_characters_buffer&& rhs) noexcept
    {
        if (this != &rhs)
        {
            memcpy(buf_, rhs.buf_, rhs.size_ * sizeof(char_type));
            std::swap(cbid_, rhs.cbid_);
            std::swap(prev_, rhs.prev_);
            std::swap(nxt_, rhs.nxt_);
            std::swap(size_, rhs.size_);
            std::swap(chars_buf_cache_, rhs.chars_buf_cache_);
        }
        
        return *this;
    }
    
    void insert_character(char_type ch, cboffset_t cboffset)
    {
        if (size_ == CHARACTERS_BUFFER_SIZE)
        {
            // New buffer creation.
            cbid_t new_cbid = chars_buf_cache_->get_new_cbid();
            chars_buf_cache_->insert(new_cbid, characters_buffer_type(
                    new_cbid, cbid_, nxt_, chars_buf_cache_));
            if (nxt_ != EMPTY)
            {
                characters_buffer_type& nxt_cb = chars_buf_cache_->get_cb(nxt_);
                nxt_cb.prev_ = new_cbid;
            }
            nxt_ = new_cbid;
            
            // Move the current buffer half data in the new buffer.
            characters_buffer_type& new_cb = chars_buf_cache_->get_cb(new_cbid);
            constexpr auto half_size = CHARACTERS_BUFFER_SIZE / 2;
            memcpy(new_cb.buf_, buf_ + half_size, half_size * sizeof(char_type));
            size_ -= half_size;
            new_cb.size_ = half_size;
        }
        
        // Insert the new character.
        if (cboffset > size_)
        {
            if (nxt_ == EMPTY)
            {
                throw characte_buffer_overflow_exception();
            }
            
            characters_buffer_type& nxt_cb = chars_buf_cache_->get_cb(nxt_);
            nxt_cb.insert_character(ch, cboffset - size_);
        }
        else
        {
            if (cboffset < size_)
            {
                memcpy((buf_ + cboffset + 1), (buf_ + cboffset),
                       (size_ - cboffset) * sizeof(char_type));
            }
    
            buf_[cboffset] = ch;
            ++size_;
        }
    }
    
    void erase_character(cboffset_t cboffset)
    {
        if (size_ > 0)
        {
            if (cboffset >= size_)
            {
                if (nxt_ == EMPTY)
                {
                    throw characte_buffer_overflow_exception();
                }
                
                characters_buffer_type& nxt_cb = chars_buf_cache_->get_cb(nxt_);
                nxt_cb.erase_character(cboffset - size_);
            }
            else
            {
                if (cboffset + 1 < size_)
                {
                    memcpy((buf_ + cboffset), (buf_ + cboffset + 1),
                           (size_ - cboffset - 1) * sizeof(char_type));
                }
    
                --size_;
            }
        }
    }
    
    cboffset_t compute_n_chars(cboffset_t cboffset)
    {
        cboffset_t i = 0;
        
        while (cboffset < size_ && buf_[cboffset] != LF && buf_[cboffset] != CR)
        {
            ++i;
            ++cboffset;
        }
        
        if (cboffset == size_ && nxt_ != EMPTY)
        {
            characters_buffer_type& nxt_cb = chars_buf_cache_->get_cb(nxt_);
            return i + nxt_cb.compute_n_chars(0);
        }
        else if (cboffset < size_)
        {
            ++i;
            if (buf_[cboffset] == CR && buf_[cboffset + 1] == LF)
            {
                ++i;
            }
        }
    
        return i;
    }
    
    void store_buffer(const stdfs::path& cb_path) const
    {
        std::ofstream ofs;
        
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofs.open(cb_path, std::ios::out | std::ios::binary);
    
        ofs.write((const char*)&cbid_, sizeof(cbid_));
        ofs.write((const char*)&prev_, sizeof(prev_));
        ofs.write((const char*)&nxt_, sizeof(nxt_));
        ofs.write((const char*)&size_, sizeof(size_));
        ofs.write((const char*)buf_, sizeof(char_type) * size_);
        
        ofs.close();
    }
    
    char_type& operator [](std::size_t i)
    {
        char_type* cur_buf = buf_;
        cbid_t cur_nxt = nxt_;
        cboffset_t cur_size = size_;
        characters_buffer_type* pnxt_cb;
        
        while (i >= cur_size)
        {
            if (cur_nxt == EMPTY)
            {
                throw characte_buffer_overflow_exception();
            }
            
            i -= cur_size;
            pnxt_cb = &chars_buf_cache_->get_cb(cur_nxt);
            cur_buf = pnxt_cb->buf_;
            cur_nxt = pnxt_cb->nxt_;
            cur_size = pnxt_cb->size_;
        }
        
        return cur_buf[i];
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
    
    cboffset_t get_size() const
    {
        return size_;
    }
    
    void set_size(cboffset_t cur_size)
    {
        size_ = cur_size;
    }
    
    characters_buffer_cache_type* get_chars_buf_cache() const
    {
        return chars_buf_cache_;
    }
    
    void set_chars_buf_cache(characters_buffer_cache_type* chars_buf_cache)
    {
        chars_buf_cache_ = chars_buf_cache;
    }
    
    constexpr cboffset_t get_real_size() const noexcept
    {
        return CHARACTERS_BUFFER_SIZE;
    }

private:
    alignas(CHARACTERS_BUFFER_SIZE * sizeof(char_type)) char_type buf_[CHARACTERS_BUFFER_SIZE];
    
    cbid_t cbid_;
    
    cbid_t prev_;
    
    cbid_t nxt_;
    
    cboffset_t size_;
    
    characters_buffer_cache_type* chars_buf_cache_;
};


}
}


#endif
