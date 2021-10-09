# test_thread_pool
## 基于libevent的C++线程池
使用C++11完成的跨平台(管道,socketpair)的线程池，是Ftp高并发服务器的一个模块。先在windows上使用vs2019进行开发，再移植到Ubuntu20.04上。

### 2021.10.07更新
更新了XThread、XThreadPoll的.h和.cpp文件。
* XThread是单个线程的类，包含id编号、线程入口函数、激活唤醒函数notify，启动函数Start等一系列成员。notify_send_fd和base是私有成员，分别表示用于线程之间通信的套接字和主线程中libevent的base对象。
* XThreadPool是线程池类，采用了单例模式，这是因为服务器启动后只需一个线程池来进行分发，整个进程只依赖于这一个线程池就行了，使用vector数组来存放单个XThread线程，这样可以给线程进行编号，并使用编号对应下标进行访问，优于list（链表）。此外还包含一系列初始化的成员函数。


### 2021.10.09更新
新建了XTask类用于管理任务，XTask是所有任务的基类，
