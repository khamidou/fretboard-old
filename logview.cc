// logview.cc
//
//    logarithmic scale for tunes
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

#include "logview.h"

#include "resources.h"



const char *note_ger[12] = {"A","B", "H","C","Db","D","Eb","E","F","Gb","G","Ab"};
const char *note_us[12] =  {"A","Bb","B","C","Db","D","Eb","E","F","Gb","G","Ab"};
const char *note_ger_alt[12] = {"A","B", "H","C","C#","D","D#","E","F","F#","G","G#"};
const char *note_us_alt[12] =  {"A","A#","B","C","C#","D","D#","E","F","F#","G","G#"};

const char **note;

const char *tuning_nat[23] = 
     {"25/24","16/15","9/8","7/6","75/64","6/5","5/4","32/25","125/96",
      "4/3","25/18","45/32","36/25","3/2","25/16","8/5","5/3","125/72",
      "16/9","9/5","15/8","48/25","125/64"};

// public:
LogView::LogView() : Gtk::DrawingArea()
{
    grundton=0;
    nat_tuning_on=false;

    note=note_us;

    lfreq=-1.0;
    x0=10;
    y0=26;
    h=10;

    i_col_draw.    set("black");     get_colormap()->alloc_color(i_col_draw   );
    i_col_shad.    set("white");     get_colormap()->alloc_color(i_col_shad   );
    i_col_bg2.     set("#999");      get_colormap()->alloc_color(i_col_bg2    );
    i_col_pointer. set("green");     get_colormap()->alloc_color(i_col_pointer);
    i_col_ticks.   set("#666");      get_colormap()->alloc_color(i_col_ticks  );
    i_col_marks.   set("red"  );     get_colormap()->alloc_color(i_col_marks  );
    
    set_events (
                  Gdk::EXPOSURE_MASK
//		| Gdk::LEAVE_NOTIFY_MASK
		| Gdk::BUTTON_PRESS_MASK
//		| Gdk::POINTER_MOTION_MASK
//                | Gdk::POINTER_MOTION_HINT_MASK
               );
}


LogView::~LogView() {};


int LogView::lfreq_pos(double lfreq)
{
    return (int)( (double)x0+(double)(get_width()-2*x0)
                  *(double)(lfreq-lfreq0)/LOG_2+0.5 );
}


int LogView::pos_note(int pos)
{
    return( (pos-x0)*12/(get_width()-2*x0) );
}


void LogView::draw_arrow()
{
    int x,y,x1,x2;
    
    Glib::RefPtr<Gdk::Window> win = get_window();

    if (!i_GC)
        i_GC = Gdk::GC::create( win );

    i_GC->set_foreground( i_col_bg2 );
    win->draw_rectangle( i_GC, true,  x0,y0+1,get_width()-2*x0,h-1 );

    if (lfreq >= 0.0)
    {
	x=lfreq_pos(lfreq);
	y=y0;
    
	i_GC->set_foreground(i_col_pointer);
	x1=x-9;  if (x1<x0)         x1 = x0;
	x2=x+9;  if (x2>get_width()-x0) x2 = get_width()-x0;
	win->draw_rectangle( i_GC, true, x1,y+1,x2-x1+1,h-1 );
    
	i_GC->set_foreground(i_col_draw);
	win->draw_line( i_GC, x,y+1,x,y+h-1 );
    }
}

void LogView::change_lfreq(double freq)
{
    double mldf,ldf;
    int i,note;

    lfreq=freq;
    while (lfreq>lfreq1) lfreq-=LOG_2;
    while (lfreq<lfreq0) lfreq+=LOG_2;
    mldf=D_NOTE_LOG; note=0;
    for( i=0; i<12; i++ ){
        ldf = fabs(lfreq-lfreqs[i]);
        if (ldf<mldf) { mldf=ldf; note=i; }
    }
    invalidate_arrow();
}


void LogView::setScale(int scale)
{
    switch(scale){
    case us_scale:
        note=note_us;
        break;
    case german_scale:
        note=note_ger;
        break;
    case us_scale_alt:
        note=note_us_alt;
        break;
    case german_scale_alt:
        note=note_ger_alt;
        break;
    }
    invalidate();
}


// protected:

void LogView::draw_it()
{
    int i,k,x,y,x2;
    char  str[100];
    double nat_freq[23];
    double nat_lfreq[23];
    int z,n;
    
    Glib::RefPtr<Pango::Layout> big_layout = create_pango_layout( "" );
    big_layout->set_font_description( Pango::FontDescription("Sans 13px") );
    Glib::RefPtr<Pango::Layout> small_layout = create_pango_layout( "" );
    small_layout->set_font_description( Pango::FontDescription("Sans 8px") );

    if (!i_GC){
	Glib::RefPtr<Gdk::Window> win_ = get_window();
        i_GC = Gdk::GC::create( win_ );
    }
    
    Glib::RefPtr<Gdk::Window> win = get_window();

    lfreq0 = KAMMERTON_LOG - D_NOTE_LOG/2.0;
    lfreq1 = KAMMERTON_LOG - D_NOTE_LOG/2.0 + LOG_2;

    for(i=0;i<12;i++){
        freqs [i] = KAMMERTON * pow(D_NOTE,i);
        lfreqs[i] = KAMMERTON_LOG + (double)i*D_NOTE_LOG;
    }

    for(i=0;i<23;i++){
        sscanf(tuning_nat[i],"%d/%d",&z,&n);
        //         printf("z=%d,n=%d\n",z,n);
        nat_freq[i]=freqs[grundton]*(double)z/(double)n;
        nat_lfreq[i]=log(nat_freq[i]);
        if ( nat_lfreq[i]>lfreq1 ){
            nat_lfreq[i]-=LOG_2;
            nat_freq[i]/=2.0;
        }
    }

//        lfreq0=lfreqs[0]-D_NOTE_LOG/2.0;
      i_GC->set_foreground( i_col_ticks );
    	for(k=0;k<120;k++){
        x2=lfreq_pos(KAMMERTON_LOG + (double)(k-5)/10.0*D_NOTE_LOG);
//	      x2=(int)( (double)x0+(double)(get_width()-2*x0)
//                 *(double)k/120.0+0.5 );
    	 win->draw_line( i_GC, x2,y0,x2,y0-((k%5==0)?5:3) );
       	 win->draw_line( i_GC, x2,y0+h,x2,y0+h+((k%5==0)?5:3) );
    	}

      if(nat_tuning_on){
          // draw natural tuning
	  small_layout->set_text( "Key" );
	  Pango::Rectangle extents = small_layout->get_pixel_logical_extents();

          i_GC->set_foreground( i_col_draw );
          x2=lfreq_pos(lfreqs[grundton]);
    	  win->draw_line( i_GC, x2,y0+h,x2,y0+h+10+
                               +extents.get_height()*2 );

          i_GC->set_foreground( i_col_shad );
          win->draw_layout( i_GC,
                           x2-extents.get_width()/2+1,
                           y0+h+10+extents.get_ascent()
			   +extents.get_height()*2+1,
			   small_layout
                         );
          
          i_GC->set_foreground( i_col_draw );
          win->draw_layout( i_GC,
                           x2-extents.get_width()/2,
                           y0+h+10+extents.get_ascent()
                           +extents.get_height()*2,
			   small_layout
                         );
          
          i_GC->set_foreground( i_col_marks );
          for(k=0;k<23;k++){
              x2=lfreq_pos(nat_lfreq[k]);
//    	      p.drawLine(x2,y0+h,x2,y0+h+10);
    	      win->draw_line( i_GC, x2,y0+h,x2,y0+h+10+
                                   +extents.get_height()*(k%3) );
          }
          
          for(k=0;k<23;k++){
	      small_layout->set_text( tuning_nat[k] );
	      extents = small_layout->get_pixel_logical_extents();

              x2=lfreq_pos(nat_lfreq[k]);
              i_GC->set_foreground( i_col_shad );
              win->draw_layout( i_GC,
                               x2-extents.get_width()/2+1,
                               y0+h+10+extents.get_ascent()
                               +extents.get_height()*(k%3)+1,
			       small_layout
                             );
              
              i_GC->set_foreground( i_col_draw );
              win->draw_layout( i_GC,
                               x2-extents.get_width()/2,
                               y0+h+10+extents.get_ascent()
                               +extents.get_height()*(k%3),
			       small_layout
                             );
    	   }
      }
        
      i_GC->set_foreground( i_col_draw );
      win->draw_line( i_GC, x0,y0,get_width()-10,y0 );
      win->draw_line( i_GC, x0,y0+h,get_width()-10,y0+h );
      for(i=0;i<12;i++){
          
          x=lfreq_pos(lfreqs[i]);
//    	   x=(int)( (double)x0+(double)(get_width()-2*x0)
//                 *(double)(lfreqs[i]-lfreq0)/LOG_2+0.5 );
          y=y0;
          win->draw_line( i_GC, x,y,x,y-7 );
          if(!nat_tuning_on)
              win->draw_line( i_GC, x,y+h,x,y+h+7 );

	  big_layout->set_text( note[i] );
	  Pango::Rectangle extents = big_layout->get_pixel_logical_extents();

          i_GC->set_foreground( i_col_shad );
          win->draw_layout( i_GC,
			    x-extents.get_width()/2+1,
			    y-10+1 - extents.get_height(),
			    big_layout
			    );
          
          i_GC->set_foreground( i_col_draw );
          win->draw_layout( i_GC,
			    x-extents.get_width()/2,
			    y-10 - extents.get_height(),
			    big_layout
                         );

          if(!nat_tuning_on){
              for(k=0;k<4;k++){
                  sprintf(str,"%.2f",(double)(freqs[i]/8.0*(1<<k)));
		  small_layout->set_text( str );
		  extents = small_layout->get_pixel_logical_extents();

                  i_GC->set_foreground( i_col_shad );
                  win->draw_layout( i_GC,
                                   x-extents.get_width()/2+1,
                                   y+h+10+extents.get_ascent()+1
                                   +extents.get_height()*k, small_layout
                                 );
                  
                  i_GC->set_foreground( i_col_draw );
                  win->draw_layout( i_GC,
                                   x-extents.get_width()/2,
                                   y+h+10+extents.get_ascent()
                                   +extents.get_height()*k, small_layout
                                 );
              }
          }
          
      }
//	y=y0+h+10+p.fontMetrics().get_height()*5;
//	p.setFont(QFont("System",12));
//	p.drawText(x0,y,"Frequancy [Hz]:");
}


bool LogView::on_expose_event(GdkEventExpose* event)
{
    Gtk::DrawingArea::on_expose_event(event);
    if (event->area.y <= y0
	|| event->area.y + event->area.height > y0 + h)
	draw_it();
    draw_arrow();
    return true;
}

bool LogView::on_button_press_event(GdkEventButton* p0)
{
    int x,y;

    get_pointer(x,y);
    printf("pointer:%d,%d\n",x,y);
    grundton=pos_note(x);
    printf("grundton=%d\n",grundton);
//    GdkEventExpose ev;
    //    expose_event(&ev);
    invalidate();
    Gtk::DrawingArea::on_button_press_event(p0);
    return true;
}


void LogView::set_nat_tuning(bool state)
{
    nat_tuning_on = state;
    invalidate();
}

void LogView::invalidate(void)
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

void LogView::invalidate_arrow(void)
{
    if (is_realized())
    {	
	Gdk::Rectangle rect = get_allocation();
	rect.set_x(0);
	rect.set_y(y0 + 1);
	rect.set_height(h - 1);
	get_window()->invalidate_rect(rect, false);
    }
}
