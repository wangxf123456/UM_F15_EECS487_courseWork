MKDEP=makedepend -Y
CC = g++
CFLAGS = -g -Wall -Wno-deprecated-declarations
OS := $(shell uname)
ifeq ($(OS), Darwin)
  LIBS = -framework OpenGL -framework GLUT -lm -lc
else ifeq ($(OS), Linux)
  LIBS = -lGL -lGLU -lGLEW -lglut -lm
else
  CC = x86_64-w64-mingw32-g++
  LIBS = -lglew32 -lglut32 -lglu32 -lopengl32
endif

HDRS = xvec.h xmat.h
SRCS =
HDRS_SLN = 
SRCS_SLN = shadows.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

glshadows: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* glshadows

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

shadows.o: xvec.h xmat.h
xmat.o: xvec.h
