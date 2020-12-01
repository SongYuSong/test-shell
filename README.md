# test-shell
操作系统实验-test shell  

对内核的封装后的接口：

>>process* pid2p(const std::string& pid);//通过pid索引对应的进程  
resource* rid2r(std::string& rid);//通过rid索引对应的资源  
bool Request(resource& r, const unsigned int& n);//请求资源  
bool Release(resource& r, const unsigned int& n);//释放资源  
void time_out();//中断  
void Create(process* father_process, const std::string& pid, const unsigned int& priority);//创造子进程  
process* Create(const std::string& pid, const unsigned int& priority);//创造进程  
void Destory(process* p);//销毁进程  
void Display_list(const std::string& str);//在终端显示状态列表  






测试的shell：

运行后输入help可以获取命令列表  
结束运行时输入exit命令可以终止shell。  

创建进程的方法：  
>>1.cr pid priority 比如：cr  p1 1 即为创建了一个pid为p1的用户级进程。  
>>2.cr pid father_pid proiority 比如：cr p2 p1 1 即为创建了一个进程p1的子进程，子进程的pid为p2，是一个用户级的子进程。 

删除进程的方法：  
>>de pid ：比如de p1 即为删除了p1进程及其子进程。  

显示队列信息：  
>>1.list 队列名称（run，ready，block，resource），分别显示对应状态的队列以及可用资源列表。  
>>2.list 无参数，显示所用进程状态队列以及可用资源列表。  

申请资源：  
>>req rid n  

释放资源：  
>>rel rid n  

>>无论是申请资源还是释放资源，都只是当前正在运行的进程才可以执行的操作。 
中断指令：  
>>to  
