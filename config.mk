LIBS = -lchlsdl-common -lpcre2-8 -ljson-c

CFLAGS = -std=gnu23 -fPIC -Wall -Wextra ${INCS}

LDFLAGS = ${LIBS}

CC = gcc
