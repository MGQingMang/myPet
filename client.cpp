/*Client*/ 

#include <iostream>
#include <winsock2.h>
#include "class.h"
#include <iomanip>
using namespace std;

const int PORT = 8000;
int pet_max = 0;

int main(){
	ifstream fp("input.txt",ios::binary);
	fp >> pet_max;
	fp.close();
	Pet pet[pet_max];
	for(int i = 0; i < pet_max; i++){
		pet[i].init_p(i, i - 1);
		//pet[i].lvup(i - 1, 0);
	}
	
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	SOCKET SocketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	SOCKADDR_IN  ClientAddr;

	ClientAddr.sin_family = AF_INET;
	ClientAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	ClientAddr.sin_port = htons(PORT);
	int n = 0;
	n = connect(SocketClient, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr));
	if (n == SOCKET_ERROR) {
		cout << "连接失败" << endl;
		return -1;
	}	
	cout << "已经连接到服务器，可以向服务器发送消息了！" << endl;
	
	int rbyte = 0;
	char sbuf[MAXBUFSIZE];
	char rbuf[MAXBUFSIZE];
	sbuf[0] = 1;
	char req = 0;
	int online = 0;
	char username[20];
	char password[20];
	
	while(1){
		cout << "————————————————" << endl;
		if(online == 0){
			cout << "当前用户未登录，请输入请求：" << endl;
			cout <<  "1-注册，2-登录，e-退出" << endl;
			cin >> req;
			if(req == '1'){
				sbuf[1] = 1;			
				for(int i = 0; i < 20; i++){
					username[i] = '\0';
					password[i] = '\0';
				}
				cout << "【注册】" << endl;
				cout << "请输入用户名，不超过20位" << endl;
				cin >> username;
				cout << "请输入密码，不超过20位" << endl;
				cin >> password;
				memcpy(&sbuf[2], username, 20);
				memcpy(&sbuf[22], password, 20);
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 1){
					if(rbuf[2] == 1){
						cout << "注册成功！您的用户名是：" << username << endl;
						int randpet;
						cout << "您获得了3只新精灵：";
						for(int i = 0; i < 3; i++){
							randpet = rbuf[3 + i];
							pet[randpet].showname(); 
						} 
						cout << endl;
					}
					else if(rbuf[2] == 0){
						cout << "抱歉，用户名已被占用，注册失败！" << endl;
					}
				}
				else{
					cout << "error" << endl; 
					cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
				}
			}
			else if(req == '2'){
				sbuf[1] = 2;
				for(int i = 0; i < 20; i++){
					username[i] = '\0';
					password[i] = '\0';
				}
				cout << "【登录】" << endl;
				cout << "请输入用户名，不超过20位" << endl;
				cin >> username;
				cout << "请输入密码，不超过20位" << endl;
				cin >> password; 
				memcpy(&sbuf[2], username, 20);
				memcpy(&sbuf[22], password, 20);
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 2){
					if(rbuf[2] == 0){
						cout << "用户名不存在，登录失败！" << endl;
					}
					else if(rbuf[2] == 1){
						cout << "密码错误，登录失败！" << endl;
					}
					else if(rbuf[2] == 3){
						cout << "该用户已经在线，登录失败！" << endl;
					}
					else if(rbuf[2] == 2){
						online = 1;
						cout << "登录成功，本客户端登录的用户是：" << username << endl;
					}
				}
				else{
					cout << "error" << endl; 
					cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
				}
			}
			else if(req == 'e'){
				break;
			}	
			else{
				cout << "error:invalid type of request!" << endl; 
			}	
		}
		else if(online == 1){
			cout << "当前登录用户：" << username << endl;
			cout <<  "3-登出，4-查看所有用户的精灵、胜率、徽章" << endl;
			cout << "5-查看所有在线用户，6-虚拟决斗" << endl; 
			cin >> req;
			if(req == '3'){
				sbuf[1] = 3;
				memcpy(&sbuf[2], username, 20);
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 3){
					if(rbuf[2] == 1){
						online = 0;
						cout << "用户" << username << " 登出成功！"<< endl;
					}
					else if(rbuf[2] == 0){
						cout << "用户" << username << " 未登录，登出失败！" << endl;
					}
					else if(rbuf[2] == 2){
						cout << "error:用户" << username << " 未注册，登出失败！" << endl;
					}
				}
				else{
					cout << "error" << endl; 
					cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
				}
			} 
			else if(req == '4'){
				sbuf[1] = 4;
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 4){
					int user_tot = rbuf[2];
					int user_pet_tot = 0, j = 3, petnum, templv, tempwin = 0, templose = 0, tempbadge = 0, tempbadge15 = 0;
					float tempwinrate;
					char tempusn[20];
					for(int i = 0; i < user_tot; i++){
						memcpy(tempusn, &rbuf[j], 20);
						j += 20;
						cout << "已注册用户：" << tempusn << endl;
						user_pet_tot = rbuf[j];
						j++;
						cout << "精灵共 " << user_pet_tot << " 只：" << endl;
						for(int k = 0; k < user_pet_tot; k++){
							petnum = rbuf[j];
							j++;
							pet[petnum].showname();
							templv = rbuf[j];
							cout << "等级：" << templv << endl;
							j++;
						}
						tempwin = rbuf[j];
						j++;
						templose = rbuf[j];
						j++;
						tempbadge = rbuf[j];
						j++;
						tempbadge15 = rbuf[j];
						j++;						
						if(tempwin + templose == 0){
							tempwinrate = 0;
						}
						else{
							tempwinrate = ((float)tempwin / (tempwin + templose));
						}
						cout << "胜率：" << tempwinrate * 100 << "%（" << tempwin << "/" << templose << "）" << endl;
						cout << "宠物个数徽章：" ;
						if(tempbadge == 0){
							cout << "未获得" << endl; 
						}
						else if(tempbadge == 1){
							cout << "铜" << endl; 
						}
						else if(tempbadge == 2){
							cout << "银" << endl; 
						}
						else if(tempbadge == 3){
							cout << "金" << endl; 
						}
						cout << "高级宠物徽章：" ;
						if(tempbadge15 == 0){
							cout << "未获得" << endl; 
						}
						else if(tempbadge15 == 1){
							cout << "铜" << endl; 
						}
						else if(tempbadge15 == 2){
							cout << "银" << endl; 
						}
						else if(tempbadge15 == 3){
							cout << "金" << endl; 
						}
						cout << endl;
					}
				}
				else{
					cout << "error" << endl; 
					cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
				}
			}
			else if(req == '5'){
				sbuf[1] = 5;
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 5){
					int temp_online_user = rbuf[2];
					char tempusn[20];
					cout << "当前共 " << temp_online_user << " 名用户在线，用户名：" << endl;
					int j = 3;
					for(int i = 0; i < temp_online_user; i++){
						memcpy(tempusn, &rbuf[j], 20);
						cout << tempusn << endl;
						j += 20;
					}
				}
				else{
					cout << "error" << endl; 
					cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
				} 
			}
			else if(req == '6'){
				sbuf[1] = 6;
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
					int user_pet_tot = 0, sys_pet_tot = 0;
					int battle_type = 0, user_choose = -1, sys_choose = -1, battle_win = 0;
					int j = 2;
					user_pet_tot = rbuf[j];
					cout << "用户精灵共 " << user_pet_tot << " 只：" << endl; 
					j++;
					for(int i = 0; i < user_pet_tot; i++){
						cout << i << " - ";
						//cout << "序号：" << rbuf[j];
						cout << "名称：";
						pet[rbuf[j]].showname();
						j++;
						cout << "等级：" << (int)rbuf[j] << endl;
						j++;
					}
					sys_pet_tot = rbuf[j];
					j++;
					cout << "系统虚拟精灵共 " << sys_pet_tot << " 只：" << endl;
					for(int i = 0; i < sys_pet_tot; i++){
						cout << i << " - ";
						//cout << "序号：" << rbuf[j];
						cout << "名称：";
						pet[rbuf[j]].showname();
						j++;
						cout << "等级：" << (int)rbuf[j] << endl;
						j++;
					}				
					cout << "请选择比赛类型：1-升级赛，2-决斗赛" << endl;
					cin >> battle_type;
					while(battle_type != 1 && battle_type != 2){
						cout << "输入不合法！请重新输入" << endl;
						cin >> battle_type;
					}
					cout << "请选择一只出战精灵，输入其序号" << endl;
					cin >> user_choose;
					while(user_choose < 0 || user_choose >= user_pet_tot){
						cout << "输入不合法！请重新输入" << endl;
						cin >> user_choose;
					}
					cout << "请选择对手的出战精灵，输入其序号" << endl;
					cin >> sys_choose;
					while(sys_choose < 0 || sys_choose >= sys_pet_tot){
						cout << "输入不合法！请重新输入" << endl;
						cin >> sys_choose;
					}
					Battle bat(rbuf[3 + 2 * user_choose], rbuf[4 + 2 * user_choose], rbuf[4 + 2 * user_pet_tot + 2 * sys_choose], rbuf[5 + 2 * user_pet_tot + 2 * sys_choose]);
					battle_win = bat.battle();
					sbuf[2] = battle_type;
					sbuf[3] = user_choose;
					sbuf[4] = sys_choose;
					sbuf[5] = battle_win;
					send(SocketClient, sbuf, sizeof(sbuf), 0);					
					if(battle_type == 1 && battle_win == 1){//升级赛胜：加经验 
						rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
						if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
							pet[rbuf[3]].showname();
							cout << "获得了经验值：" << pet[sys_choose].getinfo(2) * 20 << endl; 
							int dlv = rbuf[9];
							if(dlv > 0){
								pet[rbuf[3]].showname();
								cout << "升级了！" << endl;
								cout << "等级：" << (int)rbuf[4] << " + " << rbuf[11] - rbuf[4] << " -> " << (int)rbuf[11] << endl;
								cout << "攻击：" << (int)rbuf[5] << " + " << rbuf[12] - rbuf[5] << " -> " << (int)rbuf[12] << endl;
								cout << "防御：" << (int)rbuf[6] << " + " << rbuf[13] - rbuf[6] << " -> " << (int)rbuf[13] << endl;
								cout << "敏捷：" << (int)rbuf[7] << " + " << rbuf[14] - rbuf[7] << " -> " << (int)rbuf[14] << endl;
								cout << "生命：" << (int)rbuf[8] << " + " << rbuf[15] - rbuf[8] << " -> " << (int)rbuf[15] << endl;
							}
						}
						
					}
					else if(battle_type == 1 && battle_win == 2){//升级赛负：无事发生 
						cout << "精灵未获得经验值" << endl; 
					}
					else if(battle_type == 2 && battle_win == 1){//决斗赛胜：获得对方精灵 
						rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
						if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
							cout << "您获得了对手的精灵：";
							pet[rbuf[2]].showname();
							cout << "，等级：" << (int)rbuf[3] << endl; 
						}
					}
					else if(battle_type == 2 && battle_win == 2){//决斗赛负：自己的三只随机精灵中，选择一只失去
						rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
						if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
							int lose_max = rbuf[2];
							cout << "您需送出以下 " << lose_max << " 只精灵中的一只，请选择：" << endl;
							for(int i = 0; i < lose_max; i++){
								cout << i << " - " ;
								pet[rbuf[3 + 2 * i]].showname();
								cout << "等级：" << (int)rbuf[4 + 2 * i] << endl;;
							}
							user_choose = -1;
							cin >> user_choose;
							while(user_choose < 0 || user_choose >= user_pet_tot){
								cout << "输入不合法！请重新输入" << endl;
								cin >> user_choose;
							}
							sbuf[2] = user_choose;
							send(SocketClient, sbuf, sizeof(sbuf), 0);	
							rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
							if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
								int lose = rbuf[2];
								cout << "您失去了精灵：";
								pet[rbuf[3]].showname();
								cout << endl;
								if(rbuf[5] >= 0){
									cout << "由于您失去了所有的精灵，系统已给您发放 1 只 1 级精灵：" << endl;
									pet[rbuf[5]].showname(); 
									cout << endl;
								}
							}
						} 
					}
				}
				else{
					cout << "error" << endl; 
					cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
				}
			}
			/*else if(req == 'e'){
				break;
			}*/
		}
	}
	
	closesocket(SocketClient);
	WSACleanup();
	cout << "客户端已退出！" << endl; 
	
	return 0;
}
