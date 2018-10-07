







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















list_t *list_init(void *(*list_malloc)(size_t), void (*list_free)(void *));













void list_destroy(list_t *list);















void list_add(list_t *list, void *element);













void list_remove(list_t *list, void *element);

void list_foreach(list_t *list, void (*process_func)(void *, void *), void *context);

void* list_get_at_index(list_t* list, unsigned int index);

void *list_search(list_t *list, char (*key_comparator)(void *, void *), void *key);













size_t list_count(list_t *list);
