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
LIB_OBJ := $(patsubst %.c, %.o, $(wildcard src/common/*/*.c)) \
				$(patsubst %.c, %.o, $(wildcard src/common/*.c))

# outputs
BIN					:= chlsdl
DEBUG_BIN			:= ${BIN}-debug
# libchlsdl-common
LIB_BASENAME := lib${BIN}-common
LIB_COMMON   := ${LIB_BASENAME}.so
VER_MAJOR    := 0
VER_MINOR    := 0
VER_PATCH    := 0
LIB_MAJOR    := ${LIB_BASENAME}.so.$(VER_MAJOR)
LIB_MINOR    := ${LIB_BASENAME}.so.$(VER_MAJOR).$(VER_MINOR)
LIB_PATCH    := ${LIB_BASENAME}.so.$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)
LIB_VERSION  := ${LIB_BASENAME}.so.$(VER_MAJOR).$(VER_MINOR).$(VER_PATCH)

CFLAGS +=  -DCHLSDL_VERSION=\"${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}\" \
				-DCHLSDL_MAJOR="${VER_MAJOR}" -DCHLSDL_MINOR="${VER_MINOR}" \
				-DCHLSDL_PATCH="${VER_PATCH}"

install:
	mkdir -p ${DESTDIR}${PREFIX}/lib/${BIN}-modules
	cp -f ${LIB_VERSION} ${DESTDIR}${PREFIX}/lib/${BIN}-modules/${LIB_VERSION}
	chmod 755 ${DESTDIR}${PREFIX}/lib/${BIN}-modules/${LIB_VERSION}

	cp -f ${LIB_MINOR} ${DESTDIR}${PREFIX}/lib/${BIN}-modules/${LIB_MINOR}

	cp -f ${LIB_MAJOR} ${DESTDIR}${PREFIX}/lib/${BIN}-modules/${LIB_MAJOR}
	cp -f ${LIB_COMMON} ${DESTDIR}${PREFIX}/lib/${BIN}-modules/${LIB_COMMON}

	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${BIN} ${DESTDIR}${PREFIX}/bin/${BIN}
	chmod 755 ${DESTDIR}${PREFIX}/bin/${BIN}

release: ${BIN}

debug: CFLAGS += -DDEBUG -g3
debug: ${DEBUG_BIN}

libchlsdl-common: ${LIB_COMMON}

debug_libchlsdl-common: CFLAGS += -DDEBUG -g3
debug_libchlsdl-common: ${LIB_COMMON}

${BIN}: ${OBJ}
	${CC} ${CFLAGS} -lX11 -lXmu ${LDFLAGS} -L. -lchlsdl-common -o $@ $^ || exit 1

${DEBUG_BIN}: ${OBJ}
	${CC} ${CFLAGS} -lX11 -lXmu ${LDFLAGS} -L. -lchlsdl-common -o $@ $^ || exit 1

${LIB_COMMON}: ${LIB_MAJOR}
	ln -sf $^ $@

${LIB_MAJOR}: ${LIB_MINOR}
	ln -sf $^ $@

${LIB_MINOR}: ${LIB_VERSION}
	ln -sf $^ $@

${LIB_VERSION}: ${LIB_OBJ}
	${CC} -shared -Wl,-soname,${LIB_MAJOR} ${LDFLAGS} ${CFLAGS} -o $@ $^ || exit 1

${OBJ}: %.o: %.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ -c $^ || exit 1

${LIB_OBJ}: %.o: %.c
	${CC} ${CFLAGS} ${LDFLAGS} -o $@ -c $^ || exit 1

clean:
	rm -f ${OBJ} ${LIB_OBJ} ${BIN} ${DEBUG_BIN} &> /dev/null

.PHONY: debug release install clean debug_libchlsdl-common libchlsdl-common
# end
