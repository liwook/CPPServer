基于我的理解，模仿muduo逐步实现基于Reactor模式的多线程服务器  
基于该网络库，实现了简易的HTTP服务器,websocket服务器  
每个迭代的版本介绍讲解在我的博客内,博客地址  
[实现linux服务器开发思路](https://blog.csdn.net/m0_57408211/article/details/127062135)  

迭代的版本及讲解  
[1.从简单的socket编程开始](https://blog.csdn.net/m0_57408211/article/details/127062399?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127062399%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[2.多线程登场，服务于多客户端](https://blog.csdn.net/m0_57408211/article/details/127098028?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127098028%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[3.epoll登场，单线程也可服务多用户](https://blog.csdn.net/m0_57408211/article/details/127113376?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127113376%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[4.添加类，模块化](https://blog.csdn.net/m0_57408211/article/details/127114621?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127114621%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[5.添加Channel类，Reactor模式初形成](https://blog.csdn.net/m0_57408211/article/details/127131889?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127131889%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[6.添加类EventLoop，增添回调函数](https://blog.csdn.net/m0_57408211/article/details/127207753?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127207753%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[7.添加Acceptor class](https://blog.csdn.net/m0_57408211/article/details/127353232)  
[8.添加Buffer类，缓冲区很有必要](https://blog.csdn.net/m0_57408211/article/details/127212068?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127212068%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[9.添加Connection类](https://blog.csdn.net/m0_57408211/article/details/127523347)  
[10.继续完善Connection类,单Reactor模式成型](https://blog.csdn.net/m0_57408211/article/details/127600392)  
[11.添加线程池，单Reactor+threadpool模式成型](https://blog.csdn.net/m0_57408211/article/details/127705002)  
[12.添加可跨线程调用的函数](https://blog.csdn.net/m0_57408211/article/details/127889905?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127889905%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[13.主从Reactor模型](https://blog.csdn.net/m0_57408211/article/details/127945747?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127945747%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[14.主从Reactor+thread pool模式，和Connection对象的引用计数的困惑讲解](https://blog.csdn.net/m0_57408211/article/details/127982843?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22127982843%22%2C%22source%22%3A%22m0_57408211%22%7D)  
[15.添加定时器](https://blog.csdn.net/m0_57408211/article/details/128169787)  
[16.完善定时器功能，添加EventLoop关于定时器的相关操作函数](https://blog.csdn.net/m0_57408211/article/details/128192045)  
[17.添加异步日志——1.日志消息的存储和输出](https://blog.csdn.net/m0_57408211/article/details/127932842)  
[18.添加异步日志——2.实现异步](https://blog.csdn.net/m0_57408211/article/details/127937029)  
[19.添加异步日志——3.继续完善](https://blog.csdn.net/m0_57408211/article/details/127942786?spm=1001.2014.3001.5502)  
[20.添加HTTP模块](https://blog.csdn.net/m0_57408211/article/details/128385213)  
[21.添加websocket模块](https://blog.csdn.net/m0_57408211/article/details/130743106?csdn_share_tail=%7B%22type%22%3A%22blog%22%2C%22rType%22%3A%22article%22%2C%22rId%22%3A%22130743106%22%2C%22source%22%3A%22m0_57408211%22%7D)  


主从 Reactor 模式中，分为了 主Reactor 和 从Reactor，分别处理 **新建立的连接和分发、处理客户端读写事件**。  
主从多线程模型由多个 Reactor 线程组成。MainReactor 负责处理客户端连接的 Accept 事件，连接建立成功后将新创建的连接注册到任一SubReactor。再由SubReactor分配线程池中的 I/O 线程与其连接绑定，它将负责连接生命周期内所有的 I/O 事件。

![主从Reactor模型](https://gao-xiao-long.github.io/img/in-post/multi_reactor.png)  

