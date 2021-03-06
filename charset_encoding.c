
//字符编码
/*
    2021.9.17就遇到的字符编码问题做一个总结。

    server端使用sublime开发，sublime默认支持的编码方式是UTF8。

    在client端使用UE4 & VS2017开发，VS默认支持的编码方式是GB2312。

    在客户端进行protobuf协议打包时，遇到string是汉字的情况，发送到服务端解析出来的汉字是乱码的。

    不一致的编码方式还会导致数据无法存入mysql。

    解决方案是将VS的编码改成UTF8。验证正确。

    字符编码：
        字符编码是一个编码的集合。
*/