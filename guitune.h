// guitune.h
//
//    guitune - program for tuning instruments (actually an oscilloscope)
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

//#include <gtkmm/window.h>
#include <gtkmm/container.h>
#include <gtkmm/table.h>
#include <gtkmm/label.h>
#include <gtkmm/adjustment.h>
#include <gtkmm/checkbutton.h>

#include "osziview.h"
#include "logview.h"
#include "lcdview.h"
#include "python-interp.h"

class MainWidget : public Gtk::Table
{
 public:
   MainWidget();
   ~MainWidget();
   int sampnr;
   int sampfreq;
   double sampfreq_exact;
   int blksize;
   int trig1;
   int trig2;
   int    note_0t;
   int    note_ht;
   double freq_0t;
   double freq_ht;
   double lfreq_0t;
   double lfreq_ht;
   void setTuningNorm();
   void setTuningWien();
   void setTuningPhys();
   void setTuningEqui();
   void setTuningNat();
   void setScaleUS();
   void setScaleUSAlt();
   void setScaleGE();
   void setScaleGEAlt();
   void showOszi();
   void hideOszi();
   void showLogView();
   void hideLogView();
   double getTrigger();
// public slots:
     void setSampFreqVal( int sampfreq );
     void setSampFreq();
     void setSampNrVal( int sampnr );
     void setSampNr();
     void setTrigger();
     void setTriggerVal(double value);
     void setAdaptive();
     void setAdaptiveVal( int adaptive );
     void setDSPName(const char* name);
// signals:
//   void signalSampFreqChanged();
//   void signalSampNrChanged();
//   void signalTriggerChanged();

   Gtk::Adjustment  *  i_pAdjSF;  // sample freq
   Gtk::Adjustment  *  i_pAdjSN;  // sample numner
   Gtk::Adjustment  *  i_pAdjTR;  // trigger
   Gtk::CheckButton *  i_pCBAdapt;  // adaptive scaling
   
private:
   OsziView         *  oszi;
   LogView          *  logview;
//   Gtk::Label       *  freqview;
//   Gtk::Label       *  nfreqview;
   LCDView          *  freqview;
   LCDView          *  nfreqview;
   int oszi_height;
   short int sample[64000];
   double freqs[12];
   double lfreqs[12];
   char dsp_devicename[100];
   int  i_sampfmt;
   int  audio_fd;
   sigc::connection audio_connection;

   bool trig;
   int trigpos;
   int got_bytes;

// private slots:
   bool proc_audio(Glib::IOCondition cond);

   void init_audio();
   void uninit_audio();
};
