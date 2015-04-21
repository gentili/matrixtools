
# -----------------------
ALL_TARGETS= libutility.a
include ../makefile.in
# -----------------------

OFILES =	MTObject.o

libutility.a: $(OFILES)
	ar vr $@ $^

threadtest: threadtest.o
	g++ -pthread -std=c++11 $^ -o $@
