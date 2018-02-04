//
// Created by Killian on 20/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINE_HPP
#define COEDIT_CORE_BASIC_LINE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer_cache.hpp"
#include "fundamental_types.hpp"
#include "line_flags.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE
>
class basic_line
{
public:
    using char_type = TpChar;
    
    using characters_buffer_type = basic_characters_buffer<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using characters_buffer_cache_type = basic_characters_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using flags_type = kcontain::flags<line_flags>;
    
    class basic_line_iterator
            : public kcontain::i_mutable_iterator<
                    char_type, basic_line_iterator
              >
    {
    public:
        using self_type = basic_line_iterator;
        
        using value_type = char_type;
        
        using node_type = std::pair<cbid_t, cboffset_t>;
    
        basic_line_iterator() noexcept
                : fir_({EMPTY, 0})
                , cur_({EMPTY, 0})
                , chars_buf_cache_(nullptr)
        {
        }
    
        basic_line_iterator(
                node_type fir,
                node_type cur,
                characters_buffer_cache_type* chars_buf_cache
        ) noexcept
                : fir_(std::move(fir))
                , cur_(std::move(cur))
                , chars_buf_cache_(chars_buf_cache)
        {
        }
        
        self_type& operator ++() noexcept override
        {
            characters_buffer_type& current_cb = chars_buf_cache_->get_character_buffer(cur_.first);
            
            if (cur_.second + 1 >= CHARACTERS_BUFFER_SIZE ||
                cur_.second + 1 >= current_cb.get_cur_size())
            {
                cur_.second = 0;
                cur_.first = current_cb.get_nxt();
            }
            else
            {
                ++cur_.second;
            }
    
            if (current_cb[cur_.second] == 10)
            {
                cur_.first = EMPTY;
                cur_.second = 0;
            }
            
            return *this;
        }
        
        self_type& operator --() noexcept override
        {
            // todo : implement the operator --
            return *this;
        }
        
        bool operator ==(const self_type& rhs) const noexcept override
        {
            return cur_ == rhs.cur_;
        }
        
        bool end() const noexcept override
        {
            return cur_.first == EMPTY && cur_.second == 0;
        }
        
        value_type& operator *() noexcept override
        {
            characters_buffer_type& current_cb = chars_buf_cache_->get_character_buffer(cur_.first);
            
            return current_cb[cur_.second];
        }
        
        value_type* operator ->() noexcept override
        {
            characters_buffer_type& current_cb = chars_buf_cache_->get_character_buffer(cur_.first);
            
            return &(current_cb[cur_.second]);
        }
        
        template<
                typename TpChar__,
                std::size_t LINES_CACHE_SIZE__,
                std::size_t CHARACTERS_BUFFER_CACHE_SIZE__,
                std::size_t CHARACTERS_BUFFER_SIZE__,
                typename TpAllocator__
        >
        friend class basic_file_editor;
    
    protected:
        node_type fir_;
        
        node_type cur_;
    
        characters_buffer_cache_type* chars_buf_cache_;
    };
    
    using iterator = basic_line_iterator;
    
    basic_line()
            : lid_(EMPTY)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , cbid_(EMPTY)
            , cboffset_(0)
            , chars_buf_cache_(nullptr)
    {
    }
    
    basic_line(
            lid_t lid,
            lid_t prev,
            lid_t nxt,
            cbid_t bid,
            cboffset_t bid_offset,
            characters_buffer_cache_type* chars_cache
    )
            : lid_(lid)
            , prev_(prev)
            , nxt_(nxt)
            , cbid_(bid)
            , cboffset_(bid_offset)
            , chars_buf_cache_(chars_cache)
    {
    }
    
    inline iterator begin() noexcept
    {
        characters_buffer_type& current_cb = chars_buf_cache_->get_character_buffer(cbid_);
    
        if (current_cb[cboffset_] == 10)
        {
            return end();
        }
        
        return iterator({cbid_, cboffset_}, {cbid_, cboffset_}, chars_buf_cache_);
    }
    
    inline iterator end() noexcept
    {
        return iterator({EMPTY, 0}, {EMPTY, 0}, chars_buf_cache_);
    }
    
    void insert_character(char_type ch, loffset_t loffset)
    {
        if (cbid_ == EMPTY)
        {
            cbid_ = chars_buf_cache_->get_new_cbid();
            cboffset_ = 0;
            chars_buf_cache_->insert(cbid_, characters_buffer_type(
                    cbid_, EMPTY, EMPTY, chars_buf_cache_));
        }
    
        characters_buffer_type& current_cb = chars_buf_cache_->get_character_buffer(cbid_);
        current_cb.insert_character(ch, cboffset_, loffset);
    }
    
    lid_t get_lid() const
    {
        return lid_;
    }
    
    void set_lid(lid_t lid)
    {
        lid_ = lid;
    }
    
    lid_t get_prev() const
    {
        return prev_;
    }
    
    void set_prev(lid_t prev)
    {
        prev_ = prev;
    }
    
    lid_t get_nxt() const
    {
        return nxt_;
    }
    
    void set_nxt(lid_t nxt)
    {
        nxt_ = nxt;
    }
    
    cbid_t get_cbid() const
    {
        return cbid_;
    }
    
    void set_cbid(cbid_t cbid)
    {
        cbid_ = cbid;
    }
    
    cboffset_t get_cboffset() const
    {
        return cboffset_;
    }
    
    void set_cboffset(cboffset_t cboffset)
    {
        cboffset_ = cboffset;
    }

private:
    lid_t lid_;
    
    lid_t prev_;
    
    lid_t nxt_;
    
    cbid_t cbid_;
    
    cboffset_t cboffset_;
    
    characters_buffer_cache_type* chars_buf_cache_;
};


}
}


#endif
