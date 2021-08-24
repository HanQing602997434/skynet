//skynet框架gate
/*
    gate服务连同gateserver都是处理客户端连接的
    数据：
        watchdog：看门狗，看门狗在start的时候将自己的地址通知给gate
        connection：fd、connection映射表，fd -> connection {fd -> {fd, ip, ...}}
        forwarding：agent、connection映射表，agent -> connection {agent -> {client, agent, ...}}
        handler：gate处理接口
        CMD：命令处理接口

    接口：
        handler.open(source, conf)：记录watchdog地址
        handler.message(fd, msg, sz)：处理接收到的客户端消息，查看是否是已存在的fd的消息，如果是redirect给agent，否则发送给watchdog
        handler.connect(fd, addr)：客户端连接处理，将数据发送给watchdog作newservice(agent)
        handler.disconnect(fd)：关闭连接，告诉watchdog关闭socket
        handler.error(fd, msg)：错误处理，告诉watchdog关闭socket
        handler.warning(fd, size)：警告处理，告诉watchdog警告
        handler.command(cmd, source, ...)：gate接口
        CMD.forward(source, fd, client, address)：连接时创建用户数据
        CMD.accept(source, fd)：accpet客户端连接，unforward(c)，gateserver打开socket连接
        CMD.kick(source, fd)：剔除客户端连接，gateserver关闭socket连接
        unforward(c)：accept时回收用户数据
        close_fd(fd)：关闭fd
        
*/