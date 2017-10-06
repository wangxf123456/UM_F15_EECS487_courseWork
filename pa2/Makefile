MKDEP=makedepend -Y
CC = g++ 
CFLAGS = -g -Wall -Wno-deprecated-declarations
OS := $(shell uname)
ifeq ($(OS), Darwin)
  LIBS = -framework OpenGL -framework GLUT -lm -lc -lexpat
else ifeq ($(OS), Linux)
  LIBS = -lglew -lGL -lGLU -lglut -lm -lexpat
else
  CC = x86_64-w64-mingw32-g++
  LIBS = -lglew32 -lglut32 -lglu32 -lopengl32 -lexpat
endif

HDRS = xvec.h parseX3D.h shaders.h
SRCS = view3D.cpp parseX3D.cpp shaders.cpp
HDRS_SLN = scene.h
SRCS_SLN = scene.cpp 
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

view3D: scenes $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* view3D

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

view3D.o: parseX3D.h scene.h xvec.h shaders.h
parseX3D.o: parseX3D.h scene.h xvec.h
scene.o: scene.h xvec.h
parseX3D.o: scene.h xvec.h
scene.o: xvec.h
