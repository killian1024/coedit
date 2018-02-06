//
// Created by Killian on 19/01/18.
//

#ifndef COEDIT_CORE_BASIC_FILE_EDITOR_HPP
#define COEDIT_CORE_BASIC_FILE_EDITOR_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_characters_buffer_cache.hpp"
#include "basic_lines_cache.hpp"
#include "core_exception.hpp"
#include "file_editor_command.hpp"
#include "fundamental_types.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t LINES_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE,
        typename TpAllocator
>
class basic_file_editor
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using characters_buffer_cache_type = basic_characters_buffer_cache<
            TpChar,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE
    >;
    
    using line_type = basic_line<
            TpChar,
            LINES_CACHE_SIZE,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            TpAllocator
    >;
    
    using lines_cache_type = basic_lines_cache<
            TpChar,
            LINES_CACHE_SIZE,
            CHARACTERS_BUFFER_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            TpAllocator
    >;
    
    class iterator : public kcontain::i_mutable_iterator<line_type, iterator>
    {
    public:
        using self_type = iterator;
        
        using value_type = line_type;
        
        using node_type = lid_t;
    
        iterator() noexcept
                : fir_(EMPTY)
                , cur_(EMPTY)
                , lnes_cache_(nullptr)
        {
        }
    
        iterator(
                node_type fir,
                node_type cur,
                lines_cache_type* lnes_cache
        ) noexcept
                : fir_(fir)
                , cur_(cur)
                , lnes_cache_(lnes_cache)
        {
        }
        
        self_type& operator ++() noexcept override
        {
            line_type& current_lne = lnes_cache_->get_line(cur_);
            cur_ = current_lne.get_nxt();
            
            return *this;
        }
        
        self_type& operator --() noexcept override
        {
            line_type& current_lne = lnes_cache_->get_line(cur_);
            cur_ = current_lne.get_prev();
            
            return *this;
        }
        
        bool operator ==(const self_type& rhs) const noexcept override
        {
            return cur_ == rhs.cur_;
        }
        
        bool end() const noexcept override
        {
            return cur_ == EMPTY;
        }
        
        value_type& operator *() noexcept override
        {
            return lnes_cache_->get_line(cur_);
        }
        
        value_type* operator ->() noexcept override
        {
            return &(lnes_cache_->get_line(cur_));
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
    
        lines_cache_type* lnes_cache_;
    };
    
    basic_file_editor()
            : lnes_cache_()
            , chars_buf_cache_()
            , first_lid_(EMPTY)
            , current_lid_(EMPTY)
            , first_display_lid_(EMPTY)
            , loffset_(0)
            , n_lnes_(1)
    {
        current_lid_ = lnes_cache_.get_new_lid();
        first_lid_ = current_lid_;
        first_display_lid_ = current_lid_;
        lnes_cache_.insert(current_lid_, line_type(
                current_lid_, EMPTY, EMPTY, &lnes_cache_, &chars_buf_cache_));
    }
    
    inline iterator begin() noexcept
    {
        return iterator(first_lid_, first_display_lid_, &lnes_cache_);
    }
    
    inline iterator end() noexcept
    {
        return iterator(EMPTY, EMPTY, &lnes_cache_);
    }
    
    std::pair<std::size_t, std::size_t> get_cursor_position()
    {
        return std::make_pair(current_lid_, loffset_);
    }
    
    void handle_command(file_editor_command cmd)
    {
        switch (cmd)
        {
            case file_editor_command::NEWLINE:
                insert_character(10);
                break;
                
            case file_editor_command::BACKSPACE:
                handle_backspace();
                break;
                
            case file_editor_command::GO_LEFT:
                handle_go_left();
                break;
    
            case file_editor_command::GO_RIGHT:
                handle_go_right();
                break;
            
            case file_editor_command::GO_UP:
                handle_go_up();
                break;
    
            case file_editor_command::GO_DOWN:
                handle_go_down();
                break;
        }
    }
    
    void insert_character(char_type ch)
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        current_lne.insert_character(ch, loffset_);
        
        for (auto&& it = iterator(first_lid_, current_lid_, &lnes_cache_) + 1; !it.end(); ++it)
        {
            it->set_cboffset(it->get_cboffset() + 1);
        }
        
        if (ch == 10)
        {
            current_lne.set_n_chars(loffset_ + 1);
            
            lid_t new_lid = lnes_cache_.get_new_lid();
            
            lnes_cache_.insert(new_lid, line_type(
                    new_lid, current_lne.get_lid(), current_lne.get_nxt(),
                    &lnes_cache_, &chars_buf_cache_));
            
            if (current_lne.get_nxt() != EMPTY)
            {
                line_type& nxt_lne = lnes_cache_.get_line(current_lne.get_nxt());
                nxt_lne.set_prev(new_lid);
            }
            
            current_lne.set_nxt(new_lid);
            
            current_lid_ = new_lid;
            loffset_ = 0;
            ++n_lnes_;
        }
        else
        {
            ++loffset_;
        }
    }

private:
    void handle_backspace()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        
        //if (current_lne.get_prev() != EMPTY || loffset_ != 0)
        if (loffset_ > 0)
        {
            --loffset_;
            current_lne.erase_character(loffset_);
            
            for (auto&& it = iterator(first_lid_, current_lid_, &lnes_cache_) + 1; !it.end(); ++it)
            {
                it->set_cboffset(it->get_cboffset() - 1);
            }
        }
    }
    
    void handle_go_left()
    {
        if (loffset_ > 0)
        {
            --loffset_;
        }
    }
    
    void handle_go_right()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        if (loffset_ - 1 < current_lne.get_n_chars())
        {
            ++loffset_;
        }
    }
    
    void handle_go_up()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        lid_t prev_lid = current_lne.get_prev();
    
        if (prev_lid != EMPTY)
        {
            current_lid_ = prev_lid;
            loffset_ = 0;
        }
    }
    
    void handle_go_down()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        lid_t nxt_lid = current_lne.get_nxt();
    
        if (nxt_lid != EMPTY)
        {
            current_lid_ = nxt_lid;
            loffset_ = 0;
        }
    }

private:
    lines_cache_type lnes_cache_;
    
    characters_buffer_cache_type chars_buf_cache_;
    
    lid_t first_lid_;
    
    lid_t current_lid_;
    
    lid_t first_display_lid_;
    
    loffset_t loffset_;
    
    std::size_t n_lnes_;
};


using file_editor = basic_file_editor<char16_t, 8192, 160, 4096, std::allocator<int>>;


}
}


#endif
