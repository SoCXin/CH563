# [CH563](https://github.com/sochub/CH563) 
[![sites](SoC/qitas.png)](http://www.qitas.cn) 
#### 厂商：[WCH](https://github.com/sochub/WCH) 
#### 内核：[ARM9](https://github.com/sochub/ARM9) 
#### 芯片定级：[Level](https://github.com/sochub/Level)
### [CH563简介](https://github.com/sochub/CH563/wiki)

[CH563](https://github.com/sochub/CH563) 是一款32位100MHz ARM9内核MCU，内置100M以太网MAC和PHY，支持10/100M全双工半双工，自动协商，线路自动转换等功能，可以直接和网络终端如PC，嵌入式设备进行数据交互。

高速USB设备（全功能USB 2.0 ECHI主机），4组定时器、双异步串口、双SPI接口、ADC、唯一ID等。

[![sites](SoC/CH563.jpg)](http://wch.cn/download/CH563DS1_PDF.html)

官方SDK资源通过lib文件提供，CH563NET.LIB提供了TCP/IP子程序库，集成了TCP,UDP,ICMP,ARP,RARP,ICMP,IGMP 等
以太网协议栈。可以同时支持 TCP,UDP 和 IPRAW三种模式，最大支持31个socket同时通讯。

### [收录资源](https://github.com/sochub/CH563)

* [参考文档](docs/)
* [参考资源](src/)

[![sites](SoC/CH563P.jpg)](http://wch.cn/download/CH563DS1_PDF.html)

### [选型建议](https://github.com/sochub)

[CH563](https://github.com/sochub/CH563)是一款性能非常强大的MCU，可以用于USB和以太网之间的桥接，但是架构较老旧，开发体验不佳。

在一定程度上，特定场景中，[CH563](https://github.com/sochub/CH563)可以被[CH579](https://github.com/sochub/CH579)取代。

##  [SoC资源平台](http://www.qitas.cn)