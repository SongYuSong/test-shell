#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<unordered_map>
#include <cctype>
#include<sstream>
#include"os.h"

#define NORMAL 1
#define EXIT 0
#define UNKNOWN_CMD -1
#define WRONG_PARAMS -2
#define ERROR -3
#define NOT_INIT -4

enum class command:unsigned int {
	cr,de,ps,req,rel,to,help,init,exit,unknown,all
};

class shell {
private:
	process* init = NULL;
	os os;
	std::vector<std::string> params;
	//字符串->指令对应
	std::unordered_map<std::string, command> commandlist = { 
		{"cr",command::cr},{"de",command::de},{"list",command::ps},
		{"req",command::req},{"rel",command::rel},{"to",command::to},
		{"help",command::help},{"init",command::init},{"exit",command::exit}
	};
	//commandbook:用于help
	std::unordered_map<command,std::vector<std::string>> commandbook = {
		{command::help,{
			"* help: list all command. [help] (or [help cmd] for detial of certain command)\n",
			"! command help don't need parameters when uesing it for cmmmand list; need 1 parameter when using it for detial of certain command\n",
			"if needed, parameter should be command name, which is one of cr,de,ps,req,rel,to,help,init,exit\n"
		}},
		{command::cr,{
			"* cr: creat a process. [cr pid priority] or [cr pid father_pid priority]\n",
			"! command cr need 2 parameters when the process doesn't have father, or it needs 3\n",
			"when there is no father process use [cr pid priority] ,or use [cr pid father_pid priority], pid and father_pid is process's name, priority is 1 as 'user' or 2 as 'system'\n"
		}},
		{command::de,{
			"* de: distory a process. [de pid]\n",
			"! command de need 1 parameter\n",
			"parameter should be process name\n"
		}},
		{command::ps,{
			"* list: list all status list. [list]\n",
			"! command list doesn't neeed parameters when listing all information; need 1 parameter when listing certain information\n",
			"if needed, parameter should be one of run,ready,block,resource\nrun: list running process\nready: list ready process\nblock: list blocked process\nresource: list resources\n"
		}},
		{command::req,{
			"* req: request resource. [req rid num]\n",
			"! command req need 2 parameter\n",
			"[req rid num] rid is resource's name, one of r1,r2,r3,r4. num is needed number\n"
		}},
		{command::rel,{
			"* rel: release resource. [rel rid num]\n",
			"! command rel need 2 parameter\n",
			"[rel rid num] rid is resource's name, one of r1,r2,r3,r4. num is needed number\n"
		}},
		{command::to,{
			"* to: time out. [to]\n",
			"! command to need no parameters\n"
		}},
		{command::init,{
			"* init: initialization. [init]\n",
			"! command init need no parameters\n"
		}},
		{command::exit,{
			"* exit: exit the shell. [exit]\n",
			"! command exit need no parameters\n"
		}}
	};
	//分析函数，判断并返回指令类型同时以空格为分隔符分割参数存入params
	enum class command Parse(std::string& str) {
		int pos = str.find(" ");
		std::string cmd = str.substr(0, pos);
		for (auto iter = commandlist.begin(); iter != commandlist.end(); ++iter)
			if (iter->first == cmd) {//如果指令存在则分割参数然后返回指令类型
				for (int npos = 0; (npos = str.find(" ", pos + 1)) != -1; pos = npos)
					params.push_back(str.substr(pos+1, npos - pos-1));
				return iter->second;
			}
		std::cout << "* shell: " << cmd << ": command not found!\n";
		return command::unknown;
	}
	//判断参数数量是否正确
	bool param_num(enum class command type, int max=0, int min=0) {
		if(params.size()<=max&&params.size()>=min)return true;
		if (params.size() > max) {
			if (max == 0)std::cout << "X wrong params\n";
			else std::cout << "X too manay parameters\n";
		}
		if (params.size() < min) {
			std::cout << "X too few parameters\n";
		}
		Help(type, 1);
		return false;
	}
	//判断字符串里是否只含数字
	bool is_all_digit(enum class command type, std::string str) {
		for (int i = 0; i < str.length(); i++)
			if (!isdigit(str[i])){
				Help(type, 0, 1);
				return false;
			}
		return true;
	}
	//str->>int
	int str2int(std::string str) {
		std::stringstream stream;
		stream << str;//向流中传值
		int result;//这里存储转换结果
		stream >> result;//向result中写入值
		return result;
	}
	//执行指令cr
	int Cr() {
		if (init == NULL)return NOT_INIT;//不能在init前使用
		if (!param_num(command::cr,3,2))return WRONG_PARAMS;//参数数量检查
		if (!is_all_digit(command::cr,params.back()))return WRONG_PARAMS; //最后一个参数必须是数字
		if (params.back() != "1" && params.back() != "2") {
			std::cout << "X priority can only be 1 as 'user' or 2 as 'system'\n";
			return WRONG_PARAMS;
		}
		if (params.size() == 2)os.Create(init, params[0], str2int(params.back()));
		if (params.size() == 3)os.Create(os.pid2p(params[1]), params[0], str2int(params.back()));
		std::cout << "* Process " << params[0] << "  created successfully\n";
		os.Display_running();
		return NORMAL;
	}
	//执行指令de
	int De() {
		if (init == NULL)return NOT_INIT;
		if (!param_num(command::de, 1, 1))return WRONG_PARAMS;
		os.Destory(os.pid2p(params[0]));
		std::cout << "* Process " << params[0] << " and it's children were successfully destroyed\n";
		os.Display_running();
		return NORMAL;
	}
	//执行指令list
	int List() {
		if (init == NULL)return NOT_INIT;
		if (!param_num(command::init, 1))return WRONG_PARAMS;
		if (params.empty())params.push_back("");//空参数的情况
		std::vector<std::string> range{ "","run","ready","block","resource" };//list的参数只能是这些
		if (std::find(range.begin(), range.end(), params[0]) == range.end()) {
			std::cout << "can't list this...\n";
			Help(command::ps, 0, 1);
			return WRONG_PARAMS;
		}
		os.Display_list(params[0]);
		return NORMAL;
	}
	//执行指令req
	int Req() {
		if (init == NULL)return NOT_INIT;
		if (!param_num(command::req, 2, 2))return WRONG_PARAMS; 
		if (!is_all_digit(command::req, params.back()))return WRONG_PARAMS;
		if (os.Request(os.rid2r(params[0]), str2int(params.back()))) {
			std::cout << "* Resource request successful\n";
		}
		else
			std::cout << "* Resource request failed\n";
		os.Display_running();
		return NORMAL;
	}
	//执行指令rel
	int Rel() {
		if (init == NULL)return NOT_INIT;
		if (!param_num(command::rel, 2, 2))return WRONG_PARAMS;
		if (!is_all_digit(command::rel, params.back()))return WRONG_PARAMS;
		if (os.Release(os.rid2r(params[0]), str2int(params.back()))) {
			std::cout << "* Resource has been released.\n";
		}
		else
			std::cout << "* Resource releas failed\n";
		os.Display_running();
		return NORMAL;
	}
	//打印帮助的函数type=command::all则打印所有指令的简单说明否则打印相应指令的所有说明，参数paramnum=1则只打印指令参数数量的说明,paramtype=1则只打印参数含义的说明
	void Help(enum class command type=command::all,int paramnum=0,int paramtype=0) {
		if (type == command::all) {
			for (auto iter = commandbook.begin(); iter != commandbook.end(); ++iter)
				std::cout << iter->second[0];
		}
		else if(paramnum==0&&paramtype==0) {
			for (auto iter = commandbook[type].begin(); iter != commandbook[type].end(); ++iter)
				std::cout << *iter;
		}
		else if (paramnum == 1) std::cout << commandbook[type][1];
		else if (paramtype == 1)std::cout << commandbook[type][2];
	}
	//解析并完成指令
	int Command_Interpreter(std::string& str) {
		params.clear();
		switch (Parse(str))
		{
		case command::cr: {
			return Cr();
			break;
		}
		case command::de: {
			return De();
			break;
		}
		case command::ps: {
			return List();
			break;
		}
		case command::req: {
			return Req();
			break;
		}
		case command::rel: {
			return Rel();
			break;
		}
		case command::to: {
			if (init == NULL)return NOT_INIT;
			if (!param_num(command::to))return WRONG_PARAMS;
			if(!os.time_out(true))os.Display_running();
			break;
		}
		case command::help: {
			if (!param_num(command::help, 1))return WRONG_PARAMS;
			if (params.empty()) {
				Help();
			}
			else {
				bool find = false;
				for (auto iter = commandlist.begin(); iter != commandlist.end(); ++iter)
					if (iter->first == params[0]) {
						Help(iter->second);
						find = true;
					}
				if (!find) {
					std::cout << "* shell: " << params[0] << ": command not found!\n";
					Help(command::help, 0, 1);
				}
			}
			break;
		}
		case command::init: {
			if (!param_num(command::init))return WRONG_PARAMS;
			if (init == NULL)init = os.Create("init", 0);
			else std::cout << "*already initializate\n";
			if (init != NULL)std::cout << "* init process is running\n";
			else std::cout << "* initialization filed\n";
			break;
		}
		case command::exit: {
			if (!param_num(command::exit))return WRONG_PARAMS;
			if (init != NULL)os.Destory(init);
			return EXIT;
		}
		default:
			return UNKNOWN_CMD;
			break;
		}
		return NORMAL;
	}
public:
	void run() {
		std::string input;
		bool running = true;
		std::cout << "* use 'init' for initialization, use 'help' for help\n";
		while (running)
		{
			input = "";
			std::cout << "shell>>";
			getline(std::cin, input);
			input += " ";//在最后添加一个空格，保证字符串最后一定是以“xxx ”为循环节的循环体
			for (int pos = 0; (pos = input.find("  ", pos)) != -1; input.erase(pos, 1));//删除多余空格
			if (input == "" || input ==" ")//空输入则跳过
				continue;
			if (input[0] == ' ')
				input.erase(0, 1);//如果头部有空格则删除
			if (input.length() > 20) {//正常指令不会这么长
				std::cout << "X line too long!\n";
				continue;
			}
			switch (Command_Interpreter(input)) {
			case EXIT: {
				running = false;
				std::cout << "Goodbye!";
				break;
			}
			case UNKNOWN_CMD: {
				std::cout << "? use 'help' list all command\n";
				break;
			}
			case WRONG_PARAMS: {
				std::cout << "? use 'help <command name>' for more information of this command\n";
				break;
			}
			case NOT_INIT: {
				std::cout << "X this command can't use before init\n"
					<<"* use 'init' for initialization, use 'help' for help\n";
				break;
			}
			default:
				break;
			}
		}
	}
};
