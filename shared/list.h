#include <stddef.h>

typedef struct _list_node_t
{
    struct _list_node_t *next;
    void *element;
} list_node_t;

typedef struct _list_t
{
    list_node_t *head;
    list_node_t *tail;
    void *(*list_malloc)(size_t);
    void (*list_free)(void *);
    size_t count;
} list_t;

/**
 * @brief create a list object
 * @param list_malloc: the allocator to use for creating list nodes
 * @param list_free: the free function to use for desrtoying list nodes
 * @return a ptr to a list_t object
 */
list_t *list_init(void *(*list_malloc)(size_t), void (*list_free)(void *));

/**
 * @brief destroy a list. DOES free elements
 * @param list: the list to destroy
 * @return: void
 */
void list_destroy(list_t *list);

/**
 * @brief add an element to the list (shallow copy)
 * @param list: the list to add to
 * @paramm element: the element to add to the list
 * @return: void
 */
void list_add(list_t *list, void *element);

/**
 * @brief removal based on linux torvold's linked list impl. Does NOT free elements
 * @param list: the list from which removal should occur
 * @param element: the element to remove from the list
 */
void list_remove(list_t *list, void *element);

/**
 * @brief invoke a function on each element in the list
 * @param list: the list to process
 * @param process_func: the function to invoke on each element of the list
 * @param context: an opaquie context object to pass to the list
 * @return: void
 */
void list_foreach(list_t *list, void (*process_func)(void *, void *), void *context);


/**
 * @brief get an element at index
 * @param list: the list to search
 * @param index: the index into the list to get
 * @return ptr to element in list at index 'index', NULL if no data found at index
 */
void* list_get_at_index(list_t* list, unsigned int index);


/**
 * @brief searches the supplied list for an element with a key matching a supplied key. 
 * key_comparators should return 1 on a match, 0 otherwise. 
 * This function returns the first element that caused key_comparator to return 1.
 * @param list: the list to search
 * @param key_comparator: the supplied comparator function
 * @param key: the key to pass along with the element to the key_comparator function
 * @return: a ptr to the matching element if any found, NULL otherwise
 */
void *list_search(list_t *list, char (*key_comparator)(void *, void *), void *key);

/**
 * @brief get the count of the list
 * @param] list: the list to get the count of
 * @return: the size of the list
 */
size_t list_count(list_t *list);