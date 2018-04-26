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
class basic_line;


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
class basic_line_cache;


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
    
    using line_cache_type = basic_line_cache<
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
    
    template<typename T>
    using vector_type = std::vector<T, allocator_type<T>>;
    
    class iterator : public kcontain::i_mutable_iterator<line_type, iterator>
    {
    public:
        using self_type = iterator;
        
        using value_type = line_type;
        
        using node_type = lid_t;
    
        iterator() noexcept
                : cur_lid_(EMPTY)
                , lne_cache_(nullptr)
        {
        }
    
        iterator(
                node_type first_lid,
                line_cache_type* lne_cache
        ) noexcept
                : cur_lid_(first_lid)
                , lne_cache_(lne_cache)
        {
        }
        
        self_type& operator ++() noexcept override
        {
            line_type& cur_lne = lne_cache_->get_line(cur_lid_);
            cur_lid_ = cur_lne.get_next();
            
            return *this;
        }
        
        self_type& operator --() noexcept override
        {
            line_type& cur_lne = lne_cache_->get_line(cur_lid_);
            cur_lid_ = cur_lne.get_previous();
            
            return *this;
        }
        
        bool operator ==(const self_type& rhs) const noexcept override
        {
            return cur_lid_ == rhs.cur_lid_;
        }
        
        bool end() const noexcept override
        {
            return cur_lid_ == EMPTY;
        }
        
        value_type& operator *() noexcept override
        {
            return lne_cache_->get_line(cur_lid_);
        }
        
        value_type* operator ->() noexcept override
        {
            return &lne_cache_->get_line(cur_lid_);
        }
    
    protected:
        node_type cur_lid_;
    
        line_cache_type* lne_cache_;
    };
    
    class terminal_iterator : public kcontain::i_mutable_iterator<line_type, terminal_iterator>
    {
    public:
        terminal_iterator() noexcept
                : cur_lid_(EMPTY)
                , term_y_sze_(0)
                , cur_y_pos_(0)
                , lne_cache_(nullptr)
        {
        }
    
        terminal_iterator(
                lid_t first_lid,
                coffset_t term_y_sze,
                line_cache_type* lne_cache
        ) noexcept
                : cur_lid_(first_lid)
                , term_y_sze_(term_y_sze)
                , cur_y_pos_(0)
                , lne_cache_(lne_cache)
        {
        }
    
        terminal_iterator& operator ++() noexcept override
        {
            if (cur_y_pos_ + 1 >= term_y_sze_)
            {
                cur_lid_ = EMPTY;
            }
            else
            {
                line_type& cur_lne = lne_cache_->get_line(cur_lid_);
                cur_lid_ = cur_lne.get_next();
                ++cur_y_pos_;
            }
        
            return *this;
        }
    
        terminal_iterator& operator --() noexcept override
        {
            if (cur_y_pos_ == 0)
            {
                cur_lid_ = EMPTY;
            }
            else
            {
                line_type& cur_lne = lne_cache_->get_line(cur_lid_);
                cur_lid_ = cur_lne.get_previous();
                --cur_y_pos_;
            }
        
            return *this;
        }
    
        bool operator ==(const terminal_iterator& rhs) const noexcept override
        {
            return cur_lid_ == rhs.cur_lid_;
        }
    
        bool end() const noexcept override
        {
            return cur_lid_ == EMPTY;
        }
    
        line_type& operator *() noexcept override
        {
            return lne_cache_->get_line(cur_lid_);
        }
    
        line_type* operator ->() noexcept override
        {
            return &lne_cache_->get_line(cur_lid_);
        }
        
        coffset_t get_current_y_position() const noexcept
        {
            return cur_y_pos_;
        }

    protected:
        lid_t cur_lid_;
        
        coffset_t cur_y_pos_;
    
        coffset_t term_y_sze_;
    
        line_cache_type* lne_cache_;
    };
    
    class lazy_terminal_iterator
            : public kcontain::i_mutable_iterator<line_type, lazy_terminal_iterator>
    {
    public:
        lazy_terminal_iterator() noexcept
                : cur_lid_(EMPTY)
                , term_y_sze_(0)
                , cur_y_pos_(0)
                , iterte_(false)
                , last_printd_(false)
                , lne_cache_(nullptr)
        {
        }
        
        lazy_terminal_iterator(
                lid_t first_lid,
                coffset_t cur_y_pos,
                coffset_t term_y_sze,
                bool iterte,
                line_cache_type* lne_cache
        ) noexcept
                : cur_lid_(first_lid)
                , cur_y_pos_(cur_y_pos)
                , term_y_sze_(term_y_sze)
                , iterte_(iterte)
                , last_printd_(false)
                , lne_cache_(lne_cache)
        {
        }
    
        lazy_terminal_iterator& operator ++() noexcept override
        {
            if (!iterte_ || cur_y_pos_ + 1 >= term_y_sze_)
            {
                cur_lid_ = EMPTY;
            }
            else
            {
                line_type& cur_lne = lne_cache_->get_line(cur_lid_);
                lid_t nxt_lid = cur_lne.get_next();
    
                if (nxt_lid == EMPTY)
                {
                    if (last_printd_)
                    {
                        cur_lid_ = nxt_lid;
                    }
                    else
                    {
                        last_printd_ = true;
                    }
                }
                else
                {
                    cur_lid_ = nxt_lid;
                }
                
                ++cur_y_pos_;
            }
            
            return *this;
        }
    
        lazy_terminal_iterator& operator --() noexcept override
        {
            if (!iterte_ || cur_y_pos_ == 0)
            {
                cur_lid_ = EMPTY;
            }
            else
            {
                line_type& cur_lne = lne_cache_->get_line(cur_lid_);
                cur_lid_ = cur_lne.get_previous();
                --cur_y_pos_;
            }
            
            return *this;
        }
        
        bool operator ==(const lazy_terminal_iterator& rhs) const noexcept override
        {
            return cur_lid_ == rhs.cur_lid_;
        }
        
        bool end() const noexcept override
        {
            return cur_lid_ == EMPTY;
        }
        
        line_type& operator *() noexcept override
        {
            static auto last_lne_eraser = lne_cache_->insert_first_line();
            
            if (!last_printd_)
            {
                line_type& lne = lne_cache_->get_line(cur_lid_);
                lne.set_current_y_position(cur_y_pos_);
    
                return lne;
            }
            else
            {
                last_lne_eraser->set_current_y_position(cur_y_pos_);
                return *last_lne_eraser;
            }
        }
        
        line_type* operator ->() noexcept override
        {
            static auto last_lne_eraser = lne_cache_->insert_first_line();
    
            if (!last_printd_)
            {
                line_type& lne = lne_cache_->get_line(cur_lid_);
                lne.set_current_y_position(cur_y_pos_);
    
                return &lne;
            }
            else
            {
                last_lne_eraser->set_current_y_position(cur_y_pos_);
                return &(*last_lne_eraser);
            }
        }
    
        coffset_t get_current_y_position() const noexcept
        {
            return cur_y_pos_;
        }
    
    protected:
        lid_t cur_lid_;
        
        coffset_t cur_y_pos_;
    
        coffset_t term_y_sze_;
        
        bool iterte_;
        
        bool last_printd_;
        
        line_cache_type* lne_cache_;
    };
    
    basic_file_editor(newline_format newl_format)
            : eid_(cur_eid_)
            , first_lid_(EMPTY)
            , cur_lid_(EMPTY)
            , n_lnes_(1)
            , newl_format_(newl_format)
            , cursor_pos_({0, 0})
            , term_y_sze_(0)
            , term_x_sze_(0)
            , first_term_lid_(EMPTY)
            , first_lazy_term_lid_(EMPTY)
            , first_lazy_term_pos_({0, 0})
            , first_term_loffset_(0)
            , iterte_in_lazy_term_(true)
            , term_lnes_len_()
            , cb_cache_(cur_eid_)
            , lne_cache_(&cb_cache_, this)
    {
        cur_eid_ = klow::add(cur_eid_, 1);
        
        auto it_lne = lne_cache_.insert_first_line();
        cur_lid_ = it_lne->get_lid();
        first_lid_ = cur_lid_;
        first_term_lid_ = cur_lid_;
        first_lazy_term_lid_ = cur_lid_;
    }
    
    inline iterator begin() noexcept
    {
        return iterator(first_lid_, &lne_cache_);
    }
    
    inline terminal_iterator begin_terminal() noexcept
    {
        return terminal_iterator(first_term_lid_, term_y_sze_, &lne_cache_);
    }
    
    inline lazy_terminal_iterator begin_lazy_terminal() noexcept
    {
        if (first_lazy_term_lid_ == EMPTY)
        {
            reset_first_lazy_terminal_position();
            iterte_in_lazy_term_ = true;
        }
        
        auto it = lazy_terminal_iterator(
                first_lazy_term_lid_,
                first_lazy_term_pos_.coffset,
                term_y_sze_,
                iterte_in_lazy_term_,
                &lne_cache_);
    
        first_lazy_term_lid_ = EMPTY;
        iterte_in_lazy_term_ = false;
        
        return it;
    }
    
    inline iterator end() noexcept
    {
        return iterator();
    }
    
    inline terminal_iterator end_terminal() noexcept
    {
        return terminal_iterator();
    }
    
    inline lazy_terminal_iterator end_lazy_terminal() noexcept
    {
        return lazy_terminal_iterator();
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
            line_type& current_lne = lne_cache_.get_line(cur_lid_);
            current_lne.insert_character(ch, cursor_pos_.loffset);
    
            update_first_lazy_terminal_position();
            
            ++cursor_pos_.loffset;
        }
    }
    
    eid_t get_eid() const noexcept
    {
        return eid_;
    }
    
    const cursor_position& get_cursor_position() const noexcept
    {
        return cursor_pos_;
    }
    
    coffset_t get_terminal_y_size() const noexcept
    {
        return term_y_sze_;
    }
    
    loffset_t get_terminal_x_size() const noexcept
    {
        return term_x_sze_;
    }
    
    void set_terminal_size(coffset_t term_y_sze, loffset_t term_x_sze)
    {
        if (term_y_sze > term_lnes_len_.size())
        {
            term_lnes_len_.resize(term_y_sze, 0);
        }
        
        term_y_sze_ = term_y_sze;
        term_x_sze_ = term_x_sze;
    }
    
    loffset_t get_first_terminal_loffset() const noexcept
    {
        return first_term_loffset_;
    }
    
    cursor_position& get_first_lazy_terminal_position() noexcept
    {
        return first_lazy_term_pos_;
    }
    
    vector_type<loffset_t>& get_terminal_lines_length() noexcept
    {
        return term_lnes_len_;
    }
    
private:
    bool handle_newline()
    {
        lne_cache_.insert_line_after(cur_lid_, cursor_pos_.loffset, newl_format_);
    
        update_first_lazy_terminal_position();
        iterte_in_lazy_term_ = true;
        
        handle_go_down();
        handle_home();
        
        return true;
    }
    
    bool handle_backspace()
    {
        line_type& current_lne = lne_cache_.get_line(cur_lid_);
        lid_t prev_lid = current_lne.get_previous();
        
        if (cursor_pos_.loffset > 0)
        {
            --cursor_pos_.loffset;
            current_lne.erase_character(cursor_pos_.loffset);
            update_first_lazy_terminal_position();
            
            return true;
        }
        else if (prev_lid != EMPTY)
        {
            line_type& prev_lne = lne_cache_.get_line(prev_lid);
            auto prev_lne_len = prev_lne.get_line_length();
            
            prev_lne.merge_with_next_line();
            
            if (first_term_lid_ == cur_lid_)
            {
                first_term_lid_ = prev_lid;
            }
            cur_lid_ = prev_lid;
            
            --n_lnes_;
            if (cursor_pos_.coffset > 0)
            {
                --cursor_pos_.coffset;
            }
            // todo : When the line is bigger than the terminal length it doesn't work.
            cursor_pos_.loffset = prev_lne_len;
    
            if (first_term_lid_ == cur_lid_)
            {
                reset_first_lazy_terminal_position();
            }
            else
            {
                update_first_lazy_terminal_position();
            }
            iterte_in_lazy_term_ = true;
            
            return true;
        }
        
        return false;
    }
    
    bool handle_go_left()
    {
        line_type& cur_lne = lne_cache_.get_line(cur_lid_);
        
        if (cur_lne.can_go_left(cursor_pos_.loffset))
        {
            --cursor_pos_.loffset;
            return true;
        }
        else if (cur_lid_ != first_lid_ && handle_go_up() && handle_end())
        {
            return true;
        }
        
        return false;
    }
    
    bool handle_go_right()
    {
        line_type& cur_lne = lne_cache_.get_line(cur_lid_);
        
        if (cur_lne.can_go_right(cursor_pos_.loffset))
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
        line_type& cur_lne = lne_cache_.get_line(cur_lid_);
        lid_t prev_lid = cur_lne.get_previous();
    
        if (prev_lid != EMPTY)
        {
            cur_lid_ = prev_lid;
            cursor_pos_.loffset = 0;
            
            if (cursor_pos_.coffset > 0)
            {
                --cursor_pos_.coffset;
            }
            else
            {
                line_type& first_display_lne = lne_cache_.get_line(first_term_lid_);
                first_term_lid_ = first_display_lne.get_previous();
                reset_first_lazy_terminal_position();
                iterte_in_lazy_term_ = true;
            }
            
            return true;
        }
        
        return false;
    }
    
    bool handle_go_down()
    {
        line_type& cur_lne = lne_cache_.get_line(cur_lid_);
        lid_t nxt_lid = cur_lne.get_next();
    
        if (nxt_lid != EMPTY)
        {
            cur_lid_ = nxt_lid;
            cursor_pos_.loffset = 0;
            
            if (cursor_pos_.coffset + 1 < term_y_sze_)
            {
                ++cursor_pos_.coffset;
            }
            else
            {
                line_type& first_display_lne = lne_cache_.get_line(first_term_lid_);
                first_term_lid_ = first_display_lne.get_next();
                reset_first_lazy_terminal_position();
                iterte_in_lazy_term_ = true;
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
        line_type& cur_lne = lne_cache_.get_line(cur_lid_);
        bool done = false;
        
        while (cur_lne.can_go_right(cursor_pos_.loffset))
        {
            ++cursor_pos_.loffset;
            done = true;
        }
        
        return done;
    }
    
    void update_first_lazy_terminal_position()
    {
        if (first_lazy_term_lid_ == EMPTY || cursor_pos_.coffset < first_lazy_term_pos_.coffset)
        {
            first_lazy_term_pos_.coffset = cursor_pos_.coffset;
            first_lazy_term_pos_.loffset = cursor_pos_.loffset;
            first_lazy_term_lid_ = cur_lid_;
        }
        else if (cursor_pos_.coffset == first_lazy_term_pos_.coffset &&
                 cursor_pos_.loffset < first_lazy_term_pos_.loffset)
        {
            first_lazy_term_pos_.loffset = cursor_pos_.loffset;
        }
    }
    
    void reset_first_lazy_terminal_position()
    {
        first_lazy_term_lid_ = first_term_lid_;
        first_lazy_term_pos_ = {0, 0};
    };

private:
    eid_t eid_;
    
    lid_t first_lid_;
    
    lid_t cur_lid_;
    
    std::size_t n_lnes_;
    
    newline_format newl_format_;
    
    cursor_position cursor_pos_;
    
    coffset_t term_y_sze_;
    
    loffset_t term_x_sze_;
    
    lid_t first_term_lid_;
    
    lid_t first_lazy_term_lid_;
    
    cursor_position first_lazy_term_pos_;
    
    bool iterte_in_lazy_term_;
    
    // todo : update this value.
    loffset_t first_term_loffset_;
    
    vector_type<loffset_t> term_lnes_len_;
    
    characters_buffer_cache_type cb_cache_;
    
    line_cache_type lne_cache_;
    
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
