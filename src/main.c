#include "main.h"
#include "print.h"

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
        print_error("sighandler\n");
        cleanup(1);
    }

    cleanup(0);

    return 0;
}
