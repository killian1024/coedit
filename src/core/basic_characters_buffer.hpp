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
        std::size_t CHARACTERS_BUFFER_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_SIZE
>
class basic_characters_buffer_cache;


template<
        typename TpChar,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_SIZE
>
class basic_characters_buffer
{
public:
    using characters_buffer_type = basic_characters_buffer<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE
    >;
    
    using char_type = TpChar;
    
    using characters_buffer_cache_type = basic_characters_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE
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
    
    void insert_character(char_type ch, cboffset_t cboffset) // HERE : reimplement this function.
    {
        if (size_ == CHARACTERS_BUFFER_SIZE)
        {
            // New buffer creation.
            cbid_t new_cbid = chars_buf_cache_->get_new_cbid();
            chars_buf_cache_->insert(new_cbid, characters_buffer_type(
                    new_cbid, cbid_, nxt_, chars_buf_cache_));
            if (nxt_ != EMPTY)
            {
                characters_buffer_type& nxt_cb = chars_buf_cache_->get_characters_buffer(nxt_);
                nxt_cb.prev_ = new_cbid;
            }
            nxt_ = new_cbid;
            
            // Move the current buffer half data in the new buffer.
            characters_buffer_type& new_cb = chars_buf_cache_->get_characters_buffer(new_cbid);
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
            
            characters_buffer_type& nxt_cb = chars_buf_cache_->get_characters_buffer(nxt_);
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
        auto& chars_buf = get_characters_buffer(&cboffset);
        
        if (cboffset + 1 < chars_buf.size_)
        {
            memcpy((chars_buf.buf_ + cboffset),
                   (chars_buf.buf_ + cboffset + 1),
                   (chars_buf.size_ - cboffset - 1) * sizeof(char_type));
        }
    
        --chars_buf.size_;
    }
    
    cboffset_t get_line_length(cboffset_t cboffset)
    {
        cboffset_t line_len = 0;
        characters_buffer_type* current_chars_buf = this;
        char_type* current_buf;
        cboffset_t current_size;
        cbid_t current_nxt;
        
        do
        {
            current_chars_buf = &current_chars_buf->get_characters_buffer(&cboffset);
            current_buf = current_chars_buf->buf_;
            current_size = current_chars_buf->size_;
            current_nxt = current_chars_buf->nxt_;
            
            while (cboffset < current_size &&
                   current_buf[cboffset] != LF &&
                   current_buf[cboffset] != CR)
            {
                ++line_len;
                ++cboffset;
            }
            
            if (cboffset < current_size)
            {
                ++line_len;
                if (current_buf[cboffset] == CR &&
                    cboffset + 1 < CHARACTERS_BUFFER_SIZE &&
                    current_buf[cboffset + 1] == LF)
                {
                    ++line_len;
                }
            }
            
        } while (cboffset == current_size && current_nxt != EMPTY);
    
        return line_len;
    }
    
    void store(const stdfs::path& cb_path) const
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
    
    char_type& operator [](cboffset_t i)
    {
        auto& chars_buf = get_characters_buffer(&i);
        
        return chars_buf.buf_[i];
    }
    
    cbid_t get_cbid() const
    {
        return cbid_;
    }
    
    cbid_t get_nxt() const
    {
        return nxt_;
    }
    
    cboffset_t get_size() const
    {
        return size_;
    }

private:
    characters_buffer_type& get_characters_buffer(cboffset_t* cboffset)
    {
        cbid_t cur_nxt = nxt_;
        cboffset_t cur_size = size_;
        characters_buffer_type* pnxt_cb = this;
    
        while (*cboffset >= cur_size)
        {
            if (cur_nxt == EMPTY)
            {
                throw characte_buffer_overflow_exception();
            }
    
            *cboffset -= cur_size;
            pnxt_cb = &chars_buf_cache_->get_characters_buffer(cur_nxt);
            cur_nxt = pnxt_cb->nxt_;
            cur_size = pnxt_cb->size_;
        }
    
        return *pnxt_cb;
    }

private:
    char_type buf_[CHARACTERS_BUFFER_SIZE];
    
    cbid_t cbid_;
    
    cbid_t prev_;
    
    cbid_t nxt_;
    
    cboffset_t size_;
    
    characters_buffer_cache_type* chars_buf_cache_;
};


}
}


#endif
