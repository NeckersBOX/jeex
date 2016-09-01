DEFS = -DDATADIR=\"$(pkgdatadir)\" \
       -DSYSCONFDIR=\"@sysconfdir@\" \
       -DLOCALEDIR=\"$(localedir)\" \
       @debug_flag@ \
       @regex_time_limit_off_flag@ \
       @DEFS@

noinst_LIBRARIES = libjeex_hello.a libjoiner.a

libjeex_hello_a_SOURCES = header.h jeex_hello.h jeex_hello.c 
libjoiner_a_SOURCES = header.h joiner.h joiner.c

