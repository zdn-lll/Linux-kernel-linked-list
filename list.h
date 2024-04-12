#ifndef _LINUX_LIST_H
#define _LINUX_LIST_H

// 该内文件改编自4.10.8版本的linux内核的list.h

#include <stdbool.h>

// 原来链表删除后指向的位置，这里我们修改成 0
// #define POISON_POINTER_DELTA 0
// #define LIST_POISON1  ((void *) 0x00100100 + POISON_POINTER_DELTA)
// #define LIST_POISON2  ((void *) 0x00200200 + POISON_POINTER_DELTA)
#define NULL ((void *)0)
#define LIST_POISON1 NULL
#define LIST_POISON2 NULL
/*
 * Simple doubly linked list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

// 计算member在type中的位置
#define offsetof(TYPE, MEMBER) ((size_t) & ((TYPE *)0)->MEMBER)
/**
 * @brief container_of - cast a member of a structure out to the containing structure (根据member的地址获取type的起始地址)
 * @param ptr	the pointer to the member.
 * @param type	the type of the container struct this is embedded in.
 * @param member	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) ); })

// 内核链表结构
struct list_head
{
	struct list_head *prev;
	struct list_head *next;
};

#define LIST_HEAD_INIT(name) \
	{                        \
		&(name), &(name)     \
	}

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

/// @brief 初始化内核链表表头
/// @param list 指向节点的指针
static inline void INIT_LIST_HEAD(struct list_head *list)
{
	// WRITE_ONCE(list->next, list);
	list->next = list;
	list->prev = list;
}

#ifdef CONFIG_DEBUG_LIST
extern bool __list_add_valid(struct list_head *new,
							 struct list_head *prev,
							 struct list_head *next);
extern bool __list_del_entry_valid(struct list_head *entry);
#else
static inline bool __list_add_valid(struct list_head *new,
									struct list_head *prev,
									struct list_head *next)
{
	return true;
}
static inline bool __list_del_entry_valid(struct list_head *entry)
{
	return true;
}
#endif

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_add(struct list_head *new,
							  struct list_head *prev,
							  struct list_head *next)
{
	if (!__list_add_valid(new, prev, next))
		return;

	next->prev = new;
	new->next = next;
	new->prev = prev;
	// WRITE_ONCE(prev->next, new);
	prev->next = new;
}

/// @brief list_add - add a new entry.
/// @param new new entry to be added
/// @param head list head to add it after
/// @note Insert a new entry after the specified head. This is good for implementing stacks.
static inline void list_add(struct list_head *new, struct list_head *head)
{
	__list_add(new, head, head->next);
}

/// @brief list_add_tail - add a new entry.
/// @param new new entry to be added
/// @param head list head to add it before
/// @note Insert a new entry before the specified head.This is useful for implementing queues.
static inline void list_add_tail(struct list_head *new, struct list_head *head)
{
	__list_add(new, head->prev, head);
}

/*
 * Delete a list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal list manipulation where we know
 * the prev/next entries already!
 */
static inline void __list_del(struct list_head *prev, struct list_head *next)
{
	next->prev = prev;
	// WRITE_ONCE(prev->next, next);
	prev->next = next;
}


/// @brief list_del - deletes entry from list.
/// @param entry the element to delete from the list.
/// @note list_empty() on entry does not return true after this, the entry is in an undefined state.
static inline void __list_del_entry(struct list_head *entry)
{
	if (!__list_del_entry_valid(entry))
		return;

	__list_del(entry->prev, entry->next);
}

/// @brief list_del - deletes entry from list.
/// @param entry the element to delete from the list.
static inline void list_del(struct list_head *entry)
{
	__list_del_entry(entry);
	entry->next = LIST_POISON1;
	// entry->next = NULL;
	entry->prev = LIST_POISON2;
	// entry->prev = NULL;
}


/// @brief list_replace - replace old entry by new one
/// @param old the element to be replaced
/// @param new  the new element to insert
/// @note If old was empty, it will be overwritten.
static inline void list_replace(struct list_head *old,
								struct list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void list_replace_init(struct list_head *old,
									 struct list_head *new)
{
	list_replace(old, new);
	INIT_LIST_HEAD(old);
}


/// @brief list_del_init - deletes entry from list and reinitialize it.
/// @param entry the element to delete from the list.
static inline void list_del_init(struct list_head *entry)
{
	__list_del_entry(entry);
	INIT_LIST_HEAD(entry);
}


/// @brief list_move - delete from one list and add as another's head
/// @param list the entry to move
/// @param head the head that will precede our entry
static inline void list_move(struct list_head *list, struct list_head *head)
{
	__list_del_entry(list);
	list_add(list, head);
}


/// @brief list_move_tail - delete from one list and add as another's tail
/// @param list the entry to move
/// @param head the head that will follow our entry
static inline void list_move_tail(struct list_head *list,
								  struct list_head *head)
{
	__list_del_entry(list);
	list_add_tail(list, head);
}

/// @brief list_is_last - tests whether [ list ] is the last entry in list [ head ]
/// @param list the entry to test
/// @param head the head of the list
/// @return If [ list ] is the last entry in list [ head ] return 1, else return 0.
static inline int list_is_last(const struct list_head *list,
							   const struct list_head *head)
{
	return list->next == head;
}

/// @brief list_empty - tests whether a list is empty
/// @param head the list to test.
/// @return If list is empty return 1, else return 0.
static inline int list_empty(const struct list_head *head)
{
	// return READ_ONCE(head->next) == head;
	return head->next == head;
}

/// @brief list_empty_careful - tests whether a list is empty and not being modified
/// @param head the list to test
/// @return Returns 1 if the list is empty and no other CPU might be in the process of modifying either member (next or prev).
/// @note using list_empty_careful() without synchronization can only be safe if the only activity that can happen to the list entry is list_del_init(). 
/// Eg. it cannot be used if another CPU could re-list_add() it.
static inline int list_empty_careful(const struct list_head *head)
{
	struct list_head *next = head->next;
	return (next == head) && (next == head->prev);
}


/// @brief list_rotate_left - rotate the list to the left
/// @param head the head of the list
static inline void list_rotate_left(struct list_head *head)
{
	struct list_head *first;

	if (!list_empty(head))
	{
		first = head->next;
		list_move_tail(first, head);
	}
}


/// @brief list_is_singular - tests whether a list has just one entry.
/// @param head the list to test.
/// @return If the list has just one entry, return 1. Else, return 0.
static inline int list_is_singular(const struct list_head *head)
{
	return !list_empty(head) && (head->next == head->prev);
}

static inline void __list_cut_position(struct list_head *list,
									   struct list_head *head, struct list_head *entry)
{
	struct list_head *new_first = entry->next;
	list->next = head->next;
	list->next->prev = list;
	list->prev = entry;
	entry->next = list;
	head->next = new_first;
	new_first->prev = head;
}

/// @brief list_cut_position - cut a list into two
/// @param list a new list to add all removed entries
/// @param head a list with entries
/// @param entry an entry within head, could be the head itself and if so we won't cut the list
/// @note his helper moves the initial part of [ head ], up to and
/// including [ entry ], from [ head ] to [ list ]. You should
/// pass on [ entry ] an element you know is on [ head ]. [ list ]
/// should be an empty list or a list you do not care about
/// losing its data.
static inline void list_cut_position(struct list_head *list,
									 struct list_head *head, struct list_head *entry)
{
	if (list_empty(head))
		return;
	if (list_is_singular(head) &&
		(head->next != entry && head != entry))
		return;
	if (entry == head)
		INIT_LIST_HEAD(list);
	else
		__list_cut_position(list, head, entry);
}

static inline void __list_splice(const struct list_head *list,
								 struct list_head *prev,
								 struct list_head *next)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/// @brief list_splice - join two lists, this is designed for stacks
/// @param list the new list to add.
/// @param head the place to add it in the first list.
static inline void list_splice(const struct list_head *list,
							   struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head, head->next);
}

/// @brief list_splice_tail - join two lists, each list being a queue
/// @param list the new list to add.
/// @param head the place to add it in the first list.
static inline void list_splice_tail(struct list_head *list,
									struct list_head *head)
{
	if (!list_empty(list))
		__list_splice(list, head->prev, head);
}

/// @brief list_splice_init - join two lists and reinitialise the emptied list.
/// @param list the new list to add.
/// @param head the place to add it in the first list.
/// @note The list at [list] is reinitialised
static inline void list_splice_init(struct list_head *list,
									struct list_head *head)
{
	if (!list_empty(list))
	{
		__list_splice(list, head, head->next);
		INIT_LIST_HEAD(list);
	}
}


/// @brief list_splice_tail_init - join two lists and reinitialise the emptied list
/// @param list the new list to add.
/// @param head the place to add it in the first list.
/// @note Each of the lists is a queue. The list at [list] is reinitialised.
static inline void list_splice_tail_init(struct list_head *list,
										 struct list_head *head)
{
	if (!list_empty(list))
	{
		__list_splice(list, head->prev, head);
		INIT_LIST_HEAD(list);
	}
}

/**
 * @brief list_entry - get the struct for this entry
 * @param ptr	the &struct list_head pointer.
 * @param type	the type of the struct this is embedded in.
 * @param member	the name of the list_head within the struct.
 */
#define list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * @brief list_first_entry - get the first element from a list
 * @param ptr	the list head to take the element from.
 * @param type	the type of the struct this is embedded in.
 * @param member	the name of the list_head within the struct.
 *
 * @note that list is expected to be not empty.
 */
#define list_first_entry(ptr, type, member) \
	list_entry((ptr)->next, type, member)

/**
 * @brief list_last_entry - get the last element from a list
 * @param ptr	the list head to take the element from.
 * @param type	the type of the struct this is embedded in.
 * @param member	the name of the list_head within the struct.
 *
 * @note that list is expected to be not empty.
 */
#define list_last_entry(ptr, type, member) \
	list_entry((ptr)->prev, type, member)

/**
 * @brief list_first_entry_or_null - get the first element from a list
 * @param ptr	the list head to take the element from.
 * @param type	the type of the struct this is embedded in.
 * @param member	the name of the list_head within the struct.
 *
 * @note that if the list is empty, it returns NULL.
 */
#define list_first_entry_or_null(ptr, type, member) ({      \
	struct list_head *head__ = (ptr);                       \
	struct list_head *pos__ = READ_ONCE(head__->next);      \
	pos__ != head__ ? list_entry(pos__, type, member) NULL; \
})

/**
 * @brief list_next_entry - get the next element in list
 * @param pos	the type * to cursor
 * @param member	the name of the list_head within the struct.
 */
#define list_next_entry(pos, member) \
	list_entry((pos)->member.next, typeof(*(pos)), member)

/**
 * @brief list_prev_entry - get the prev element in list
 * @param pos	the type * to cursor
 * @param member	the name of the list_head within the struct.
 */
#define list_prev_entry(pos, member) \
	list_entry((pos)->member.prev, typeof(*(pos)), member)

/**
 * @brief list_for_each	-	iterate over a list
 * @param pos	the &struct list_head to use as a loop cursor.
 * @param head	the head for your list.
 */
#define list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * @brief list_for_each_prev	-	iterate over a list backwards
 * @param pos	the &struct list_head to use as a loop cursor.
 * @param head	the head for your list.
 */
#define list_for_each_prev(pos, head) \
	for (pos = (head)->prev; pos != (head); pos = pos->prev)

/**
 * @brief list_for_each_safe - iterate over a list safe against removal of list entry
 * @param pos	the &struct list_head to use as a loop cursor.
 * @param n		another &struct list_head to use as temporary storage
 * @param head	the head for your list.
 */
#define list_for_each_safe(pos, n, head)                   \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		 pos = n, n = pos->next)

/**
 * @brief list_for_each_prev_safe - iterate over a list backwards safe against removal of list entry
 * @param pos	the &struct list_head to use as a loop cursor.
 * @param n		another &struct list_head to use as temporary storage
 * @param head	the head for your list.
 */
#define list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev;   \
		 pos != (head);                       \
		 pos = n, n = pos->prev)

/**
 * @brief list_for_each_entry	-	iterate over list of given type
 * @param pos	the type * to use as a loop cursor.
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 */
#define list_for_each_entry(pos, head, member)               \
	for (pos = list_first_entry(head, typeof(*pos), member); \
		 &pos->member != (head);                             \
		 pos = list_next_entry(pos, member))

/**
 * @brief list_for_each_entry_reverse - iterate backwards over list of given type.
 * @param pos	the type * to use as a loop cursor.
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 */
#define list_for_each_entry_reverse(pos, head, member)      \
	for (pos = list_last_entry(head, typeof(*pos), member); \
		 &pos->member != (head);                            \
		 pos = list_prev_entry(pos, member))

/**
 * @brief list_prepare_entry - prepare a pos entry for use in list_for_each_entry_continue()
 * @param pos	the type * to use as a start point
 * @param head	the head of the list
 * @param member	the name of the list_head within the struct.
 *
 * @note Prepares a pos entry for use as a start point in list_for_each_entry_continue().
 */
#define list_prepare_entry(pos, head, member) \
	((pos) ? list_entry(head, typeof(*pos), member))

/**
 * @brief list_for_each_entry_continue - continue iteration over list of given type
 * @param pos	the type * to use as a loop cursor.
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 *
 * @note Continue to iterate over list of given type, continuing after
 * the current position.
 */
#define list_for_each_entry_continue(pos, head, member) \
	for (pos = list_next_entry(pos, member);            \
		 &pos->member != (head);                        \
		 pos = list_next_entry(pos, member))

/**
 * @brief list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @param pos	the type * to use as a loop cursor.
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 *
 * @note Start to iterate over list of given type backwards, continuing after
 * the current position.
 */
#define list_for_each_entry_continue_reverse(pos, head, member) \
	for (pos = list_prev_entry(pos, member);                    \
		 &pos->member != (head);                                \
		 pos = list_prev_entry(pos, member))

/**
 * @brief list_for_each_entry_from - iterate over list of given type from the current point
 * @param pos	the type * to use as a loop cursor.
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 *
 * @note Iterate over list of given type, continuing from current position.
 */
#define list_for_each_entry_from(pos, head, member) \
	for (; &pos->member != (head);                  \
		 pos = list_next_entry(pos, member))

/**
 * @brief list_for_each_entry_safe - iterate over list of given type safe against removal of list entry
 * @param pos	the type * to use as a loop cursor.
 * @param n		another type * to use as temporary storage
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 */
#define list_for_each_entry_safe(pos, n, head, member)       \
	for (pos = list_first_entry(head, typeof(*pos), member), \
		n = list_next_entry(pos, member);                    \
		 &pos->member != (head);                             \
		 pos = n, n = list_next_entry(n, member))

/**
 * @brief list_for_each_entry_safe_continue - continue list iteration safe against removal
 * @param pos	the type * to use as a loop cursor.
 * @param n		another type * to use as temporary storage
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 *
 * @note Iterate over list of given type, continuing after current point,
 * safe against removal of list entry.
 */
#define list_for_each_entry_safe_continue(pos, n, head, member) \
	for (pos = list_next_entry(pos, member),                    \
		n = list_next_entry(pos, member);                       \
		 &pos->member != (head);                                \
		 pos = n, n = list_next_entry(n, member))

/**
 * @brief list_for_each_entry_safe_from - iterate over list from current point safe against removal
 * @param pos	the type * to use as a loop cursor.
 * @param n		another type * to use as temporary storage
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 *
 * @note Iterate over list of given type from current point, safe against
 * removal of list entry.
 */
#define list_for_each_entry_safe_from(pos, n, head, member) \
	for (n = list_next_entry(pos, member);                  \
		 &pos->member != (head);                            \
		 pos = n, n = list_next_entry(n, member))

/**
 * @brief list_for_each_entry_safe_reverse - iterate backwards over list safe against removal
 * @param pos	the type * to use as a loop cursor.
 * @param n		another type * to use as temporary storage
 * @param head	the head for your list.
 * @param member	the name of the list_head within the struct.
 *
 * @note Iterate backwards over list of given type, safe against removal
 * of list entry.
 */
#define list_for_each_entry_safe_reverse(pos, n, head, member) \
	for (pos = list_last_entry(head, typeof(*pos), member),    \
		n = list_prev_entry(pos, member);                      \
		 &pos->member != (head);                               \
		 pos = n, n = list_prev_entry(n, member))

/**
 * @brief list_safe_reset_next - reset a stale list_for_each_entry_safe loop
 * @param pos	the loop cursor used in the list_for_each_entry_safe loop
 * @param n		temporary storage used in list_for_each_entry_safe
 * @param member	the name of the list_head within the struct.
 *
 * @note list_safe_reset_next is not safe to use in general if the list may be
 * modified concurrently (eg. the lock is dropped in the loop body). An
 * exception to this is if the cursor element (pos) is pinned in the list,
 * and list_safe_reset_next is called after re-taking the lock and before
 * completing the current iteration of the loop body.
 */
#define list_safe_reset_next(pos, n, member) \
	n = list_next_entry(pos, member)
#endif
