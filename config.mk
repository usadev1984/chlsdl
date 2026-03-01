LIBS = -lpcre2-8

CFLAGS = -std=gnu23 -fPIC -Wall -Wextra ${INCS}

LDFLAGS = ${LIBS}

CC = gcc
