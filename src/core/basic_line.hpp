//
// Created by Killian on 20/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINE_HPP
#define COEDIT_CORE_BASIC_LINE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer_cache.hpp"
#include "basic_lines_cache.hpp"
#include "fundamental_types.hpp"
#include "line_flags.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t LINES_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_SIZE,
        typename TpAllocator
>
class basic_lines_cache;


template<
        typename TpChar,
        std::size_t LINES_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_IDS_BUFFER_SIZE,
        typename TpAllocator
>
class basic_line
{
public:
    using line_type = basic_line<
            TpChar,
            LINES_CACHE_SIZE,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE,
            TpAllocator
    >;
    
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using characters_buffer_type = basic_characters_buffer<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE
    >;
    
    using characters_buffer_cache_type = basic_characters_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE
    >;
    
    using lines_cache_type = basic_lines_cache<
            TpChar,
            LINES_CACHE_SIZE,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_IDS_BUFFER_SIZE,
            TpAllocator
    >;
    
    using flags_type = kcontain::flags<line_flags>;
    
    class iterator : public kcontain::i_mutable_iterator<char_type, iterator>
    {
    public:
        using self_type = iterator;
        
        using value_type = char_type;
        
        using node_type = std::pair<cbid_t, cboffset_t>;
    
        iterator() noexcept
                : cur_({EMPTY, 0})
                , chars_buf_cache_(nullptr)
        {
        }
    
        iterator(
                node_type cur,
                characters_buffer_cache_type* chars_buf_cache
        ) noexcept
                : cur_(std::move(cur))
                , chars_buf_cache_(chars_buf_cache)
        {
        }
        
        self_type& operator ++() noexcept override
        {
            characters_buffer_type& current_cb =
                    chars_buf_cache_->get_characters_buffer(cur_.first);
            
            if (cur_.second + 1 >= CHARACTERS_BUFFER_SIZE ||
                cur_.second + 1 >= current_cb.get_size())
            {
                cur_.second = 0;
                cur_.first = current_cb.get_nxt();
            }
            else
            {
                ++cur_.second;
            }
    
            if (current_cb[cur_.second] == LF || current_cb[cur_.second] == CR)
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
            characters_buffer_type& current_cb =
                    chars_buf_cache_->get_characters_buffer(cur_.first);
            
            return current_cb[cur_.second];
        }
        
        value_type* operator ->() noexcept override
        {
            characters_buffer_type& current_cb =
                    chars_buf_cache_->get_characters_buffer(cur_.first);
            
            return &(current_cb[cur_.second]);
        }
        
        template<
                typename TpChar__,
                std::size_t LINES_CACHE_SIZE__,
                std::size_t CHARACTERS_BUFFER_CACHE_SIZE__,
                std::size_t CHARACTERS_BUFFER_SIZE__,
                std::size_t CHARACTERS_BUFFER_IDS_BUFFER_CACHE_SIZE__,
                std::size_t CHARACTERS_BUFFER_IDS_BUFFER_SIZE__,
                typename TpAllocator__
        >
        friend class basic_file_editor;
    
    protected:
        node_type cur_;
    
        characters_buffer_cache_type* chars_buf_cache_;
    };
    
    basic_line()
            : lid_(EMPTY)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , cbid_(EMPTY)
            , cboffset_(0)
            , n_chars_(0)
            , lnes_cache_(nullptr)
            , chars_buf_cache_(nullptr)
    {
    }
    
    basic_line(
            lid_t lid,
            lid_t prev,
            lid_t nxt,
            lines_cache_type* lnes_cache,
            characters_buffer_cache_type* chars_cache
    )
            : lid_(lid)
            , prev_(prev)
            , nxt_(nxt)
            , cbid_(EMPTY)
            , cboffset_(0)
            , n_chars_(0)
            , lnes_cache_(lnes_cache)
            , chars_buf_cache_(chars_cache)
    {
        if (prev_ == EMPTY)
        {
            cbid_ = chars_buf_cache_->get_new_cbid();
            cboffset_ = 0;
            chars_buf_cache_->insert(cbid_, characters_buffer_type(
                    cbid_, EMPTY, EMPTY, chars_buf_cache_));
        }
        else
        {
            line_type& prev_lne = lnes_cache_->get_line(prev_);
            characters_buffer_type& prev_cb =
                    chars_buf_cache_->get_characters_buffer(prev_lne.get_cbid());
            
            cbid_ = prev_cb.get_cbid();
            cboffset_ = prev_lne.get_cboffset() + prev_lne.get_n_chars();
            n_chars_ = prev_cb.get_line_length(cboffset_);
        }
    }
    
    inline iterator begin() noexcept
    {
        characters_buffer_type& current_cb =
                chars_buf_cache_->get_characters_buffer(cbid_);
    
        // todo : when the sharing buffers will be implemented, fix that shit :)
        try
        {
            current_cb[cboffset_];
        }
        catch (...)
        {
            return end();
        }
        // end_shit
    
        if (current_cb[cboffset_] == LF || current_cb[cboffset_] == CR)
        {
            return end();
        }
        
        return iterator({cbid_, cboffset_}, chars_buf_cache_);
    }
    
    inline iterator end() noexcept
    {
        return iterator({EMPTY, 0}, chars_buf_cache_);
    }
    
    void insert_character(char_type ch, loffset_t loffset)
    {
        characters_buffer_type& current_cb = chars_buf_cache_->get_characters_buffer(cbid_);
        lid_t current_nxt_lid = nxt_;
        line_type* nxt_lne;
        
        current_cb.insert_character(ch, cboffset_ + loffset);
        ++n_chars_;
        
        while (current_nxt_lid != EMPTY)
        {
            nxt_lne = &(lnes_cache_->get_line(current_nxt_lid));
            nxt_lne->increment_cboffset(1);
            current_nxt_lid = nxt_lne->get_nxt();
        }
    }
    
    void erase_character(loffset_t loffset)
    {
        characters_buffer_type& current_cb = chars_buf_cache_->get_characters_buffer(cbid_);
        lid_t current_nxt_lid = nxt_;
        line_type* nxt_lne;
        
        current_cb.erase_character(cboffset_ + loffset);
        --n_chars_;
    
        while (current_nxt_lid != EMPTY)
        {
            nxt_lne = &(lnes_cache_->get_line(current_nxt_lid));
            nxt_lne->increment_cboffset(~(lid_t)0);
            current_nxt_lid = nxt_lne->get_nxt();
        }
    }
    
    void merge_with_next_line()
    {
        if (nxt_ == EMPTY)
        {
            throw invalid_operation_exception();
        }
    
        line_type* nxt_lne = &(lnes_cache_->get_line(nxt_));
        characters_buffer_type& current_cb = chars_buf_cache_->get_characters_buffer(cbid_);
        
        if (n_chars_ - 1 > 0 && current_cb[cboffset_ + n_chars_ - 2] == CR)
        {
            erase_character(n_chars_ - 1);
        }
        
        erase_character(n_chars_ - 1);
        
        nxt_ = nxt_lne->get_nxt();
        n_chars_ += nxt_lne->get_n_chars();
        
        if (nxt_ != EMPTY)
        {
            nxt_lne = &(lnes_cache_->get_line(nxt_));
            nxt_lne->set_prev(lid_);
        }
    }
    
    bool can_go_left(loffset_t loffset)
    {
        return loffset > 0;
    }
    
    bool can_go_right(loffset_t loffset)
    {
        characters_buffer_type& current_cb = chars_buf_cache_->get_characters_buffer(cbid_);
        auto val = current_cb[cboffset_ + loffset];
        
        return val != LF && val != CR && (nxt_ != EMPTY || loffset < n_chars_);
    }
    
    std::size_t get_line_length()
    {
        characters_buffer_type& current_cb = chars_buf_cache_->get_characters_buffer(cbid_);
        
        if (n_chars_ > 0)
        {
            auto val = current_cb[cboffset_ + n_chars_ - 1];
            
            if (n_chars_ - 1 > 0 && current_cb[cboffset_ + n_chars_ - 2] == CR)
            {
                return n_chars_ - 2;
            }
            else if (current_cb[cboffset_ + n_chars_ - 1] == LF)
            {
                return n_chars_ - 1;
            }
        }
        
        return n_chars_;
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
    
    void increment_cboffset(cboffset_t cboffset)
    {
        cboffset_ += cboffset;
    }
    
    std::size_t get_n_chars() const
    {
        return n_chars_;
    }
    
    void set_n_chars(std::size_t n_chars)
    {
        n_chars_ = n_chars;
    }

private:
    lines_cache_type* lnes_cache_;
    
    characters_buffer_cache_type* chars_buf_cache_;
    
    lid_t lid_;
    
    lid_t prev_;
    
    lid_t nxt_;
    
    cbid_t cbid_;
    
    cboffset_t cboffset_;
    
    std::size_t n_chars_;
};


}
}


#endif
