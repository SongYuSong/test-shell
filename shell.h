#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include"os.h"

enum class command:unsigned int {
	cr,de,ps,req,rel,to,help
};

class shell {
private:
	process* init = os.Create("init", 0);
	os os;
	std::unordered_map<std::string, command> command = { 
		{"cr",command::cr},{"de",command::de},{"list",command::ps},
		{"req",command::req},{"rel",command::rel},{"to",command::to},
		{"help",command::help}
	};
	void Parse(std::string& str,std::string& operand1,std::string& operand2, std::string& operand3) {
		std::string s;
		unsigned short flag=0;
		for (unsigned int i = 0; i < str.size(); i++) {
			if (str[i] != ' ') {
				switch (flag)
				{
				case 0:
					s += str[i];
					break;
				case 1:
					operand1 += str[i];
					break;
				case 2:
					operand2 += str[i];
					break;
				case 3:
					operand3 += str[i];
				default:
					break;
				}
			}
			else {
				if(i>=1)
					if (str[i - 1] != ' ')
						flag++;
			}
		}
		str = s;
	}
	void Command_Interpreter(std::string& str) {
		std::string operand1, operand2, operand3;
		Parse(str, operand1, operand2, operand3);
		switch (command[str])
		{
		case command::cr: {
			if (operand3 == "") {
				unsigned short i = operand2[0] - 48;
				os.Create(init, operand1, i);
			}
			else {
				unsigned short i = operand3[0] - 48;
				os.Create(os.pid2p(operand2), operand1, i);
			}
			std::cout << "* Process " << operand1 << "  created successfully\n";
			break;
		}
		case command::de: {
			os.Destory(os.pid2p(operand1));
			std::cout << "* Process " << operand1 << " and it's children were successfully destroyed\n";
			break;
		}
		case command::ps: {
			os.Display_list(operand1);
			break;
		}
		case command::req: {
			unsigned short i = 0;
			for (unsigned short j = 0; j < operand2.size(); j++) {
				i = i * 10 + operand2[j] - 48;
			}
			if (os.Request(*os.rid2r(operand1), i)) {
				std::cout << "* Resource request successful\n";
			}
			else
				std::cout << "* Resource request failed\n";
			break;
		}
		case command::rel: {
			unsigned short i = 0;
			for (unsigned short j = 0; j < operand2.size(); j++)
			{
				i=i*10+ operand2[0] - 48;
			}
			if (os.Release(*os.rid2r(operand1),i))
			{
				std::cout << "* Resource has been released.\n";
			}
			break;
		}
		case command::to: {
			os.time_out();
			break;
		}
		case command::help: {
			std::cout
				<< "* help: list all command. [help]\n"
				<< "* exit: exit the shell. [exit]\n"
				<< "* cr: creat a process. [cr pid priority] or [cr pid father_pid priority]\n"
				<< "* de: distory a process. [de pid]\n"
				<< "* list: list all status list. [list]\n"
				<< "* req: request resource. [req rid num]\n"
				<< "* rel: release resource. [rel rid num]\n"
				<< "* to: time out. [to]\n";
			break;
		}
		default:
			std::cout << "* shell: " << str << ": command not found!\n";
			break;
		}
	}
public:
	void display() {
		std::cout << "*Process init is running\n";
		while (true)
		{
			std::string str;
			std::cout << "shell>>";
			getline(std::cin, str);
			if (str == "")
				continue;
			if (str=="exit")
				break;
			Command_Interpreter(str);
		}
		os.Destory(init);
		std::cout << "Goodbye!";
	}
};
