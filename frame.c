
// skynet框架搭建
/*
    linux环境搭建：
        安装VMware：
        配置静态IP：sudo mv /etc/netplan/01-network-manager-all.yaml /etc/netplan/01-network-manager-all.yaml.bak、sudo vim /etc/netplan/01-network-manager-all.yaml
                   # create new
                   network:
                     ethernets:
                       ens33:
                         dhcp4: no
                         addresses: [192.168.0.53/24]
                         gateway4: 192.168.0.254
                         nameservers:
                           addresses: [114.114.114.114]
                         dhcp6: no
                     version: 2

        安装vim：sudo apt-get install vim
        安装git：sudo apt-get install git
        安装readline-devel：sudo apt-get install libreadline-dev
        安装autoconf：sudo apt-get install autoconf
        安装openssh-server：sudo apt-get update、sudo apt-get install openssh-server
        安装samba：sudo apt install samba、修改smb.conf、chown -R samba:samba /home/server、sudo systemctl start smbd、sudo systemctl enable smbd
    protobuf协议接入：
        编译安装luarocks：
        编译安装lua-protobuf：
        protobuf转pb文件：
*/