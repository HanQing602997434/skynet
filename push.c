// skynet推送消息
/*
    具体的推送实现由游戏中的key键决定
    建立一个映射，以steamid为游戏key键为例：
    local steamid_agent = {}
    将该结构置于gate服务中，每次登录成功通知gate记录信息
    需要推送给其它的steamid，则将消息推送到网关，网关根据steamid找到对应的agent，将消息重定向给agent
*/