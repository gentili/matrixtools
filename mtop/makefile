
LIBS=	-lutility -ldisplay -lpthread -lprocps -lncurses

LIBDIRS=	-L../utility -L../display 

INCDIRS=	-I../utility -I../display -I../server

# -----------------------
ALL_TARGETS= mtop 
include ../makefile.in
# -----------------------

#CFLAGS += -DDEBUG
CFLAGS +=

OFILES =

mtop: main.o MatrixTop.o
	$(CXX) -o $@ $(CFLAGS) $^ $(INCDIRS) $(LIBDIRS) $(LIBS)
