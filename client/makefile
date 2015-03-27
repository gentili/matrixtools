
LIBS=	-lutility -ldisplay -lpthread -lprocps -lncurses

LIBDIRS=	-L../utility -L../display 

INCDIRS=	-I../utility -I../display -I../server

# -----------------------
ALL_TARGETS= matrix 
include ../makefile.in
# -----------------------

#CFLAGS += -DDEBUG
CFLAGS +=

OFILES =

matrix: matrix.o PSModule.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
