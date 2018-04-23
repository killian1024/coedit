//
// Created by Killian on 19/01/18.
//

#ifndef COEDIT_CORE_BASIC_FILE_EDITOR_HPP
#define COEDIT_CORE_BASIC_FILE_EDITOR_HPP

#include <sys/user.h>
#include <memory>

#include <kboost/kboost.hpp>

#include "basic_character_buffer_cache.hpp"
#include "basic_id_buffer_cache.hpp"
#include "basic_line_cache.hpp"
#include "core_exception.hpp"
#include "cursor_position.hpp"
#include "file_editor_command.hpp"
#include "fundamental_types.hpp"
#include "newline_format.hpp"


namespace coedit {
namespace core {


template<
        typename TpChar,
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        std::size_t LINE_CACHE_SIZE,
        std::size_t LINE_ID_BUFFER_SIZE,
        std::size_t LINE_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
class basic_file_editor
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using characters_buffer_cache_type = basic_character_buffer_cache<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    using line_type = basic_line<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            LINE_CACHE_SIZE,
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    using lines_cache_type = basic_line_cache<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            LINE_CACHE_SIZE,
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE,
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
            cur_ = current_lne.get_next();
            
            return *this;
        }
        
        self_type& operator --() noexcept override
        {
            line_type& current_lne = lnes_cache_->get_line(cur_);
            cur_ = current_lne.get_previous();
            
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
            return &lnes_cache_->get_line(cur_);
        }
        
        template<
                typename TpChar__,
                std::size_t CHARACTER_BUFFER_SIZE__,
                std::size_t CHARACTER_BUFFER_CACHE_SIZE__,
                std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE__,
                std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE__,
                std::size_t LINE_CACHE_SIZE__,
                std::size_t LINE_ID_BUFFER_SIZE__,
                std::size_t LINE_ID_BUFFER_CACHE_SIZE__,
                typename TpAllocator__
        >
        friend class basic_file_editor;
    
    protected:
        node_type cur_;
    
        lines_cache_type* lnes_cache_;
    };
    
    basic_file_editor(newline_format newl_format)
            : cb_cache_(cur_eid_)
            , l_cache_(&cb_cache_, cur_eid_)
            , first_lid_(EMPTY)
            , current_lid_(EMPTY)
            , cursor_pos_({0, 0})
            , first_display_lid_(EMPTY)
            , n_lnes_(1)
            , newl_format_(newl_format)
            , eid_(cur_eid_)
    {
        cur_eid_ = klow::add(cur_eid_, 1);
        
        auto it_lne = l_cache_.insert_first_line();
        current_lid_ = it_lne->get_lid();
        first_lid_ = current_lid_;
        first_display_lid_ = current_lid_;
    }
    
    inline iterator begin() noexcept
    {
        return iterator(first_lid_, &l_cache_);
    }
    
    inline iterator end() noexcept
    {
        return iterator(EMPTY, &l_cache_);
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
            line_type& current_lne = l_cache_.get_line(current_lid_);
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
        auto it_lne = l_cache_.insert_line_after(current_lid_, cursor_pos_.loffset, newl_format_);
        
        current_lid_ = it_lne->get_lid();
        cursor_pos_.loffset = 0;
        ++n_lnes_;
        ++cursor_pos_.coffset;
        
        return true;
    }
    
    bool handle_backspace()
    {
        line_type& current_lne = l_cache_.get_line(current_lid_);
        lid_t prev_lid = current_lne.get_previous();
        
        if (cursor_pos_.loffset > 0)
        {
            --cursor_pos_.loffset;
            current_lne.erase_character(cursor_pos_.loffset);
            
            return true;
        }
        else if (prev_lid != EMPTY)
        {
            line_type& previous_lne = l_cache_.get_line(prev_lid);
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
        line_type& current_lne = l_cache_.get_line(current_lid_);
        
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
        line_type& current_lne = l_cache_.get_line(current_lid_);
        
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
        line_type& current_lne = l_cache_.get_line(current_lid_);
        lid_t prev_lid = current_lne.get_previous();
    
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
        line_type& current_lne = l_cache_.get_line(current_lid_);
        lid_t nxt_lid = current_lne.get_next();
    
        if (nxt_lid != EMPTY)
        {
            current_lid_ = nxt_lid;
            cursor_pos_.loffset = 0;
            
            // todo : get window size to check
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
        line_type& current_lne = l_cache_.get_line(current_lid_);
        bool done = false;
        
        while (current_lne.can_go_right(cursor_pos_.loffset))
        {
            ++cursor_pos_.loffset;
            done = true;
        }
        
        return done;
    }

private:
    characters_buffer_cache_type cb_cache_;
    
    lines_cache_type l_cache_;
    
    lid_t first_lid_;
    
    lid_t current_lid_;
    
    cursor_position cursor_pos_;
    
    lid_t first_display_lid_;
    
    std::size_t n_lnes_;
    
    newline_format newl_format_;
    
    eid_t eid_;
    
    static eid_t cur_eid_;
};


using file_editor = basic_file_editor<
        char16_t,
        PAGE_SIZE / sizeof(char16_t),
        320,
        128,
        1024,
        16384,
        128,
        4096,
        std::allocator<int>
>;


template<
        typename TpChar,
        std::size_t CHARACTER_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_CACHE_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_SIZE,
        std::size_t CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        std::size_t LINE_CACHE_SIZE,
        std::size_t LINE_ID_BUFFER_SIZE,
        std::size_t LINE_ID_BUFFER_CACHE_SIZE,
        typename TpAllocator
>
std::size_t basic_file_editor<
        TpChar,
        CHARACTER_BUFFER_SIZE,
        CHARACTER_BUFFER_CACHE_SIZE,
        CHARACTER_BUFFER_ID_BUFFER_SIZE,
        CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
        LINE_CACHE_SIZE,
        LINE_ID_BUFFER_SIZE,
        LINE_ID_BUFFER_CACHE_SIZE,
        TpAllocator
>::cur_eid_ = 0;


}
}


#endif
