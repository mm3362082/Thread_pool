#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include <pthread.h>

/**
 * @brief 任务函数类型的新声明
*/
typedef void*(*thread_task_t)(void*);

/**
 * @brief 任务链表结构体类型声明
*/
typedef struct tasks
{
    //任务指针
    void*(*tast_point)(void*);

    //任务参数
    void *argv;

    //下一个任务指针
    struct tasks *next;
    
    //上一个任务指针
    struct tasks *pre;

}task_t;

/**
 * @brief 线程池结构体类型声明
*/
typedef struct 
{
    //线程池状态，是否开启
    int thread_state;

    //线程池内线程的个数
    int thread_number;

    //线程id的集合
    pthread_t *thread_id;

    //线程池共享的条件变量
    pthread_cond_t share_cond;

    //线程池共享的互斥锁
    pthread_mutex_t share_mutex;

    //线程池的任务链表集合
    task_t *pool_tasklist;

}thread_pool_t;

/**
 * @brief 初始化线程池
 * @param thread_number 线程池内线程的数量
 * @return thread_pool_t* 返回一个指向初始化了的线程池的指针
*/
thread_pool_t *thread_pool_Init(int thread_number);

/**
 * @brief 添加任务到指定线程池的任务链表集合内
 * @param pool 任务添加指定的线程池
 * @param task_name 需要添加的任务的指针
 * @param argv 任务的参数
 * @return 成功返回1，失败返回-1
*/
int task_add(thread_pool_t *pool,thread_task_t task_name,void* argv);

/**
 * @brief 销毁线程池
 * @param pool 二级指针，指定销毁的线程池指针
 * @return 成功返回1，失败返回-1
*/
int thread_pool_Destroy(thread_pool_t **pool);


#endif//__THREAD_POOL_H__