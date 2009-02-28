// guitune.cc
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

//#include <iostream.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include <gtkmm/main.h>
#include <gtkmm/table.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/style.h>
#include <gtkmm/frame.h>

#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <math.h>

#include "guitune.h"

#include "resources.h"



//globally
double KAMMERTON, KAMMERTON_LOG;

void MainWidget::setTuningNorm()
{
    KAMMERTON=KAMMERTON_NORM;
    KAMMERTON_LOG=KAMMERTON_LOG_NORM;
    logview->invalidate();
}

void MainWidget::setTuningWien()
{
    KAMMERTON=KAMMERTON_WIEN;
    KAMMERTON_LOG=KAMMERTON_LOG_WIEN;
    logview->invalidate();
}

void MainWidget::setTuningPhys()
{
    KAMMERTON=KAMMERTON_PHYS;
    KAMMERTON_LOG=KAMMERTON_LOG_PHYS;
    logview->invalidate();
}


void MainWidget::setTuningEqui()
{
    logview->set_nat_tuning(false);
}


void MainWidget::setTuningNat()
{
    logview->set_nat_tuning(true);
}


void MainWidget::setScaleUS()
{
    logview->setScale(LogView::us_scale);
}

void MainWidget::setScaleUSAlt()
{
    logview->setScale(LogView::us_scale_alt);
}

void MainWidget::setScaleGE()
{
    logview->setScale(LogView::german_scale);
}

void MainWidget::setScaleGEAlt()
{
    logview->setScale(LogView::german_scale_alt);
}



void MainWidget::showLogView()
{
    logview->show();
}

void MainWidget::hideLogView()
{
    logview->hide();
}

void MainWidget::showOszi()
{
    oszi->show();
}

void MainWidget::hideOszi()
{
    oszi->hide();
}

double MainWidget::getTrigger()
{
    return(oszi->getTrigFact());
}

void MainWidget::setSampFreqVal( int sampfreq )
{
    i_pAdjSF->set_value(sampfreq);
}


void MainWidget::setSampFreq()
{
    sampfreq = (int) i_pAdjSF->get_value();
//    timer->stop();
    init_audio();
//    timer->start(0);
    oszi->setSampleFreq(sampfreq_exact);
//    emit signalSampFreqChanged();
}


void MainWidget::setSampNrVal( int sampnr )
{
    i_pAdjSN->set_value(sampnr);
}


void MainWidget::setSampNr()
{
    sampnr = (int) i_pAdjSN->get_value();
//    emit signalSampNrChanged();
}


void MainWidget::setTrigger()
{
    oszi->setTrigFact(i_pAdjTR->get_value());
//    emit signalTriggerChanged();
}


void MainWidget::setTriggerVal(double value)
{
//    oszi->setTrigFact(value);
    i_pAdjTR->set_value(value);
//    i_pAdjTR->changed();
//    emit signalTriggerChanged();
}


void MainWidget::setAdaptive()
{
    bool active;
    active = i_pCBAdapt->get_active();
    oszi->setAdaptive((int)active);
}


void MainWidget::setAdaptiveVal( int adaptive )
{
    i_pCBAdapt->set_active((bool)adaptive);
}



void MainWidget::setDSPName(const char *name)
{
//    timer->stop();
    strcpy(dsp_devicename,name);
    init_audio();
//    timer->start(0);
}

void MainWidget::init_audio()
{
   printf("initializing audio at %s\n",dsp_devicename);

   uninit_audio();
   
   audio_fd = open(dsp_devicename, O_RDONLY);
   if (audio_fd == -1) {
      perror(dsp_devicename);
      exit(1);
   }
   fcntl(audio_fd,F_SETFD,FD_CLOEXEC);

//   ioctl(audio_fd, SNDCTL_DSP_RESET, 0);
   if( strcmp(dsp_devicename,"/dev/stdin")==0 ){

      printf("reading data from stdin\n");
   		blksize=32;
      printf("  blocksize=%d\n",blksize);
      printf("  sampfreq=%d\n",sampfreq);
      sampfreq_exact=sampfreq;

   }else{

      ioctl(audio_fd, SNDCTL_DSP_SETDUPLEX, 0);

      {int caps;
          ioctl( audio_fd, SNDCTL_DSP_GETCAPS, &caps );
          printf("OSS-Version %d\n", caps & DSP_CAP_REVISION );
       		printf("  DUPLEX   = %X\n",caps & DSP_CAP_DUPLEX   );
          printf("  REALTIME = %X\n",caps & DSP_CAP_REALTIME );
          printf("  BATCH    = %X\n",caps & DSP_CAP_BATCH    );
          printf("  COPROC   = %X\n",caps & DSP_CAP_COPROC   );
          printf("  TRIGGER  = %X\n",caps & DSP_CAP_TRIGGER  );
          printf("  MMAP     = %X\n",caps & DSP_CAP_MMAP     );
   		}

   		blksize=8;  // 2^8 = 256
  		ioctl(audio_fd, SNDCTL_DSP_SETFRAGMENT, &blksize);
//   ioctl(audio_fd, SNDCTL_DSP_SETBLKSIZE, &blksize);

      ioctl(audio_fd, SNDCTL_DSP_GETBLKSIZE, &blksize);
      printf("blocksize=%d\n",blksize);
   //blksize=4096;

      ioctl(audio_fd, SNDCTL_DSP_SYNC, NULL);
//   int a_sampsize = 8;
//      int a_sampsize = AFMT_U8;
      i_sampfmt = AFMT_S16_LE;
      ioctl(audio_fd, SNDCTL_DSP_SAMPLESIZE, &i_sampfmt);
      int a_stereo = 0;
      ioctl(audio_fd, SNDCTL_DSP_STEREO, &a_stereo);

      int a_speed = sampfreq;
      printf("sampfreq=%d\n",sampfreq);
      ioctl(audio_fd, SNDCTL_DSP_SPEED, &a_speed);
      ioctl(audio_fd, SOUND_PCM_READ_RATE, &sampfreq);
      printf("sampfreq=%d\n",sampfreq);
      sampfreq_exact=sampfreq;

   }

   //int mixer = open("/dev/mixer", O_RDONLY, 0);
   //int vol=0xFFFF;
   //ioctl(mixer, SOUND_MIXER_WRITE_MIC, &vol);
   //printf("MIC-Volume=%d\n",vol);
   //close(mixer);

// printf("close(audio_fd)=%d\n",close(audio_fd));
// printf("close(audio_fd)=%d\n",close(audio_fd));

   trig = false;
   got_bytes = 0;

   audio_connection = Glib::signal_io()
      .connect(sigc::mem_fun(this, &MainWidget::proc_audio),
	       audio_fd, Glib::IO_IN | Glib::IO_HUP);
}

void MainWidget::uninit_audio()
{
   audio_connection.disconnect();

   if (audio_fd != -1)
   {
      ::close(audio_fd);
      audio_fd = -1;
   }
}

MainWidget::MainWidget()
    : Gtk::Table(7, 4)
{
    int i;
   //InitAudio();
//   setMinimumSize(490,240);

    set_row_spacings( 12 );
    set_col_spacings( 12 );

    KAMMERTON=KAMMERTON_NORM;
    KAMMERTON_LOG=KAMMERTON_LOG_NORM;

    strcpy(dsp_devicename,"/dev/dsp");

    sampnr=1024;
    sampfreq=11048;
    audio_fd = -1;
    init_audio();
    printf("Audiodriver initialized\n");

    freqs[0]=KAMMERTON; lfreqs[0]=KAMMERTON_LOG;
    for(i=1;i<12;i++){
        freqs [i] = freqs [i-1] * D_NOTE;
        lfreqs[i] = lfreqs[i-1] + D_NOTE_LOG;
    }

    oszi = new OsziView;
    oszi->setSampleFreq(sampfreq_exact);
    logview = new LogView;
    
//    Gtk::Style style = new Gtk::Style( new void );
//    Gdk_Font font("-*-helvetica-bold-r-*-*-15-*-*-*-*-*-*-*");
    //    get_style()->set_font( font );
    
//    freqview  = new Gtk::Label("freq");
//    nfreqview = new Gtk::Label("nfreq");
    freqview  = new LCDView("");
    freqview->set_size_request(100,20);
    nfreqview = new LCDView("");
    nfreqview->set_size_request(100,20);
    
//    nfreqview->set_style( style );
    
    
    i_pAdjSF = new Gtk::Adjustment(
                               11024.0,  // value
                                5000.0,  // lower
                               48000.0,  // upper
                                   1.0,  // step inc
                                 100.0,  // page inc
                                     0   // page size
                                  );
    
    i_pAdjSN = new Gtk::Adjustment(
                                1024.0,  // value
                                 100.0,  // lower
                               10000.0,  // upper
                                   1.0,  // step inc
                                 100.0,  // page inc
                                     0   // page size
                                  );
    
    i_pAdjTR = new Gtk::Adjustment(
                                   0.6,  // value
                                   0.0,  // lower
                                   1.0,  // upper
                                  0.05,  // step inc
                                   0.1,  // page inc
                                     0   // page size
                                  );

    attach( *oszi, 0, 1, 0, 5 );

    attach( *manage( new Gtk::Label("Freq:") ),
	    1, 2, 1, 2,
	    Gtk::AttachOptions(), Gtk::AttachOptions() );
    attach( *freqview, 2, 3, 1, 2,
	    Gtk::FILL, Gtk::AttachOptions() );

    attach( *manage( new Gtk::Label("Tune:") ),
	    1, 2, 2, 3,
	    Gtk::AttachOptions(), Gtk::AttachOptions() );
    attach( *nfreqview, 2, 3, 2, 3,
	    Gtk::FILL, Gtk::AttachOptions() );

    Gtk::Frame *adj_frame = new Gtk::Frame("Adjust:");
    
    Gtk::Table *adj_table = new Gtk::Table(6, 4);

    adj_table->set_row_spacings( 12 );
    adj_table->set_col_spacings( 12 );

    adj_frame->add( *manage( adj_table ) );

    adj_table->attach( *manage( new Gtk::Label("SF:") ),
		       1, 2, 1, 2,
		       Gtk::FILL, Gtk::AttachOptions() );

    Gtk::SpinButton *spin_sf = new Gtk::SpinButton( *i_pAdjSF );

    adj_table->attach( *manage( spin_sf ),
		       2, 3, 1, 2 );

    adj_table->attach( *manage( new Gtk::Label("SN:") ),
		       1, 2, 2, 3,
		       Gtk::AttachOptions(), Gtk::AttachOptions() );

    Gtk::SpinButton *spin_sn = new Gtk::SpinButton( *i_pAdjSN );

    adj_table->attach( *manage( spin_sn ),
		       2, 3, 2, 3 );

    adj_table->attach( *manage( new Gtk::Label("Trig:") ),
		       1, 2, 3, 4,
		       Gtk::AttachOptions(), Gtk::AttachOptions() );

    Gtk::SpinButton *spin_tr = new Gtk::SpinButton( *i_pAdjTR );

    adj_table->attach( *manage( spin_tr ),
		       2, 3, 3, 4 );

    i_pCBAdapt = new Gtk::CheckButton( "Adapt" );

    adj_table->attach( *manage( i_pCBAdapt ),
		       2, 3, 4, 5 );

    attach( *manage( adj_frame ),
	    1, 3, 3, 4,
	    Gtk::FILL, Gtk::FILL );

    attach( *manage( new Gtk::Label( "" ) ),
	    1, 3, 4, 5, Gtk::AttachOptions() );

    attach( *logview,
	    0, 3, 5, 6,
	    Gtk::EXPAND | Gtk::FILL, Gtk::AttachOptions() );

    logview->set_size_request(0,86);
       
    spin_tr->set_digits(2);
       
    i_pAdjSF->signal_value_changed().connect( sigc::mem_fun( this, &MainWidget::setSampFreq ) );
    i_pAdjSN->signal_value_changed().connect( sigc::mem_fun( this, &MainWidget::setSampNr ) );
    i_pAdjTR->signal_value_changed().connect( sigc::mem_fun( this, &MainWidget::setTrigger ) );

    i_pAdjSF->signal_value_changed().connect( sigc::mem_fun( oszi, &OsziView::invalidate ) );
    i_pAdjSN->signal_value_changed().connect( sigc::mem_fun( oszi, &OsziView::invalidate ) );

    i_pCBAdapt->signal_toggled().connect( sigc::mem_fun( this, &MainWidget::setAdaptive ) );
}


bool MainWidget::proc_audio(Glib::IOCondition cond)
{int i,j,r,got_samples;
 unsigned char *c;
 short int *s;
 unsigned char * sample_uc;
 short int *     sample_s16le;
 double ldf,mldf;
 char str[50];

 sample_uc    = (unsigned char *)sample;
 sample_s16le = (short int *)sample;
 c = sample_uc;
 s = sample_s16le;

 r = read(audio_fd, c + got_bytes, blksize);

 if (r == -1)
 {
    g_critical("error reading audio: %s", strerror(errno));
    uninit_audio();
    return false;
 }
 if (r == 0)
 {
    g_critical("EOF on audio");
    uninit_audio();
    return false;
 }

 got_bytes += r;

 got_samples = got_bytes;
 if (i_sampfmt == AFMT_S16_LE)
    got_samples /= 2;

 if (!trig)
 {
   // Look for a trigger
   trigpos=0;

   if(i_sampfmt==AFMT_U8){
       for( i=0; i<got_samples && Abs(c[i]-128)<2; i++ );
   } else if(i_sampfmt==AFMT_S16_LE) {
       for( i=0; i<got_samples && Abs(s[i])<256; i++ );
   }
   //i=-1;
   j=0; trig=0;
   if (i<got_samples) {
      if(i_sampfmt==AFMT_U8){
          for( ; i<got_samples-1; i++ )   /* n-1 because of POSTRIG uses i+1 */
              if ( POSTRIG(c,i) ) { trig = true; trigpos=i; }
      }else if(i_sampfmt==AFMT_S16_LE){
          for( ; i<got_samples-1; i++ )   /* n-1 because of POSTRIG uses i+1 */
              if ( POSTRIG_S16(s,i) ) { trig = true; trigpos=i; }
      }
   }
   // If we didn't find a trigger then discard the bytes we read
   if (!trig)
      got_bytes = 0;
 }
 else if (got_samples - trigpos >= sampnr)
 {
      if(i_sampfmt==AFMT_U8)
	 oszi->setSampleData( sample_uc + trigpos, sampnr );
      else if(i_sampfmt==AFMT_S16_LE)
	 oszi->setSampleData( sample_s16le + trigpos, sampnr );
      
      freq_0t = (double)sampfreq*oszi->get_freq();
      lfreq_0t = log(freq_0t);
      while ( lfreq_0t < lfreqs[0]-D_NOTE_LOG/2.0 ) lfreq_0t+=LOG_2;
      while ( lfreq_0t >= lfreqs[0]+LOG_2-D_NOTE_LOG/2.0 ) lfreq_0t-=LOG_2;
      mldf=D_NOTE_LOG; note_0t=0;
      for( i=0; i<12; i++ ){
          ldf = fabs(lfreq_0t-lfreqs[i]);
          if (ldf<mldf) { mldf=ldf; note_0t=i; }
      }
      logview->change_lfreq(lfreq_0t);
      sprintf(str,"%0.3f",freq_0t);
      freqview->set(str);
      double nfreq_0t=freqs[note_0t];
      while( nfreq_0t/freq_0t > D_NOTE_SQRT ) nfreq_0t/=2.0;
      while( freq_0t/nfreq_0t > D_NOTE_SQRT ) nfreq_0t*=2.0;
      sprintf(str,"%0.3f",nfreq_0t);
      nfreqview->set(str);
      
//      printf("Note: %s (%lfHz) Freq=%lf\n",
//	        note[note_0t],freqs[note_0t],freq_0t);
      call_callback_function(freq_0t); 
      trig = false;
      got_bytes = 0;
 }
   
   return true;
}

MainWidget::~MainWidget()
{
   uninit_audio();
}
