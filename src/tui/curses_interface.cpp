//
// Created by Killian on 29/04/18.
//

#include <iomanip>
#include <sstream>

#include "curses_interface.hpp"


namespace coedit {
namespace tui {


curses_interface::curses_interface(file_editor_type* file_editr)
        : file_editr_(file_editr)
        , left_margin_(3)
        , colors_enabled_(false)
        , win_(nullptr)
{
}


void curses_interface::init()
{
    core::coffset_t term_y_sze;
    core::loffset_t term_x_sze;
    
    // Inicialización del modo curses.
    win_ = initscr();
    
    // Permite la utilización de teclas especiales.
    keypad(win_, TRUE);
    
    // Desabilita la espera de entrada estándar.
    //nodelay(win_, TRUE);
    
    // Los caracteres insertados estarán inmediatamente disponibles.
    cbreak();
    
    // Hace que la entrada del teclado no aparezca en la pantalla.
    noecho();
    
    // Definir la espera en milisegundos realizada por curses cuando ESC es encotrado.
    set_escdelay(100);
    
    // El cursor_offset físico estará posicionado en el mismo lugar que el cursor_offset lógico
    //  después de un refresh.
    leaveok(win_, false);
    
    // Verificar si la terminal soporta la utilización de colores.
    if(has_colors())
    {
        // Inicializar la utilización de colores.
        start_color();
        init_color(COLOR_WHITE, 1000, 1000, 1000);
        
        init_pair(static_cast<short>(curses_color::DEFAULT),
                  COLOR_BLACK, COLOR_WHITE);
        init_pair(static_cast<short>(curses_color::LINE_NUMBER),
                  COLOR_BLUE, COLOR_WHITE);
        
        wbkgd(win_, COLOR_PAIR(static_cast<short>(curses_color::DEFAULT)));
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
    
    file_editr_->handle_command(core::file_editor_command::CTRL_HOME);
    //if (file_editr_->is_file_loaded())
    //{
    //    print();
    //    wrefresh(win_);
    //}
}


void curses_interface::free()
{
    // Terminación del modo curses.
    endwin();
}


bool curses_interface::get_command(file_editor_command_type* cmd, char_type* ch)
{
    int inpt;
    core::coffset_t term_y_sze;
    core::loffset_t term_x_sze;
    
    *ch = 0;
    
    do
    {
        inpt = wgetch(win_);
        
        switch (inpt)
        {
            case ERR:
                *cmd = file_editor_command_type::NIL;
                break;
            
            case 27:
                *cmd = file_editor_command_type::EXIT;
                break;
            
            case KEY_RESIZE:
                getmaxyx(win_, term_y_sze, term_x_sze);
                file_editr_->set_terminal_size(term_y_sze, term_x_sze - left_margin_);
                break;
            
            case 10:
            case KEY_ENTER:
                *cmd = file_editor_command_type::NEWLINE;
                break;
            
            case KEY_BACKSPACE:
                *cmd = file_editor_command_type::BACKSPACE;
                break;
            
            case KEY_LEFT:
                *cmd = file_editor_command_type::GO_LEFT;
                break;
            
            case KEY_RIGHT:
                *cmd = file_editor_command_type::GO_RIGHT;
                break;
            
            case KEY_UP:
                *cmd = file_editor_command_type::GO_UP;
                break;
            
            case KEY_DOWN:
                *cmd = file_editor_command_type::GO_DOWN;
                break;
            
            case KEY_HOME:
                *cmd = file_editor_command_type::HOME;
                break;
            
            case KEY_END:
                *cmd = file_editor_command_type::END;
                break;
    
            case 447:
                *cmd = file_editor_command_type::CTRL_HOME;
                break;
            
            case CTRL('s'):
                *cmd = file_editor_command_type::SAVE_FILE;
                break;
            
            default:
                *cmd = file_editor_command_type::INSERT;
                *ch = static_cast<char_type>(inpt);
                break;
        }
        
    } while (inpt == KEY_RESIZE);
    
    return true;
}


void curses_interface::print()
{
    std::string cur_numb;
    std::size_t cur_n_digits;
    
    update_cursor();
    
    if (file_editr_->needs_refresh())
    {
        wclear(win_);
        
        cur_n_digits = kscalars::get_n_digits(file_editr_->get_n_lines());
        if (left_margin_ < cur_n_digits + 2)
        {
            left_margin_ = cur_n_digits + 2;
        }
        
        //for (auto it_lne = file_editr_->begin_lazy_terminal();
        //     it_lne != file_editr_->end_lazy_terminal();
        //     ++it_lne)
        for (auto it_lne = file_editr_->begin_terminal();
             it_lne != file_editr_->end_terminal();
             ++it_lne)
        {
            print_line_number(it_lne);
            
            //for (auto it_ch = it_lne->begin_lazy_terminal();
            //     it_ch != it_lne->end_lazy_terminal();
            //     ++it_ch)
            for (auto it_ch = it_lne->begin_terminal();
                 it_ch != it_lne->end_terminal();
                 ++it_ch)
            {
                mvwprintw(win_, it_lne.get_y_position(), it_ch.get_x_position() + left_margin_,
                          "%c", *it_ch);
            }
        }
    
        update_cursor();
        wrefresh(win_);
    }
}


//void curses_interface::print_line_number(typename file_editor_type::lazy_terminal_iterator& it_lne)
void curses_interface::print_line_number(typename file_editor_type::terminal_iterator& it_lne)
{
    std::stringstream sstr_lne_numb;
    auto cur_numb = it_lne->get_number();
    
    if (colors_enabled_)
    {
        attron(COLOR_PAIR(static_cast<short>(curses_color::LINE_NUMBER)));
    }
    
    if (cur_numb > 0)
    {
        sstr_lne_numb << ' ' << std::setw(left_margin_ - 2) << cur_numb << ' ';
    }
    else
    {
        sstr_lne_numb << std::setw(left_margin_) << ' ';
    }
    
    mvwprintw(win_, it_lne.get_y_position(), 0, sstr_lne_numb.str().c_str());
    
    if (colors_enabled_)
    {
        attroff(COLOR_PAIR(static_cast<short>(curses_color::LINE_NUMBER)));
    }
}


void curses_interface::update_cursor()
{
    const cursor_position_type& fle_cursor_pos = file_editr_->get_cursor_position();
    cursor_position_type term_cursor_pos;
    getyx(win_, term_cursor_pos.coffset, term_cursor_pos.loffset);
    
    if (fle_cursor_pos != term_cursor_pos)
    {
        wmove(win_, fle_cursor_pos.coffset, fle_cursor_pos.loffset + left_margin_);
    }
}


}
}
