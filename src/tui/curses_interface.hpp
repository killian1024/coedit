//
// Created by Killian on 19/01/18.
//

#ifndef COEDIT_TUI_CURSES_INTERFACE_HPP
#define COEDIT_TUI_CURSES_INTERFACE_HPP

#include <curses.h>

#include "../core/basic_file_editor.hpp"


namespace coedit {
namespace tui {


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
class curses_interface
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using file_editor_type = core::basic_file_editor<
            char_type,
            CHARACTER_BUFFER_SIZE,
            CHARACTER_BUFFER_CACHE_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_SIZE,
            CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
            LINE_CACHE_SIZE,
            LINE_ID_BUFFER_SIZE,
            LINE_ID_BUFFER_CACHE_SIZE,
            TpAllocator
    >;
    
    using cursor_position_type = core::cursor_position;
    
    curses_interface(file_editor_type* file_editr)
            : file_editr_(file_editr)
            , left_margin_(3)
            , execution_finish_(false)
            , colors_enabled_(false)
            , win_(nullptr)
    {
    }
    
    int execute()
    {
        init_curses();
    
        while (!execution_finish_)
        {
            get_input();
            
            if (file_editr_->needs_refresh())
            {
                print_current_text();
                update_cursor();
                wrefresh(win_);
            }
            else
            {
                update_cursor();
            }
        
            ksys::nanosleep(0, 1'000'000);
        }
    
        end_curses();
    
        return 0;
    }

private:
    void init_curses()
    {
        core::coffset_t term_y_sze;
        core::loffset_t term_x_sze;
        
        // Inicialización del modo curses.
        win_ = initscr();
    
        // Permite la utilización de teclas especiales.
        keypad(win_, TRUE);
    
        // Desabilita la espera de entrada estándar.
        nodelay(win_, TRUE);
    
        // Los caracteres insertados estarán inmediatamente disponibles.
        cbreak();
    
        // Hace que la entrada del teclado no aparezca en la pantalla.
        noecho();
        
        // El cursor_offset físico estará posicionado en el mismo lugar que el cursor_offset lógico
        //  después de un refresh.
        leaveok(win_, false);
    
        // Verificar si la terminal soporta la utilización de colores.
        if(has_colors())
        {
            // Inicializar la utilización de colores.
            start_color();
            colors_enabled_ = true;
        }
    
        // Redimencionar el tamaño de la terminal.
        //wresize(win_, 25, 80);
        //resize_term(10, 20);
    
        // Establecimiento de la talla de la terminal.
        getmaxyx(win_, term_y_sze, term_x_sze);
        file_editr_->set_terminal_size(term_y_sze, term_x_sze - left_margin_);
    
        // Posicionar el cursor al inicio de la terminal.
        wmove(win_, 0, left_margin_);
    }
    
    void end_curses()
    {
        // Terminación del modo curses.
        endwin();
    }
    
    void get_input()
    {
        int inpt;
        core::coffset_t term_y_sze;
        core::loffset_t term_x_sze;
        
        while ((inpt = wgetch(win_)) != ERR)
        {
            switch (inpt)
            {
                case 48:
                    execution_finish_ = true;
                    break;
            
                case KEY_RESIZE:
                    getmaxyx(win_, term_y_sze, term_x_sze);
                    file_editr_->set_terminal_size(term_y_sze, term_x_sze - left_margin_);
                    break;
            
                case KEY_ENTER:
                    file_editr_->handle_command(core::fec_t::NEWLINE);
                    break;
            
                case KEY_BACKSPACE:
                    file_editr_->handle_command(core::fec_t::BACKSPACE);
                    break;
            
                case KEY_LEFT:
                    file_editr_->handle_command(core::fec_t::GO_LEFT);
                    break;
            
                case KEY_RIGHT:
                    file_editr_->handle_command(core::fec_t::GO_RIGHT);
                    break;
            
                case KEY_UP:
                    file_editr_->handle_command(core::fec_t::GO_UP);
                    break;
            
                case KEY_DOWN:
                    file_editr_->handle_command(core::fec_t::GO_DOWN);
                    break;
            
                case KEY_HOME:
                    file_editr_->handle_command(core::fec_t::HOME);
                    break;
            
                case KEY_END:
                    file_editr_->handle_command(core::fec_t::END);
                    break;
            
                default:
                    file_editr_->insert_character(inpt);
                    break;
            }
        }
    }
    
    void print_current_text()
    {
        //wclear(win_);
        std::string cur_numb;
    
        for (auto lne = file_editr_->begin_lazy_terminal();
             lne != file_editr_->end_lazy_terminal();
             ++lne)
        {
            cur_numb = std::to_string(lne->get_number());
            mvwprintw(win_, lne.get_y_position(), 0, cur_numb.c_str());
            
            for (auto ch = lne->begin_lazy_terminal();
                 ch != lne->end_lazy_terminal();
                 ++ch)
            {
                mvwprintw(win_, lne.get_y_position(), ch.get_x_position() + left_margin_,
                          "%c", *ch);
            }
        }
    }
    
    void update_cursor()
    {
        const cursor_position_type& fle_cursor_pos = file_editr_->get_cursor_position();
        cursor_position_type term_cursor_pos;
        getyx(win_, term_cursor_pos.coffset, term_cursor_pos.loffset);
        
        if (fle_cursor_pos != term_cursor_pos)
        {
            wmove(win_, fle_cursor_pos.coffset, fle_cursor_pos.loffset + left_margin_);
        }
    }
    
private:
    file_editor_type* file_editr_;
    
    std::size_t left_margin_;
    
    bool execution_finish_;
    
    bool colors_enabled_;
    
    WINDOW* win_;
};


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
curses_interface(
        core::basic_file_editor<
                TpChar,
                CHARACTER_BUFFER_SIZE,
                CHARACTER_BUFFER_CACHE_SIZE,
                CHARACTER_BUFFER_ID_BUFFER_SIZE,
                CHARACTER_BUFFER_ID_BUFFER_CACHE_SIZE,
                LINE_CACHE_SIZE,
                LINE_ID_BUFFER_SIZE,
                LINE_ID_BUFFER_CACHE_SIZE,
                TpAllocator
        >*
) -> curses_interface<
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


}
}


#endif
