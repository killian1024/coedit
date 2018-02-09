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
        std::size_t LINES_CACHE_SIZE,
        std::size_t CHARACTERS_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE,
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
            LINES_CACHE_SIZE,
            CHARACTERS_CACHE_SIZE,
            CHARACTERS_BUFFER_SIZE,
            allocator_type<int>
    >;
    
    curses_interface(file_editor_type* file_editr, std::uint16_t fps)
            : file_editr_(file_editr)
            , win_(nullptr)
            , fps_(fps)
    {
    }
    
    int execute()
    {
        init_curses();
    
        int inpt;
        bool needs_refresh;
    
        while (true)
        {
            needs_refresh = false;
        
            while ((inpt = wgetch(win_)) != ERR)
            {
                switch(inpt)
                {
                    case 48:
                        goto execution_finish;
    
                    case KEY_ENTER:
                        file_editr_->handle_command(core::fec_t::NEWLINE);
                        needs_refresh = true;
                        break;
    
                    case KEY_BACKSPACE:
                        file_editr_->handle_command(core::fec_t::BACKSPACE);
                        needs_refresh = true;
                        break;
    
                    case KEY_LEFT:
                        file_editr_->handle_command(core::fec_t::GO_LEFT);
                        needs_refresh = true;
                        break;
    
                    case KEY_RIGHT:
                        file_editr_->handle_command(core::fec_t::GO_RIGHT);
                        needs_refresh = true;
                        break;
    
                    case KEY_UP:
                        file_editr_->handle_command(core::fec_t::GO_UP);
                        needs_refresh = true;
                        break;
    
                    case KEY_DOWN:
                        file_editr_->handle_command(core::fec_t::GO_DOWN);
                        needs_refresh = true;
                        break;
    
                    case KEY_HOME:
                        file_editr_->handle_command(core::fec_t::HOME);
                        needs_refresh = true;
                        break;
    
                    case KEY_END:
                        file_editr_->handle_command(core::fec_t::END);
                        needs_refresh = true;
                        break;
    
                    default:
                        file_editr_->insert_character(inpt);
                        needs_refresh = true;
                        break;
                }
            }
        
            if (needs_refresh)
            {
                std::size_t i;
                std::size_t j;
                auto cursr = file_editr_->get_cursor_position();
                
                clear();
                
                i = 0;
                for (auto& line : *file_editr_)
                {
                    j = 0;
                    for (auto& ch : line)
                    {
                        mvwprintw(win_, i, j, "%c", ch);
                        ++j;
                    }
                    
                    ++i;
                }
    
                wmove(win_, cursr.coffset, cursr.loffset);
                wrefresh(win_);
            }
        
            ksys::nanosleep(0, 1'000'000);
        }
    
        execution_finish:
    
        end_curses();
    
        return 0;
    }

private:
    void init_curses()
    {
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
        
        // El cursor_offset físico estará posicionado en el mismo lugar que el cursor_offset lógico después de un
        //  refresh.
        leaveok(win_, false);
    
        // Verificar si la terminal soporta la utilización de colores.
        if(has_colors())
        {
            // Inicializar la utilización de colores.
            start_color();
        }
    }
    
    void end_curses()
    {
        // Terminación del modo curses.
        endwin();
    }
    
private:
    file_editor_type* file_editr_;
    
    WINDOW* win_;
    
    std::uint16_t fps_;
};


template<
        typename TpChar,
        std::size_t LINES_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_CACHE_SIZE,
        std::size_t CHARACTERS_BUFFER_SIZE,
        typename TpAllocator
>
curses_interface(
        core::basic_file_editor<
                TpChar,
                LINES_CACHE_SIZE,
                CHARACTERS_BUFFER_CACHE_SIZE,
                CHARACTERS_BUFFER_SIZE,
                TpAllocator
        >*,
        std::uint16_t
) -> curses_interface<
        TpChar,
        LINES_CACHE_SIZE,
        CHARACTERS_BUFFER_CACHE_SIZE,
        CHARACTERS_BUFFER_SIZE,
        TpAllocator
>;


}
}


#endif
