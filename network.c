// skynet网络模块

/*

    1.在服务器启动时的main脚本中new出一个watchdog服务：local watchdog = skynet.newservice("watchdog")
    2.watchdog在start时new出一个gate服务：gate = skynet.newservice("gate")
    3.在gate中启动了gateserver服务
    4.接着main脚本调用watchdog的start接口，start函数调用gate的open接口，gate服务开启监听端口并记录watchdog地址
    5.watchdog开启gateserver
    6.gateserver开始监听socket
    7.gateserver打开gate
    8.socket启动监听
    9.客户端连接socket
    10.socket dispatch open给gateserver
    11.连接gate
    12.socket.open
    13.watchdog new agent
    14.watchdog start agent
    15.agent forward gateserver
    16.gateserver CMD.forward gate
    17.gate openclient
    18.gateserver start socket
    19.client send msg
    20.gateserver dispatch_msg
    21.gate get message
    22.gate send message to agent
    23.client close socket
    24.gateserver MSG.close
    25.gate disconnect
    26.watchdog SOCKET.close
    27.watchdog kick gate
    28.watchdog CMD.disconnect agent
*/