# compile time flags:
# COLOR	- enable colored output

.ONESHELL:

MAKEFLAGS += -rR

include config.mk
override INCS += -Isrc -Ilib -Iinclude
# CFLAGS += INCS

ifeq ($(strip $(COLOR)),1)
	CFLAGS += -DCOLOR
endif

ifeq ($(strip $(PREFIX)),)
	PREFIX = /usr/local
endif

OBJ	 := $(patsubst %.c, %.o, $(wildcard src/*.c)) # src/main.o

# outputs
BIN					:= chlsdl
DEBUG_BIN			:= ${BIN}-debug
VER_MAJOR    := 0
VER_MINOR    := 0
VER_PATCH    := 0

CFLAGS +=  -DCHLSDL_VERSION=\"${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}\" \
				-DCHLSDL_MAJOR="${VER_MAJOR}" -DCHLSDL_MINOR="${VER_MINOR}" \
				-DCHLSDL_PATCH="${VER_PATCH}" \
				-DMODULES_PATH=\"${PREFIX}/lib/${BIN}-modules\"

install:
	mkdir -p ${DESTDIR}${PREFIX}/include/
	cp -rf include/${BIN} ${DESTDIR}${PREFIX}/include/
	# chmod -R 444 ${DESTDIR}${PREFIX}/include/${BIN}

	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${BIN} ${DESTDIR}${PREFIX}/bin/${BIN}
	chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN}

release: ${BIN}

debug: CFLAGS += -DDEBUG -g3
debug: ${DEBUG_BIN}

${BIN}: ${OBJ}
	${CC} ${CFLAGS} -lX11 -lXmu ${LDFLAGS} -o $@ $^ || exit 1

${DEBUG_BIN}: ${OBJ}
	${CC} ${CFLAGS} -lX11 -lXmu ${LDFLAGS} -o $@ $^ || exit 1

${OBJ}: %.o: %.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ -c $^ || exit 1

clean:
	rm -f ${OBJ} ${BIN} ${DEBUG_BIN} &> /dev/null

.PHONY: debug release install clean
# end
