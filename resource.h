#pragma once
class resource {
public:
	std::string rid;//资源标识
	unsigned int totalnum;
	unsigned int status;//空闲单元的数量
	std::vector<std::string> waiting_list;//等待队列
};
void Remove(std::vector<std::string>& list, std::string& pid) {
	for (unsigned int i = 0; i < list.size(); i++) {
		if (list[i] == pid) {
			list.erase(list.begin() + i);
			break;
		}
	}
}