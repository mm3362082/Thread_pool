#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "thread_pool.h"

/**
 * @brief 任务函数
*/
void*mission(void*argv)
{
    /* 将传参进行类型强转 */
    thread_pool_t *pool=(thread_pool_t *)argv;
    /* 
        当线程池开启时，一直循环，知道线程池关闭
        条件阻塞，等待被唤醒后执行任务
    */
    do
    {
        /* 上锁 */
        pthread_mutex_lock( &(pool->share_mutex) );

        /* 等待被唤醒 */
        pthread_cond_wait( &(pool->share_cond),&(pool->share_mutex) );

        task_t *task=NULL;
        task->next=NULL;
        task->pre=NULL;
        /* 从线程池任务链表中获取任务并指行 */
        if(pool->pool_tasklist == NULL)
        {
            pthread_mutex_unlock( &(pool->share_mutex) );
            task==NULL;
            continue;
        }
        task = pool->pool_tasklist;
        pool->pool_tasklist = pool->pool_tasklist->next;

        /* 解锁 */
        pthread_mutex_unlock( &(pool->share_mutex) );

        /* 执行任务 */
        pthread_t tid=pthread_self();
        (task->tast_point)(&tid);

        /* 销毁工作 */
        task->pre=NULL;
        task->next=NULL;
        free(task);
        task=NULL;
    } 
    while(pool->thread_state);

    return NULL;
}

/**
 * @brief 初始化线程池
 * @param thread_number 线程池内线程的数量
 * @return thread_pool_t* 返回一个指向初始化了的线程池的指针
*/
thread_pool_t *thread_pool_Init(int thread_number)
{
    /* 
        初始化线程池，给线程池开辟一个动态内存空间
     */
    thread_pool_t *pool=malloc( sizeof(thread_pool_t) );

    /*  
        线程池成员初始化操作
        1）线程池内线程个数
        2）线程ID集合，开辟空间,并置零，再分配
        3）线程池状态，开启
        4）线程池共享条件变量
        5）线程池共享互斥锁
    */

    /* 初始化线程个数，线程ID集合 */
    pool->thread_number    = thread_number;
    pool->thread_id        = malloc( sizeof(pthread_t) * thread_number );
    
    /* 线程ID集合全部置零 */
    memset( pool->thread_id,0,sizeof(pthread_t)*thread_number );//通过内存设置，将thread_id成员进行置零操作

    /* 初始化共享条件变量和共享互斥锁 */
    pthread_mutex_init(&(pool->share_mutex),NULL);
    pthread_cond_init(&(pool->share_cond),NULL);
    
    /* 初始化线程池状态 */
    pool->thread_state=1;//开启

    /* 创建线程 */
    for(int i=0;i<thread_number;i++)
    {
        if( pthread_create( (pool->thread_id+i),NULL,mission,pool) ==-1 )
        {
            i--;
            continue;
        }
    }

    /* 返回初始化了的线程池 */
    return pool;

}

/**
 * @brief 添加任务到指定线程池的任务链表集合内
 * @param pool 任务添加指定的线程池
 * @param task_name 需要添加的任务的指针
 * @param argv 任务的参数
 * @return 成功返回1，失败返回-1
*/
int task_add(thread_pool_t *pool,thread_task_t task_name,void* argv)
{

    /* 创建一个临时的指向任务链表的指针,并初始化成员变量 */
    task_t *task=malloc( sizeof(task_t) );
    task->argv         = argv;
    task->next         = NULL;
    task->pre          = NULL;
    task->tast_point   = task_name;

    /* 上锁 */
    pthread_mutex_lock ( &(pool->share_mutex) );

    /* 尾插法把任务插入指定线程池的任务链表中 */
    if(pool->pool_tasklist == NULL)
    {
        pool->pool_tasklist=task;
    }
    else
    {
        /* 找尾节点 */
        task_t *temp=pool->pool_tasklist;
        while(temp->next)
        {
            temp=temp->next;
        }
        temp->next=task;
        task->pre=temp;
    }
    
    /* 唤醒线程执行任务 */
    pthread_cond_signal( &(pool->share_cond) );

    /* 解锁 */
    pthread_mutex_unlock( &(pool->share_mutex) );

    return 1;
}

/**
 * @brief 销毁线程池
 * @param pool 二级指针，指定销毁的线程池指针
 * @return 成功返回1，失败返回-1
*/
int thread_pool_Destroy(thread_pool_t **pool)
{
    /* 改变线程池状态，关闭 */
    pthread_mutex_lock( &( (*pool)->share_mutex ) );
    (*pool)->thread_state=0;
    pthread_mutex_unlock( &( (*pool)->share_mutex ) );

    /* 唤醒所有线程 */
    for(int i=0;i<(*pool)->thread_number;i++)
    {
        pthread_mutex_lock( &( (*pool)->share_mutex ) );
        pthread_cond_signal( &( (*pool)->share_cond ) );
        pthread_mutex_unlock( &( (*pool)->share_mutex ) );
    }

    /* 让线程结束，并回收资源 */
    for(int i=0;i<(*pool)->thread_number;i++)
    {
        pthread_cancel( *( (*pool)->thread_id+i ) );
        pthread_join( *( (*pool)->thread_id+i ),NULL );
    }

    /* 销毁任务链表 */
    task_t *task=(*pool)->pool_tasklist;
    while(task)
    {
        (*pool)->pool_tasklist = (*pool)->pool_tasklist->next;
        task->next=NULL;
        task->pre=NULL;
        free(task);
        task = (*pool)->pool_tasklist;
    }
    (*pool)->pool_tasklist=NULL;

    /* 销毁线程ID集合空间 */
    free( (*pool)->thread_id );
    (*pool)->thread_id=NULL;

    /* 销毁共享条件变量和共享互斥锁 */
    pthread_cond_destroy( &( (*pool)->share_cond ) );
    pthread_mutex_destroy( &( (*pool)->share_mutex ) );

    /* 将线程池指针滞空 */
    free( (*pool) );
    (*pool) = NULL;

    return 1;
}