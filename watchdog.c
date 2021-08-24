//skynet框架watchdog
/*
    data：
        CMD：非socket消息接口
        SOCKET：socket消息接口
        gate：网关
        agent：代理map
    
    iterface：
        SOCKET.open(fd, addr)：客户端连接，newservice("agent")，向agent发送一个start消息，将gate、fd、watchdog传过去
        SOCKET.close(fd)：关闭客户端连接，close_agent(fd)
        SOCKET.error(fd, msg)：socket错误，close_agent(fd)
        SOCKET.warning(fd, size)：socket警告
        SOCKET.data(fd, msg)：socket数据
        close_agent(fd)：关闭agent，向gate发送一个kick消息，向agent发送一个disconnect消息，agent退出
        CMD.start(conf)：通知gate开启服务
        CMD.close(fd)：close_agent(fd)
*/