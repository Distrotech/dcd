## This is the makefile for dcd, dave's cd player.

# Select your compiler; gcc is probably just fine.
CC = gcc

# Prefix for installing dcd. `make install' will put files in PREFIX/bin
# and PREFIX/man/man1 .
PREFIX = /usr

# Where should dcd store its process ID? (relative to ~)
# This is the default, which you probably don't need to change.
# PID = .dcdpid

# Some CD-ROMs are a bit buggy, and take a second or two to switch
# tracks. The net result is that, usually, the last second or two
# of a track is cut off. If you have that problem, change this 'define'
# to be something greater than zero and uncomment it. (It's in seconds.)
# SLEEP = -DEXTRA_SLEEP_TIME=0

# This is your CD-ROM device. The default is /dev/cdrom so if you're okay
# with that, you don't even need to uncomment the next line.
# CDROM = /dev/cdrom
# Or, if you use devfs, uncomment this line:
CDROM = /dev/cdroms/cdrom0



## Less-configurable stuff here.

# Uncomment the next line to enable debugging flags. This will bloat the
# programs, and dump all sorts of mildly-useful stuff to stderr.
# In all likelihood you never want to do this. BTW, defining DEBUG as 1
# will put in some debugging, and defining it > 1 puts in bucketloads of it.
# DEBUG = 1
# DEBUG = 2

# Don't enable this unless you have severe problems, as it's dorky.
# (For the gearheads: this uses CDROMSTOP instead of CDROMSTART to handle
# some track-change behaviours. CDROMSTART is more standards-compliant,
# friendlier, and emits 20% fewer oxides of nitrogen.)
# OLDSTOP = 1

RM = rm

## If you want to disable the network code, try this. It probably doesn't
## work, but you're welcome to try ... Just comment out the next line.

#NETWORK = 1

## Here be dragons... (Non-configurable parts past here.)

ifdef DEBUG
EXTRA_CFLAGS = -DDEBUG=${DEBUG} -Wall -pedantic -g
else
EXTRA_CFLAGS = -O2 # -march=k6 # the arch optimization is for dave!
endif

ifdef CDROM
EXTRA_CFLAGS += -DCDROM_DEVICE=\"${CDROM}\"
endif

ifdef PID
EXTRA_CFLAGS += -DPIDFILE=\"${PID}\"
endif

ifdef SLEEP
EXTRA_CFLAGS += ${SLEEP}
endif

ifdef NETWORK
EXTRA_SRCS   += mbo.c minilzo.c minilzo.h lzoconf.h
EXTRA_OBJS   += mbo.o minilzo.o
EXTRA_CFLAGS += -DNETWORK
EXTRA_LFLAGS += -lmusicbrainz -lstdc++ -lm
else
EXTRA_SRCS   += mbo-nonet.c
EXTRA_OBJS   += mbo-nonet.o
endif

ifdef OLDSTOP
EXTRA_CFLAGS += -DOLD_STOP_BEHAVIOUR
endif

# end of compile-time option twiddlers

PROGS = dcd
OBJECTS = screenop.o infinite.o libcdplay.o process.o \
	sha.o cd-misc.o dcd.o base64.o ${EXTRA_OBJS}
SRCS = screenop.c screenop.h infinite.c infinite.h libcdplay.c libcdplay.h \
       process.c process.h dcd.c dcd.h sha.c sha.h endian.h version.h \
       cd-misc.h cd-misc.c Makefile mbo.h base64.c base64.h sha.c \
       sha.h ${EXTRA_SRCS}

all: ${PROGS}

.c.o: ${SRCS}
	$(CC) -c $(CFLAGS) ${EXTRA_CFLAGS} -I/usr/local/include $< -o $@

${PROGS}: ${OBJECTS}
	${CC} ${CFLAGS} ${EXTRA_CFLAGS} *.o ${EXTRA_LFLAGS} -o dcd 

clean:;
	-${RM} *.o ${PROGS}

install: dcd
	install -m 755 -d $(DESTDIR)${PREFIX}/bin $(DESTDIR)${PREFIX}/man/man1
	install -m 755 dcd $(DESTDIR)${PREFIX}/bin
	install -m 644 dcd.1 $(DESTDIR)${PREFIX}/man/man1
	install -m 755 -d $(DESTDIR)${HOME}/${CDI}

depend:
	makedepend -- ${CFLAGS} ${EXTRA_CFLAGS} -- ${SRCS}
# The above option seems to be extraneous now.

# DO NOT DELETE
