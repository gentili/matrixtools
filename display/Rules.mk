# External build integration
# !! Not used by matrixtools makefile system !!

SRCS := MatrixColumn.cpp Screen.cpp

TARGETS := libdisplay.a

libdisplay.a_DEPS = $(OBJS_$(d))
