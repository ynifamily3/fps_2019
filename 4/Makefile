############## SunOS ########################
CC       = gcc -g
CCOPTS   = -c #-Wall
LINK     = gcc -g

DEPEND  = makedepend

LINKOPTS = -lm -lnsl
#############################################

SOURCES = ftlmgr.c devicedriver.c main.c

OBJECTS = ftlmgr.o devicedriver.o main.o

.SUFFIXES : .C .c .o

.c.o:
	$(CC) $(CCOPTS) $<

all: blkmapftl

blkmapftl: $(OBJECTS)
	$(LINK) -o blkmapftl $(OBJECTS) $(LINKOPTS)

clean:
	rm *.o blkmapftl core

depend : $(SOURCES)
	$(DEPEND) $(DEFINES) -I/usr/local/lib/g++-include $(SOURCES)

# DO NOT DELETE
