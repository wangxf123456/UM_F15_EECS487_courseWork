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

HDRS = objects.h viewing.h xmat.h
SRCS = 
HDRS_SLN = xvec.h 
SRCS_SLN = viewing.cpp objects.cpp transforms.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

viewing: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* viewing

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

viewing.o: xvec.h xmat.h objects.h viewing.h
objects.o: xvec.h objects.h xmat.h viewing.h
transforms.o: xvec.h xmat.h viewing.h
objects.o: xvec.h xmat.h viewing.h
viewing.o: xvec.h
xmat.o: xvec.h
