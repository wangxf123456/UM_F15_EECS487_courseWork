CC = g++
CFLAGS = -g -Wall -Wno-deprecated-declarations
MKDEP=makedepend -Y
ifeq ($(shell uname), Darwin)
	LIBS = -framework OpenGL -framework GLUT -lm
else
	LIBS = -lGL -lGLU -lglut -lm
endif

HDRS = scene.h xvec.h
SRCS = 
HDRS_SLN = 
SRCS_SLN = raytrace.cpp scene.cpp
OBJS = $(patsubst %.cpp, %.o, $(SRCS)) $(patsubst %.cpp,%.o,$(SRCS_SLN))

raytrace: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean: 
	-rm -f -r $(OBJS) *.o *~ *core* raytrace

depend: $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) Makefile
	$(MKDEP) $(CFLAGS) $(SRCS) $(SRCS_SLN) $(HDRS) $(HDRS_SLN) >& /dev/null

# DO NOT DELETE

raytrace.o: scene.h xvec.h
scene.o: xvec.h scene.h
scene.o: xvec.h
