// main.cc
//
//    guitune(gtk--) - program for tuning instruments
//                     (actually an oscilloscope)
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include <gtk/gtk.h>
#include <gtkmm/window.h>
#include <gtkmm/menubar.h>
#include <gtkmm/menu.h>
#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/stock.h>

#include <getopt.h>

#include "aboutbox.h"

#include "guitune.h"

#include "python-interp.h"

#define DEBUG

Gtk::Menu * menu_input_rec;


char   options_audiodev[256];
double options_trigger;
int    options_sampnr;
int    options_sampfreq;
int    options_width;
int    options_height;
int    options_adaptive_scale;



enum optionType
{
    OPT_AUDIODEV,
    OPT_TRIGGER,
    OPT_SAMPFREQ,
    OPT_SAMPNR,
    OPT_WIDTH,
    OPT_HEIGHT,
    OPT_HELP,
    OPT_ADAPTIVE,
    OPT_DUMMY,
//    OPT_DEBUG,
};


static const char appname_str[] = "fretboard";


static struct option long_options[] =
{
    {"device",       required_argument, (int *)"arg=device audio-in device",       OPT_AUDIODEV},
    {"trigger",      required_argument, (int *)"arg=threshold of schmitt trigger", OPT_TRIGGER},
    {"sampfreq",     required_argument, (int *)"arg=sample frequency in Hz",       OPT_SAMPFREQ},
    {"sampnr",       required_argument, (int *)"arg=number of samples",            OPT_SAMPNR},
    {"width",        required_argument, (int *)"arg=width in pixels",              OPT_WIDTH},
    {"height",       required_argument, (int *)"arg=height in pixels",             OPT_HEIGHT},
    {"adaptive",     required_argument, (int *)"arg=1|0 adaptive scaling of sample data", OPT_ADAPTIVE},
    {"help",         no_argument,       (int *)"this help",                        OPT_HELP},
//    {"debug",        no_argument,       (int *)"turn debug mode on",               OPT_DEBUG},
    {NULL,           0,                 NULL, 0}
};




void process_option(int act_option)
{
       switch(act_option){
       case OPT_AUDIODEV:
           sscanf(optarg,"%s",options_audiodev);
       break;
       case OPT_TRIGGER:
           sscanf(optarg,"%lf",&options_trigger);
       break;
       case OPT_SAMPFREQ:
           sscanf(optarg,"%d",&options_sampfreq);
       break;
       case OPT_SAMPNR:
           sscanf(optarg,"%d",&options_sampnr);
       break;
       case OPT_WIDTH:
           sscanf(optarg,"%d",&options_width);
       break;
       case OPT_HEIGHT:
           sscanf(optarg,"%d",&options_height);
       break;
       case OPT_ADAPTIVE:
           sscanf(optarg,"%d",&options_adaptive_scale);
       break;
//        case OPT_DEBUG:
// 	       debug_mode = 1;
//        break;
       case OPT_HELP:        exit(1);   break;
       case OPT_DUMMY:       break;
       }
}


void print_help(struct option * opt, const char *appname, FILE * io)
{
    int i;

    fprintf(io,"\nusage: %s [--option [<arg>]]\n",appname);
    fprintf(io,"  options:\n");
    for(i=0;opt[i].name!=0;i++){
        fprintf(io,"--%s %s\n",opt[i].name,opt[i].has_arg?"<arg>":"");
        fprintf(io,"     %s\n",(char *)(opt[i].flag));
        opt[i].flag=NULL;
    }
//    printf("the color <0xrrggbb> means one byte for each red, green, blue\n");
//  printf("the transparency specification is optional e.g. <0xrrggbb>\n");
    fprintf(io,"\n");
}


int load_config( char *** confv, int * confc, char ** argv, int argc )
{
    FILE * f;
    int c,i;
    char * str;
    char allstr[64000];
    char filename[512];

    *confc=1;
    str=allstr;

    sprintf(filename,"%s/.gtkguitunerc",getenv("HOME"));

    if( (f=fopen(filename,"rb")) != NULL ){

        do{
            str[0]='-'; str[1]='-';
            for( i=2 ; (c=fgetc(f))!='\n' && c!=EOF ; i++ ){
                if( c!=' ' && c!=0x13 && c!=0x0A ) str[i]=c;
                else {
                    str[i]=0;
                    (*confc)++;
//                    while((c=fgetc(f))==' ' && c!=EOF);
                }
            }
            str[i]=0;
            if( str[2]!=0 ){
                (*confc)++;
/*           fprintf(stderr,"confstring:<%s> confc=%d\n",str,*confc);*/
                str+=i+1;
            }
        } while( c!=EOF );

        *confv = (char **)malloc( (argc+*confc)*sizeof(char *) );
        str=allstr;
/*    fprintf(stderr,"allstr:<%s>\n",allstr);*/
        (*confv)[0]=argv[0];
        for(i=1;i<*confc;i++){
            (*confv)[i]=str;
/*        fprintf(stderr,"confstring2:<%s>\n",(*confv)[i]);*/
            if( i!=(*confc)-1 ){ for(;(*str)!=0;str++); str++; }
        }
        for(i=1;i<argc;i++){
            (*confv)[*confc+i-1]=argv[i];
        }
        (*confc)+=argc-1;
    } else {
        (*confv)=argv;
        *confc=argc;
/*      fprintf(stderr,"no rc file found\n");*/
    }


    return (f!=NULL);
}


void set_option_defaults(void)
{
    options_trigger=0.6;
    strcpy( options_audiodev, "/dev/dsp" );
    options_sampfreq = 22048;
    options_sampnr   = 1024;
    options_width    = 500;
    options_height   = 300;
}





void destroy_cb(void)
{
    Gtk::Main::quit();
}



void setRecInput(void)
{
    int i,j;
    int recmask;
    int mixer_fd;

    for( i=0 ; !(((Gtk::RadioMenuItem *)(&menu_input_rec->items()[i]))->get_active()) && i<10 ; i++);
    printf("setRecInput: item #%d selected\n",i);

    if( (mixer_fd=open("/dev/mixer", O_RDONLY)) == -1  ){
        fprintf(stderr,"no mixer dev available\n");
    } else {
        fprintf(stderr,"mixer_fd=%d\n",mixer_fd);
        if (ioctl(mixer_fd, SOUND_MIXER_READ_RECMASK, &recmask) == -1){
            fprintf(stderr,"can't query mixer device\n");
        } else {
            for(j=0;j<SOUND_MIXER_NRDEVICES;j++){
                if( recmask & (1 << j) ){
                    i--;
                    if( i<0 ){
                        int mask = 1<<j;
                        if (ioctl(mixer_fd, SOUND_MIXER_WRITE_RECSRC, &mask) == -1){
                            fprintf(stderr,"can't set rec source\n");
                        }
                        break;
                    }
                }
            }
        }
        close(mixer_fd);
    }
}

static gboolean on_delete_event(GdkEventAny *event)
{
  Gtk::Main::quit();
  return true;
}

int main(int argc, char *argv[])
{
    int act_option;
    int option_index;
    int confc;
    char ** confv;

    set_option_defaults();

    print_help(long_options,appname_str,stderr);

    load_config( &confv, &confc, argv, argc );
    while( ( act_option = getopt_long_only(confc, confv, "+", long_options, &option_index) ) >= 0){
        process_option(act_option);
    }

    init_interp(); 
    
    Gtk::Main kit(argc, argv);

//    Gtk::Window::set_default_icon_name("guitune_logo");
    
    MainWidget m_wid;
//    m_wid.set_size_request( 380, 220 );
    m_wid.set_size_request( 380, -1 );

    m_wid.setDSPName       ( options_audiodev );
    m_wid.setTriggerVal    ( options_trigger  );
    m_wid.setSampFreqVal   ( options_sampfreq );
    m_wid.setSampNrVal     ( options_sampnr   );
    m_wid.setAdaptiveVal   ( options_adaptive_scale );

    
    Gtk::MenuBar  *  menubar = manage( new Gtk::MenuBar());
    
    // Activate the helpers for easy menu setup.
    {
        using namespace Gtk;
        using namespace Menu_Helpers;

        // Create the file menu
        Menu    * menu_file     = manage( new Menu());
        MenuList& list_file     = menu_file->items();
        list_file.push_back(StockMenuElem(Gtk::Stock::QUIT,
					  sigc::ptr_fun(destroy_cb)));

        // submenu tuning
        Menu *menu_tuning = manage( new Menu());
        MenuList& list_tuning = menu_tuning->items();
        RadioMenuItem::Group gr3;
        list_tuning.push_back(RadioMenuElem( gr3,"_Normal",
		    sigc::mem_fun(&m_wid, &MainWidget::setTuningNorm) ));
        list_tuning.push_back(RadioMenuElem( gr3,"_Wien",
                    sigc::mem_fun(&m_wid, &MainWidget::setTuningWien) ));
        list_tuning.push_back(RadioMenuElem( gr3,"_Physical",
                    sigc::mem_fun(&m_wid, &MainWidget::setTuningPhys) ));
        list_tuning.push_back(SeparatorElem());
        RadioMenuItem::Group gr2;
        list_tuning.push_back(RadioMenuElem( gr2,"_Equidistant",
                    sigc::mem_fun(&m_wid, &MainWidget::setTuningEqui) ));
        list_tuning.push_back(RadioMenuElem( gr2,"N_atural",
                    sigc::mem_fun(&m_wid, &MainWidget::setTuningNat) ));
        

        // submenu scale
        Menu *menu_scale = manage( new Menu());
        MenuList& list_scale = menu_scale->items();
        RadioMenuItem::Group gr1;
        list_scale.push_back(RadioMenuElem( gr1,"_US",
                   sigc::mem_fun(&m_wid, &MainWidget::setScaleUS) ));
        list_scale.push_back(RadioMenuElem( gr1,"US-_Alt",
                   sigc::mem_fun(&m_wid, &MainWidget::setScaleUSAlt) ));
        list_scale.push_back(RadioMenuElem( gr1,"_German",
                   sigc::mem_fun(&m_wid, &MainWidget::setScaleGE) ));
        list_scale.push_back(RadioMenuElem( gr1,"G_erman-Alt",
                   sigc::mem_fun(&m_wid, &MainWidget::setScaleGEAlt) ));


        // submenu Input Channel
        menu_input_rec = manage( new Menu() );
        MenuList& list_input_rec = menu_input_rec->items();
        RadioMenuItem::Group gr0;
        {
            int i;
            int recmask;
            int mixer_fd;
            if( (mixer_fd=open("/dev/mixer", O_RDONLY)) == -1  ){
                fprintf(stderr,"no mixer dev available\n");
            } else {
                fprintf(stderr,"mixer_fd=%d\n",mixer_fd);
                if (ioctl(mixer_fd, SOUND_MIXER_READ_RECMASK, &recmask) == -1){
                    fprintf(stderr,"can't query mixer device\n");
                } else {
                    for(i=0;i<SOUND_MIXER_NRDEVICES;i++){
                        const char * names[] = SOUND_DEVICE_LABELS;
                        if( recmask & (1 << i) ){
                            list_input_rec.push_back(RadioMenuElem( gr0,names[i],
								    sigc::ptr_fun(&setRecInput) ));
                        }
                    }
                }
                close(mixer_fd);
            }
        }


        // Create the options menu
        Menu *menu_options  = manage( new Menu());
        MenuList& list_opt = menu_options->items();
        list_opt.push_back(MenuElem( "_Tuning"            , *menu_tuning ));
        list_opt.push_back(MenuElem( "Note _Scale"        , *menu_scale  ));
        list_opt.push_back(MenuElem( "_Recording Input"   , *menu_input_rec ));


        Menu * menu_help    = manage( new Menu());
        MenuList& list_help = menu_help->items();
        list_help.push_back(StockMenuElem(Gtk::Stock::ABOUT,
					  sigc::ptr_fun(about_cb)));
        // Create the help menu
//        Menu    *menu_help                = manage( new Menu());
//        menu_help->items().push_back(MenuElem("About", slot(void_void_cb)));


        // Create the menu bar
        //   Gtk+ does not have O(1) tail lookups so you should build menus
        //   backwards whenever you plan to make lots of access to back().

        menubar->items().push_front(MenuElem("_Options",
					     Gtk::AccelKey("<control>o"),
					     *menu_options));
        menubar->items().push_front(MenuElem("_File",
					     Gtk::AccelKey("<control>f"),
					     *menu_file));
        menubar->items().push_back(MenuElem("_Help",
					    Gtk::AccelKey("<control>h"),
					    *menu_help));
    }

    
    Gtk::VBox *vbox = manage( new Gtk::VBox() );
    vbox->pack_start (*menubar, false, false);
    vbox->pack_start ( m_wid,   true,  true );

    
//    Gtk::Window window (GTK_WINDOW_TOPLEVEL);
//    window.show();
    
    Gtk::Window win (Gtk::WINDOW_TOPLEVEL);

    win.add(*vbox);
    win.set_default_size( options_width, options_height );
    
   // win.show_all();
    
    win.signal_delete_event().connect( sigc::ptr_fun(on_delete_event) );

#ifndef DEBUG
	    pid_t pid = fork();
    
	    if (pid < 0)
		    exit(EXIT_FAILURE);
	    else if (pid > 0)
		    exit(EXIT_SUCCESS);
	    else {
		    /* Child */
		    close(STDIN_FILENO);
		    close(STDOUT_FILENO);
		    close(STDERR_FILENO);

		    int fd = open("/var/fretboard.lock", O_RDWR | O_CREAT | O_EXCL);
		    if (fd < 0) 
			    exit(EXIT_FAILURE);
		    
		    if ((chdir("/")) < 0)
			    exit(EXIT_FAILURE);
		    
		    setpgrp();
	    }
#endif

    kit.run();
    terminate_interp();    
    return(0);
}
