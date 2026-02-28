#ifndef CLIPBOARD_H_
#define CLIPBOARD_H_

#include <chlsdl/macros.h>

#include <X11/Xlib.h>
#include <stdlib.h>

extern void
clipboard_init();
extern unsigned char *
clipboard_get();
extern void
clipboard_deinit();

CHLSDL_ALWAYS_INLINE inline void
clipboard_clear()
{
    /* ... */
    system("echo -n '' | xclip -selection clipboard -i");
}

CHLSDL_ALWAYS_INLINE inline void
clipboard_string_free(char * s)
{
    XFree(s);
}

#endif // CLIPBOARD_H_
