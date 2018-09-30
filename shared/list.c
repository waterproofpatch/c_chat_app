#include <stdlib.h>
#include "list.h"

list_t *list_init(void *(*list_malloc)(size_t), void (*list_free)(void *))
{

    list_t *list = (list_t *)list_malloc(sizeof(list_t));
    list->head = list->tail = NULL;
    list->count = 0;
    list->list_malloc = list_malloc;
    list->list_free = list_free;
    return list;
}

void list_destroy(list_t *list)
{
    list_node_t *cur = list->head;
    list_node_t *next = NULL;
    while (cur)
    {
        next = cur->next;
        list->list_free(cur->element);
        list->list_free(cur);
        cur = next;
    }
    list->list_free(list);
}

void list_add(list_t *list, void *element)
{
    list_node_t *new_node = (list_node_t *)list->list_malloc(sizeof(list_node_t));
    new_node->next = NULL;
    new_node->element = element;

    // first element
    if (list->head == NULL)
    {
        list->head = new_node;
        list->tail = new_node;
        list->count++;
        return;
    }

    // append element
    list->tail->next = new_node;
    list->tail = new_node;
    list->count++;
}

void list_remove(list_t *list, void *element)
{
    list_node_t **pp = &list->head; /* pointer to a pointer */
    list_node_t *entry = list->head;

    while (entry)
    {
        // ptr comparison (user should provide addr of element to remove)
        if (entry->element == element)
        {
            *pp = entry->next;
            list->list_free(entry);
            list->count--;
            return;
        }
        pp = &entry->next;
        entry = entry->next;
    }
}

void list_foreach(list_t *list, void (*process_func)(void *, void *), void *context)
{
    list_node_t *cur = list->head;
    while (cur)
    {
        process_func(cur->element, context);
        cur = cur->next;
    }
}

void *list_get_at_index(list_t *list, unsigned int index)
{
    unsigned int i = 0;
    list_node_t *cur = list->head;
    while (cur && i++ < index)
    {
        cur = cur->next;
    }

    return cur != NULL ? cur->element : NULL;
}

void *list_search(list_t *list, char (*key_comparator)(void *, void *), void *key)
{
    list_node_t *cur = list->head;
    while (cur)
    {
        if (key_comparator(cur->element, key) == 1)
        {
            return cur->element;
        }
        cur = cur->next;
    }
    return NULL;
}

size_t list_count(list_t *list)
{
    return list->count;
}