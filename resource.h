#pragma once
class resource {
public:
	std::string rid;//��Դ��ʶ
	unsigned int totalnum;
	unsigned int status;//���е�Ԫ������
	std::vector<std::string> waiting_list;//�ȴ�����
};
void Remove(std::vector<std::string>& list, std::string& pid) {
	for (unsigned int i = 0; i < list.size(); i++) {
		if (list[i] == pid) {
			list.erase(list.begin() + i);
			break;
		}
	}
}