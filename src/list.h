struct list_head {
	struct list_head *prev;
	struct list_head *next;
};


#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void INIT_LIST_HEAD(struct list_head *list) {
	list->next = list;
	list->prev = list;
}
static inline void __list_add(struct list_head *new_list, struct list_head *prev, struct list_head *next); 
static inline void list_add(struct list_head *new_list, struct list_head *head) 
{
	__list_add(new_list, head, head->next);
}
static inline void __list_add(struct list_head *new_list, struct list_head *prev, struct list_head *next) 
{
	next->prev = new_list;
	new_list->next = next;
	new_list->prev = prev;
	prev->next = new_list;
}
#define list_for_each(pos, head) for (pos = (head)->next; pos != (head); pos = pos->next)
#define list_for_each_safe(pos, n, head) for (pos = (head)->next, n = pos->next; pos != (head); pos = n, n = pos->next)




