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
#include "cursor_position.hpp"
#include "file_editor_command.hpp"
#include "fundamental_types.hpp"
#include "newline_format.hpp"


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
                : cur_(EMPTY)
                , lnes_cache_(nullptr)
        {
        }
    
        iterator(
                node_type cur,
                lines_cache_type* lnes_cache
        ) noexcept
                : cur_(cur)
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
        node_type cur_;
    
        lines_cache_type* lnes_cache_;
    };
    
    basic_file_editor(newline_format newl_format)
            : lnes_cache_()
            , chars_buf_cache_()
            , first_lid_(EMPTY)
            , current_lid_(EMPTY)
            , cursor_pos_({0, 0})
            , first_display_lid_(EMPTY)
            , n_lnes_(1)
            , newl_format_(newl_format)
    {
        current_lid_ = lnes_cache_.get_new_lid();
        first_lid_ = current_lid_;
        first_display_lid_ = current_lid_;
        lnes_cache_.insert(current_lid_, line_type(
                current_lid_, EMPTY, EMPTY, &lnes_cache_, &chars_buf_cache_));
    }
    
    inline iterator begin() noexcept
    {
        return iterator(first_lid_, &lnes_cache_);
    }
    
    inline iterator end() noexcept
    {
        return iterator(EMPTY, &lnes_cache_);
    }
    
    bool handle_command(file_editor_command cmd)
    {
        switch (cmd)
        {
            case file_editor_command::NEWLINE:
                return handle_newline();
                
            case file_editor_command::BACKSPACE:
                return handle_backspace();
                
            case file_editor_command::GO_LEFT:
                return handle_go_left();
    
            case file_editor_command::GO_RIGHT:
                return handle_go_right();
            
            case file_editor_command::GO_UP:
                return handle_go_up();
    
            case file_editor_command::GO_DOWN:
                return handle_go_down();
    
            case file_editor_command::HOME:
                return handle_home();
    
            case file_editor_command::END:
                return handle_end();
        }
    }
    
    void insert_character(char_type ch)
    {
        if (ch == LF || ch == CR)
        {
            handle_newline();
        }
        else
        {
            line_type& current_lne = lnes_cache_.get_line(current_lid_);
            current_lne.insert_character(ch, cursor_pos_.loffset);
            ++cursor_pos_.loffset;
        }
    }
    
    const cursor_position& get_cursor_position()
    {
        return cursor_pos_;
    }

private:
    bool handle_newline()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        
        switch (newl_format_)
        {
            case newline_format::UNIX:
                current_lne.insert_character(LF, cursor_pos_.loffset);
                break;
            
            case newline_format::MAC:
                current_lne.insert_character(CR, cursor_pos_.loffset);
                break;
            
            case newline_format::WINDOWS:
                current_lne.insert_character(CR, cursor_pos_.loffset);
                ++cursor_pos_.loffset;
                current_lne.insert_character(LF, cursor_pos_.loffset);
                break;
        }
        
        current_lne.set_n_chars(cursor_pos_.loffset + 1);
        
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
        cursor_pos_.loffset = 0;
        ++n_lnes_;
        ++cursor_pos_.coffset;
        
        return true;
    }
    
    bool handle_backspace()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        lid_t prev_lid = current_lne.get_prev();
        
        if (cursor_pos_.loffset > 0)
        {
            --cursor_pos_.loffset;
            current_lne.erase_character(cursor_pos_.loffset);
            
            return true;
        }
        else if (prev_lid != EMPTY)
        {
            line_type& previous_lne = lnes_cache_.get_line(prev_lid);
            auto prev_lne_length = previous_lne.get_line_length();
            
            previous_lne.merge_with_next_line();
            
            if (first_display_lid_ == current_lid_)
            {
                first_display_lid_ = prev_lid;
            }
            current_lid_ = prev_lid;
            
            --n_lnes_;
            if (cursor_pos_.coffset > 0)
            {
                --cursor_pos_.coffset;
            }
            cursor_pos_.loffset = prev_lne_length;
            
            return true;
        }
        
        return false;
    }
    
    bool handle_go_left()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        
        if (current_lne.can_go_left(cursor_pos_.loffset))
        {
            --cursor_pos_.loffset;
            return true;
        }
        else if (current_lid_ != first_lid_ && handle_go_up() && handle_end())
        {
            return true;
        }
        
        return false;
    }
    
    bool handle_go_right()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        
        if (current_lne.can_go_right(cursor_pos_.loffset))
        {
            ++cursor_pos_.loffset;
            return true;
        }
        else
        {
            if (handle_go_down())
            {
                cursor_pos_.loffset = 0;
                return true;
            }
        }
        
        return false;
    }
    
    bool handle_go_up()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        lid_t prev_lid = current_lne.get_prev();
    
        if (prev_lid != EMPTY)
        {
            current_lid_ = prev_lid;
            cursor_pos_.loffset = 0;
            
            if (cursor_pos_.coffset > 0)
            {
                --cursor_pos_.coffset;
            }
            
            return true;
        }
        
        return false;
    }
    
    bool handle_go_down()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        lid_t nxt_lid = current_lne.get_nxt();
    
        if (nxt_lid != EMPTY)
        {
            current_lid_ = nxt_lid;
            cursor_pos_.loffset = 0;
            
            // todo : get window size to check here
            if (cursor_pos_.coffset < ~0)
            {
                ++cursor_pos_.coffset;
            }
            
            return true;
        }
        
        return false;
    }
    
    bool handle_home()
    {
        cursor_pos_.loffset = 0;
        return true;
    }
    
    bool handle_end()
    {
        line_type& current_lne = lnes_cache_.get_line(current_lid_);
        bool done = false;
        
        while (current_lne.can_go_right(cursor_pos_.loffset))
        {
            ++cursor_pos_.loffset;
            done = true;
        }
        
        return done;
    }

private:
    lines_cache_type lnes_cache_;
    
    characters_buffer_cache_type chars_buf_cache_;
    
    lid_t first_lid_;
    
    lid_t current_lid_;
    
    cursor_position cursor_pos_;
    
    lid_t first_display_lid_;
    
    std::size_t n_lnes_;
    
    newline_format newl_format_;
};


//using file_editor = basic_file_editor<char16_t, 8192, 160, 4096, std::allocator<int>>;
using file_editor = basic_file_editor<char16_t, 8192, 160, 16, std::allocator<int>>;


}
}


#endif
