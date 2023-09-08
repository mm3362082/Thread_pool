# 项目名称：

```
基于C语言的线程池
```

# 项目开发时间：

```
2023.9.7-2023.9.8
```

# 项目需求：

```
用C语言实现一个线程池，该线程池能实现简单的初始化，添加任务，销毁线程池等功能
```

# 项目关键词：

```
并发，多线程，线程池，C语言
```

# 项目文件：

```
Thread_pool
```

# 子文件夹及其子文件：					

```
(1).gitattributes
(2)README.md
(3)thread_pool.c
(4)thread_pool.h
(5)thread_pool.o
(6)thread_pool.so
```

# 项目大致框架：

```
1.用一个结构体定义线程池，线程池内含有成员变量：
（1）线程池状态
（2）线程池内线程的数量
（3）线程ID的集合
（4）线程池共享的互斥锁和条件变量
（5）任务链表
2.用一个结构体定义任务链表，任务链表内含有成员变量：
（1）指向的任务
（2）指向任务的参数
（3）指向上一个任务的指针
（4）指向下一个任务的指针
3.对线程池进行初始化，对线程池进行添加任务，对线程池进行销毁操作
```

# 项目程序和函数模块：

## （1）项目源码文件：

```
(1)thread_pool.c//线程池主体源码
(2)thread_pool.h//线程池头文件
(3)thread_pool.o//线程池.o库
(4)thread_pool.so//线程池.so库
```

## （2）函数模块：

### 1.thread_pool.c

```c
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
```

# 项目待开发功能：

```
（1）对线程池结构体功能的进一步开发
（2）对任务链表结构体功能的进一步开发
（3）对线程池的使用功能的进一步开发
```

