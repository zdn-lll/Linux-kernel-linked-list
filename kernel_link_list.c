/*
 * @Author: ZhangDingNian
 * @Date: 2024-04-11 11:00:32
 * @LastEditors: ZhangDingNian
 * @LastEditTime: 2024-04-12 10:43:21
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "list.h"

typedef struct node
{
    int data;
    struct list_head list;
} listnode, *linklist;

void pressAnyKeyToContinue();                                            // 按任意键继续
int control_panel(linklist mylist);                                      // 控制面板
linklist init_list();                                                    // 初始化一个具有表头节点的空链表
linklist creat_new_node(int data);                                       // 创建新节点
int head_insert_node(linklist mylist, int data);                         // 从表头插入新节点
int tail_insert_node(linklist mylist, int data);                         // 从表尾插入新节点
int display_linked_list(linklist mylist);                                // 打印链表数据
linklist find_node(linklist mylist, int data);                           // 查找包含指定数据的节点
int insert_node_anywhere(linklist mylist, linklist dest_node, int data); // 任意位置插入数据
int move_node(linklist dest_node, linklist src_node);                    // 移动节点
int display_node(linklist node);                                         // 打印节点数据
int del_node(linklist node);                                             // 删除节点
int destroy_link_list(linklist mylist);                                  // 摧毁链表

/// @brief 按任意键继续
void pressAnyKeyToContinue()
{
    getchar();
    printf("Press any key to continue...\n");
    fflush(stdout);
    getchar(); // 等待用户按下任意键
}

/// @brief 控制面板
/// @param mylist 指向表头的指针
int control_panel(linklist mylist)
{
    if (mylist == (linklist)NULL)
    {
        printf("head node is NULL\n");
        return -1;
    }

    int mode = 0;
    int data = 0;
    int find_data = 0;
    while (1)
    {
        system("clear");
        printf("mode 1: insert head\n");
        printf("mode 2: insert tail\n");
        printf("mode 3: insert any where\n");
        printf("mode 4: find node\n");
        printf("mode 5: print linked list\n");
        printf("mode 6: deleted node\n");
        printf("mode 7: move node\n");
        printf("mode 8: destroy linked list\n");
        printf("mode 0: program exit\n");
        printf("Mode Selection: ");
        scanf("%d", &mode);

        switch (mode)
        {
        case 0:
            destroy_link_list(mylist);
            free(mylist);
            printf("Quit...\n");
            exit(0);

        case 1:
            printf("Please enter the data to be inserted: ");
            scanf("%d", &data);
            head_insert_node(mylist, data);
            break;

        case 2:
            printf("Please enter the data to be inserted: ");
            scanf("%d", &data);
            tail_insert_node(mylist, data);
            break;

        case 3:
            printf("Please enter the data to be inserted: ");
            scanf("%d", &data);
            printf("Please enter data for the target node: ");
            scanf("%d", &find_data);
            insert_node_anywhere(mylist, find_node(mylist, find_data), data);
            break;

        case 4:
            printf("Please enter the data you want to search for: ");
            scanf("%d", &find_data);
            display_node(find_node(mylist, data));
            break;

        case 5:
            display_linked_list(mylist);
            break;

        case 6:
            printf("Please enter the data you want to delete: ");
            scanf("%d", &data);
            del_node(find_node(mylist, data));
            break;

        case 7:
            printf("Please enter the data you want to move(src_node): ");
            scanf("%d", &find_data);
            printf("Please enter data for the target node(dest_node): ");
            scanf("%d", &data);
            move_node(find_node(mylist, data), find_node(mylist, find_data));
            break;

        case 8:
            destroy_link_list(mylist);
            break;

        default:
            printf("There is no such mode!\n");
            break;
        }
        pressAnyKeyToContinue();
    }
}

/// @brief 初始化一个具有表头节点的空链表
/// @return 成功，返回指向表头的指针。失败，返回 NULL。
linklist init_list()
{
    linklist mylist = (linklist)calloc(1, sizeof(listnode));
    if (mylist != (linklist)NULL)
    {
        INIT_LIST_HEAD(&mylist->list);
    }
    else
    {
        perror("calloc");
    }

    return mylist;
}

/// @brief 创建一个新节点
/// @param data 数据
/// @return 成功，返回指向新节点的指针。失败，返回 NULL。
linklist creat_new_node(int data)
{
    linklist new = (linklist)calloc(1, sizeof(listnode));
    if (new != (linklist)NULL)
    {
        new->data = data;
        INIT_LIST_HEAD(&new->list);
    }
    else
    {
        perror("calloc");
    }

    return new;
}

/// @brief 从表头插入新节点
/// @param mylist 指向表头的指针
/// @param data 新节点的数据
/// @return 成功，返回 0。失败，返回 -1。
int head_insert_node(linklist mylist, int data)
{
    linklist new = creat_new_node(data);
    if (new != NULL)
    {
        list_add(&new->list, &mylist->list);
        printf("Node add success!\n");
        return 0;
    }
    else
    {
        printf("Node add failed!\n");
        return -1;
    }
}

/// @brief 从表尾插入新节点
/// @param mylist 指向表头的指针
/// @param data 新节点的数据
/// @return 成功，返回 0。失败，返回 -1。
int tail_insert_node(linklist mylist, int data)
{
    linklist new = creat_new_node(data);
    if (new != NULL)
    {
        list_add_tail(&new->list, &mylist->list);
        printf("Node add sucess!\n");
        return 0;
    }
    else
    {
        printf("Node add failed!\n");
        return -1;
    }
}

/// @brief 打印链表数据
/// @param mylist 指向表头的指针
/// @return 成功，返回 0。失败，返回 -1。
int display_linked_list(linklist mylist)
{
    linklist tmp;

    printf("link list: ");
    list_for_each_entry(tmp, &mylist->list, list)
    {
        printf("%d ", tmp->data);
    }
    printf("\n");
}

/// @brief 查找包含指定数据的节点
/// @param mylist 指向表头的指针
/// @param data 新节点的数据
/// @return 成功，返回指向包含指定数据的节点的指针。失败，返回 NULL。
linklist find_node(linklist mylist, int data)
{
    if (list_empty(&mylist->list))
    {
        return (linklist)NULL;
    }

    linklist pos;
    list_for_each_entry(pos, &mylist->list, list)
    {
        if (pos->data == data)
        {
            // printf("Target node found!\n");
            return pos;
        }
    }
    // printf("Target node not found!\n");
    return NULL;
}

/// @brief 任意位置插入数据
/// @param mylist 指向表头的指针
/// @param dest_node 指向插入位置节点的指针
/// @param data 新节点的数据
/// @return 成功，返回 0。失败，返回 -1。
int insert_node_anywhere(linklist mylist, linklist dest_node, int data)
{
    if (dest_node == (linklist)NULL)
    {
        return -1;
    }

    linklist new = creat_new_node(data);
    if (new != (linklist)NULL)
    {
        list_add(&new->list, &dest_node->list);
        printf("Node add sucess!\n");
        return 0;
    }
    else
    {
        printf("Node add failed!\n");
        return -1;
    }
}

/// @brief 移动节点
/// @param dest_node 指向目标节点的指针
/// @param src_node 指向原节点的指针
/// @return 成功，返回 0。失败，返回 -1。
int move_node(linklist dest_node, linklist src_node)
{
    if (dest_node == (linklist)NULL)
    {
        printf("Dest node not found!\n");
        return -1;
    }
    if (src_node == (linklist)NULL)
    {
        printf("Src node not found!\n");
        return -1;
    }
    if (src_node == dest_node)
    {
        printf("Same nodes do not need to be moved!\n");
        return 0;
    }
    
    list_move(&src_node->list, &dest_node->list);
    printf("Node move sucess!\n");
    return 0;
}

/// @brief 打印节点数据
/// @param node 指向节点的指针
/// @return 成功，返回 0。失败，返回 -1。
int display_node(linklist node)
{
    if (node == (linklist)NULL)
    {
        printf("invalid node!\n");
        return -1;
    }
    else
    {
        printf("%d\n", node->data);
        return 0;
    }
}

/// @brief 删除节点
/// @param node 指向节点的指针
/// @return 成功，返回 0。失败，返回 -1。
int del_node(linklist node)
{
    if (node == (linklist)NULL)
    {
        printf("invalid node!\n");
        return -1;
    }
    else
    {
        list_del(&node->list);
        free(node);
        printf("Node deleted successfully!\n");
        return 0;
    }
}

/// @brief 摧毁链表
/// @param mylist 指向表头的指针
/// @return 成功，返回 0。失败，返回 -1。
int destroy_link_list(linklist mylist)
{
    if (list_empty(&mylist->list))
    {
        // free(mylist);
        printf("List is empty!\n");
        return 0;
    }
    else
    {
        struct list_head *pos, *q;
        list_for_each_safe(pos, q, &mylist->list)
        {
            linklist tmp = list_entry(pos, listnode, list);
            list_del(pos);
            free(tmp);
        }
        // free(mylist);
        printf("Successfully destroyed the linked list!\n");
        return 0;
    }
    return -1;
}

int main()
{
    linklist mylist = init_list();

    control_panel(mylist);

    return 0;
}