
//skynet框架gateserver：
/*
    gateserver call by gate，message accept by gate，but almost message handle in gateserver
    data：
        socket：listen socket
        queue：message queue
        maxclient：max client
        client_number：connect client number
        nodelay：no delay
        connection：connect fd map

    iterface：
        MSG.open(fd, msg)：open fd，accept message
        MSG.close(fd)：close fd
        dispatch_queue()：handle message queue
        dispatch_msg(fd, msg, sz)：handle message
        CMD.open(source, conf)：open listen socket
        CMD.close()：close listen socket
        MSG.error(fd, msg)：message error
        MSG.warning(fd, size)：message warning
*/