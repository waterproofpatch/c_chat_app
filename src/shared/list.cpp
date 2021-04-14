#include <stdlib.h>
#include "list.hpp"

list_t *list_init(void *(*list_malloc)(size_t), void (*list_free)(void *))
{

    list_t *list = (list_t *)list_malloc(sizeof(list_t));
    list->head = list->tail = NULL;
    list->count             = 0;
    list->list_malloc       = list_malloc;
    list->list_free         = list_free;
    return list;
}

void list_destroy(list_t *list)
{
    list_node_t *cur  = list->head;
    list_node_t *next = NULL;
    while (cur)
    {
        next = cur->next;
        list->list_free(cur);
        cur = next;
    }
    list->list_free(list);
}

void list_add(list_t *list, void *element)
{
    list_node_t *new_node =
        (list_node_t *)list->list_malloc(sizeof(list_node_t));
    if (!new_node)
    {
        return;   // TODO should have error return
    }
    new_node->next    = NULL;
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
    list->tail       = new_node;
    list->count++;
}

void list_remove(list_t *list, void *element)
{
    list_node_t *cur  = list->head;
    list_node_t *prev = NULL;

    while (cur && cur->element != element)
    {
        prev = cur;
        cur  = cur->next;
    }
    // not found
    if (!cur)
    {
        return;
    }
    // if we're removing the tail, move the tail back
    if (cur == list->tail)
    {
        list->tail = prev;
    }
    if (prev)
    {
        prev->next = cur->next;
    }
    else
    {
        list->head = cur->next;
    }
    list->list_free(cur);
    list->count--;
    return;
}

void list_foreach(list_t *list,
                  void (*process_func)(void *, void *),
                  void *context)
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
    unsigned int i   = 0;
    list_node_t *cur = list->head;
    while (cur && i++ < index)
    {
        cur = cur->next;
    }

    return cur != NULL ? cur->element : NULL;
}

void *list_search(list_t *list,
                  char (*key_comparator)(void *, void *),
                  void *key)
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
