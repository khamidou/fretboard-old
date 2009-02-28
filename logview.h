// logview.cc
//
//    logarithmic scale for tunes
//
//    Copyright (C) 1999  Florian Berger
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

#include <gtkmm/drawingarea.h>


extern double KAMMERTON, KAMMERTON_LOG;


class LogView : public Gtk::DrawingArea
{
//    Q_OBJECT

public:

    LogView();
    ~LogView();

    enum {us_scale,us_scale_alt,german_scale,german_scale_alt};

    int  lfreq_pos(double lfreq);
    int  pos_note(int pos);
    void change_lfreq(double freq);
    void setScale(int scale);
    void set_nat_tuning(bool state);

    void invalidate();
    void invalidate_arrow();

protected:

    bool on_expose_event(GdkEventExpose* p0);
//    void draw_default_impl();
    bool on_button_press_event(GdkEventButton* p0);

private:

    bool                  nat_tuning_on; 
    int                   x0,y0,h;       
    int                   grundton;      // for natural tuning 
    double                lfreq0,lfreq1; 
    double                lfreq;         
    double                freqs[12];     
    double                lfreqs[12];    
    Gdk::Color            i_col_bg;      
    Gdk::Color            i_col_draw;    
    Gdk::Color            i_col_shad;    
    Gdk::Color            i_col_bg2;     
    Gdk::Color            i_col_pointer; 
    Gdk::Color            i_col_ticks;   
    Gdk::Color            i_col_marks;   
    Glib::RefPtr<Gdk::GC> i_GC;          

    void draw_arrow();
    void draw_it();
};
