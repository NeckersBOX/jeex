DEFS = -DDATADIR=\"$(pkgdatadir)\" \
       -DSYSCONFDIR=\"@sysconfdir@\" \
       -DLOCALEDIR=\"$(localedir)\" \
       @debug_flag@ \
       @regex_time_limit_off_flag@ \
       @DEFS@

bin_PROGRAMS = jeex

jeex_SOURCES = \
               header.h \
               edit.h edit.c \
               file.h file.c \
               main.c \
               view.h view.c \
               preferences.h preferences.c \
               bookmark.h bookmark.c \
               info.h info.c \
               general-function.h general-function.c \
               notebook-manage.h notebook-manage.c \
               menu.h menu.c \
               html-export.h html-export.c \
               logging.h logging.c \
               search.h search.c \
               terminal-actions.h terminal-actions.c \
               tools.h tools.c \
               plugin.h plugin.c

jeex_LDADD = ../plugin/libjeex_hello.a \
             ../plugin/libjoiner.a \
             @GTK_LIBS@
