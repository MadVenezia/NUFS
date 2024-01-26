#ifndef SLIST_H
#define SLIST_H

/*
 * Represents a singly-linked list node that holds a string, 
 * the number of references, and a link to the next node.
 */
typedef struct slist {
    char* data;          // String data stored in the node
    int   refs;          // Number of references to the node
    struct slist* next;  // Pointer to the next node in the list
} slist_t;

/*
 * Creates a new slist node with the given text and the rest of the list.
 *
 * Parameters:
 *   - text: The string data for the new node.
 *   - rest: The rest of the linked list.
 *
 * Returns:
 *   A pointer to the newly created slist node.
 */
slist_t* s_cons(const char* text, slist_t* rest);

/*
 * Frees the memory associated with the slist node and its references.
 *
 * Parameters:
 *   - xs: A pointer to the slist node to be freed.
 */
void s_free(slist_t* xs);

/*
 * Splits a text into an slist of parts using the specified delimiter.
 *
 * Parameters:
 *   - text: The input text to be split.
 *   - delim: The delimiter used for splitting the text.
 *
 * Returns:
 *   A pointer to the head of the resulting slist.
 */
slist_t* s_split(const char* text, char delim);

#endif
