//
// Created by Killian on 20/01/18.
//

#ifndef COEDIT_CORE_BASIC_LINE_HPP
#define COEDIT_CORE_BASIC_LINE_HPP

#include <memory>

#include <kboost/kboost.hpp>

#include "basic_character_buffer_cache.hpp"
#include "basic_file_editor.hpp"
#include "basic_line_cache.hpp"
#include "core_exception.hpp"
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
        typename TpAllocator
>
class basic_character_buffer;


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
class basic_file_editor;


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
class basic_line
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
    
    using file_editor_type = basic_file_editor<
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
    
    // todo : Implement this class with a more efficient logic.
    class iterator : public kcontain::i_mutable_iterator<char_type, iterator>
    {
    public:
        iterator() noexcept
                : lne_(nullptr)
                , lne_len_(0)
                , cur_loffset_(0)
        {
        }
    
        iterator(line_type* lne) noexcept
                : lne_(lne)
                , lne_len_(lne->get_line_length())
                , cur_loffset_(0)
        {
            char_type cur_ch;
    
            if (lne_len_ == 0 || (cur_ch = (*lne_)[0], cur_ch == LF) || cur_ch ==  CR)
            {
                reset_iterator();
            }
        }
    
        iterator& operator ++() noexcept override
        {
            if (cur_loffset_ + 1 >= lne_len_)
            {
                reset_iterator();
            }
            else
            {
                ++cur_loffset_;
            }
            
            return *this;
        }
    
        iterator& operator --() noexcept override
        {
            if (cur_loffset_ == 0)
            {
                reset_iterator();
            }
            else
            {
                --cur_loffset_;
            }
            
            return *this;
        }
        
        bool operator ==(const iterator& rhs) const noexcept override
        {
            return lne_ == rhs.lne_ &&
                   lne_len_ == rhs.lne_len_ &&
                   cur_loffset_ == rhs.cur_loffset_;
        }
        
        bool end() const noexcept override
        {
            return lne_ == nullptr &&
                   lne_len_ == 0 &&
                   cur_loffset_ == 0;
        }
    
        char_type& operator *() noexcept override
        {
            return (*lne_)[cur_loffset_];
        }
    
        char_type* operator ->() noexcept override
        {
            return &(*lne_)[cur_loffset_];
        }

    protected:
        void reset_iterator() noexcept
        {
            lne_ = nullptr;
            lne_len_ = 0;
            cur_loffset_ = 0;
        }
    
    protected:
        line_type* lne_;
    
        loffset_t lne_len_;
        
        loffset_t cur_loffset_;
    };
    
    // todo : Make this class like lazy_terminal_iterator.
    class terminal_iterator : public kcontain::i_mutable_iterator<char_type, terminal_iterator>
    {
    public:
        terminal_iterator() noexcept
                : lne_(nullptr)
                , lne_len_(0)
                , cur_loffset_(0)
                , limit_loffset_(0)
                , term_x_sze_(0)
        {
        }
    
        terminal_iterator(
                line_type* lne,
                loffset_t first_display_character,
                loffset_t term_x_sze
        ) noexcept
                : lne_(lne)
                , lne_len_(lne->get_line_length())
                , cur_loffset_(first_display_character)
                , limit_loffset_(term_x_sze + first_display_character - 1)
                , term_x_sze_(term_x_sze)
        {
            char_type cur_ch;
    
            if (lne_->get_n_characters() == 0 || (cur_ch = (*lne_)[0], cur_ch == LF) || cur_ch ==  CR)
            {
                reset_iterator();
            }
        }
    
        terminal_iterator& operator ++() noexcept override
        {
            if (cur_loffset_ + 1 >= lne_len_ || cur_loffset_ >= limit_loffset_)
            {
                reset_iterator();
            }
            else
            {
                ++cur_loffset_;
            }
            
            return *this;
        }
    
        terminal_iterator& operator --() noexcept override
        {
            //throw invalid_operation_exception();
            static terminal_iterator dummy;
            return dummy;
        }
        
        bool operator ==(const terminal_iterator& rhs) const noexcept override
        {
            return lne_ == rhs.lne_ &&
                   lne_len_ == rhs.lne_len_ &&
                   cur_loffset_ == rhs.cur_loffset_ &&
                   limit_loffset_ == rhs.limit_loffset_ &&
                   term_x_sze_ == rhs.term_x_sze_;
        }
        
        bool end() const noexcept override
        {
            return lne_ == nullptr &&
                   lne_len_ == 0 &&
                   cur_loffset_ == 0 &&
                   limit_loffset_ == 0 &&
                   term_x_sze_ == 0;
        }
        
        char_type& operator *() noexcept override
        {
            return (*lne_)[cur_loffset_];
        }
        
        char_type* operator ->() noexcept override
        {
            return &(*lne_)[cur_loffset_];
        }
    
        coffset_t get_x_position() const noexcept
        {
            return term_x_sze_ - (limit_loffset_ + 1 - cur_loffset_);
        }

    protected:
        void reset_iterator() noexcept
        {
            lne_ = nullptr;
            lne_len_ = 0;
            cur_loffset_ = 0;
            limit_loffset_ = 0;
            term_x_sze_ = 0;
        }
    
    protected:
        line_type* lne_;
    
        loffset_t lne_len_;
    
        loffset_t cur_loffset_;
    
        loffset_t limit_loffset_;
    
        loffset_t term_x_sze_;
    };
    
    // todo : D'ont get a reference to the line, take the lid and the cache.
    class lazy_terminal_iterator
            : public kcontain::i_mutable_iterator<char_type, lazy_terminal_iterator>
    {
    public:
        lazy_terminal_iterator() noexcept
                : lne_(nullptr)
                , lne_len_(0)
                , cur_loffset_(0)
                , cur_x_pos_(0)
                , term_x_sze_(0)
                , len_printed_(0)
                , last_len_printed_(nullptr)
        {
        }
    
        lazy_terminal_iterator(
                line_type* lne,
                loffset_t first_loffset,
                loffset_t cur_x_pos,
                loffset_t term_x_sze,
                loffset_t* last_len_printed_
        ) noexcept
                : lne_(lne)
                , lne_len_(lne->get_line_length())
                , cur_loffset_(first_loffset)
                , cur_x_pos_(cur_x_pos)
                , term_x_sze_(term_x_sze)
                , len_printed_()
                , last_len_printed_(last_len_printed_)
        {
            len_printed_ = lne_len_ - (cur_loffset_ - cur_x_pos_);
            if (len_printed_ > term_x_sze)
            {
                len_printed_ = term_x_sze;
            }
            
            if ((cur_loffset_ >= lne_len_ &&
                 cur_x_pos_ >= *last_len_printed_) ||
                cur_x_pos_ >= term_x_sze_)
            {
                *last_len_printed_ = len_printed_;
                reset_iterator();
            }
        }
    
        lazy_terminal_iterator& operator ++() noexcept override
        {
            if ((cur_loffset_ + 1 >= lne_len_ &&
                 cur_x_pos_ + 1 >= *last_len_printed_) ||
                cur_x_pos_ + 1 >= term_x_sze_)
            {
                *last_len_printed_ = len_printed_;
                reset_iterator();
            }
            else
            {
                ++cur_loffset_;
                ++cur_x_pos_;
            }
            
            return *this;
        }
    
        // todo : In speed, delete the throw specifier.
        lazy_terminal_iterator& operator --() noexcept override
        {
            //throw invalid_operation_exception();
            static lazy_terminal_iterator dummy;
            return dummy;
        }
        
        bool operator ==(const lazy_terminal_iterator& rhs) const noexcept override
        {
            return lne_ == rhs.lne_ &&
                   lne_len_ == rhs.lne_len_ &&
                   cur_loffset_ == rhs.cur_loffset_ &&
                   cur_x_pos_ == rhs.cur_x_pos_ &&
                   term_x_sze_ == rhs.term_x_sze_ &&
                   len_printed_ == rhs.len_printed_ &&
                   last_len_printed_ == rhs.last_len_printed_;
        }
        
        bool end() const noexcept override
        {
            return lne_ == nullptr &&
                   lne_len_ == 0 &&
                   cur_loffset_ == 0 &&
                   cur_x_pos_ == 0 &&
                   term_x_sze_ == 0 &&
                   len_printed_ == 0 &&
                   last_len_printed_ == nullptr;
        }
    
        // todo : Make blank a static attribute.
        char_type& operator *() noexcept override
        {
            static char_type blank = 0x20;
    
            if (out_of_line() && less_than_last_length_printed())
            {
                return blank;
            }
            
            return (*lne_)[cur_loffset_];
        }
    
        // todo : Make blank a static attribute.
        char_type* operator ->() noexcept override
        {
            static char_type blank = 0x20;
    
            if (out_of_line() && less_than_last_length_printed())
            {
                return &blank;
            }
            
            return &(*lne_)[cur_loffset_];
        }
    
        coffset_t get_x_position() const noexcept
        {
            return cur_x_pos_;
        }

    protected:
        bool out_of_line() const noexcept
        {
            return cur_loffset_ >= lne_len_;
        }
        
        bool less_than_last_length_printed() const noexcept
        {
            return cur_x_pos_ < *last_len_printed_;
        }
        
        void reset_iterator() noexcept
        {
            lne_ = nullptr;
            lne_len_ = 0;
            cur_loffset_ = 0;
            cur_x_pos_ = 0;
            term_x_sze_ = 0;
            len_printed_ = 0;
            last_len_printed_ = nullptr;
        }
    
    protected:
        line_type* lne_;
        
        loffset_t lne_len_;
    
        loffset_t cur_loffset_;
        
        loffset_t cur_x_pos_;
    
        loffset_t term_x_sze_;
        
        loffset_t len_printed_;
        
        loffset_t* last_len_printed_;
    };
    
    basic_line()
            : lid_(EMPTY)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , cbid_(EMPTY)
            , cboffset_(0)
            , n_chars_(0)
            , y_pos_(0)
            , numb_(0)
            , cb_cache_(nullptr)
            , lne_cache_(nullptr)
            , file_editr_(nullptr)
    {
    }
    
    basic_line(file_editor_type* file_editr)
            : lid_(EMPTY)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , cbid_(EMPTY)
            , cboffset_(0)
            , n_chars_(0)
            , y_pos_(0)
            , numb_(0)
            , cb_cache_(nullptr)
            , lne_cache_(nullptr)
            , file_editr_(file_editr)
    {
    }
    
    basic_line(
            lid_t lid,
            character_buffer_cache_type* cb_cache,
            line_cache_type* lne_cache,
            file_editor_type* file_editr
    )
            : lid_(lid)
            , prev_(EMPTY)
            , nxt_(EMPTY)
            , cbid_(EMPTY)
            , cboffset_(0)
            , n_chars_(0)
            , y_pos_(0)
            , numb_(1)
            , cb_cache_(cb_cache)
            , lne_cache_(lne_cache)
            , file_editr_(file_editr)
    {
        auto it_cb = cb_cache->insert_first_character_buffer();
        cbid_ = it_cb->get_cbid();
    }
    
    basic_line(
            lid_t lid,
            lid_t prev,
            character_buffer_cache_type* cb_cache,
            line_cache_type* lne_cache,
            file_editor_type* file_editr
    )
            : lid_(lid)
            , prev_(prev)
            , nxt_(EMPTY)
            , cbid_(EMPTY)
            , cboffset_(0)
            , n_chars_(0)
            , y_pos_(0)
            , numb_()
            , cb_cache_(cb_cache)
            , lne_cache_(lne_cache)
            , file_editr_(file_editr)
    {
        line_type& prev_lne = lne_cache_->get_line(prev_);
        character_buffer_type& prev_cb = cb_cache_->get_character_buffer(prev_lne.cbid_);
        cboffset_t cur_cboffset = prev_lne.cboffset_ + prev_lne.n_chars_;
        character_buffer_type& cur_cb = prev_cb.get_character_buffer_for_insertion(&cur_cboffset);
        
        prev_lne.link_line_to(*this);
        
        cbid_ = cur_cb.get_cbid();
        cboffset_ = cur_cboffset;
        
        if (nxt_ == EMPTY && cur_cb.get_size() - 1 < cboffset_)
        {
            n_chars_ = 0;
        }
        else
        {
            n_chars_ = cur_cb.get_line_length(cboffset_);
        }
        
        update_number_with_previous_line();
    }
    
    basic_line(
            const std::filesystem::path& l_path,
            character_buffer_cache_type* cb_cache,
            line_cache_type* l_cache,
            file_editor_type* file_editr
    )
            : cb_cache_(cb_cache)
            , lne_cache_(l_cache)
            , file_editr_(file_editr)
    {
        std::ifstream ifs;
        
        ifs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ifs.open(l_path, std::ios::in | std::ios::binary);
        
        ifs.read((char*)&lid_, sizeof(lid_));
        ifs.read((char*)&prev_, sizeof(prev_));
        ifs.read((char*)&nxt_, sizeof(nxt_));
        ifs.read((char*)&cbid_, sizeof(cbid_));
        ifs.read((char*)&cboffset_, sizeof(cboffset_));
        ifs.read((char*)&n_chars_, sizeof(n_chars_));
        ifs.read((char*)&y_pos_, sizeof(y_pos_));
        ifs.read((char*)&numb_, sizeof(numb_));
        
        ifs.close();
    }
    
    inline iterator begin() noexcept
    {
        return iterator(this);
    }
    
    inline terminal_iterator begin_terminal() noexcept
    {
        return terminal_iterator(
                this,
                file_editr_->get_first_terminal_loffset(),
                file_editr_->get_terminal_x_size());
    }
    
    inline lazy_terminal_iterator begin_lazy_terminal() noexcept
    {
        auto& first_lazy_term_pos = file_editr_->get_first_lazy_terminal_position();
        
        auto it = lazy_terminal_iterator(
                this,
                file_editr_->get_first_terminal_loffset() +
                        first_lazy_term_pos.loffset,
                first_lazy_term_pos.loffset,
                file_editr_->get_terminal_x_size(),
                &file_editr_->get_terminal_lines_length()[y_pos_]);
    
        first_lazy_term_pos.loffset = 0;
        
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
    
    void insert_character(char_type ch, loffset_t loffset)
    {
        using cbtot_t = typename character_buffer_type::operation_types;
        
        if (loffset > n_chars_)
        {
            throw line_overflow_exception();
        }
    
        if (loffset > 0)
        {
            auto aux = (*this)[loffset - 1];
            
            if (aux == LF || (aux == CR && ch != LF))
            {
                throw line_overflow_exception();
            }
        }
        
        character_buffer_type* cur_cb = &cb_cache_->get_character_buffer(cbid_);
        line_type* cur_lne;
        
        auto op_done = cur_cb->insert_character(ch, cboffset_ + loffset);
        
        if (ch == LF || ch == CR)
        {
            n_chars_ = loffset + 1;
        }
        else
        {
            ++n_chars_;
        }
        
        if (op_done.types.is_set(cbtot_t::CHARACTER_BUFFER_INSERTED))
        {
            for (cur_lne = get_first_line_in_character_buffer(*this);
                 cur_lne->cbid_ == cur_cb->get_cbid();
                 cur_lne = &lne_cache_->get_line(cur_lne->nxt_))
            {
                if (cur_lne->cboffset_ > cur_cb->get_size())
                {
                    cur_lne->cbid_ = cur_cb->get_next();
                    cur_lne->cboffset_ -= cur_cb->get_size();
                }
                
                if (cur_lne->nxt_ == EMPTY)
                {
                    break;
                }
            }
        }
        else
        {
            for (cur_lne = this; cur_lne->nxt_ != EMPTY; )
            {
                cur_lne = &lne_cache_->get_line(cur_lne->nxt_);
                if (cur_lne->cbid_ != op_done.cbid)
                {
                    break;
                }

                cur_lne->cboffset_ += 1;
            }
        }
    }
    
    // TODO(killian.poulaud@etu.upmc.fr): This method is the evil, but you can do it better, good luck.
    void erase_character(loffset_t loffset)
    {
        using cbtot_t = typename character_buffer_type::operation_types;
        
        if (loffset >= n_chars_)
        {
            throw line_overflow_exception();
        }
        
        character_buffer_type* cur_cb;
        line_type* cur_lne;
        cbid_t old_nxt_cbid;
        cbid_t old_prev_cbid;
        std::size_t old_cb_sze;
        cbid_t old_nxt_nxt_cbid = EMPTY;
        cbid_t old_prev_prev_cbid = EMPTY;
        
        cboffset_t real_cboffset = cboffset_ + loffset;
        cur_cb = &cb_cache_->get_character_buffer(cbid_);
        cur_cb = &cur_cb->get_character_buffer(&real_cboffset);
        old_nxt_cbid = cur_cb->get_next();
        old_prev_cbid = cur_cb->get_previous();
        old_cb_sze = cur_cb->get_size();
        
        if (old_nxt_cbid != EMPTY)
        {
            old_nxt_nxt_cbid = cb_cache_->get_character_buffer(old_nxt_cbid).get_next();
        }
        if (old_prev_cbid != EMPTY)
        {
            old_prev_prev_cbid = cb_cache_->get_character_buffer(old_prev_cbid).get_previous();
        }
        
        auto op_done = cur_cb->erase_character(real_cboffset);
        character_buffer_type& op_cb = cb_cache_->get_character_buffer(op_done.cbid);
        --n_chars_;
    
        if (op_done.types.is_set(cbtot_t::CHARACTER_BUFFER_DEFRAGMENTED))
        {
            if (op_done.types.is_set(cbtot_t::CHARACTER_BUFFER_MERGED_WITH_NEXT))
            {
                for (cur_lne = this;
                     cur_lne->cbid_ != old_nxt_nxt_cbid;
                     cur_lne = &lne_cache_->get_line(cur_lne->nxt_))
                {
                    if (cur_lne->cbid_ == op_done.cbid)
                    {
                        if (cur_lne->cboffset_ > op_done.cboffset)
                        {
                            cur_lne->cboffset_ -= 1;
                        }
                    }
                    
                    if (cur_lne->cbid_ == old_nxt_cbid)
                    {
                        cur_lne->cbid_ = op_done.cbid;
                        cur_lne->cboffset_ += old_cb_sze - 1;
                    }
                    
                    if (cur_lne->nxt_ == EMPTY)
                    {
                        break;
                    }
                }
            }
            else if (op_done.types.is_set(cbtot_t::CHARACTER_BUFFER_MERGED_WITH_PREVIOUS))
            {
                cboffset_t old_prev_cb_sze = cur_cb->get_size() - old_cb_sze;
                cur_lne = get_first_line_in_character_buffer(*this);
                
                if (cur_lne->cbid_ == op_done.cbid)
                {
                    do
                    {
                        cur_lne = &lne_cache_->get_line(cur_lne->prev_);
                        
                    } while (cur_lne->prev_ != EMPTY && cur_lne->cbid_ == old_prev_cbid);
                }
    
                for (;
                     cur_lne->cbid_ != old_nxt_cbid;
                     cur_lne = &lne_cache_->get_line(cur_lne->nxt_))
                {
                    if (cur_lne->cbid_ == op_done.cbid)
                    {
                        if (cur_lne->cboffset_ > op_done.cboffset)
                        {
                            cur_lne->cboffset_ -= 1;
                        }
                        
                        cur_lne->cboffset_ += old_prev_cb_sze;
                    }
        
                    if (cur_lne->cbid_ == old_prev_cbid)
                    {
                        cur_lne->cbid_ = op_done.cbid;
                    }
        
                    if (cur_lne->nxt_ == EMPTY)
                    {
                        break;
                    }
                }
            }
            else if (op_done.types.is_set(cbtot_t::CHARACTER_BUFFER_BALANCED_WITH_NEXT))
            {
                cboffset_t n_moved = cur_cb->get_size() - old_cb_sze;
                
                for (cur_lne = this;
                     cur_lne->cbid_ != old_nxt_nxt_cbid;
                     cur_lne = &lne_cache_->get_line(cur_lne->nxt_))
                {
                    if (cur_lne->cbid_ == op_done.cbid)
                    {
                        if (cur_lne->cboffset_ > op_done.cboffset)
                        {
                            cur_lne->cboffset_ -= 1;
                        }
                    }
        
                    if (cur_lne->cbid_ == old_nxt_cbid)
                    {
                        if (cur_lne->cboffset_ < n_moved)
                        {
                            cur_lne->cbid_ = op_done.cbid;
                            cur_lne->cboffset_ += old_cb_sze - 1;
                        }
                        else
                        {
                            cur_lne->cboffset_ -= n_moved;
                        }
                    }
        
                    if (cur_lne->nxt_ == EMPTY)
                    {
                        break;
                    }
                }
            }
            else if (op_done.types.is_set(cbtot_t::CHARACTER_BUFFER_BALANCED_WITH_PREVIOS))
            {
                cboffset_t n_moved = cur_cb->get_size() - old_cb_sze;
                cboffset_t prev_cb_sze =
                        cb_cache_->get_character_buffer(old_prev_cbid).get_size() - n_moved;
                
                for (cur_lne = this;
                     cur_lne->cbid_ != old_nxt_cbid;
                     cur_lne = &lne_cache_->get_line(cur_lne->nxt_))
                {
                    if (cur_lne->cbid_ == op_done.cbid)
                    {
                        if (cur_lne->cboffset_ > op_done.cboffset)
                        {
                            cur_lne->cboffset_ -= 1;
                        }
                        
                        cur_lne->cboffset_ += n_moved;
                    }
        
                    if (cur_lne->cbid_ == old_prev_cbid)
                    {
                        if (cur_lne->cboffset_ >= prev_cb_sze)
                        {
                            cur_lne->cbid_ = op_done.cbid;
                            cur_lne->cboffset_ -= prev_cb_sze;
                        }
                    }
        
                    if (cur_lne->nxt_ == EMPTY)
                    {
                        break;
                    }
                }
            }
        }
        else
        {
            for (cur_lne = this; cur_lne->nxt_ != EMPTY; )
            {
                cur_lne = &lne_cache_->get_line(cur_lne->nxt_);
                if (cur_lne->cbid_ != op_done.cbid)
                {
                    break;
                }
        
                cur_lne->cboffset_ -= 1;
            }
        }
    }
    
    void merge_with_next_line()
    {
        if (nxt_ == EMPTY)
        {
            throw invalid_operation_exception();
        }
    
        line_type& nxt_lne = lne_cache_->get_line(nxt_);
        character_buffer_type& cur_cb = cb_cache_->get_character_buffer(cbid_);
        
        // Erase the endline characters.
        if (n_chars_ - 1 > 0 && cur_cb[cboffset_ + n_chars_ - 2] == CR)
        {
            erase_character(n_chars_ - 1);
        }
        erase_character(n_chars_ - 1);
        
        // Assigning the pointers to the new target.
        n_chars_ += nxt_lne.n_chars_;
        update_number_with_next_line();
        unlink_line_to(nxt_lne);
    }
    
    bool can_go_left(loffset_t cur_pos) const noexcept
    {
        return cur_pos > 0;
    }
    
    bool can_go_right(loffset_t cur_pos) noexcept
    {
        if (cur_pos >= n_chars_)
        {
            return false;
        }
        
        char_type val = (*this)[cur_pos];
        
        return val != LF && val != CR;
    }
    
    loffset_t get_line_length()
    {
        if (cb_cache_ == nullptr)
        {
            return 0;
        }
        
        character_buffer_type& cur_cb = cb_cache_->get_character_buffer(cbid_);
        
        if (n_chars_ > 0)
        {
            if (n_chars_ - 1 > 0 && cur_cb[cboffset_ + n_chars_ - 2] == CR)
            {
                return n_chars_ - 2;
            }
            else if (cur_cb[cboffset_ + n_chars_ - 1] == LF)
            {
                return n_chars_ - 1;
            }
        }
        
        return n_chars_;
    }
    
    void store(const std::filesystem::path& l_path) const
    {
        std::ofstream ofs;
        
        ofs.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        ofs.open(l_path, std::ios::out | std::ios::binary);
    
        ofs.write((char*)&lid_, sizeof(lid_));
        ofs.write((char*)&prev_, sizeof(prev_));
        ofs.write((char*)&nxt_, sizeof(nxt_));
        ofs.write((char*)&cbid_, sizeof(cbid_));
        ofs.write((char*)&cboffset_, sizeof(cboffset_));
        ofs.write((char*)&n_chars_, sizeof(n_chars_));
        ofs.write((char*)&y_pos_, sizeof(y_pos_));
        ofs.write((char*)&numb_, sizeof(numb_));
        
        ofs.close();
    }
    
    char_type& operator [](loffset_t i)
    {
        if (i >= n_chars_)
        {
            throw line_overflow_exception();
        }
        
        character_buffer_type& cur_cb = cb_cache_->get_character_buffer(cbid_);
        
        return cur_cb[cboffset_ + i];
    }
    
    inline lid_t get_lid() const noexcept
    {
        return lid_;
    }
    
    inline lid_t get_previous() const noexcept
    {
        return prev_;
    }
    
    inline lid_t get_next() const noexcept
    {
        return nxt_;
    }
    
    inline cbid_t get_cbid() const noexcept
    {
        return cbid_;
    }
    
    inline cboffset_t get_cboffset() const noexcept
    {
        return cboffset_;
    }
    
    inline loffset_t get_n_characters() const noexcept
    {
        return n_chars_;
    }
    
    inline std::size_t get_number() const noexcept
    {
        return numb_;
    }

private:
    void link_line_to(line_type& lne_to_link)
    {
        lne_to_link.nxt_ = nxt_;
        lne_to_link.prev_ = lid_;
        
        if (nxt_ != EMPTY)
        {
            line_type& nxt_lne = lne_cache_->get_line(nxt_);
            nxt_lne.prev_ = lne_to_link.lid_;
        }
        
        nxt_ = lne_to_link.lid_;
    }
    
    void unlink_line_to(line_type& lne_to_unlink)
    {
        nxt_ = lne_to_unlink.nxt_;
        
        if (nxt_ != EMPTY)
        {
            line_type& nxt_lne = lne_cache_->get_line(nxt_);
            nxt_lne.prev_ = lid_;
        }
    }
    
    line_type* get_first_line_in_character_buffer(line_type& lne_ref)
    {
        line_type* cur_lne = &lne_ref;
        line_type* prev_lne = cur_lne;
        
        while (prev_lne->cbid_ == lne_ref.cbid_)
        {
            cur_lne = prev_lne;
            
            if (cur_lne->prev_ == EMPTY)
            {
                break;
            }
            
            prev_lne = &lne_cache_->get_line(cur_lne->prev_);
        }
        
        return cur_lne;
    }
    
    inline void set_y_position(loffset_t y_pos) noexcept
    {
        y_pos_ = y_pos;
    }
    
    bool update_number_with_previous_line()
    {
        if (prev_ != EMPTY)
        {
            line_type& prev_lne = lne_cache_->get_line(prev_);
            numb_ = prev_lne.get_number() + 1;
            
            return true;
        }
        
        return false;
    }
    
    bool update_number_with_next_line()
    {
        if (nxt_ != EMPTY)
        {
            line_type& nxt_lne = lne_cache_->get_line(nxt_);
            numb_ = nxt_lne.get_number() - 1;
            
            return true;
        }
        
        return false;
    }
    
    bool update_previous_line_number_with_current()
    {
        if (prev_ != EMPTY)
        {
            line_type& prev_lne = lne_cache_->get_line(prev_);
            prev_lne.numb_ = numb_ - 1;
            
            return true;
        }
        
        return false;
    }
    
    bool update_next_line_number_with_current()
    {
        if (nxt_ != EMPTY)
        {
            line_type& nxt_lne = lne_cache_->get_line(nxt_);
            nxt_lne.numb_ = numb_ + 1;
            
            return true;
        }
        
        return false;
    }
    
    inline void set_number(std::size_t numb) noexcept
    {
        numb_ = numb;
    }
    
private:
    lid_t lid_;
    
    lid_t prev_;
    
    lid_t nxt_;
    
    cbid_t cbid_;
    
    cboffset_t cboffset_;
    
    loffset_t n_chars_;
    
    loffset_t y_pos_;
    
    std::size_t numb_;
    
    character_buffer_cache_type* cb_cache_;
    
    line_cache_type* lne_cache_;
    
    file_editor_type* file_editr_;
    
    friend class basic_file_editor<
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
    
    friend class basic_file_editor<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            LINE_CACHE_SIZE,
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE,
            TpAllocator
            
    >::terminal_iterator;
    
    friend class basic_file_editor<
            TpChar,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            LINE_CACHE_SIZE,
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    
    >::lazy_terminal_iterator;
};


}
}


#endif
