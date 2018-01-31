//
// Created by Killian on 19/01/18.
//

#ifndef COEDIT_TUI_CURSES_INTERFACE_HPP
#define COEDIT_TUI_CURSES_INTERFACE_HPP

#include <curses.h>

#include "../core/basic_file_editor.hpp"


namespace coedit {
namespace tui {


template<typename TpChar, std::size_t LINES_CACHE_SIZE, typename TpAllocator = std::allocator<int>>
class curses_interface
{
public:
    using char_type = TpChar;
    
    template<typename T>
    using allocator_type = typename TpAllocator::template rebind<T>::other;
    
    using file_editor_type = core::basic_file_editor<char_type, LINES_CACHE_SIZE, allocator_type<int>>;
    
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
        bool chr_read;
        bool exit = false;
    
        while (!exit)
        {
            chr_read = false;
        
            while ((inpt = wgetch(win_)) != ERR)
            {
                if (inpt == 48)
                {
                    exit = true;
                    goto execution_finish;
                }
            
                wprintw(win_, "%c", inpt);
                chr_read = true;
            }
        
            if (chr_read)
            {
                wrefresh(win_);
            }
        
            ksys::nanosleep(0, 1'000'000);
        }
    
        execution_finish:
    
        end_curses();
    
        ksys::kbhit("Curses terminated...\n");
    
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


template<class TpChar, std::size_t LINES_CACHE_SIZE, class TpAllocator>
curses_interface(core::basic_file_editor<TpChar, LINES_CACHE_SIZE, TpAllocator>*, std::uint16_t)
        -> curses_interface<TpChar, LINES_CACHE_SIZE, TpAllocator>;


}
}


#endif
