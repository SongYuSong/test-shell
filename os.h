#pragma once
#include"process.h"
void Remove(std::vector<process*>& list, process* p) {
	for (unsigned int i = 0; i < list.size(); i++) {
		if (list[i] == p) {
			list.erase(list.begin() + i);
			break;
		}
	}
}
class os {
private:
	resource r1 = { "r1",1,1 };
	resource r2 = { "r2",2,2 };
	resource r3 = { "r3",3,3 };
	resource r4 = { "r4",4,4 };
	std::vector<process*> ready_list[3], running_list, block_list;
	std::vector<resource*> resource_list = { &r1,&r2,&r3,&r4 };
	void Scheduler();
	void Kill_Brother(process* p) {
		if (p->brother)
			p->brother = NULL;
	}
	void Kill_Father(process* p) {
		if (p->father) {
			if (p->brother)
				p->father->children = p->brother;
			else
				p->father->children = NULL;
			p->father = NULL;
		}
	}
	void Empty_Resource(process* p) {
		while (!p->other_resources.empty())//将进程所占据的资源释放出来
		{
			Release(p, *p->other_resources.front().r, p->other_resources.front().rnum);
		}
		p->other_resources.clear();
	}
	void Remove_List(process* p) {
		switch (p->status)//将进程从对应的状态队列中移除
		{
		case ready:
			Remove(ready_list[p->priority], p);
			break;
		case block://当进程处于阻塞状态时，记得将其从等待资源的等待队列中移除。
			Remove(p->block_flag->waiting_list, p->pid);
			Remove(block_list, p);
			break;
		case running:
			Remove(running_list, p);
			break;
		default:
			break;
		}
	}
	void Kill_Tree(process* p) {
		if (p) {
			if (p->children)
				Kill_Tree(p->children);
			Kill_Father(p);
			Kill_Brother(p);
			Empty_Resource(p);
			Remove_List(p);
			delete(p);
		}
	}
	//显示队列内容
	void Display_Vector(std::vector<process*>& list) {
		for (unsigned int i = 0; i < list.size(); i++) {
			std::cout << list[i]->pid << " ";
		}
		std::cout << std::endl;
	}
	void Display_Vector(std::vector<resource*>& list) {
		for (unsigned int i = 0; i < list.size(); i++) {
			if(list[i]->status!=0)
				std::cout << list[i]->rid << " ";
		}
		std::cout << std::endl;
	}
	//同上
	void Display_Vector(std::vector<std::string>& list) {
		for (unsigned int i = 0; i < list.size(); i++) {
			std::cout << list[i] << " ";
		}
		std::cout << std::endl;
	}
	//pid检索进程
	process* pid2p(const std::string& pid, std::vector<process*>& list) {
		unsigned int i = 0;
		for (i; i < list.size(); i++) {
			if (list[i]->pid == pid) {
				break;
			}
		}
		if (i < list.size()) {
			return list[i];
		}
		else
			return NULL;
	}
	//释放资源
	void Release(process* p, resource& r, const unsigned int& n) {//资源释放
		OTHER_RESOURSE o_r = { &r,n };
		Remove(p->other_resources, o_r);//将资源从进程的占用资源列表中移除
		o_r.r->status += o_r.rnum;
		while (r.waiting_list.size() != 0) {//如果资源的等待队列非空，将访问列表头部的进程，将其从阻塞队列中移除，放入就绪队列
			process* pr = pid2p(o_r.r->waiting_list.front(), block_list);
			if (pr->block_num <= o_r.r->status) {
				o_r.r->waiting_list.erase(o_r.r->waiting_list.begin());
				Remove(block_list, pr);
				pr->status = ready;
				pr->other_resources.push_back(o_r);
				o_r.r->status -= pr->block_num;
				ready_list[pr->priority].push_back(pr);
			}
			else {
				break;
			}
		}
		Scheduler();
	}
public:
	//通过pid索引进程
	process* pid2p(const std::string& pid) {
		process* p = NULL;
		for (int i = 0; i < 3; i++) {
			p = pid2p(pid, ready_list[i]);
			if (p != NULL)
				break;
		}
		if (p == NULL)
			p = pid2p(pid, running_list);
		if (p == NULL)
			p = pid2p(pid, block_list);
		return p;
	}
	//通过rid索引资源
	resource* rid2r(std::string& rid) {
		unsigned short i = 0;
		for (i; i < resource_list.size(); i++) {
			if (resource_list[i]->rid == rid) {
				break;
			}
		}
		if (i < resource_list.size()) {
			return resource_list[i];
		}
		else
			return NULL;
	}
	//请求资源
	bool Request(resource& r, const unsigned int& n) {//资源申请
		if (n > r.totalnum) {
			std::cout << "The number of request exceeds the total.\n";
			return false;
		}
		else {
			process* p = running_list[0];
			if (r.status >= n) {//如果资源r由空闲，则为进程分配
				r.status -= n;
				p->other_resources.push_back({ &r,n });//将该资源写入进程p的占用资源列表中
				Scheduler();
				return true;
			}
			else {//如果该资源没有剩余了，将该进程的pid写入资源的等待列表，并将进程状态设置为阻塞，并从运行队列中移除，放入阻塞队列中
				Remove(running_list, p);
				r.waiting_list.push_back(p->pid);
				p->status = block;
				p->block_flag = &r;
				p->block_num = n;
				block_list.push_back(p);
				std::cout << "The number of requests exceeds the number available. The process " << p->pid << " has been blocked.\n";
				Scheduler();
				return false;
			}
		}
	}
	//释放资源
	bool Release(resource& r, const unsigned int& n) {
		process* p = running_list.front();
		if (!p->other_resources.empty()) {
			unsigned short i=0;
			for (i; i < p->other_resources.size(); i++) {
				if (p->other_resources[i].r->rid == r.rid && n <= p->other_resources[i].rnum) {
					Release(p, r, n);
					return true;
				}
			}
			if (i == p->other_resources.size()) {
				std::cout << "process " << p->pid << " doesn't have this resource.\n";
				return false;
			}
		}
		else {
			std::cout << "process " << p->pid << " doesn't have any resource.\n";
			return false;
		}
		return false;
	}
	//模拟中断
	void time_out() { //模拟中断：找到running_list中的头部进程，将其移除放入ready_list中，将进程的状态调整为就绪
		for (int i = 2; i >= 0; i--) {
			if (!ready_list[i].empty()) {
				ready_list[i].front()->status = running;
				running_list.push_back(ready_list[i].front());
				ready_list[i].erase(ready_list[i].begin());
				running_list.front()->status = ready;
				ready_list[running_list.front()->priority].push_back(running_list.front());
				running_list.erase(running_list.begin());
				break;
			}
		}
	}
	//创造进程
	void Create(process* father_process, const std::string& pid, const unsigned int& priority) {//创造一个进程，有两个重载，分别生成根节点与子节点。
		if (pid2p(pid) == NULL) {
			process* p = new process(pid, priority);
			p->father = father_process;
			if (father_process->children)
				p->brother = father_process->children;
			ready_list[p->priority].push_back(p);
			father_process->children = ready_list[p->priority].back();
			Scheduler();
		}
		else {
			std::cout << "This process already exists.\n";
		}
	}
	process* Create(const std::string& pid, const unsigned int& priority) {
		process* p = new process(pid, priority);
		ready_list[p->priority].push_back(p);
		Scheduler();
		return p;
	}
	//删除进程
	void Destory(process* p) {
		Kill_Tree(p);
		Scheduler();
	}
	//显示所有队列的内容
	void Display_list(const std::string& str) {
		if (str == "") {
			std::cout << "* ready_list[0]: ";
			Display_Vector(ready_list[0]);
			std::cout << "* ready_list[1]: ";
			Display_Vector(ready_list[1]);
			std::cout << "* ready_list[2]: ";
			Display_Vector(ready_list[2]);
			std::cout << "* block_list: ";
			Display_Vector(block_list);
			std::cout << "* runing_list: ";
			Display_Vector(running_list);
			std::cout << "* resource_list: ";
			Display_Vector(resource_list);
		}
		else if (str == "run") {
			std::cout << "* runing_list: ";
			Display_Vector(running_list);
		}
		else if (str == "ready") {
			std::cout << "* ready_list[0]: ";
			Display_Vector(ready_list[0]);
			std::cout << "* ready_list[1]: ";
			Display_Vector(ready_list[1]);
			std::cout << "* ready_list[2]: ";
			Display_Vector(ready_list[2]);
		}
		else if (str == "block") {
			std::cout << "* block_list: ";
			Display_Vector(block_list);
		}
		else if (str == "resource") {
			std::cout << "* resource_list: ";
			Display_Vector(resource_list);
		}
	}
};
void os::Scheduler() {
	for (int i = 2; running_list.empty() && i >= 0; i--) {
		if (!ready_list[i].empty()) {
			running_list.push_back(ready_list[i].front());
			ready_list[i].front()->status = running;
			if (ready_list[i].size() == 1)
				ready_list[i].clear();
			else
				ready_list[i].erase(ready_list[i].begin());
			break;
		}
	}
	if (!running_list.empty() && running_list.front()->priority == 0)
		time_out();
}
