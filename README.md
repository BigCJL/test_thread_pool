# test_thread_pool
## 基于libevent的C++线程池
使用C++11完成的跨平台(管道,socketpair)的线程池，是Ftp高并发服务器的一个模块。先在windows上使用vs2019进行开发，再移植到Ubuntu20.04上。

### 2021.10.07更新
更新了XThread、XThreadPoll的.h和.cpp文件。
* XThread是单个线程的类，包含id编号、线程入口函数、激活唤醒函数notify，启动函数Start等一系列成员。notify_send_fd和base是私有成员，分别表示用于线程之间通信的套接字和主线程中libevent的base对象。
* XThreadPool是线程池类，采用了单例模式，这是因为服务器启动后只需一个线程池来进行分发，整个进程只依赖于这一个线程池就行了，使用vector数组来存放单个XThread线程，这样可以给线程进行编号，并使用编号对应下标进行访问，优于list（链表）。此外还包含一系列初始化的成员函数。


### 2021.10.09更新
* 新建了XTask类用于管理任务，XTask是所有任务的基类，其中定义了纯虚函数Init()来实现多态，要求不同的派生类有不同的初始化方法
* 对XThreadPool类，需要考虑添加任务时的同步问题，加锁解决线程安全问题，使用C++11提供的thread库，在访问任务list的临界区加锁。
* 每个连接进来时，通过listen_cb函数把sock传递给task对象，线程池将新来的task加入任务队列里（AddTask函数），并唤醒（Activate函数）子线程，这里唤醒子线程是通过管道（socketpair）向子线程发送消息，更新notify_send_fd，子线程的fds[0]读到消息后，触发回调函数NotifyCallback（），在回调函数里，this所指的线程对象需要获取任务，并初始化任务，这里用到了互斥锁。
* 任务队列list<Task*>类型，通过指向派生类XFtpServerCMD来写业务需求，每个子线程都创建一个event_base对象，提升并发性能。
* XFtpServerCMD::Init重写了父类的纯虚函数，bufferevent_setcb 添加读、写、异常处理三种事件，默认的epoll触发模式。

