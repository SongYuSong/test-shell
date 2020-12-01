#pragma once
#include"resource.h"
enum pstatus { ready, block, running };//进程状态：ready：就绪态；block：阻塞态；running：运行态

typedef struct OTHER_RESOURSE {
	resource* r;
	unsigned int rnum;
	OTHER_RESOURSE& operator =(const OTHER_RESOURSE& o_r) {
		if (this != &o_r) {
			r = o_r.r;
			rnum = o_r.rnum;
		}
		return *this;
	}
}OTHER_RESOURSE;

bool operator ==(OTHER_RESOURSE& a, OTHER_RESOURSE& b) {
	return a.r == b.r;
}

class process {
private:

public:
	process(const std::string& id, const unsigned int& riority);
	std::string pid = "";//进程标识
	std::vector<OTHER_RESOURSE> other_resources;//占用的资源
	pstatus status;//进程的状态
	process* father = NULL, * brother = NULL, * children = NULL;//生成树节点
	resource* block_flag = NULL;//标记进入阻塞状态所等待的资源
	unsigned int block_num = 0;//标记等待的资源数量
	unsigned int priority = 0;//进程类型：init=0：初始进程；user_level=1：用户级进程；kernel_level=2：内核级进程

	process& operator =(const process& p) {
		if (this != &p) {
			pid = p.pid;
			other_resources = p.other_resources;
			status = status;
			father = p.father;
			brother = p.brother;
			children = p.children;
			block_flag = p.block_flag;
			block_num = p.block_num;
			priority = p.priority;
		}
		return *this;
	}
};
inline process::process(const std::string& id, const unsigned int& riority)
{
	pid = id;
	status = ready;
	priority = riority;
	father = NULL;
	brother = NULL;
	children = NULL;
}
void Remove(std::vector<OTHER_RESOURSE>& list, OTHER_RESOURSE r) {
	for (unsigned int i = 0; i < list.size(); i++) {
		if (list[i] == r) {
			list.erase(list.begin() + i);
			break;
		}
	}
}
