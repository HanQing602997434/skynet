//skynet框架agent
/*
    agent一对一处理客户端消息，一个客户端连接就会生成一个agent服务，消息由gate服务redirect过来
    数据：
        WATCHDOG：看门狗地址
        host：协议解析模块
        send_request：发送请求
        CMD：lua消息处理接口

    接口：
       CMD.start(conf)：agent启动，agent启动会fork一个5秒一次的heartbeat，并通知gate服务forward
       CMD.disconnect()：agent关闭，服务注销
*/