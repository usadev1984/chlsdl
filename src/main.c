#include "main.h"

#include <signal.h>
#include <stdlib.h>

static void
cleanup(int sig)
{
    exit(sig);
}

int
main()
{
    if (sigaction(SIGINT, &(struct sigaction) { .sa_handler = cleanup }, NULL)
        == -1) {
        cleanup(1);
    }

    cleanup(0);

    return 0;
}
