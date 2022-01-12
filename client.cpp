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
		cout << "����ʧ��" << endl;
		return -1;
	}	
	cout << "�Ѿ����ӵ��������������������������Ϣ�ˣ�" << endl;
	
	int rbyte = 0;
	char sbuf[MAXBUFSIZE];
	char rbuf[MAXBUFSIZE];
	sbuf[0] = 1;
	char req = 0;
	int online = 0;
	char username[20];
	char password[20];
	
	while(1){
		cout << "��������������������������������" << endl;
		if(online == 0){
			cout << "��ǰ�û�δ��¼������������" << endl;
			cout <<  "1-ע�ᣬ2-��¼��e-�˳�" << endl;
			cin >> req;
			if(req == '1'){
				sbuf[1] = 1;			
				for(int i = 0; i < 20; i++){
					username[i] = '\0';
					password[i] = '\0';
				}
				cout << "��ע�᡿" << endl;
				cout << "�������û�����������20λ" << endl;
				cin >> username;
				cout << "���������룬������20λ" << endl;
				cin >> password;
				memcpy(&sbuf[2], username, 20);
				memcpy(&sbuf[22], password, 20);
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 1){
					if(rbuf[2] == 1){
						cout << "ע��ɹ��������û����ǣ�" << username << endl;
						int randpet;
						cout << "�������3ֻ�¾��飺";
						for(int i = 0; i < 3; i++){
							randpet = rbuf[3 + i];
							pet[randpet].showname(); 
						} 
						cout << endl;
					}
					else if(rbuf[2] == 0){
						cout << "��Ǹ���û����ѱ�ռ�ã�ע��ʧ�ܣ�" << endl;
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
				cout << "����¼��" << endl;
				cout << "�������û�����������20λ" << endl;
				cin >> username;
				cout << "���������룬������20λ" << endl;
				cin >> password; 
				memcpy(&sbuf[2], username, 20);
				memcpy(&sbuf[22], password, 20);
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 2){
					if(rbuf[2] == 0){
						cout << "�û��������ڣ���¼ʧ�ܣ�" << endl;
					}
					else if(rbuf[2] == 1){
						cout << "������󣬵�¼ʧ�ܣ�" << endl;
					}
					else if(rbuf[2] == 3){
						cout << "���û��Ѿ����ߣ���¼ʧ�ܣ�" << endl;
					}
					else if(rbuf[2] == 2){
						online = 1;
						cout << "��¼�ɹ������ͻ��˵�¼���û��ǣ�" << username << endl;
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
			cout << "��ǰ��¼�û���" << username << endl;
			cout <<  "3-�ǳ���4-�鿴�����û��ľ��顢ʤ�ʡ�����" << endl;
			cout << "5-�鿴���������û���6-�������" << endl; 
			cin >> req;
			if(req == '3'){
				sbuf[1] = 3;
				memcpy(&sbuf[2], username, 20);
				send(SocketClient, sbuf, sizeof(sbuf), 0);
				rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
				if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 3){
					if(rbuf[2] == 1){
						online = 0;
						cout << "�û�" << username << " �ǳ��ɹ���"<< endl;
					}
					else if(rbuf[2] == 0){
						cout << "�û�" << username << " δ��¼���ǳ�ʧ�ܣ�" << endl;
					}
					else if(rbuf[2] == 2){
						cout << "error:�û�" << username << " δע�ᣬ�ǳ�ʧ�ܣ�" << endl;
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
						cout << "��ע���û���" << tempusn << endl;
						user_pet_tot = rbuf[j];
						j++;
						cout << "���鹲 " << user_pet_tot << " ֻ��" << endl;
						for(int k = 0; k < user_pet_tot; k++){
							petnum = rbuf[j];
							j++;
							pet[petnum].showname();
							templv = rbuf[j];
							cout << "�ȼ���" << templv << endl;
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
						cout << "ʤ�ʣ�" << tempwinrate * 100 << "%��" << tempwin << "/" << templose << "��" << endl;
						cout << "����������£�" ;
						if(tempbadge == 0){
							cout << "δ���" << endl; 
						}
						else if(tempbadge == 1){
							cout << "ͭ" << endl; 
						}
						else if(tempbadge == 2){
							cout << "��" << endl; 
						}
						else if(tempbadge == 3){
							cout << "��" << endl; 
						}
						cout << "�߼�������£�" ;
						if(tempbadge15 == 0){
							cout << "δ���" << endl; 
						}
						else if(tempbadge15 == 1){
							cout << "ͭ" << endl; 
						}
						else if(tempbadge15 == 2){
							cout << "��" << endl; 
						}
						else if(tempbadge15 == 3){
							cout << "��" << endl; 
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
					cout << "��ǰ�� " << temp_online_user << " ���û����ߣ��û�����" << endl;
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
					cout << "�û����鹲 " << user_pet_tot << " ֻ��" << endl; 
					j++;
					for(int i = 0; i < user_pet_tot; i++){
						cout << i << " - ";
						//cout << "��ţ�" << rbuf[j];
						cout << "���ƣ�";
						pet[rbuf[j]].showname();
						j++;
						cout << "�ȼ���" << (int)rbuf[j] << endl;
						j++;
					}
					sys_pet_tot = rbuf[j];
					j++;
					cout << "ϵͳ���⾫�鹲 " << sys_pet_tot << " ֻ��" << endl;
					for(int i = 0; i < sys_pet_tot; i++){
						cout << i << " - ";
						//cout << "��ţ�" << rbuf[j];
						cout << "���ƣ�";
						pet[rbuf[j]].showname();
						j++;
						cout << "�ȼ���" << (int)rbuf[j] << endl;
						j++;
					}				
					cout << "��ѡ��������ͣ�1-��������2-������" << endl;
					cin >> battle_type;
					while(battle_type != 1 && battle_type != 2){
						cout << "���벻�Ϸ�������������" << endl;
						cin >> battle_type;
					}
					cout << "��ѡ��һֻ��ս���飬���������" << endl;
					cin >> user_choose;
					while(user_choose < 0 || user_choose >= user_pet_tot){
						cout << "���벻�Ϸ�������������" << endl;
						cin >> user_choose;
					}
					cout << "��ѡ����ֵĳ�ս���飬���������" << endl;
					cin >> sys_choose;
					while(sys_choose < 0 || sys_choose >= sys_pet_tot){
						cout << "���벻�Ϸ�������������" << endl;
						cin >> sys_choose;
					}
					Battle bat(rbuf[3 + 2 * user_choose], rbuf[4 + 2 * user_choose], rbuf[4 + 2 * user_pet_tot + 2 * sys_choose], rbuf[5 + 2 * user_pet_tot + 2 * sys_choose]);
					battle_win = bat.battle();
					sbuf[2] = battle_type;
					sbuf[3] = user_choose;
					sbuf[4] = sys_choose;
					sbuf[5] = battle_win;
					send(SocketClient, sbuf, sizeof(sbuf), 0);					
					if(battle_type == 1 && battle_win == 1){//������ʤ���Ӿ��� 
						rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
						if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
							pet[rbuf[3]].showname();
							cout << "����˾���ֵ��" << pet[sys_choose].getinfo(2) * 20 << endl; 
							int dlv = rbuf[9];
							if(dlv > 0){
								pet[rbuf[3]].showname();
								cout << "�����ˣ�" << endl;
								cout << "�ȼ���" << (int)rbuf[4] << " + " << rbuf[11] - rbuf[4] << " -> " << (int)rbuf[11] << endl;
								cout << "������" << (int)rbuf[5] << " + " << rbuf[12] - rbuf[5] << " -> " << (int)rbuf[12] << endl;
								cout << "������" << (int)rbuf[6] << " + " << rbuf[13] - rbuf[6] << " -> " << (int)rbuf[13] << endl;
								cout << "���ݣ�" << (int)rbuf[7] << " + " << rbuf[14] - rbuf[7] << " -> " << (int)rbuf[14] << endl;
								cout << "������" << (int)rbuf[8] << " + " << rbuf[15] - rbuf[8] << " -> " << (int)rbuf[15] << endl;
							}
						}
						
					}
					else if(battle_type == 1 && battle_win == 2){//�������������·��� 
						cout << "����δ��þ���ֵ" << endl; 
					}
					else if(battle_type == 2 && battle_win == 1){//������ʤ����öԷ����� 
						rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
						if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
							cout << "������˶��ֵľ��飺";
							pet[rbuf[2]].showname();
							cout << "���ȼ���" << (int)rbuf[3] << endl; 
						}
					}
					else if(battle_type == 2 && battle_win == 2){//�����������Լ�����ֻ��������У�ѡ��һֻʧȥ
						rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
						if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
							int lose_max = rbuf[2];
							cout << "�����ͳ����� " << lose_max << " ֻ�����е�һֻ����ѡ��" << endl;
							for(int i = 0; i < lose_max; i++){
								cout << i << " - " ;
								pet[rbuf[3 + 2 * i]].showname();
								cout << "�ȼ���" << (int)rbuf[4 + 2 * i] << endl;;
							}
							user_choose = -1;
							cin >> user_choose;
							while(user_choose < 0 || user_choose >= user_pet_tot){
								cout << "���벻�Ϸ�������������" << endl;
								cin >> user_choose;
							}
							sbuf[2] = user_choose;
							send(SocketClient, sbuf, sizeof(sbuf), 0);	
							rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
							if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
								int lose = rbuf[2];
								cout << "��ʧȥ�˾��飺";
								pet[rbuf[3]].showname();
								cout << endl;
								if(rbuf[5] >= 0){
									cout << "������ʧȥ�����еľ��飬ϵͳ�Ѹ������� 1 ֻ 1 �����飺" << endl;
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
	cout << "�ͻ������˳���" << endl; 
	
	return 0;
}
