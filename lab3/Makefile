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

HDRS = objects.h matmv.h xmat.h
SRCS = modeling.cpp
HDRS_SLN = xvec.h transforms.h
SRCS_SLN = matmv.cpp transforms.cpp objects.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

modeling: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* modeling

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

modeling.o: xvec.h xmat.h objects.h transforms.h
matmv.o: matmv.h xmat.h xvec.h
transforms.o: matmv.h xmat.h xvec.h
objects.o: xvec.h objects.h xmat.h transforms.h matmv.h
objects.o: xvec.h xmat.h transforms.h
matmv.o: xmat.h xvec.h
xmat.o: xvec.h
