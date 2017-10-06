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
  CFLAGS = -g -Wall -Wno-deprecated -Wno-comment -I./common/include
  OPT = ./common/src/wingetopt.o
endif

TASK1 = 0
HDRS = xvec.h objects.h shaders.h ltga.h
SRCS = shaders.cpp ltga.cpp
HDRS_SLN = 
ifeq ($(TASK1), 0)
	SRCS_SLN = scene.cpp objects.cpp
else
	SRCS_SLN = scene0.cpp objects0.cpp
endif
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

scene: $(OBJS) $(OPT)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(OPT) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* scene

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

ltga.o: ltga.h
scene.o: ltga.h xvec.h objects.h shaders.h
objects.o: ltga.h xvec.h objects.h
