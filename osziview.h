// osziview.cc
//
//    oszi - widget for gtk--
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


class OsziView : public Gtk::DrawingArea
{
// Q_OBJECT
public:

    OsziView();
    ~OsziView();

    double  get_freq      (void) const { return freq; }
    void    setSampleData (const unsigned char *s, int num_samples);
    void    setSampleData (const short *s, int num_samples);
    void    setSampleFreq (double f);
    void    setTrigFact   (double fact);
    void    setAdaptive   (int active);
    double  getTrigFact   ();
    void    invalidate    ();
    void    invalidate_sample ();

protected:

    bool    on_expose_event(GdkEventExpose* p0);
    void    on_realize();
//    void    draw_default_impl();
//    void    draw_impl(GdkRectangle* p0);

private:

    enum SampFmt { U8, S16_LE };

    short int *      samp;
    GByteArray *     samp_byte_array;
    int              sampnr;
    double           sampfreq;
    int              wscr, hscr, xscr, yscr;
    double           trigfact;
    Glib::RefPtr<Gdk::GC> i_GC;
    Gdk::Color       i_col_bg;
    Gdk::Color       i_col_trig;
    Gdk::Color       i_col_mark;
    Gdk::Color       i_col_data;
    Gdk::Color       i_col_scale;
    Gdk::Color       i_col_ticks;
    Gdk::Color       i_col_zero;
    enum SampFmt     i_sampfmt;
    int              i_minsamp;
    int              i_maxsamp;
    int              i_divisor;
    int              i_adaptive_scale;
    int              startpoint;
    int              endpoint;
    int              trigger_a;
    int              trigger_b;
    double           freq;

    void    paint_sample  ();
    void    paint_marks   ();
    void    paint_scale   ();

    void  calc_minmax(void);
    void  calc_freq(void);
    void  recalc(void);
    void  copy_sample_data(const void *ptr, int data_size);
};
