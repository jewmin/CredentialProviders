使用说明

1）安装凭证
如果是x86系统，复制Win32的dll到C:\Windows\System32目录
如果是x64系统，复制x64的dll到C:\Windows\System32目录
管理员权限运行Register.reg

2）卸装凭证
管理员权限运行Unregister.reg

3）安装服务
使用管理员权限运行cmd，在cmd中执行指令DemoService.exe --install，然后可在服务管理中找到对应服务Demo Service
注1：安装时，最好使用全路径，不然默认路径是C:\Windows\System32，需要把DemoService.exe复制到C:\Windows\System32目录才能启动
注2：win+r打开运行框，输入services.msc，可打开服务管理器，可搜索到Demo Service，双击打开后点击启动，或者直接重启系统，自动启动

4）卸装服务
使用管理员权限运行cmd，在cmd中执行指令DemoService.exe --uninstall

5）凭证流程
1.系统调用CSampleProviderFilter::Filter，屏蔽其他凭证，只保留自己的凭证，如果不想屏蔽，可以把Register.reg里的下面这项删除掉
HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Authentication\Credential Provider Filters\{FF032558-38DA-4472-B969-31A636B7E5C7}
注：FF032558-38DA-4472-B969-31A636B7E5C7这个是凭证的唯一ID，需要与guid.h的值一致
DEFINE_GUID(CLSID_CSample, 0xff032558, 0x38da, 0x4472, 0xb9, 0x69, 0x31, 0xa6, 0x36, 0xb7, 0xe5, 0xc7);

2.系统调用CSampleProvider::SetUsageScenario，初始化凭证

3.系统调用CSampleCredential::Initialize，初始化凭证中的控件

4.当输入完用户名和密码，然后点击确定按钮，系统调用CSampleCredential::Connect，在这里阻塞主线程，等待授权验证结果返回

5.系统调用CSampleCredential::GetSerialization，这里进行系统登录验证，如果授权失败，则直接返回，否则进入登录验证

6.系统调用CSampleCredential::ReportResult，如果登录失败，会在这里处理

6）凭证控件
1、CSampleCredential::GetBitmapValue，可在这里修改图片
2、CSampleCredential::CommandLinkClicked，可在这里修改超链接，默认这里显示MessageBox，可改成创建对话框

7）注意事项
1、Win10，正常情况下，关机并不是真正的关机，而是休眠。所以启动系统后，显示的登录界面，其实是在解锁用户，并非登录用户。可以在Service_xxx.log日志中观察到
2、MyService是测试业务逻辑的，应该由你们重写
3、最新发布版本、测试时，需要输入多次用户名和密码，因为返回的授权结果是随机生成的，连续多次登录就行
4、如果服务程序无法停止，可以打开任务管理器，找到对应的服务进程，强制结束任务
5、日志保存在C:\Windows\System32