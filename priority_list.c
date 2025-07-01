#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 自定义语音信息结构体
typedef struct {
    int priority;     // 逻辑优先级（可用于排序或其它用途）
    char path[128];   // 文件路径
    int count;        // 播放次数
    int volume;       // 音量
} xst_voice_info_t;

// 优先队列节点结构体
typedef struct Node {
    void* data;         // 支持任意类型数据
    int priority;       // 排序依据
    struct Node* next;
} Node;

// 优先队列句柄
typedef struct PriorityQueue {
    Node* head;
} PriorityQueue;

/**
 * 创建一个优先队列
 */
PriorityQueue* pq_create() {
    PriorityQueue* queue = (PriorityQueue*)malloc(sizeof(PriorityQueue));
    if (!queue) {
        printf("Memory allocation failed for queue.\n");
        return NULL;
    }
    queue->head = NULL;
    return queue;
}

/**
 * 向队列中插入一个带优先级的数据节点
 * @param queue 队列句柄
 * @param data 数据指针（这里是 xst_voice_info_t*）
 * @param priority 用于排序的优先级值
 * @return 成功返回0，失败返回-1
 */
int pq_insert(PriorityQueue* queue, void* data, int priority) {
    if (!queue || !data) return -1;

    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        printf("Memory allocation failed for node.\n");
        return -1;
    }

    newNode->data = data;
    newNode->priority = priority;
    newNode->next = NULL;

    // 插入到合适位置（按 priority 排序）
    if (queue->head == NULL || queue->head->priority > priority) {
        newNode->next = queue->head;
        queue->head = newNode;
    } else {
        Node* current = queue->head;
        while (current->next != NULL && current->next->priority <= priority) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    printf("Inserted voice with priority %d\n", priority);
    return 0;
}

/**
 * 弹出优先级最高的节点数据
 * @param queue 队列句柄
 * @param out_data 输出参数，传出数据指针
 * @param out_priority 输出参数，传出优先级
 * @return 成功返回0，失败返回-1
 */
int pq_pop_highest(PriorityQueue* queue, void** out_data, int* out_priority) {
    if (!queue || queue->head == NULL || !out_data || !out_priority) {
        printf("Invalid input or queue is empty.\n");
        return -1;
    }

    Node* temp = queue->head;
    *out_data = temp->data;
    *out_priority = temp->priority;

    queue->head = queue->head->next;
    free(temp);

    return 0;
}

/**
 * 删除队列并释放内存（不释放 data 内容）
 */
void pq_delete(PriorityQueue* queue) {
    if (!queue) return;

    Node* current = queue->head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }

    free(queue);
    printf("Queue deleted and all nodes freed.\n");
}

/**
 * 打印整个队列内容（调试用）
 * @param queue 队列句柄
 * @param print_func 用户自定义打印函数
 */
void pq_print(PriorityQueue* queue, void (*print_func)(void*)) {
    if (!queue) return;

    Node* temp = queue->head;
    printf("\n=== Voice Priority Queue ===\n");
    while (temp != NULL) {
        printf("Data: ");
        print_func(temp->data);
        printf(" | Priority: %d\n", temp->priority);
        temp = temp->next;
    }
    printf("============================\n\n");
}

// 示例打印函数：用于打印 xst_voice_info_t 结构体
void print_voice_info(void* ptr) {
    xst_voice_info_t* info = (xst_voice_info_t*)ptr;
    printf("[Path: %s | Count: %d | Volume: %d]", info->path, info->count, info->volume);
}

// 主函数测试
int main() {
    // 1. 创建队列
    PriorityQueue* myQueue = pq_create();
    if (!myQueue) {
        printf("Failed to create queue.\n");
        return -1;
    }

    // 2. 准备几个语音信息结构体
    xst_voice_info_t* v1 = (xst_voice_info_t*)malloc(sizeof(xst_voice_info_t));
    strcpy(v1->path, "/voice/1.mp3");
    v1->priority = 3;
    v1->count = 1;
    v1->volume = 50;

    xst_voice_info_t* v2 = (xst_voice_info_t*)malloc(sizeof(xst_voice_info_t));
    strcpy(v2->path, "/voice/2.mp3");
    v2->priority = 1;
    v2->count = 2;
    v2->volume = 70;

    xst_voice_info_t* v3 = (xst_voice_info_t*)malloc(sizeof(xst_voice_info_t));
    strcpy(v3->path, "/voice/3.mp3");
    v3->priority = 2;
    v3->count = 3;
    v3->volume = 60;

    xst_voice_info_t* v4 = (xst_voice_info_t*)malloc(sizeof(xst_voice_info_t));
    strcpy(v4->path, "/voice/4.mp3");
    v4->priority = 1;
    v4->count = 2;
    v4->volume = 70; 

    // 3. 插入节点（priority 是排序字段）
    if (pq_insert(myQueue, v1, v1->priority) != 0)
        printf("Insert failed for v1\n");

    if (pq_insert(myQueue, v2, v2->priority) != 0)
        printf("Insert failed for v2\n");

    if (pq_insert(myQueue, v3, v3->priority) != 0)
        printf("Insert failed for v3\n");

    if (pq_insert(myQueue, v4, v4->priority) != 0)
    printf("Insert failed for v4\n");

    // 4. 打印队列
    pq_print(myQueue, print_voice_info);

    // 5. 弹出优先级最高的节点
    void* data;
    int priority;
    while (pq_pop_highest(myQueue, &data, &priority) == 0) {
        xst_voice_info_t* info = (xst_voice_info_t*)data;
        printf("Popped:\n");
        printf("Path: %s | Priority: %d | Count: %d | Volume: %d\n",
               info->path,
               info->priority,
               info->count,
               info->volume);
    }

    // 6. 删除队列（注意：data 需要你自己决定是否 free）
    pq_delete(myQueue);

    return 0;
}
