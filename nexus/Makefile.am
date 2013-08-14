header-links: remove-links
	HEADERLIST="$(top_srcdir)/geometry/*.h \
		    $(top_srcdir)/physics/*.h \
		    $(top_srcdir)/generator/*.h \
		    $(top_srcdir)/manager/*.h \
		    $(top_srcdir)/base/*.h"; \
	for hh in $$HEADERLIST; do \
	  BASENAME=`basename $$hh`; \
	  $(LN_S) $$hh $$BASENAME;\
	done

remove-links: 
	rm -f *.h

all: all-am header-links

clean: clean-am remove-links
distclean: distclean-am remove-links
