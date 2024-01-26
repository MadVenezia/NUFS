#include <string.h>
#include <stdlib.h>
#include <alloca.h>

#include "slist.h"

// Creates a new slist node with the given text and rest of the list.
slist_t*
s_cons(const char* text, slist_t* rest)
{
    slist_t* xs = malloc(sizeof(slist_t));
    xs->data = strdup(text);
    xs->refs = 1;
    xs->next = rest;
    return xs;
}

// Frees the memory associated with the slist node and its references.
void
s_free(slist_t* xs)
{
    if (xs == 0) {
        return;
    }

    xs->refs -= 1;

    if (xs->refs == 0) {
        s_free(xs->next);
        free(xs->data);
        free(xs);
    }
}

// Splits a text into an slist of parts using the specified delimiter.
slist_t*
s_split(const char* text, char delim)
{
    if (*text == 0) {
        return 0;
    }

    int plen = 0;
    while (text[plen] != 0 && text[plen] != delim) {
        plen += 1;
    }

    int skip = 0;
    if (text[plen] == delim) {
        skip = 1;
    }

    slist_t* rest = s_split(text + plen + skip, delim);
    char* part = alloca(plen + 2);
    memcpy(part, text, plen);
    part[plen] = 0;

    return s_cons(part, rest);
}
