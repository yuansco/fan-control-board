#ifndef __LINKLIST_H
#define __LINKLIST_H

#include <stdlib.h> // For NULL, malloc, free

// Linked list node structure
typedef struct ListNode {
        void *data;            // Pointer to the actual data (making it generic)
        struct ListNode *next; // Pointer to the next node
} ListNode;

/**
 * @brief Adds a new node to the beginning of the linked list.
 *
 * @param head Pointer to the pointer of the linked list's head (because head might change).
 * @param data Pointer to the data to be stored.
 * @return ListNode* Pointer to the newly created node, or NULL if allocation fails.
 */
ListNode* list_add(ListNode **head, void *data);

/**
 * @brief Removes the specified node from the linked list.
 *
 * @param head Pointer to the pointer of the linked list's head.
 * @param node_to_remove Pointer to the node to be removed.
 * @param free_data_func A function pointer used to free the memory pointed to by the node's data.
 * If freeing the data is not required (e.g., if the data is statically allocated or managed elsewhere),
 * NULL can be passed.
 * @return int Returns 1 if the node was successfully removed, 0 if the node was not found.
 */
int list_remove(ListNode **head, ListNode *node_to_remove, void (*free_data_func)(void *data));

/**
 * @brief Destroys the entire linked list, freeing all nodes and optional data memory.
 *
 * @param head Pointer to the pointer of the linked list's head.
 * @param free_data_func Function pointer used to free the data of each node, or NULL if not needed.
 */
void list_destroy(ListNode **head, void (*free_data_func)(void *data));

#endif // __LINKLIST_H