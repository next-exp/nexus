#INCLUDES= -I. -I.. 

#EXTRA_DIST = run_examples.sh 
bin_PROGRAMS = xnext

xnext_SOURCES = main.cc
xnext_LDADD = ../base/libBase.la \
	       ../geom/libGeom.la \
	       ../physics/libPhysics.la $(LIBS)



#applications: $(EXTRA_PROGRAMS)
CLEANFILES = *~* 


