# compile time flags:
# COLOR	- enable colored output

.ONESHELL:

MAKEFLAGS += -rR

include config.mk
override INCS += -Isrc -Ilib -Iinclude
CFLAGS += INCS

ifeq ($(strip $(COLOR)),1)
	CFLAGS += -DCOLOR
endif

ifeq ($(strip $(PREFIX)),)
	PREFIX = /usr/local
endif

OBJ	 := $(patsubst %.c, %.o, $(wildcard src/*/*.c)) src/main.o

# outputs
BIN					:= chlsdl
DEBUG_BIN			:= ${BIN}-debug

install:
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${BIN} ${DESTDIR}${PREFIX}/bin/${BIN}
	chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN}

release: ${BIN}

debug: CFLAGS += -DDEBUG -g3
debug: ${DEBUG_BIN}

${BIN}: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ || exit 1

${DEBUG_BIN}: ${OBJ}
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ $^ || exit 1

${OBJ}: %.o: %.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ -c $^ || exit 1

clean:
	rm -f ${OBJ} ${BIN} ${DEBUG_BIN} &> /dev/null

.PHONY: debug release install clean
# end
