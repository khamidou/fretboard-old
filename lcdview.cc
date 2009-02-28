// lcdview.cc
//
//    gtk-- class for LCD-Display
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


#include "lcdview.h"

LCDView::LCDView(const char * str = 0)
{
    i_LCD_num = 11;
    i_string_maxlen = 1000;
    i_string = new char [i_string_maxlen];
    set( str );
        
    i_col_bg. set( "black" );      get_colormap()->alloc_color( i_col_bg  );
    i_col_LED.set( "red"   );      get_colormap()->alloc_color( i_col_LED );
        
    i_rect[ 0].x =  0.5;  i_rect[ 0].y =  0.0;   // |~|
    i_rect[ 0].w =  5.0;  i_rect[ 0].h =  1.0;
    i_rect[ 1].x =  1.5;  i_rect[ 1].y =  6.0;   // |-|
    i_rect[ 1].w =  3.0;  i_rect[ 1].h =  1.0;
    i_rect[ 2].x =  0.5;  i_rect[ 2].y = 12.0;   // |_|
    i_rect[ 2].w =  5.0;  i_rect[ 2].h =  1.0;
    i_rect[ 3].x =  0.0;  i_rect[ 3].y =  1.5;   // |_
    i_rect[ 3].w =  1.0;  i_rect[ 3].h =  4.75;
    i_rect[ 4].x =  5.0;  i_rect[ 4].y =  1.5;   //  _|
    i_rect[ 4].w =  1.0;  i_rect[ 4].h =  4.75;
    i_rect[ 5].x =  0.0;  i_rect[ 5].y =  6.75;  // |~
    i_rect[ 5].w =  1.0;  i_rect[ 5].h =  4.75;
    i_rect[ 6].x =  5.0;  i_rect[ 6].y =  6.75;  //  ~|
    i_rect[ 6].w =  1.0;  i_rect[ 6].h =  4.75;
    i_rect[ 7].x =  2.5;  i_rect[ 7].y =  4.5;
    i_rect[ 7].w =  1.0;  i_rect[ 7].h =  1.0;
    i_rect[ 8].x =  2.5;  i_rect[ 8].y = 10.5;
    i_rect[ 8].w =  1.0;  i_rect[ 8].h =  1.0;
    i_rect[ 9].x =  2.5;  i_rect[ 9].y =  1.5;
    i_rect[ 9].w =  1.0;  i_rect[ 9].h =  1.0;
    i_rect[10].x =  2.5;  i_rect[10].y =  7.5;
    i_rect[10].w =  1.0;  i_rect[10].h =  1.0;

    for(int i=0;i<128;i++) i_code[i]=0;
        
    //            -   _       |_  _|    _   _     .           '
    //                    -            |     |          .           '
//  i_code[??]  = 1 + 2 + 4 + 8 + 16 + 32 + 64 + 128 + 256 + 512 + 1024
    i_code['0'] = 1 +     4 + 8 + 16 + 32 + 64              ;
    i_code['1'] =                 16 +      64              ;
    i_code['2'] = 1 + 2 + 4 +     16 + 32                   ;
    i_code['3'] = 1 + 2 + 4 +     16 +      64              ;
    i_code['4'] = 0 + 2 +     8 + 16 +      64              ;
    i_code['5'] = 1 + 2 + 4 + 8 +           64              ;
    i_code['6'] = 1 + 2 + 4 + 8 +      32 + 64              ;
    i_code['7'] = 1 +             16 +      64              ;
    i_code['8'] = 1 + 2 + 4 + 8 + 16 + 32 + 64              ;
    i_code['9'] = 1 + 2 + 4 + 8 + 16 +      64              ;
    i_code[' '] = 0                                         ;
    i_code['.'] =                                      256  ;
    i_code[','] =                                      256  ;
    i_code[':'] =                                128             + 1024 ;
    i_code['A'] = 1 + 2 +     8 + 16 + 32 + 64              ;
    i_code['B'] =     2 + 4 + 8 +      32 + 64              ;
    i_code['C'] = 1 +     4 + 8 +      32                   ;
    i_code['D'] =     2 + 4 +     16 + 32 + 64              ;
    i_code['E'] = 1 + 2 + 4 + 8 +      32                   ;
    i_code['F'] = 1 + 2 +     8 +      32                   ;
    i_code['G'] = 1 +     4 + 8 +      32 + 64              ;
    i_code['H'] =     2 +     8 + 16 + 32 + 64              ;
    i_code['I'] =                           64 + 128        ;
    i_code['J'] =         4 +     16 + 32 + 64              ;
    i_code['K'] = 0                                         ;
    i_code['L'] =         4 + 8 +      32                   ;
    i_code['M'] =     2 +              32 + 64                   + 1024 ;
    i_code['N'] =     2 +              32 + 64              ;
    i_code['O'] = 1 +     4 + 8 + 16 + 32 + 64              ;
    i_code['F'] = 1 + 2 +     8 + 16 + 32                   ;
    i_code['Q'] = 1 +     4 + 8 + 16 + 32 + 64       + 256  ;
    i_code['R'] =     2 +              32                   ;
    i_code['S'] = 1 + 2 + 4 + 8 +           64              ;
    i_code['T'] =     2 + 4 + 8 +      32                   ;
    i_code['U'] =         4 + 8 + 16 + 32 + 64              ;
    i_code['V'] = 0                                         ;
    i_code['W'] =         4 + 8 + 16 + 32 + 64       + 256  ;
    i_code['X'] = 0                                         ;
    i_code['Y'] =     2 + 4 + 8 + 16 +      64              ;
    i_code['Z'] = 1 + 2 + 4 +     16 + 32                   ;
    i_code['-'] =     2                                     ;
    i_code['+'] =     2                        +128             + 1024 ;
    i_code['+'] =     2                        +128             + 1024 ;
        
    for(int i='a';i<'z';i++) i_code[i] = i_code[i-'a'+'A'];
}
    
LCDView::~LCDView()
{
    delete [] i_string;
}
    
    
void LCDView::set( const char * str )
{
    for(int i=0;(i_string[i]=str[i])!=0 && i<i_string_maxlen;i++);
    i_string[i_string_maxlen-1]=0;
    invalidate();
}

void LCDView::invalidate()
{
    // Invalidate the whole window
    if (is_realized())
    {
	Gdk::Rectangle rect = get_allocation();
	rect.set_x(0);
	rect.set_y(0);
	// The gtkmm bindings won't let you call invalidate_rect
	// with a NULL argument to clear the whole window, so you
	// have to jump through these hoops
	get_window()->invalidate_rect(rect, false);
    }
}
    
bool LCDView::on_expose_event(GdkEventExpose* event)
{
    int x,y, w,h;
    int x0,y0;
    int i,j;
    double scale;
    int actcode;

    scale = (double)get_height() / 15.0;
        
    if (!i_draw_gc)
    {
	Glib::RefPtr<Gdk::Window> win = get_window();
	i_draw_gc = Gdk::GC::create( win );
	get_colormap()->alloc_color(i_col_LED);
	get_colormap()->alloc_color(i_col_bg);
    }
        
    i_draw_gc->set_foreground(i_col_LED);
    y0 = (int)( 1.0*scale + 0.5 );
    for(i=0;i_string[i]!=0;i++);
    x0 = get_width() - (int)( 8.5 * scale + 0.5 );
    for(i--;i>=0;i--){
	actcode = i_code[ (int) i_string[i] ];
	for( j=0; j<i_LCD_num; j++ ){
	    if( actcode & (1<<j) ){
		x = x0 + (int)( scale * i_rect[j].x + 0.5 );
		y = y0 + (int)( scale * i_rect[j].y + 0.5 );
		w = (int)( scale * i_rect[j].w + 0.5 );
		h = (int)( scale * i_rect[j].h + 0.5 );
		get_window()->draw_rectangle( i_draw_gc, true, x,y,w,h );
	    }
	}
	x0 -= (int)( 8.5 * scale + 0.5 );
    }

    return false;
}

void LCDView::on_realize()
{
    Gtk::DrawingArea::on_realize();

    get_window()->set_background( i_col_bg );
}
