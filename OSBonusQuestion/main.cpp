#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
// 存储资源表结构
struct map {
    int m_addr;  // 起始地址
    int m_size;  // 大小
    struct map *next;  // 指向下一个分区
};

// 全局变量 - 空闲分区链表头指针
struct map *free_list = NULL;

// 函数声明
void init_free_list(int addr, int size);// 初始化空闲分区链表
void display_free_list();// 显示空闲分区链表
int first_fit_alloc(int size);// 最先适应分配算法
int best_fit_alloc(int size);// 最佳适应分配算法
int worst_fit_alloc(int size);// 最坏适应分配算法
void free_memory(int addr, int size);// 释放内存
void merge_free_blocks();// 合并相邻的空闲分区


int main() {

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif
    int total_memory;
    printf("请输入总内存大小: ");
    scanf("%d", &total_memory);
    init_free_list(0, total_memory);

    int choice;
    while (1) {
        printf("\n动态分区存储管理系统\n");
        printf("1. 显示空闲分区表\n");
        printf("2. 最先适应分配(FF)\n");
        printf("3. 最佳适应分配(BF)\n");
        printf("4. 最坏适应分配(WF)\n");
        printf("5. 释放内存\n");
        printf("0. 退出\n");
        printf("请选择操作: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                display_free_list();
                break;

            case 2: {
                int size;
                printf("请输入要分配的内存大小: ");
                scanf("%d", &size);
                int addr = first_fit_alloc(size);
                if (addr == -1) {
                    printf("分配失败，没有足够的连续内存空间!\n");
                } else {
                    printf("分配成功，起始地址: %d\n", addr);
                }
                break;
            }

            case 3: {
                int size;
                printf("请输入要分配的内存大小: ");
                scanf("%d", &size);
                int addr = best_fit_alloc(size);
                if (addr == -1) {
                    printf("分配失败，没有足够的连续内存空间!\n");
                } else {
                    printf("分配成功，起始地址: %d\n", addr);
                }
                break;
            }

            case 4: {
                int size;
                printf("请输入要分配的内存大小: ");
                scanf("%d", &size);
                int addr = worst_fit_alloc(size);
                if (addr == -1) {
                    printf("分配失败，没有足够的连续内存空间!\n");
                } else {
                    printf("分配成功，起始地址: %d\n", addr);
                }
                break;
            }

            case 5: {
                int addr, size;
                printf("请输入要释放的内存起始地址和大小: ");
                scanf("%d %d", &addr, &size);
                free_memory(addr, size);
                printf("内存释放成功!\n");
                break;
            }

            case 0:
                printf("程序退出。\n");
                return 0;

            default:
                printf("无效的选择，请重新输入!\n");
        }
    }

    return 0;
}
// 初始化空闲分区链表
void init_free_list(int addr, int size) {
    free_list = (struct map *)malloc(sizeof(struct map));
    free_list->m_addr = addr;
    free_list->m_size = size;
    free_list->next = NULL;
}
// 显示空闲分区链表
void display_free_list() {
    struct map *p = free_list;
    printf("空闲分区表:\n");
    printf("起始地址\t大小\n");

    while (p != NULL) {
        printf("%8d\t%8d\n", p->m_addr, p->m_size);
        p = p->next;
    }
    printf("\n");
}
// 最先适应分配算法
int first_fit_alloc(int size) {
    struct map *p = free_list;
    struct map *prev = NULL;

    while (p != NULL) {
        if (p->m_size >= size) {
            int alloc_addr = p->m_addr;

            // 如果分区大小正好等于请求大小，则删除该分区
            if (p->m_size == size) {
                if (prev == NULL) {
                    free_list = p->next;
                } else {
                    prev->next = p->next;
                }
                free(p);
            } else {
                // 否则，分割分区
                p->m_addr += size;
                p->m_size -= size;
            }

            return alloc_addr;
        }
        prev = p;
        p = p->next;
    }

    return -1;  // 分配失败
}

// 最佳适应分配算法
int best_fit_alloc(int size) {
    struct map *p = free_list;
    struct map *best = NULL;
    struct map *prev = NULL;
    struct map *best_prev = NULL;
    int min_diff = -1;

    // 查找最适合的分区
    while (p != NULL) {
        if (p->m_size >= size) {
            int diff = p->m_size - size;
            if (min_diff == -1 || diff < min_diff) {
                min_diff = diff;
                best = p;
                best_prev = prev;
            }
        }
        prev = p;
        p = p->next;
    }

    if (best == NULL) {
        return -1;  // 分配失败
    }

    int alloc_addr = best->m_addr;

    // 如果分区大小正好等于请求大小，则删除该分区
    if (best->m_size == size) {
        if (best_prev == NULL) {
            free_list = best->next;
        } else {
            best_prev->next = best->next;
        }
        free(best);
    } else {
        // 否则，分割分区
        best->m_addr += size;
        best->m_size -= size;
    }

    return alloc_addr;
}

// 最坏适应分配算法
int worst_fit_alloc(int size) {
    struct map *p = free_list;
    struct map *worst = NULL;
    struct map *prev = NULL;
    struct map *worst_prev = NULL;
    int max_size = -1;

    // 查找最大的分区
    while (p != NULL) {
        if (p->m_size >= size && p->m_size > max_size) {
            max_size = p->m_size;
            worst = p;
            worst_prev = prev;
        }
        prev = p;
        p = p->next;
    }

    if (worst == NULL) {
        return -1;  // 分配失败
    }

    int alloc_addr = worst->m_addr;

    // 如果分区大小正好等于请求大小，则删除该分区
    if (worst->m_size == size) {
        if (worst_prev == NULL) {
            free_list = worst->next;
        } else {
            worst_prev->next = worst->next;
        }
        free(worst);
    } else {
        // 否则，分割分区
        worst->m_addr += size;
        worst->m_size -= size;
    }

    return alloc_addr;
}
// 释放内存
void free_memory(int addr, int size) {
    struct map *new_block = (struct map *)malloc(sizeof(struct map));
    new_block->m_addr = addr;
    new_block->m_size = size;
    new_block->next = NULL;

    // 将释放的内存块插入空闲链表
    if (free_list == NULL) {
        free_list = new_block;
    } else if (addr < free_list->m_addr) {
        new_block->next = free_list;
        free_list = new_block;
    } else {
        struct map *p = free_list;
        while (p->next != NULL && p->next->m_addr < addr) {
            p = p->next;
        }
        new_block->next = p->next;
        p->next = new_block;
    }

    // 合并相邻的空闲分区
    merge_free_blocks();
}

// 合并相邻的空闲分区
void merge_free_blocks() {
    struct map *p = free_list;

    while (p != NULL && p->next != NULL) {
        if (p->m_addr + p->m_size == p->next->m_addr) {
            // 合并两个相邻分区
            p->m_size += p->next->m_size;
            struct map *temp = p->next;
            p->next = temp->next;
            free(temp);
        } else {
            p = p->next;
        }
    }
}