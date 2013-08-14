
header-links: remove-links
	HEADERLIST="$(top_srcdir)/geom/*.hh \
		    $(top_srcdir)/physics/*.hh \
		    $(top_srcdir)/base/*.hh"; \
	for hh in $$HEADERLIST; do \
	  BASENAME=`basename $$hh`; \
	  $(LN_S) $$hh $$BASENAME;\
	done

remove-links: 
	rm -f *.hh


all: all-am header-links

clean: clean-am remove-links
distclean: distclean-am remove-links
