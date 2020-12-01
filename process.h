#pragma once
#include"resource.h"
enum pstatus { ready, block, running };//����״̬��ready������̬��block������̬��running������̬

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
	return a.r = b.r;
}

class process {
private:

public:
	process(const std::string& id, const unsigned int& riority);
	std::string pid = "";//���̱�ʶ
	std::vector<OTHER_RESOURSE> other_resources;//ռ�õ���Դ
	pstatus status;//���̵�״̬
	process* father = NULL, * brother = NULL, * children = NULL;//�������ڵ�
	resource* block_flag = NULL;//��ǽ�������״̬���ȴ�����Դ
	unsigned int block_num = 0;//��ǵȴ�����Դ����
	unsigned int priority = 0;//�������ͣ�init=0����ʼ���̣�user_level=1���û������̣�kernel_level=2���ں˼�����

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