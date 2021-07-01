
//skynet设计原理
/*
    多核并发编程
    1.多线程
        在一个进程中开启多线程，为了充分利用多核，一般设置工作线程的个数为CPU的核心数；
        memcached就是采用这种方式；
        多线程在一个进程当中，所以数据共享来自进程当中的内存，这里会涉及很多临界资源
        的访问，所以需要考虑加锁；

    2.多进程
        在一台机器当中，开启多个进程充分利用多核，一般设置工作进程的个数为CPU的核心数；
        nginx就是采用这种方式；
        nginx当中的worker进程，通过共享内存来进行共享数据；也需要考虑使用锁；
        
    3.CSP
        以go语言为代表，并发实体是协程（用户态线程、轻量级线程）；内部也是采用多少个核心
        开启多少个内核线程来充分利用多核；

    4.Actor
        erlang从语言层面支持actor并发模型，并发实体是actor（在skynet中称之为服务）；
        skynet采用C+lua来实现actor并发模型；底层也是通过采用多少个核心开启多少个内核
        线程来充分利用多核；

    总结：
        不要通过共享内存来通信，而应该通过通信来共享内存；
        CSP和Actor都复合这一哲学；
        通过通信来共享数据，其实是一种解耦合的过程；并发实体之间可以分别开发并进行单独
        优化，而它们唯一的耦合在于消息；这能让我们快速地进行开发；同时也符合我们开发的
        思路，将一个大问题拆分成若干个小问题；

    skynet
        它是一个轻量级游戏服务器框架，而不仅仅用于游戏；
        轻量级体现在：
            1.仅实现actor模型，以及相关的脚手架；
            2.实现了服务器框架的基础组件；
                a）实现了reactor并发网络库；并提供了大量连接的接入方案；
                b）基于自身网络库，实现了常用的数据库驱动（异步连接方案），并融合了lua
                数据结构；

        
*/