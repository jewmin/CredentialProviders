使用说明

开发环境：win10 + vs2008
目标环境：需要安装vs2008运行时库
为什么选择vs2008，因为vs2010以后编译的程序不再支持win2000系统

x86：https://www.microsoft.com/zh-CN/download/details.aspx?id=29
x64：https://www.microsoft.com/zh-CN/download/details.aspx?id=15336

1）安装Gina
如果是x86系统，复制Win32的dll到C:\Windows\System32目录
如果是x64系统，复制x64的dll到C:\Windows\System32目录
管理员权限运行Register.reg

2）卸装Gina
管理员权限运行Unregister.reg

3）不管安装还是卸装，都需要重启后才生效，如果Gina有问题，无法进入桌面，那么可以重启时，按F8，进入安全模式，再卸装Gina

注1：日志保存在C盘
注2：需要登录2次，第1次会返回失败，第2次才会成功，主要是用来显示失败如何提示的。后面你们可以改成直接成功