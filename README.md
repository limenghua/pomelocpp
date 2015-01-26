# pomelocpp
pomelo C++ 客户端，libpomelo2 的简单C++包装

libpomelo2出现后，除了C版本为，也有Java和python绑定，但迟迟没有C++版本的包装。
也可能是官方认为这个各功能太简单，使用者自己可以搞定吧。但我认为还是应该造出这么一个车轮，或者车轮模板吧。
所以，先做出这样一个样例，希望引起官方注意，然后大家共同完善。

我对这样的类库要求是这样的：

1. 接口尽可能简单，尽量隐藏不必要的接口。

2. 实现轻量级的包装，只实现事件的分发和json解析两项基本功能，应用层协议交由用户扩展。

3. 操作进行异步调用，接口模仿node.js异步调用（利用C++11标准的lambda语法)