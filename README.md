# EventGeneratingForCK3
A tool for generating events for Crusader Kings 3, used to help mod makers.

一个为十字军之王3制作的事件生成器，主要针对新手modder。目前还在开发中，但是已经有了基本功能  
  -可以生成事件的基础结构，包括选项和用于触发其他事件的trigger_event  
  -可以在生成器中为事件添加本地化，这让你不需要在多个文件中反复切换  
  -可以读取已经写好的事件和本地化，读取的事件文件最多只能有一个命名空间（即使有多个命名空间定义也能读取，但是会在随后的编辑中出现bug，谨慎选择），本地化可以随意读取  
  -可以为事件间的触发关系进行可视化，目前的可视化制作仍比较粗糙但是已经实现了基本功能  


  
使用示例：
![](https://raw.githubusercontent.com/Frostbite-time/EventGeneratingForCK3/main/img-storage/example.png?raw=true)
