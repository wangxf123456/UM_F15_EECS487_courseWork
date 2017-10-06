MKDEP=makedepend -Y
CC = g++
CFLAGS = -g -Wall -Wno-deprecated
OS := $(shell uname)
ifeq ($(OS), Darwin)
  LIBS = -framework OpenGL -framework GLUT -lm -lc
else ifeq ($(OS), Linux)
  LIBS = -lGL -lGLU -lglut -lm
# CFLAGS = -g -Wall -Wno-deprecated -I./GLEW/include
  CFLAGS = -g -Wall -Wno-deprecated
  GLEW = ./GLEW/src/glew.o
else
  CC = x86_64-w64-mingw32-g++
  LIBS = -lglew32 -lglut32 -lglu32 -lopengl32
endif

HDRS = objects.h modeling.h viewing.h xvec.h transforms.h
SRCS = modeling.cpp viewing.cpp
HDRS_SLN = 
SRCS_SLN = lighting.cpp objects.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

lighting: $(OBJS) $(GLEW)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(GLEW) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $<

.PHONY: clean
clean: 
	-rm -f -r *.o *~ *core* lighting

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

viewing.o: xvec.h viewing.h transforms.h
lighting.o: xvec.h objects.h modeling.h viewing.h transforms.h
objects.o: xvec.h objects.h viewing.h transforms.h
viewing.o: xvec.h
