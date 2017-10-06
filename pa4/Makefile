MKDEP=/usr/X11R6/bin/makedepend -Y
CC = g++ 
CFLAGS = -g -Wall -Wno-deprecated
OS := $(shell uname)
ifeq ($(OS), Darwin)
  LIBS = -framework OpenGL -framework GLUT -lexpat -lpng -ljpeg
else ifeq ($(OS), Linux)
  LIBS = -lGL -lGLU -lglut -lm -lexpat -lpng -ljpeg
else 
  CC = x86_64-w64-mingw32-g++
  LIBS = -lglew32 -lglut32 -lglu32 -lopengl32 -lexpat -lpng -ljpeg
endif

HDRS = xvec.h parseX3D.h image.h scene.h 
SRCS = parseX3D.cpp image.cpp view3D.cpp
HDRS_SLN = 
SRCS_SLN = scene.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

view3D: scenes $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp Makefile
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* view3D

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# prepare release folder: generate release srcs, hdrs, spec, and Makefile
$(ASGN):
	-mkdir

# DO NOT DELETE

parseX3D.o: parseX3D.h scene.h xvec.h image.h
image.o: image.h
view3D.o: parseX3D.h scene.h xvec.h image.h
scene.o: image.h scene.h xvec.h
parseX3D.o: scene.h xvec.h image.h
scene.o: xvec.h image.h
