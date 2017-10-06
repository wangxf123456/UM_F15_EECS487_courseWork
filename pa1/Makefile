#RELEASE BELOW ONLY 
MKDEP=makedepend -Y
CC = g++ 
CFLAGS = -g -Wall -Wno-deprecated-declarations
OS := $(shell uname)
ifeq ($(OS), Darwin)
  LIBS = -framework OpenGL -framework GLUT -lm -lc
else ifeq ($(OS), Linux)
  LIBS = -lGL -lGLU -lglut -lm
else 
  CC = x86_64-w64-mingw32-g++
  LIBS = -lglut32 -lglu32 -lopengl32
endif

HDRS = canvas.h colorpicker.h xvec.h targa.h
SRCS = draw.cpp canvas.cpp colorpicker.cpp
HDRS_SLN = rasterizer.h
SRCS_SLN = rasterizer.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

all: vcanvas draw

vcanvas: vcanvas.o $(patsubst %.cpp,%.o,$(SRCS_SLN))
	$(CC) $(CFLAGS) -o $@ vcanvas.o $(patsubst %.cpp,%.o,$(SRCS_SLN)) $(LIBS)

draw: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean:
	-rm -rf *.o *~ *core* vcanvas draw

depend: $(SRCS) vcanvas.cpp $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

draw.o: canvas.h rasterizer.h xvec.h colorpicker.h targa.h
canvas.o: canvas.h rasterizer.h xvec.h
colorpicker.o: colorpicker.h xvec.h
rasterizer.o: rasterizer.h xvec.h
canvas.o: rasterizer.h xvec.h
colorpicker.o: xvec.h
rasterizer.o: xvec.h
