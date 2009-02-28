## Makefile.am
## 05.02.2009

bin_PROGRAMS       = fretboard

fretboard_SOURCES = main.cc \
                     guitune.cc lcdview.cc logview.cc osziview.cc python-interp.cc\
                     guitune.h  lcdview.h  logview.h  osziview.h\
                     resources.h FB_logo.xpm \
                     aboutbox.c aboutbox.h

fretboard_LDADD   = @GTKMM_LIBS@ @PYTHON_LDFLAGS@
INCLUDES           = @GTKMM_CFLAGS@ @PYTHON_CPPFLAGS@
