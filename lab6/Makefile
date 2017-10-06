MKDEP=/usr/X11R6/bin/makedepend -Y
CC = g++
CFLAGS = -g -Wall -Wno-deprecated-declarations
OS := $(shell uname)
ifeq ($(OS), Darwin)
  LIBS = -framework OpenGL -framework GLUT -lm -lc
else ifeq ($(OS), Linux)
  LIBS = -lGL -lGLU -lGLEW -lglut -lm
#  CFLAGS = -g -Wall -Wno-deprecated -I./GLEW/include
#  GLEW = ./GLEW/src/glew.o
else
  CC = x86_64-w64-mingw32-g++
  LIBS = -lglew32 -lglut32 -lglu32 -lopengl32
endif

HDRS = ltga.h xvec.h objects.h
SRCS = ltga.cpp
HDRS_SLN = 
SRCS_SLN = globe.cpp objects.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

globe: $(OBJS) $(GLEW)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(GLEW) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* globe

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

ltga.o: ltga.h
globe.o: ltga.h xvec.h objects.h
objects.o: ltga.h xvec.h objects.h
