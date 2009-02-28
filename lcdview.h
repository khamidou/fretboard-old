// lcdview.h
// gtk-- class for LCD-Display
//
//    Copyright (C) 1999  Florian Berger
//    Email: florian.berger@jk.uni-linz.ac.at
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License Version 2 as
//    published by the Free Software Foundation;
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//
//

//#include "spec_view.h"
#include <stdio.h>
#include <string.h>
#include <gtkmm/drawingarea.h>
#include <gtkmm/style.h>
#include <gdkmm/color.h>
#include <math.h>

class LCDView : public Gtk::DrawingArea
{
public:
    
    LCDView(const char * str);
    ~LCDView();
    
    void set( const char * str );
    
private:
    Gdk::Color                i_col_bg;
    Gdk::Color                i_col_LED;

    char                    * i_string;
    int                       i_string_maxlen;
    int                       i_code[128];
    int                       i_LCD_num;

    Glib::RefPtr<Gdk::GC>     i_draw_gc;
    struct{ double x,y,w,h; } i_rect[20];

    void invalidate();

protected:
    virtual bool on_expose_event(GdkEventExpose* event);
    virtual void on_realize();
};
