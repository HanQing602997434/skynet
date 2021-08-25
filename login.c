// skynet框架的登录服务
/*
    登录服务：现在的网络游戏大部分是需要登录的，一般会有一个专门的登录服务来处理，
        登录服务要解决的问题：1、用户登录信息保密工作。2、实际登录点分配工作。
    
        1.加密算法(DHexchange密钥交换算法)：DHexchange密钥交换算法主要用来协商一个服务器和客户端的密钥，可以直接这么使用：
            package.cpath = "luaclib/?.so"
            local crypt = require "client.crypt"
            在skynet中使用直接local crypt = require "skynet.crypt"
            dhexchange转换8字节的key：crypt.dhexchange(key)
            通过key1与key2得到密钥

            示例代码testdhexchange.lua：
                package.cpath = "luaclib/?.so"
                local crypt = require "client.crypt"

                local clientkey = "11111111" --8byte random
                local ckey = crypt.dhexchange(clientkey)
                print("ckey:\t", crypt.hexencode(ckey))

                local serverkey = "22222222"
                local skey = crypt.dhexchange(serverkey)
                print("skey:\t", crypt.hexencode(skey))

                local csecret = crypt.dhsecret(skey, clientkey)
                print("csecret:\t", crypt.hexencode(csecret))

                local ssecret = crypt.dhsecret(ckey, serverkey)
                print("ssecret:\t", crypt.hexencode(ssecret))

        2.随机数：
            crypt.randomkey()

        3.hmac64哈希算法：
            HMAC64利用哈希算法，以一个密钥secret和一个消息challenge为输入，生成一个消息摘要hmac作为输出
            local hmac = crypt.hmac64(challenge, secret)

        4.base64编解码：
            base64是一种基于64个可打印字符来表示二进制数据的方法
            
            -- 编码
            crypt.base64encode(str)

            --解码
            crypt.base64decode(str)

        5.DES加解密：
            -- 用key加密plaintext得到密文，key必须是8字节
            crypt.desencode(key, plaintext)

            -- 用key解密ciphertext得到明文，key必须是8字节
            crypt.desdecode(key, ciphertext)

        6.hashkey算法：
            云风自实现的hash算法，只能哈希小于8字节的数据，返回8字节数据的hash

    loginserver原理：
        login服务开启监听，客户端主动去连接login服务，他们之间的通信协议是行结尾协议(即：每个数据包都是一行ascii字符，如果要发送
        byte字节流，则通过base64编码)。这里称login服务为L，客户端为C。
        1.L产生随机数challenge，并发给C，主要用于最后验证密钥secret是否交换成功。
        2.C产生随机数clientkey，clientkey是保密的，只有C知道，并通过dhexchange算法换算clientkey，得到ckey。把base64编码的
        ckey发送给L。
        3.L也产生随机数serverkey，serverkey是保密的，只有L知道，并通过dhexchange算法换算serverkey，得到skey。把base64编码
        的skey发送给C。
        4.C使用clientkey和skey，通过dhsecret算法得到最终安全密钥secret。
        5.L使用serverkey和ckey，通过dhsecret算法得到最终安全密钥secret。C和L得到的secret是一样的，而传输过程只有ckey和skey
        是通过网络公开的，即使ckey和skey泄露了，也无法推算出secret。
        6.密钥交换完成后，需要验证一下双方的密钥是否是一致的，C使用密钥secret通过hmac64哈希算法加密第一步中接收到的challenge，
        得到CHmac，然后转码成base64 CHmac发送给L。
        7.L收到CHmac后，自己也使用密钥secret通过hmac64哈希算法加密第1步中发送出去的challenge，得到SHmac，对比SHmac与CHmac
        是否一致，如果一致，则密钥交换成功。不成功就断开连接。
        8.C组合base64 user@base64 server:base64 passwd字符串(server为客户端具体想要登录的登录点，远端服务器可能有多个实际
        登录点)，使用secret通过DES加密，得到etoken，发送base64 etoken。
        9.使用secret通过DES解密etoken，得到user@server:passwd，校验user与passwd是否正确，通知实际登录点server，传递user与
        secret给server，server生成subid返回。发送状态码200 base64 subid给C。
        10.C得到subid后就断开login服务的连接，然后去连接实际登录点server了。(实际登录点server，可以由L通知C，也可以C指定想要
        登录哪个点)

    loginserver模板：

        local login = require "snax.loginserver"
        local server = {
            host = "127.0.0.1",
            port = 8001,
            multilogin = false, -- disallow multilogin
            name = "login_master",
            -- config, etc
        }

        login(server)

        host是监听地址，通常是"0.0.0.0"。
        port是监听端口。
        name是一个内部使用的名字，不要和skynet其他服务重名。在上面的例子中，登录服务器会注册为.login_master这个名字，相当于
        skynet.register(".login_master")
        multilogin是一个boolean，默认是false。关闭后，当一个用户正在走登录流程时，禁止同一用户名进行登录。如果你希望用户可以
        同时登录，可以打开这个开关，但需要自己处理好潜在的并行的状态管理问题。
        
        同时，你还需要注册一系列相关的必要方法。
            -- 你需要实现这个方法，对一个客户端发送过来的 token 做验证。如果验证不能通过，可以通过 error 抛出异常。如果验证通过，需要返回用户希望进入的登陆点以及用户名。（登陆点可以是包含在 token 内由用户自行决定,也可以在这里实现一个负载均衡器来选择）
            function server.auth_handler(token) end

            --你需要实现这个方法，处理当用户已经验证通过后，该如何通知具体的登陆点（server ）。框架会交给你用户名（uid）和已经安全交换到的通讯密钥。你需要把它们交给登陆点，并得到确认（等待登陆点准备好后）才可以返回。
            function server.login_handler(server, uid, secret) end
​
            --实现command_handler，用来处理lua消息，必须注册
            function server.command_handler(command, ...) end
        
    账户核对失败的处理：
        一旦有登录请求进来，在调用回调函数server.auth_handler以及server.login_handler都时开启一个协程来处理，
        assert和error都能终止掉协程，并不是终止掉整个服务。
        
        虽然我们在启动logind服务的时候一下启动了9个服务，但这9个服务中一个是监听使用，其他服务负责与客户端交换密钥以及
        处理账号验证，8个服务共同分担处理任务，可以通过不断启动客户端来观察。8个服务轮流处理一次登录请求。

    login_handler错误处理：
        与skynet.auth_handler的处理一样，一旦错误，也不需要我们返回任何值，只要终止掉当前协程，skynet.loginserver框架
        会自动发送406 Not Acceptable给客户端。

    登录重入报错：
        multilogin设置为false表示不允许同时重复登录，这里的同时重复登录不是说一个client登录完成之后，另一个client使用
        相同的账号密码就不能登录。而是说在登录过程当中，还没完成登录，这个时候突然又有一个client尝试登录。那么会报给这个
        客户端406 Not Acceptabel。

    密钥交换失败：
        密钥交换失败一般不会发生在前几个步骤，因为前几个步骤不会去验证双方的数据是否正确，只要在交换完密钥使用密钥加密
        challenge的时候才会验证一下，如果这个时候验证不成功将会返回客户端一个：400 Bad Request。
*/