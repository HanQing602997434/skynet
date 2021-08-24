
//skynet框架gateserver：
/*
    gate require gateserver, 消息接收在gate模块，消息处理在gateserver
    数据：
        socket：listen socket
        queue：消息队列
        maxclient：最大连接数
        client_number：连接数
        nodelay：延迟
        connection：连接的fd

    接口：
        MSG.open(fd, msg)：打开fd，接收消息
        MSG.close(fd)：关闭fd
        dispatch_queue()：处理消息队列
        dispatch_msg(fd, msg, sz)：处理消息
        CMD.open(source, conf)：打开 listen socket
        CMD.close()：关闭 listen socket
        MSG.error(fd, msg)：消息错误
        MSG.warning(fd, size)：消息警告
*/