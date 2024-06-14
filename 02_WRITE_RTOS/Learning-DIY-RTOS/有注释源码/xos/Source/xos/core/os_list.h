/**
 * @file 双向循环链表
 * @author lishutong
 * @brief 为内核所有的功能模块使用的链表程序，采用的是双向循环链表，方便进行任意
 * 位置的插入和删除，以及获取头部和尾部。且链表为带头结点的，方便在插入和删除时避免
 * 判断整个链表是否为空的情况（链表不可能为空）
 * @version 0.1
 * @date 2022-09-18
 * 
 * @copyright Copyright (c) 2022
 */
#ifndef OS_LIST_H
#define OS_LIST_H

struct _os_task_t;

typedef struct os_task_t * tid_t;

/**
 * @brief 双向循环链表结点
 */
typedef struct _os_list_node_t {
    struct _os_list_node_t * pre, * next;   // 前后指针
    tid_t tid;                              // 任务的id
}os_list_node_t;

void os_list_node_init (os_list_node_t * node, void * parent);
tid_t os_list_tid_pre (tid_t tid);
tid_t os_list_tid_next (tid_t tid);

/**
 * @brief 双向循环链表
 */
typedef struct _os_list_t {   
    os_list_node_t * start;
    os_list_node_t * first;
    os_list_node_t * free;
    int count;
}os_list_t;

void os_list_init (os_list_t * list);
int os_list_count (os_list_t * list);
tid_t os_list_first (os_list_t * list);
tid_t os_list_last (os_list_t * list);
void os_list_add_front (os_list_t * list, tid_t tid);
tid_t os_list_rm_front (os_list_t * list);
void os_list_push (os_list_t * list, tid_t tid);
void os_list_insert_after (os_list_t * list, tid_t after, tid_t tid);
void os_list_remove (os_list_t * list, tid_t tid);
int os_list_clear (os_list_t * list);

#endif /* TLIB_H */
