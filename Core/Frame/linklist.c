

#include "task.h"
#include "linklist.h"
#include "console.h"
#include "util.h"

#ifdef CONFIG_LINKLIST

#ifdef CONFIG_LINKLIST_DEBUG
#define CPRINTF(format, args...) PRINTF("LL: " format, ##args)
#define CPRINTS(format, args...) PRINTS("LL: " format, ##args)
#else
#define CPRINTF(format, args...)
#define CPRINTS(format, args...)
#endif


ListNode* list_add(ListNode **head, void *data) {

        if (!head) {
                CPRINTS("Invalid head pointer provided.");
                return NULL;
        }

        // allocate new node memory
        ListNode *new_node = (ListNode *)malloc(sizeof(ListNode));

        if (!new_node) {
                CPRINTS("Failed to allocate memory for new node.");
                return NULL;
        }

        // Set the data and next indicator of the new node
        new_node->data = data;
        // new node points to the old head
        new_node->next = *head; 

        // new head points to the new node
        *head = new_node;

        // Returns the point of new node
        return new_node; 
}

int list_remove(ListNode **head, ListNode *node_to_remove, void (*free_data_func)(void *data)) {

        ListNode *current = *head;
        ListNode *prev = NULL;

        if (!head || !(*head) || !node_to_remove) {
                CPRINTS("Invalid arguments.");
                return EC_ERROR_INVAL;
        }

        // loop the linked list to find the node to remove
        while (current != NULL && current != node_to_remove) {
                prev = current;
                current = current->next;
        }

        // if node is not in the linked list
        if (current == NULL) {
                CPRINTS("Node not found in the list.");
                return EC_ERROR_UNKNOWN; 
        }

        // if you want to remove the head node
        if (prev == NULL) {
                *head = current->next; // Update header point
        } else {
                // if it is a middle or tail node
                // Point the previous node to the next node and skip the current node
                prev->next = current->next; 
        }

        // free node data (if free function is provided)
        if (free_data_func && current->data) {
                free_data_func(current->data);
        }

        // Release the node's own memory
        free(current);

        return EC_SUCCESS;
}

void list_destroy(ListNode **head, void (*free_data_func)(void *data)) {

        ListNode *current = *head;
        ListNode *next_node;

        if (!head) 
                return;

        while (current != NULL) {

                // save the next node
                next_node = current->next; 

                // free the data (if necessary)
                if (free_data_func && current->data) {
                free_data_func(current->data);
                }

                // free the current node
                free(current);

                // Move to the next node
                current = next_node; 
        }

        // Set the head pointer to NULL,
        // indicating that the linked list is empty
        *head = NULL;
}

// Helper function specifically for freeing hook_data
// You might place this elsewhere or make list_remove/list_destroy handle it directly
// if you don't need the data pointer after removal.
void free_hook_data(void *data) {
        if (data) {
                free(data);
        }
}

#endif /* CONFIG_LINKLIST */