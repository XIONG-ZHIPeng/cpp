# 为什么需要io+协程

服务器需要处理大量的socketfd,比如读写事件处理，因此需要使用IO多路复用来管理这些文件描述符

## IO协程调度器是什么，有什么功能

IO协程调度可视为增强版的协程调度。

继承协程调度器实现，增加了IO事件调度功能，针对套接字描述符

# epoll简单介绍

## epoll_create函数

用于创建epoll文件描述符

## epoll_ctl函数
用于增加，删除，修改epoll事件，存储在epoll结构体红黑树中

#include <sys/epoll.h>
 
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event);
 
参数：
epfd：epoll文件描述符
op：操作码
EPOLL_CTL_ADD:插入事件
EPOLL_CTL_DEL:删除事件
EPOLL_CTL_MOD:修改事件
fd：事件绑定的套接字文件描述符
events：事件结构体
 
返回值：
成功：返回0
失败：返回-1

## epoll_wait函数

用于监听套接字事件，可以通过设置超时时间timeout来控制监听的行为为阻塞模式还是超时模式。

#include <sys/epoll.h>
 
int epoll_wait(int epfd, struct epoll_event *events,              
int maxevents, int timeout);
 
参数：
epfd：epoll文件描述符
events：epoll事件数组
maxevents：epoll事件数组长度
timeout：超时时间
小于0：一直等待
等于0：立即返回
大于0：等待超时时间返回，单位毫秒
 
返回值：
小于0：出错
等于0：超时
大于0：返回就绪事件个数