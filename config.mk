LIBS = -lpcre2-8 -lcurl -ljson-c

CFLAGS = -std=gnu23 -fPIC -Wall -Wextra ${INCS}

LDFLAGS = ${LIBS}

CC = gcc
