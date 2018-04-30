//
// Created by Killian on 31/01/18.
//

#ifndef COEDIT_CORE_BASIC_CHARACTER_BUFFER_HPP
#define COEDIT_CORE_BASIC_CHARACTER_BUFFER_HPP

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <unordered_set>

#include "basic_character_buffer_cache.hpp"
#include "core_exception.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_character_buffer_cache;


template<
        typename TpChar,
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_character_buffer
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using character_buffer_type = basic_character_buffer<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    using character_buffer_cache_type = basic_character_buffer_cache<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    enum class operation_types : uint8_t
    {
        NIL = 0x0,
        CHARACTER_BUFFER_INSERTED = 0x1,
        CHARACTER_BUFFER_DEFRAGMENTED = 0x2,
        CHARACTER_BUFFER_MERGED_WITH_NEXT = 0x4,
        CHARACTER_BUFFER_MERGED_WITH_PREVIOUS = 0x8,
        CHARACTER_BUFFER_BALANCED_WITH_NEXT = 0x10,
        CHARACTER_BUFFER_BALANCED_WITH_PREVIOS = 0x20,
        ALL = 0x3F
    };
    
    struct operation_done
    {
        cbid_t cbid;
        cboffset_t cboffset;
        kcontain::flags<operation_types> types;
    };
    
    basic_character_buffer() noexcept
            : buf_(nullptr)
            , cbid_(EMPTY)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , sze_(0)
            , cb_cache_(nullptr)
    {
    }
    
    basic_character_buffer(
            cbid_t cbid,
            character_buffer_cache_type* cb_cache
    )
            : buf_()
            , cbid_(cbid)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , sze_(0)
            , cb_cache_(cb_cache)
    {
        allocate_memory();
    }
    
    basic_character_buffer(
            cbid_t cbid,
            cbid_t prev,
            character_buffer_cache_type* cb_cache
    )
            : buf_()
            , cbid_(cbid)
            , prev_(prev)
            , nxt_(EMPTY)
            , sze_(0)
            , cb_cache_(cb_cache)
    {
        allocate_memory();
        character_buffer_type& prev_cb = cb_cache->get_character_buffer(prev_);
        prev_cb.link_character_buffer_to(*this);
        prev_cb.move_half_characters_to(*this);
    }
    
    basic_character_buffer(
            const std::filesystem::path& cb_path,
            character_buffer_cache_type* cb_cache
    )
            : cb_cache_(cb_cache)
    {
        std::ifstream ifs;
        
        allocate_memory();
        ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ifs.open(cb_path, std::ios::in | std::ios::binary);
    
        ifs.read((char*)&cbid_, sizeof(cbid_));
        ifs.read((char*)&prev_, sizeof(prev_));
        ifs.read((char*)&nxt_, sizeof(nxt_));
        ifs.read((char*)&sze_, sizeof(sze_));
        ifs.read((char*)buf_, sizeof(char_type) * sze_);
    
        ifs.close();
    }
    
    basic_character_buffer(const basic_character_buffer& rhs) = delete;
    
    basic_character_buffer(basic_character_buffer&& rhs) = delete;
    
    ~basic_character_buffer()
    {
        if (buf_ != nullptr)
        {
            free(buf_);
            buf_ = nullptr;
        }
    }
    
    basic_character_buffer& operator =(const basic_character_buffer& rhs)
    {
        if (this != &rhs)
        {
            if (buf_ == nullptr)
            {
                allocate_memory();
            }
            
            memcpy(buf_, rhs.buf_, rhs.sze_ * sizeof(char_type));
            cbid_ = rhs.cbid_;
            prev_ = rhs.prev_;
            nxt_ = rhs.nxt_;
            sze_ = rhs.sze_;
            cb_cache_ = rhs.cb_cache_;
        }
        
        return *this;
    }
    
    basic_character_buffer& operator =(basic_character_buffer&& rhs) noexcept
    {
        if (this != &rhs)
        {
            std::swap(buf_, rhs.buf_);
            std::swap(cbid_, rhs.cbid_);
            std::swap(prev_, rhs.prev_);
            std::swap(nxt_, rhs.nxt_);
            std::swap(sze_, rhs.sze_);
            std::swap(cb_cache_, rhs.cb_cache_);
        }
        
        return *this;
    }
    
    operation_done insert_character(char_type ch, cboffset_t cboffset)
    {
        operation_done op_done;
        character_buffer_type* cur_cb = &get_character_buffer_for_insertion(&cboffset);
        
        // Fragmentation policy
        if (cur_cb->sze_ == CHARACTER_BUFFER_SIZE)
        {
            cb_cache_->insert_character_buffer_after(cur_cb->cbid_);
            op_done.types.set(operation_types::CHARACTER_BUFFER_INSERTED);
        }
        
        // Take the operands.
        if (cboffset > cur_cb->sze_)
        {
            cur_cb = &cur_cb->get_character_buffer_for_insertion(&cboffset);
        }
        op_done.cbid = cur_cb->get_cbid();
        op_done.cboffset = cboffset;
        
        //Move the dat for make the space for the new character.
        if (cboffset < cur_cb->sze_)
        {
            memcpy((cur_cb->buf_ + cboffset + 1),
                   (cur_cb->buf_ + cboffset),
                   (cur_cb->sze_ - cboffset) * sizeof(char_type));
        }
    
        // Set the new character.
        cur_cb->buf_[cboffset] = ch;
        ++cur_cb->sze_;
        
        return op_done;
    }
    
    operation_done erase_character(cboffset_t cboffset)
    {
        constexpr const cboffset_t _75percent = (CHARACTER_BUFFER_SIZE * 75) / 100;
        constexpr const cboffset_t _25percent = (CHARACTER_BUFFER_SIZE * 25) / 100;
        
        operation_done op_done;
        character_buffer_type& cur_cb = get_character_buffer(&cboffset);
        character_buffer_type* prev_cb = nullptr;
        character_buffer_type* nxt_cb = nullptr;
        cboffset_t prev_size = 0;
        cboffset_t nxt_size = 0;
        
        // Erase the target character.
        if (cboffset + 1 < cur_cb.sze_)
        {
            memcpy((cur_cb.buf_ + cboffset),
                   (cur_cb.buf_ + cboffset + 1),
                   (cur_cb.sze_ - cboffset - 1) * sizeof(char_type));
        }
        op_done.cbid = cur_cb.get_cbid();
        op_done.cboffset = cboffset;
        --cur_cb.sze_;
        
        // Defragmentation policy.
        if (cur_cb.sze_ < _75percent)
        {
            if (cur_cb.nxt_ != EMPTY)
            {
                nxt_cb = &cb_cache_->get_character_buffer(cur_cb.nxt_);
                nxt_size = nxt_cb->sze_;
            }
            if (cur_cb.prev_ != EMPTY)
            {
                prev_cb = &cb_cache_->get_character_buffer(cur_cb.prev_);
                prev_size = prev_cb->sze_;
            }
    
            if (nxt_cb != nullptr &&
                cur_cb.sze_ + nxt_size <= _75percent)
            {
                merge_with_next_character_buffer();
                op_done.types.set(operation_types::CHARACTER_BUFFER_DEFRAGMENTED);
                op_done.types.set(operation_types::CHARACTER_BUFFER_MERGED_WITH_NEXT);
            }
            else if (prev_cb != nullptr &&
                     cur_cb.sze_ + prev_size <= _75percent)
            {
                merge_with_previous_character_buffer();
                op_done.types.set(operation_types::CHARACTER_BUFFER_DEFRAGMENTED);
                op_done.types.set(operation_types::CHARACTER_BUFFER_MERGED_WITH_PREVIOUS);
            }
            else if (cur_cb.sze_ <= _25percent)
            {
                if (nxt_cb != nullptr &&
                    nxt_size >= prev_size)
                {
                    balance_with_next_character_buffer();
                    op_done.types.set(operation_types::CHARACTER_BUFFER_DEFRAGMENTED);
                    op_done.types.set(operation_types::CHARACTER_BUFFER_BALANCED_WITH_NEXT);
                }
                else if (prev_cb != nullptr)
                {
                    balance_with_previous_character_buffer();
                    op_done.types.set(operation_types::CHARACTER_BUFFER_DEFRAGMENTED);
                    op_done.types.set(operation_types::CHARACTER_BUFFER_BALANCED_WITH_PREVIOS);
                }
            }
        }
        
        return op_done;
    }
    
    character_buffer_type& get_character_buffer(cboffset_t* cboffset)
    {
        character_buffer_type* cur_cb = this;
        
        while (*cboffset >= cur_cb->sze_)
        {
            if (cur_cb->nxt_ == EMPTY)
            {
                throw characte_buffer_overflow_exception();
            }
            
            *cboffset -= cur_cb->sze_;
            cur_cb = &cb_cache_->get_character_buffer(cur_cb->nxt_);
        }
        
        return *cur_cb;
    }
    
    character_buffer_type& get_character_buffer_for_insertion(cboffset_t* cboffset)
    {
        character_buffer_type* cur_cb = this;
        
        while (*cboffset > cur_cb->sze_)
        {
            if (cur_cb->nxt_ == EMPTY)
            {
                throw characte_buffer_overflow_exception();
            }
            
            *cboffset -= cur_cb->sze_;
            cur_cb = &cb_cache_->get_character_buffer(cur_cb->nxt_);
        }
        
        return *cur_cb;
    }
    
    cboffset_t get_line_length(cboffset_t cboffset)
    {
        character_buffer_type* cur_cb = this;
        cboffset_t line_len = 0;
        
        do
        {
            cur_cb = &cur_cb->get_character_buffer(&cboffset);
            
            while (cboffset < cur_cb->sze_ &&
                   cur_cb->buf_[cboffset] != LF &&
                   cur_cb->buf_[cboffset] != CR)
            {
                ++line_len;
                ++cboffset;
            }
            
            if (cboffset < cur_cb->sze_)
            {
                ++line_len;
                if (cur_cb->buf_[cboffset] == CR &&
                    cboffset + 1 < CHARACTER_BUFFER_SIZE &&
                    cur_cb->buf_[cboffset + 1] == LF)
                {
                    ++line_len;
                }
            }
            
        } while (cboffset == cur_cb->sze_ && cur_cb->nxt_ != EMPTY);
    
        return line_len;
    }
    
    void store(const std::filesystem::path& cb_path) const
    {
        std::ofstream ofs;
        
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofs.open(cb_path, std::ios::out | std::ios::binary);
    
        ofs.write((const char*)&cbid_, sizeof(cbid_));
        ofs.write((const char*)&prev_, sizeof(prev_));
        ofs.write((const char*)&nxt_, sizeof(nxt_));
        ofs.write((const char*)&sze_, sizeof(sze_));
        ofs.write((const char*)buf_, sizeof(char_type) * sze_);
        
        ofs.close();
    }
    
    inline char_type& operator [](cboffset_t i)
    {
        character_buffer_type& cb = get_character_buffer(&i);
        
        return cb.buf_[i];
    }
    
    inline cbid_t get_cbid() const noexcept
    {
        return cbid_;
    }
    
    inline cbid_t get_previous() const noexcept
    {
        return prev_;
    }
    
    inline cbid_t get_next() const noexcept
    {
        return nxt_;
    }
    
    inline cboffset_t get_size() const noexcept
    {
        return sze_;
    }

private:
    // TODO(killian.poulaud@etu.upmc.fr): Implement an align allocator and give it to this class.
    void allocate_memory()
    {
        std::size_t page_sze = CHARACTER_BUFFER_SIZE * sizeof(char_type);
        
        if (page_sze < sizeof(void*))
        {
            page_sze = sizeof(void*);
        }
        
        if (posix_memalign((void**)&buf_, page_sze, page_sze) != 0)
        {
            throw bad_allocation_exception();
        }
    }
    
    void link_character_buffer_to(character_buffer_type& cb_to_link)
    {
        cb_to_link.nxt_ = nxt_;
        cb_to_link.prev_ = cbid_;
        
        if (nxt_ != EMPTY)
        {
            character_buffer_type& nxt_cb = cb_cache_->get_character_buffer(nxt_);
            nxt_cb.prev_ = cb_to_link.cbid_;
        }
        
        nxt_ = cb_to_link.cbid_;
    }
    
    void unlink_character_buffer_to(character_buffer_type& cb_to_unlink)
    {
        nxt_ = cb_to_unlink.nxt_;
        
        if (nxt_ != EMPTY)
        {
            character_buffer_type& nxt_cb = cb_cache_->get_character_buffer(nxt_);
            nxt_cb.prev_ = cbid_;
        }
    }
    
    bool move_half_characters_to(character_buffer_type& cb_dest) noexcept
    {
        constexpr const cboffset_t half_size = CHARACTER_BUFFER_SIZE / 2;
    
        if (sze_ == CHARACTER_BUFFER_SIZE)
        {
            memcpy(cb_dest.buf_, buf_ + half_size, half_size * sizeof(char_type));
            sze_ -= half_size;
            cb_dest.sze_ = half_size;
            
            return true;
        }
        
        return false;
    }
    
    bool merge_with_next_character_buffer()
    {
        if (nxt_ == EMPTY)
        {
            return false;
        }
        
        character_buffer_type& nxt_cb = cb_cache_->get_character_buffer(nxt_);
        
        if (klow::add(sze_, nxt_cb.sze_) > CHARACTER_BUFFER_SIZE)
        {
            return false;
        }
        
        memcpy(buf_ + sze_, nxt_cb.buf_, nxt_cb.sze_ * sizeof(char_type));
        sze_ += nxt_cb.sze_;
        unlink_character_buffer_to(nxt_cb);
        
        return true;
    }
    
    bool merge_with_previous_character_buffer()
    {
        if (prev_ == EMPTY)
        {
            return false;
        }
    
        character_buffer_type& prev_cb = cb_cache_->get_character_buffer(prev_);
        
        if (klow::add(sze_, prev_cb.sze_) > CHARACTER_BUFFER_SIZE)
        {
            return false;
        }
    
        memcpy(buf_ + prev_cb.sze_, buf_, sze_ * sizeof(char_type));
        memcpy(buf_, prev_cb.buf_, prev_cb.sze_ * sizeof(char_type));
        
        sze_ += prev_cb.sze_;
        
        if (prev_cb.prev_ != EMPTY)
        {
            character_buffer_type& prev_prev_cb = cb_cache_->get_character_buffer(prev_cb.prev_);
            prev_prev_cb.unlink_character_buffer_to(prev_cb);
        }
        
        return true;
    }
    
    bool balance_with_next_character_buffer()
    {
        if (nxt_ == EMPTY)
        {
            return false;
        }
    
        character_buffer_type& nxt_cb = cb_cache_->get_character_buffer(nxt_);
        size_t total_sze = klow::add(sze_, nxt_cb.sze_) / 2;
        size_t nxt_diff = nxt_cb.sze_ - total_sze;
        
        memcpy(buf_ + sze_, nxt_cb.buf_, nxt_diff * sizeof(char_type));
        memcpy(nxt_cb.buf_, nxt_cb.buf_ + nxt_diff, total_sze * sizeof(char_type));
        
        sze_ = total_sze;
        nxt_cb.sze_ = total_sze;
        
        return true;
    }
    
    bool balance_with_previous_character_buffer()
    {
        if (prev_ == EMPTY)
        {
            return false;
        }
        
        character_buffer_type& prev_cb = cb_cache_->get_character_buffer(prev_);
        size_t total_sze = klow::add(sze_, prev_cb.sze_) / 2;
        size_t prev_diff = prev_cb.sze_ - total_sze;
        
        memcpy(buf_ + prev_diff, buf_, sze_ * sizeof(char_type));
        memcpy(buf_, prev_cb.buf_ + total_sze, prev_diff * sizeof(char_type));
        
        sze_ = total_sze;
        prev_cb.sze_ = total_sze;
        
        return true;
    }

private:
    char_type* buf_;
    
    cbid_t cbid_;
    
    cbid_t prev_;
    
    cbid_t nxt_;
    
    cboffset_t sze_;
    
    character_buffer_cache_type* cb_cache_;
};


}
}


#endif
