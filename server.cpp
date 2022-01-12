/*Server*/ 

#include <iostream>
#include <winsock2.h>
#include "class.h"
#include <cstdlib>
#include <ctime>
#include <iomanip>
using namespace std;

Users user[256];
DWORD WINAPI ServerThread(LPVOID lpParameter);
const int PORT = 8000;
int user_max = 0;			/*��ע���û�����*/ 
int lock = 0; 			
int pet_max = 0;			
int user_online = 0;		/*��ǰ�����û���*/ 

class Func{
	public:
		void mullock();	
		void unlock();
		void sort(char*, int);
		int findusernum(char*);		/*�����û������������*/ 
};
Func func;


int main(){
	ifstream fp("input.txt",ios::binary);
	fp >> pet_max;
	fp.close();
	
	WSADATA wsd;
	WSAStartup(MAKEWORD(2, 2), &wsd);
	
	SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN ListenAddr;
	ListenAddr.sin_family = AF_INET;
	ListenAddr.sin_addr.S_un.S_addr = INADDR_ANY;
	ListenAddr.sin_port = htons(PORT);
	
	int n;
	n = bind(ListenSocket, (LPSOCKADDR)&ListenAddr, sizeof(ListenAddr));
	if(n == SOCKET_ERROR){
		cout << "�˿ڰ�ʧ�ܣ�" << endl;
		return -1;
	}
	else{
		cout << "�˿ڰ󶨳ɹ���" << PORT << endl;
	}
	
	int l = listen(ListenSocket, 20);
	cout << "������׼���������ȴ���������" << endl;
	
	while(1){
		SOCKET* ClientSocket = new SOCKET;
		ClientSocket = (SOCKET*)malloc(sizeof(SOCKET));
		int SockAddlen = sizeof(sockaddr);
		*ClientSocket = accept(ListenSocket, 0, 0);
		cout << "һ���ͻ��������ӵ���������socket�ǣ�" << *ClientSocket << endl;
		CreateThread(NULL, 0, &ServerThread, ClientSocket, 0, NULL);		 
	}
	
	closesocket(ListenSocket);
	WSACleanup();
	
	return 0; 
} 

DWORD WINAPI ServerThread(LPVOID lpParameter){
	SOCKET* ClientSocket = (SOCKET*)lpParameter;
	
	Pet pet[pet_max];
	for(int i = 0; i < pet_max; i++){
		pet[i].init_p(i, i - 1);
		//pet[i].lvup(i - 1, 0);
	}
	int rbyte = 0;
	char sbuf[MAXBUFSIZE];
	char rbuf[MAXBUFSIZE];
	sbuf[0] = 2;
	char usn[20];
	char psw[20];
	int taken = -1; 
	int s_online = 0;
	char online_usn[20]; 
	for(int i = 0; i < 20; i++){
		online_usn[i] = '\0';
	}
	int online_usernum = -1;
	
	while(1){
		rbyte = recv(*ClientSocket, rbuf, sizeof(rbuf),0);
		cout << "��������������������������������" << endl;
		cout << "�ͻ��ˣ�"  << *ClientSocket << endl;
		cout << "״̬��";
		if(s_online == 1){
			cout << "���ߣ������û���" << online_usn << "����ţ�" << online_usernum << endl;
		} 
		else{
			cout << "����" << endl;
		}
		if(rbyte > 0){
			if(rbuf[0] == 1){
				for(int i = 0; i < 20; i++){
					usn[i] = '\0';
					psw[i] = '\0';
				}
				taken = -1;
				if(rbuf[1] == 1){//���յ�ע������ 
					sbuf[1] = 1;
					memcpy(usn, &rbuf[2], 20);
					memcpy(psw, &rbuf[22], 20);				
					cout << "����ע�ᣬ�û�����" << usn << "�����룺" << psw << endl;
					taken = func.findusernum(usn);
					if(taken == -1){//�û������ظ���ע��ɹ� 
						func.mullock();
						user[user_max].signin(usn, psw);
						func.unlock();					
						sbuf[2] = 1;
						cout << "ע��ɹ����û���ţ�" << user_max << endl;
						srand(time(NULL));
						int randpet;
						cout << "�������3ֻ�¾��飺"; 
						for(int i = 0; i < 3; i++){
							randpet = rand() % pet_max;
							user[user_max].newpet(randpet, 1);
							sbuf[3 + i] = randpet;
							pet[randpet].showname();
							cout << "��" << randpet << "����" ;
						}
						cout << endl;
						user_max++;
					}
					else if(taken >= 0){//�û����ظ���ע��ʧ�� 
						sbuf[2] = 0;
						cout << "ע��ʧ�ܣ����û�" << taken << "���û����ظ�" << endl;
					}				 
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 2){//���յ���¼���� 
					sbuf[1] = 2;
					memcpy(usn, &rbuf[2], 20);
					memcpy(psw, &rbuf[22], 20);
					cout << "�����¼��������û�����" << usn << "�����룺" << psw << endl;
					taken = func.findusernum(usn);
					if(taken == -1){//�Ҳ����û�������¼ʧ�� 					
						sbuf[2] = 0;
						cout << "���û���δע�ᣬ��¼ʧ��" << endl;
					}
					else if(taken >= 0){//�ҵ��û���
						func.mullock(); 
						if(user[taken].usntaken(usn) == 1 && user[taken].pswtaken(psw) == 1){
							if(user[taken].checkstatus() == 0){
								user[taken].log(1); 
								sbuf[2] = 2;
								sbuf[3] = taken;
								user_online++;							
								cout << "��¼�ɹ����û���ţ�" << taken << endl;
								s_online = 1;
								online_usernum = taken;
								memcpy(online_usn, usn, 20);
							}
							else if(user[taken].checkstatus() == 1){
								sbuf[2] = 3;
								cout << "��¼ʧ�ܣ����û��Ѿ����ߣ��û���ţ�" << taken << endl;
							}
						}
						else if(user[taken].usntaken(usn) == 1 && user[taken].pswtaken(psw) != 1){
							sbuf[2] = 1;
							cout << "������󣬵�¼ʧ�ܣ��û���ţ�" << taken << endl;
						}
						func.unlock();
					}				 
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 3){//���յ��ǳ�����
					sbuf[1] = 3;				 
					memcpy(usn, &rbuf[2], 20);
					taken = func.findusernum(usn);
					cout << "����ǳ����û�����" << usn << "���û���ţ�" << taken << endl;
					func.mullock();
					if(user[taken].checkstatus() == -1){
						sbuf[2] = 2;
						cout << "error:���û�δע��" << endl; 
					}
					else if(user[taken].checkstatus() == 0){
						sbuf[2] = 0;
						cout << "�ǳ�ʧ�ܣ����û�������" << endl;
					}
					else if(user[taken].checkstatus() == 1){
						sbuf[2] = 1;
						user[taken].log(0);
						user_online--;
						cout << "�ǳ��ɹ�" << endl;
						s_online = 0;
						for(int i = 0; i < 20; i++){
							online_usn[i] = '\0';
						}
					}
					func.unlock();
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 4){
					sbuf[1] = 4;
					cout << "����鿴��ע���û������о���" << endl;
					cout << "��ѯ�����" << endl;
					sbuf[2] = user_max;
					cout << "��ע���û��� " << user_max << "λ��" << endl;
					int j = 3;
					func.mullock();
					for(int i = 0; i < user_max; i++){
						int offset = 0,templv = 0, tempwin = 0, templose = 0, tempbadge = 0, tempbadge15 = 0;
						float tempwinrate = 0;
						user[i].getusername(usn);
						memcpy(&sbuf[j], usn, 20);
						cout << "�û�" << i << "���û�����" << usn << endl; 
						j += 20;
						offset = user[i].getpet(&sbuf[j + 1]);
						sbuf[j] = offset;
						j++;
						cout << "���鹲 " << offset << " ֻ��" << endl;

						for(int k = 0; k < offset; k++){
							pet[sbuf[j + k * 2]].showname();
							templv = sbuf[j + k * 2 + 1];
							cout << "�ȼ���" << templv << endl;
						} 
						j += offset * 2;
						tempwin = user[i].getuserinfo(1);
						sbuf[j] = tempwin;
						j++;
						templose = user[i].getuserinfo(2);
						sbuf[j] = templose;
						j++;
						if(tempwin + templose == 0){
							tempwinrate = 0;
						}
						else{
							tempwinrate = ((float)tempwin / (tempwin + templose));
						}
						cout << "ʤ�ʣ�" << tempwinrate * 100 << "%��" << tempwin << "/" << templose << "��" << endl;
						tempbadge =  user[i].getuserinfo(3);
						sbuf[j] = tempbadge;
						j++;
						tempbadge15 = user[i].getuserinfo(4);
						sbuf[j] = tempbadge15;
						j++;
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
					func.unlock();
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 5){
					sbuf[1] = 5;
					cout << "����鿴��ǰ�����û�" << endl;
					cout << "��ѯ�������ǰ�� " << user_online << " ���û�����" << endl;
					sbuf[2] = user_online;
					int j = 3;
					func.mullock();
					for(int i = 0; i < user_max; i++){
						if(user[i].checkstatus() == 1){
							user[i].getusername(usn);
							memcpy(&sbuf[j], usn, 20);
							cout << "�û�" << i << "���û�����" << usn <<endl; 
							j += 20;
						}
					} 
					func.unlock();
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
				}
				else if(rbuf[1] == 6){
					sbuf[1] = 6;
					cout << "�����ս" << endl; 
					int user_pet_tot = user[online_usernum].getpet(&sbuf[3]);
					sbuf[2] = user_pet_tot;
					int j = 3;
					cout << "�û����鹲 " << user_pet_tot << " ֻ��" << endl; 
					for(int i = 0; i < user_pet_tot; i++){
						cout << i << " - ";
						cout << "��ţ�" << pet[sbuf[j]].getinfo(1);
						cout << "���ƣ�";
						pet[sbuf[j]].showname();
						j++;
						cout << "�ȼ���" << pet[sbuf[j]].getinfo(2) << endl;
						j++;
					}
					cout << "ϵͳ���⾫�鹲 " << pet_max << " ֻ��" << endl;
					sbuf[j] = pet_max;
					j++;
					for(int i = 0; i < pet_max; i++){
						cout << i << " - ";
						sbuf[j] = pet[i].getinfo(1);
						cout << "��ţ�" << pet[sbuf[j]].getinfo(1);
						cout << "���ƣ�";
						pet[sbuf[j]].showname();
						j++;
						sbuf[j] = pet[i].getinfo(2);
						cout << "�ȼ���" << pet[i].getinfo(2) << endl;
						j++;
					}
					send(*ClientSocket, sbuf, sizeof(sbuf), 0);
					
					rbyte = recv(*ClientSocket, rbuf, sizeof(rbuf),0);
					int battle_type = 0, user_choose = -1, sys_choose = -1, battle_win = 0;
					battle_type = rbuf[2];
					user_choose = rbuf[3];
					sys_choose = rbuf[4];
					battle_win = rbuf[5];
					cout << "�û�ѡ��Ķ�ս���ͣ�";
					if(battle_type == 1){
						cout << "������" << endl;
					} 
					else if(battle_type == 2){
						cout << "������" << endl;
					}
					cout << "�û���ս���飺" << user_choose << endl;
					cout << "ϵͳ��ս���飺" << sys_choose << endl;
					cout << "��ս�����";
					if(battle_win == 1){//ʤ�� 
						cout << "�û�ʤ��" << endl; 
						user[online_usernum].userwin(1);
						if(battle_type == 1){//������ʤ�� 
							cout << "��þ���ֵ��" << pet[sys_choose].getinfo(2) * 20 << endl;
							sbuf[2] = user_choose;
							user[online_usernum].getpet_one(&sbuf[3], user_choose);
							int dlv = user[online_usernum].addexp_pet(user_choose, pet[sys_choose].getinfo(2) * 20);							
							sbuf[9] = dlv;
							user[online_usernum].getpet_one(&sbuf[10], user_choose);
							send(*ClientSocket, sbuf, sizeof(sbuf), 0);
						}
						else if(battle_type = 2){//������ʤ�� 
							int newpet_id = pet[sys_choose].getinfo(1);
							int newpet_lv = pet[sys_choose].getinfo(2);
							user[online_usernum].newpet(newpet_id, newpet_lv);
							sbuf[2] = newpet_id;
							sbuf[3] = newpet_lv;
							cout << "�û�����¾��飺" ;
							pet[newpet_id].showname();
							cout << "���ȼ���" << newpet_lv << endl; 
							send(*ClientSocket, sbuf, sizeof(sbuf), 0);
						}
					} 
					else if(battle_win == 2){//ʧ�� 
						cout << "�û�ʧ��" << endl;
						user[online_usernum].userwin(2); 
						if(battle_type == 2){
							int lose_max = 3;
							
							srand(time(NULL));
							if(user_pet_tot < 3){
								lose_max = user_pet_tot;
							}
							char losepet[lose_max];
							srand(time(NULL));
							for(int i = 0; i < lose_max; i++){
								losepet[i] = rand() % user_pet_tot;
								for(int j = 0; j < i; j++){
									if(losepet[j] == losepet[i]){
										i--;
										break;
									}
								}
							}
							func.sort(losepet, lose_max);
							sbuf[2] = lose_max;
							cout << "�û����ͳ� " << lose_max << " ֻ�����е�һֻ����ţ�" ;
							for(int i = 0; i < lose_max; i++){
								user[online_usernum].getpet_one(&sbuf[3 + 2 * i], losepet[i]);
								cout << (int)losepet[i] << " ";
							}							 
							cout << endl;
							send(*ClientSocket, sbuf, sizeof(sbuf), 0);
							rbyte = recv(*ClientSocket, rbuf, sizeof(rbuf),0);
							int lose = losepet[rbuf[2]];
							cout << "�û�ѡ��ʧȥ�������ţ�" << lose << endl;
							sbuf[2] = lose;
							user[online_usernum].getpet_one(&sbuf[3], lose);
							user[online_usernum].losepet(lose);
							sbuf[5] = -1;
							if(lose_max <= 1){
								srand(time(NULL));
								int randpet = rand() % pet_max;
								user[online_usernum].newpet(randpet, 1);
								cout << "�û����1ֻ�¾��飺"; 
								pet[randpet].showname();
								cout << "��" << randpet << "��" << endl;
								sbuf[5] = randpet;
								sbuf[6] = 1;
							}
							send(*ClientSocket, sbuf, sizeof(sbuf), 0);
						}
					} 
				}
			}
			else{
				cout << "error " << (int)rbuf[0] << endl;
			}
		}		
		else{
			break;
		}
		memset(rbuf, 0, sizeof(rbuf));
	}	
	
	cout << "һ���ͻ����ѶϿ����ӣ�socket�ǣ�" << *ClientSocket << endl;
	closesocket(*ClientSocket);
	free(ClientSocket);
	
	return 0;
}

void Func::mullock(){
	while(lock != 0){
	}
	lock = 1;
}
void Func::unlock(){
	lock = 0;
}

int Func::findusernum(char* usn){
	int tkn = -1;
	for(int i = 0; i < user_max; i++){
		func.mullock();
		if(user[i].usntaken(usn) == 1){
			tkn = i;
			//break;
		}
		func.unlock();
	}
	return tkn;
}

void Func::sort(char* left, int size){
	char* Ptr = NULL;
	int loc, temp;
	for(loc = size-1; loc >= 1; loc--){
		for(Ptr = left; Ptr < (left+loc); Ptr++){
			if(*Ptr > *(Ptr+1)){
				temp = *Ptr;
				*Ptr = *(Ptr+1);
				*(Ptr+1) = temp;
			}			
		} 
	}
}
	
