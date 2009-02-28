// osziview.cc
//
//    oszi - widget for gtk--
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

#include <stdio.h>
#include <string.h>

#include <math.h>

#include "resources.h"

#include "osziview.h"

#include <gtkmm/style.h>
#include <gdkmm/color.h>
#include <math.h>

#define SCALE_HEIGHT 20

#define xscr 20
#define yscr 5
#define wscr (get_width()-xscr*2)
#define hscr (get_height()-yscr-SCALE_HEIGHT-1)

OsziView::OsziView() : Gtk::DrawingArea()
{
    samp_byte_array = g_byte_array_new();

    i_col_bg.   set("black");         get_colormap()->alloc_color(i_col_bg   );
    i_col_trig. set("cyan" );         get_colormap()->alloc_color(i_col_trig );
    i_col_mark. set("red"  );         get_colormap()->alloc_color(i_col_mark );
    i_col_data. set("green");         get_colormap()->alloc_color(i_col_data );
    i_col_scale.set("#B8B8B8");       get_colormap()->alloc_color(i_col_scale);
    i_col_ticks.set("#666");          get_colormap()->alloc_color(i_col_ticks);
    i_col_zero. set("gray" );         get_colormap()->alloc_color(i_col_zero );
    trigfact=0.6;
    i_adaptive_scale=1;
    freq=0.0;
    startpoint=0;
    endpoint=0;

    // Default to three dummy samples
    setSampleData((const unsigned char *) "\0\0\0", 3);
}


OsziView::~OsziView()
{
    g_byte_array_free (samp_byte_array, TRUE);
}

void OsziView::calc_minmax(void)
{
    int i;
    unsigned char * samp_uc;
    short int *     samp_s16le;

    samp_s16le =     (short int *) samp;
    samp_uc    = (unsigned char *) samp;

    if(i_sampfmt == U8){
        i_minsamp=255;
        i_maxsamp=0;
        for(i=0;i<sampnr;i++){
            if ( samp_uc[i-1] > i_maxsamp ) i_maxsamp = samp_uc[i-1];
            if ( samp_uc[i-1] < i_minsamp ) i_minsamp = samp_uc[i-1];
        }
        if ( !i_adaptive_scale ){
            i_divisor = 0x100;
        } else {
            i_divisor = 8;
            if ( 2*Abs(i_maxsamp-128) > i_divisor ) i_divisor=2*Abs(i_maxsamp-128);
            if ( 2*Abs(i_minsamp-128) > i_divisor ) i_divisor=2*Abs(i_minsamp-128);
        }
    } else if(i_sampfmt == S16_LE){
        i_minsamp= 33000;
        i_maxsamp=-33000;
        for(i=0;i<sampnr;i++){
            if ( samp_s16le[i-1] > i_maxsamp ) i_maxsamp = samp_s16le[i-1];
            if ( samp_s16le[i-1] < i_minsamp ) i_minsamp = samp_s16le[i-1];
        }
        if ( !i_adaptive_scale ){
            i_divisor = 0x10000;
        } else {
            i_divisor = 2048;
            if ( 2*Abs(i_maxsamp) > i_divisor ) i_divisor=2*Abs(i_maxsamp);
            if ( 2*Abs(i_minsamp) > i_divisor ) i_divisor=2*Abs(i_minsamp);
        }
    }
}

void OsziView::calc_freq(void)
/* in units of sampfreq */
{
    int t1, t2, A1, A2, tc, i, schmitt_triggered;
    short int *     samp_s16le;
    unsigned char * samp_uc;

    samp_s16le =     (short int *) samp;
    samp_uc    = (unsigned char *) samp;
    
    if(i_sampfmt==U8){
        for(i=0,A1=0;i<sampnr;i++)
            if (A1<Abs(samp_uc[i]-128) && samp_uc[i]-128>0) A1=Abs(samp_uc[i]-128);
        for(i=0,A2=0;i<sampnr;i++)
            if (A2<Abs(samp_uc[i]-128) && samp_uc[i]-128<0) A2=Abs(samp_uc[i]-128);
    } else if(i_sampfmt==S16_LE){
        for(i=0,A1=0;i<sampnr;i++)
            if (A1<Abs(samp_s16le[i]) && samp_s16le[i]>0) A1=Abs(samp_s16le[i]);
        for(i=0,A2=0;i<sampnr;i++)
            if (A2<Abs(samp_s16le[i]) && samp_s16le[i]<0) A2=Abs(samp_s16le[i]);
    }
//      A1 = (int)( (double)A*M_PI/2.0/(double)sampnr+0.5 );
//      A1 = (int)( (double)A*M_PI/2.0/(double)sampnr+0.5 );
    if(i_sampfmt==U8){
        t1 = 128 + (int)( A1 * trigfact + 0.5 );
        t2 = 128 - (int)( A2 * trigfact + 0.5 );
    } else if(i_sampfmt==S16_LE){
        t1 =   (int)( A1 * trigfact + 0.5 );
        t2 = - (int)( A2 * trigfact + 0.5 );
    }
    startpoint=0;
    if(i_sampfmt==U8){
        for( i=1; samp_uc[i]<=t1 && i<sampnr; i++ );
        for( ; !LEVTRIGN(samp_uc,i,t2) && i<sampnr; i++ );
    } else if(i_sampfmt==S16_LE){
        for( i=1; samp_s16le[i]<=t1 && i<sampnr; i++ );
        for( ; !LEVTRIGN(samp_s16le,i,t2) && i<sampnr; i++ );
    }
    startpoint=i;
    schmitt_triggered=NO;
    endpoint=startpoint+1;
    tc=0;
    if(i_sampfmt==U8){
        for( i=startpoint, tc=0; i<sampnr; i++ ) {
            if( !schmitt_triggered )
                schmitt_triggered = (samp_uc[i]>=t1);
            else if( LEVTRIGN(samp_uc,i,t2) ) {
                endpoint=i; tc++;
                schmitt_triggered = NO;
            }
        }
    } else if(i_sampfmt==S16_LE){
        for( i=startpoint, tc=0; i<sampnr; i++ ) {
            if( !schmitt_triggered )
                schmitt_triggered = (samp_s16le[i]>=t1);
            else if( LEVTRIGN(samp_s16le,i,t2) ) {
                endpoint=i; tc++;
                schmitt_triggered = NO;
            }
        }
    }
    if (endpoint==startpoint) endpoint++;
    freq = (double)tc/(double)(endpoint-startpoint);
    if (freq<1E-15) freq = 1E-15;

    trigger_a = t1;
    trigger_b = t2;
}

void OsziView::recalc(void)
{
    calc_minmax();
    calc_freq();
}

void OsziView::paint_sample(void)
{
    int i,x1,x2,y1,y2;
    unsigned char * samp_uc;
    short int *     samp_s16le;
    //erase();
    samp_s16le =     (short int *) samp;
    samp_uc    = (unsigned char *) samp;

    if (!i_GC)
	i_GC = Gdk::GC::create( get_window() );

    i_GC->set_foreground(i_col_zero);
    get_window()->
        draw_line( i_GC, xscr,yscr+hscr/2,xscr+wscr-1,yscr+hscr/2);

    i_GC->set_foreground(i_col_data);
    for(i=1;i<sampnr;i++){
        x1=xscr+(i-1)*wscr/sampnr;
        x2=xscr+i*wscr/sampnr;
        if(i_sampfmt == U8){
            y1=yscr+(samp_uc[i-1]-128+i_divisor/2)*hscr/i_divisor;
            y2=yscr+(samp_uc[i]-128+i_divisor/2)*hscr/i_divisor;
        } else if(i_sampfmt == S16_LE){
            y1=yscr+((int)samp_s16le[i-1]+i_divisor/2)*hscr/i_divisor;
            y2=yscr+((int)samp_s16le[i]  +i_divisor/2)*hscr/i_divisor;
        }
        get_window()->draw_line( i_GC,  x1, y1, x2, y2 );
    }
    //printf("hallo\n");
}

void OsziView::paint_marks(void)
{
    int linex, liney;
    int t1 = trigger_a, t2 = trigger_b;

    i_GC->set_foreground(i_col_mark);
    linex = xscr+endpoint*wscr/sampnr;
    get_window()->draw_line( i_GC, linex, yscr, linex, yscr+hscr );
    linex = xscr+startpoint*wscr/sampnr;
    get_window()->draw_line( i_GC, linex, yscr, linex, yscr+hscr );
    
    i_GC->set_foreground(i_col_trig);

    if (i_sampfmt == U8)
    {
	t1 -= 128;
	t2 -= 128;
    }

    liney = yscr+(t1+i_divisor/2)*hscr/i_divisor;
    get_window()->draw_line( i_GC, xscr, liney, xscr+wscr, liney );
    liney = yscr+(t2+i_divisor/2)*hscr/i_divisor;
    get_window()->draw_line( i_GC, xscr, liney, xscr+wscr, liney );
}

void OsziView::copy_sample_data(const void *ptr, int data_size)
{
    g_byte_array_set_size(samp_byte_array, 0);
    g_byte_array_append(samp_byte_array, (guint8 *) ptr, data_size);
    samp = (short int *) samp_byte_array->data;
}

void OsziView::setSampleData(const unsigned char *s, int num_samples)
{
    copy_sample_data(s, num_samples);
    i_sampfmt = U8;
    if (num_samples != sampnr)
    {
	sampnr = num_samples;
	invalidate();
    }
    else
	invalidate_sample();
    recalc();
}

void OsziView::setSampleData(const short int *s, int num_samples)
{
    copy_sample_data(s, num_samples * sizeof (short int));
    i_sampfmt = S16_LE;
    if (num_samples != sampnr)
    {
	sampnr = num_samples;
	invalidate();
    }
    else
	invalidate_sample();
    recalc();
}

void OsziView::setSampleFreq(double f) { sampfreq = f; invalidate(); }


void OsziView::setAdaptive(int active) { i_adaptive_scale = active; invalidate(); }


void OsziView::setTrigFact(double fact) { trigfact=fact; invalidate(); }


double OsziView::getTrigFact() { return(trigfact); }

void OsziView::invalidate(void)
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

void OsziView::invalidate_sample(void)
{
    if (is_realized())
    {
	Gdk::Rectangle rect = get_allocation();
	rect.set_x(0);
	rect.set_y(0);
	rect.set_height(rect.get_height() - SCALE_HEIGHT);
	get_window()->invalidate_rect(rect, false);
    }
}

void OsziView::paint_scale()
{
    double i,j;
    double di,dj;
    double dim; //mantissa of di
    char str[100];
//       printf("nr=%d, freq=%lf\n",nr,f);

    if (!i_GC)
        i_GC = Gdk::GC::create( get_window() );

    di = (double)sampnr/sampfreq/10.0;
    for(dim=di;dim>=10.0;dim/=10.0);
    for(;dim<1.0;dim*=10.0);
    if (dim<=5.0)
    { di=di/dim*5.0; dim=5.0; }
    else
    { di=di/dim*10.0; dim=10.0;}
    dj=di/10.0;

    int scale_top = get_height() - SCALE_HEIGHT;
    
    i_GC->set_foreground(i_col_ticks);
    for( j=0.0; j<(double)sampnr/sampfreq; j+=dj ){
        get_window()->
            draw_line( i_GC, xscr+(j*sampfreq)*wscr/sampnr,scale_top,
                             xscr+(j*sampfreq)*wscr/sampnr,scale_top+3 );
    }
    
    if (dim!=5.0){
        dj=di/2.0;
        i_GC->set_foreground(i_col_scale);
        for( j=0.0; j<(double)sampnr/sampfreq; j+=dj ){
            get_window()->
                draw_line( i_GC, xscr+(j*sampfreq)*wscr/sampnr,scale_top,
                                 xscr+(j*sampfreq)*wscr/sampnr,scale_top+5 );
        }
    }
    
    i_GC->set_foreground(i_col_scale);


    Glib::RefPtr<Pango::Layout> layout = create_pango_layout("");
    layout->set_font_description( Pango::FontDescription("Sans 8 ") );
    
    for( i=0.0; i<(double)sampnr/sampfreq; i+=di ){
        
        get_window()->
            draw_line( i_GC, xscr+(i*sampfreq)*wscr/sampnr,scale_top,
                             xscr+(i*sampfreq)*wscr/sampnr,scale_top+7 );
        
        sprintf(str,"%.0f",i*1000); // in ms
	layout->set_text( str );
	Pango::Rectangle extents = layout->get_pixel_logical_extents();

        get_window()->
            draw_layout(i_GC,
                        xscr+(i*sampfreq)*wscr/sampnr-
                        extents.get_width()/2,
                        scale_top+7+extents.get_ascent(),
                        layout
                       );
    }
}


bool OsziView::on_expose_event(GdkEventExpose *event)
{
    Gtk::DrawingArea::on_expose_event(event);

    // Avoid painting the scale if only the sample area has changed
    if (event->area.y + event->area.height > get_height() - SCALE_HEIGHT)
	paint_scale();

    paint_sample();
    paint_marks();

    return true;
}

void OsziView::on_realize()
{
    Gtk::DrawingArea::on_realize();

    get_window()->set_background( i_col_bg );
}
