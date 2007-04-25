
LIBS=	-lncurses -lutility -ldisplay -lpthread -lproc-3.2.7

LIBDIRS=	-L../utility -L../display -L/lib -L/home/gentili/software/procps-3.2.7/proc

INCDIRS=	-I../utility -I../display -I../server -I/home/gentili/software/procps-3.2.7/proc

# -----------------------
ALL_TARGETS= matrix 
include ../makefile.in
# -----------------------

#CFLAGS += -DDEBUG
CFLAGS +=

OFILES =

matrix: matrix.o PSModule.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
