## 使用vm写的一个小型的CTF题目

输入两个正确数据a b 即可通过！

```
➜  lil-vm ./lil_vm
请输入2个数字：a b 
  1 2
i8load  !!00    ?00
i8load  !!01    ?03
i8load  !!02    ?FF
i8load  !!03    ?09
i8add   !!00    !!00    !!01
i8load  !!03    ?09
i8add   !!00    !!00    !!01
i8load  !!03    ?09
i8add   !!00    !!00    !!01
错误请重新输入!
请输入2个数字：a b 
```




## 一个简单的vm学习

由指令集，指令解释引擎两部分组成。
其中指令解释引擎又由寄存器和内存两部分组成。

## 常用工具

c语言在线运行的网站：
https://www.dooccn.com/c/


c语言转汇编的网址
https://godbolt.org/


汇编语言执行的网址：
https://wunkolo.github.io/OakSim/