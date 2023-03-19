PROGNAME = main

CXXFLAGS = -Iinclude -D_POSIX_C_SOURCE=200809L -D_DEFAULT_SOURCE -D_GNU_SOURCE -DPROGNAME=\"${PROGNAME}\"
CFLAGS = -std=gnu17 -Wall -pedantic -pipe -O2 ${CXXFLAGS}
LDFLAGS =

ifneq ($(strip ${DEBUG}),)
	DEBUG_CXXFLAGS = -D_JOVAMI_DEBUG
	DEBUG_CFLAGS = -g ${DEBUG_CXXFLAGS}
endif

CC = cc

CSRC = main.c

OBJ = ${CSRC:.c=.o} ${SSRC:.s=.o}


all: flags ${PROGNAME}

flags:
	@echo "${PROGNAME} build options:"
	@echo "CFLAGS      =    ${CFLAGS}"
	@echo "LDFLAGS     =    ${LDFLAGS}"
	@[ -z "${DEBUG}" ] || echo "DEBUG_CFLAGS =    ${DEBUG_CFLAGS}"
	@echo "CC          =    ${CC}"
	@echo


%.o: %.c
	$(CC) -c ${CFLAGS} ${DEBUG_CFLAGS} -o $@ $<

%.o: %.s
	$(CC) -c ${DEBUG_CFLAGS} -o $@ $<

${PROGNAME}: ${OBJ}
	$(CC) -o $@ $^ ${LDFLAGS}

run: all
	./${PROGNAME}

clean:
	rm -f ${OBJ} ${PROGNAME}

.PHONY: all flags clean
