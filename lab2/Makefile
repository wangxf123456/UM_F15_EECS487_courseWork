#RELEASE BELOW ONLY
MKDEP=makedepend -Y
CC = g++
CFLAGS = -g -Wall -Wno-deprecated
OS := $(shell uname)
ifeq ($(OS), Darwin)
  LIBS = -framework OpenGL -framework GLUT -lm -lc
else ifeq ($(OS), Linux)
  LIBS = -lGL -lGLU -lglut -lm
else
  CC = x86_64-w64-mingw32-g++
  LIBS = -lglut32 -lglu32 -lopengl32
endif

HDRS = 
SRCS = 
HDRS_SLN = 
SRCS_SLN = drawtriangle.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

drawtriangle: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean: 
	-rm -f -r *.o *~ *core* drawtriangle

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE
