/*Client - 图形界面版本*/

#include <iostream>
#include <winsock2.h>
#include "head.h"
#include <iomanip>
#include <fstream>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QPixmap>
#include <QElapsedTimer>
#include <math.h>
using namespace std;

const int PORT = 8000;
int pet_max;

int rbyte;
char sbuf[MAXBUFSIZE];
char rbuf[MAXBUFSIZE];
char req;
int online;
char username[20];
char password[20];
SOCKET SocketClient;
Pet pet[20];
MainWindow* wP;
int user_pet_tot, sys_pet_tot = 0;

class Func{ /*函数类*/
    public:
        char* qs2ch(char*, QString, int);       /*图形界面使用，字符数组与QSTRING的互相转化*/
        QString* ch2qs(QString*, char*, int);
        int getlen(int);
        char* int2ch(char*, int, int);
};
Func func;

class QBattle{  /*对战类*/
    public:
        Pet userpet;                /*我方出战精灵*/
        Pet syspet;                 /*对方出战精灵*/
        int userhp;                 /*我方当前生命值*/
        int syshp;                  /*对方当前生命值*/
        QBattle(int, int, int, int);
        int battle();               /*开始对战*/
        const int dmg(int);         /*造成伤害*/
};

int main(int argc, char *argv[])
{
    rbyte = 0;
    online = 0;
    req = 0;
    pet_max = 0;
    user_pet_tot = 0;
    sys_pet_tot = 0;

    fstream fp;
    fp.open("input.txt",ios::in);
    //cout << fp.is_open() << endl;

    fp >> pet_max;
    //cout << pet_max << endl;
    fp.close();
    for(int i = 0; i < pet_max; i++){
        pet[i].init_p(i, i - 1);
        //pet[i].lvup(i - 1, 0);
    }
    sbuf[0] = 1;
    //cout << pet_max << endl;

    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    wP = &w;

    WSADATA wsd;
    WSAStartup(MAKEWORD(2, 2), &wsd);
    SocketClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    SOCKADDR_IN  ClientAddr;

    ClientAddr.sin_family = AF_INET;
    ClientAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
    ClientAddr.sin_port = htons(PORT);
    int n = 0;
    n = connect(SocketClient, (struct sockaddr*)&ClientAddr, sizeof(ClientAddr));
    if (n == SOCKET_ERROR) {
        cout << "Connection failed!" << endl;
        return -1;
    }
    cout << "Connection succeed!" << endl;

    /*closesocket(SocketClient);
    WSACleanup();
    cout << "客户端已退出！" << endl;*/

    return a.exec();
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->onlineframe->setVisible(false);
    ui->offlineframe->setVisible(true);
    ui->battleframe->setVisible(false);
    ui->battlechooseframe->setVisible(false);
    ui->confirmButton->setVisible(false);
    ui->losepetframe->setVisible(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_loginButton_clicked()//登录
{
    sbuf[1] = 2;
    for(int i = 0; i < 20; i++){
        sbuf[2 + i] = '\0';
        sbuf[22 + i] = '\0';
        username[i] = '\0';
        password[i] = '\0';
    }

    QString usnin = ui->usnEdit->text();
    QString pswin = ui->pswEdit->text();

    func.qs2ch(username, usnin, 20);
    func.qs2ch(password, pswin, 20);

    memcpy(&sbuf[2], username, 20);
    memcpy(&sbuf[22], password, 20);

    send(SocketClient, sbuf, sizeof(sbuf), 0);
    rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
    if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 2){
        if(rbuf[2] == 0){
            cout << "Login failed! Username do not exist!" << endl;
            ui->textBrowser->setText("用户名不存在，登录失败！");
        }
        else if(rbuf[2] == 1){
            cout << "Login failed! Wrong password!" << endl;
            ui->textBrowser->setText("密码错误，登录失败！");
        }
        else if(rbuf[2] == 3){
            cout << "Login failed! User is online!" << endl;
            ui->textBrowser->setText("该用户已经在线，登录失败！");
        }
        else if(rbuf[2] == 2){
            online = 1;
            cout << "Login succeed! This client is used by: " << username << endl;
            QString onlineusn;
            func.ch2qs(&onlineusn, username, 20);
            ui->textBrowser->setText(username);
            ui->textBrowser->insertPlainText("登录成功，本客户端登录的用户是：");
            ui->onlineusnlabel->setText(onlineusn);
            ui->offlineframe->setVisible(false);
            ui->onlineframe->setVisible(true);
            ui->textBrowser_2->setText(username);
            ui->textBrowser_2->insertPlainText("登录成功，本客户端登录的用户是：");
        }
    }
    else{
        cout << "error" << endl;
        cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
    }
}

void MainWindow::on_signinButton_clicked()//注册
{
    sbuf[1] = 1;
    for(int i = 0; i < 20; i++){
        sbuf[2 + i] = '\0';
        sbuf[22 + i] = '\0';
        username[i] = '\0';
        password[i] = '\0';
    }

    QString usnin = ui->usnEdit->text();
    QString pswin = ui->pswEdit->text();

    func.qs2ch(username, usnin, 20);
    func.qs2ch(password, pswin, 20);

    memcpy(&sbuf[2], username, 20);
    memcpy(&sbuf[22], password, 20);

    send(SocketClient, sbuf, sizeof(sbuf), 0);
    rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
    if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 1){
        if(rbuf[2] == 1){
            cout << "Signin succeed! Your username is: " << username << endl;
            int randpet;
            cout << "Get 3 pets: " << endl;
            ui->textBrowser->setText("");
            ui->textBrowser->insertPlainText("注册成功！您的用户名是：");
            ui->textBrowser->insertPlainText(username);
            ui->textBrowser->insertPlainText("\n");
            ui->textBrowser->insertPlainText("您获得了3只初始精灵：");
            for(int i = 0; i < 3; i++){
                ui->textBrowser->insertPlainText("\n");
                randpet = rbuf[3 + i];
                pet[randpet].showname();
                cout << endl;
                char petname[20];
                pet[randpet].Qshowname(petname);
                ui->textBrowser->insertPlainText(petname);
                //cout << petname << endl;
                //cout << randpet << endl;
            }
            cout << endl;

        }
        else if(rbuf[2] == 0){
            cout << "Signin failed! The username has been occupied!" << endl;
            ui->textBrowser->setText("抱歉，用户名已被占用，注册失败！");
        }
    }
    else{
        cout << "error" << endl;
        cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
    }
}

void MainWindow::on_logoutButton_clicked()//登出
{
    sbuf[1] = 3;
    memcpy(&sbuf[2], username, 20);
    send(SocketClient, sbuf, sizeof(sbuf), 0);
    rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
    if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 3){
        if(rbuf[2] == 1){
            online = 0;
            cout << "User:" << username << " logout succeed!"<< endl;
            ui->textBrowser_2->setText(" 登出成功！");
            ui->textBrowser_2->insertPlainText(username);
            ui->onlineframe->setVisible(false);
            ui->offlineframe->setVisible(true);
            ui->onlineusnlabel->setText("未登录");
            ui->textBrowser->setText(" 登出成功！");
            ui->textBrowser->insertPlainText(username);
        }
        else if(rbuf[2] == 0){
            cout << "User:" << username << " logout failed! Not login yet!" << endl;
            ui->textBrowser_2->setText("登出失败！用户尚未登录");
        }
        else if(rbuf[2] == 2){
            cout << "User:" << username << " logout failed! Not singed yet" << endl;
            ui->textBrowser_2->setText("登出成功！用户尚未注册");
        }
    }
    else{
        cout << "error" << endl;
        cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
    }
}

void MainWindow::on_checkuserButton_clicked()//查看在线用户
{
    sbuf[1] = 5;
    send(SocketClient, sbuf, sizeof(sbuf), 0);
    rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
    if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 5){
        int temp_online_user = rbuf[2];
        char tempusn[20];
        cout << temp_online_user << " user(s) online, username:" << endl;
        ui->textBrowser_2->setText("");
        ui->textBrowser_2->insertPlainText("当前共 ");
        ui->textBrowser_2->insertPlainText(QString::number(temp_online_user));
        ui->textBrowser_2->insertPlainText(" 名用户在线，用户名：");
        int j = 3;
        for(int i = 0; i < temp_online_user; i++){
            memcpy(tempusn, &rbuf[j], 20);
            cout << tempusn << endl;
            ui->textBrowser_2->insertPlainText("\n");
            ui->textBrowser_2->insertPlainText(tempusn);
            j += 20;
        }
    }
    else{
        cout << "error" << endl;
        cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
    }
}

void MainWindow::on_checkinfoButton_clicked()//查看用户信息
{
    sbuf[1] = 4;
    send(SocketClient, sbuf, sizeof(sbuf), 0);
    rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
    if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 4){
        int user_tot = rbuf[2];
        int user_pet_tot = 0, j = 3, petnum, templv, tempwin = 0, templose = 0, tempbadge = 0, tempbadge15 = 0;
        float tempwinrate;
        char tempusn[20];
        ui->textBrowser_2->setText("");
        for(int i = 0; i < user_tot; i++){
            memcpy(tempusn, &rbuf[j], 20);
            j += 20;
            cout << "User:" << tempusn << endl;
            ui->textBrowser_2->insertPlainText("已注册用户：");
            ui->textBrowser_2->insertPlainText(tempusn);
            ui->textBrowser_2->insertPlainText("\n");
            user_pet_tot = rbuf[j];
            j++;
            cout << user_pet_tot << " pet(s)" << endl;
            ui->textBrowser_2->insertPlainText("精灵共 ");
            ui->textBrowser_2->insertPlainText(QString::number(user_pet_tot));
            ui->textBrowser_2->insertPlainText(" 只");
            ui->textBrowser_2->insertPlainText("\n");
            for(int k = 0; k < user_pet_tot; k++){
                petnum = rbuf[j];
                j++;
                pet[petnum].showname();
                templv = rbuf[j];
                cout << "level:" << templv << endl;
                char petname[20];
                pet[petnum].Qshowname(petname);
                ui->textBrowser_2->insertPlainText(petname);
                ui->textBrowser_2->insertPlainText("， 等级：");
                ui->textBrowser_2->insertPlainText(QString::number(templv));
                ui->textBrowser_2->insertPlainText("\n");
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
            cout << "Winrate：" << tempwinrate * 100 << "%(" << tempwin << "/" << templose << ")" << endl;
            ui->textBrowser_2->insertPlainText("胜率：");
            ui->textBrowser_2->insertPlainText(QString::number(tempwinrate * 100));
            ui->textBrowser_2->insertPlainText("%（");
            ui->textBrowser_2->insertPlainText(QString::number(tempwin));
            ui->textBrowser_2->insertPlainText("胜/");
            ui->textBrowser_2->insertPlainText(QString::number(templose));
            ui->textBrowser_2->insertPlainText("负）");
            ui->textBrowser_2->insertPlainText("\n");
            cout << "Badge of pet:" ;
            ui->textBrowser_2->insertPlainText("宠物个数徽章：");
            if(tempbadge == 0){
                cout << "Not get yet" << endl;
                ui->textBrowser_2->insertPlainText("未获得");
            }
            else if(tempbadge == 1){
                cout << "Bronze" << endl;
                ui->textBrowser_2->insertPlainText("铜");
            }
            else if(tempbadge == 2){
                cout << "Silver" << endl;
                ui->textBrowser_2->insertPlainText("银");
            }
            else if(tempbadge == 3){
                cout << "Gold" << endl;
                ui->textBrowser_2->insertPlainText("金");
            }
            ui->textBrowser_2->insertPlainText("\n");
            cout << "Badge of high level pet:" ;
            ui->textBrowser_2->insertPlainText("高级宠物徽章：");
            if(tempbadge15 == 0){
                cout << "Not get yet" << endl;
                ui->textBrowser_2->insertPlainText("未获得");
            }
            else if(tempbadge15 == 1){
                cout << "Bronze" << endl;
                ui->textBrowser_2->insertPlainText("铜");
            }
            else if(tempbadge15 == 2){
                cout << "Silver" << endl;
                ui->textBrowser_2->insertPlainText("银");
            }
            else if(tempbadge15 == 3){
                cout << "Gold" << endl;
                ui->textBrowser_2->insertPlainText("金");
            }
            cout << endl;
            ui->textBrowser_2->insertPlainText("\n");
            ui->textBrowser_2->insertPlainText("\n");
        }
    }
    else{
        cout << "error" << endl;
        cout << rbyte << " "<< (int)rbuf[0] << " " << (int)rbuf[1] << endl;
    }
}

void MainWindow::on_battleButton_clicked()//对战
{
    ui->onlineframe->setVisible(false);
    ui->battlechooseframe->setVisible(true);
    sbuf[1] = 6;
    send(SocketClient, sbuf, sizeof(sbuf), 0);
    rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
    if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
        user_pet_tot = 0;
        sys_pet_tot = 0;
        int j = 2;
        user_pet_tot = rbuf[j];
        cout << "User owns " << user_pet_tot << " pets:" << endl;
        j++;
        ui->comboBox_l->clear();
        ui->comboBox_r->clear();
        ui->comboBox_type->clear();
        ui->comboBox_type->addItem("升级赛");
        ui->comboBox_type->addItem("决斗赛");
        for(int i = 0; i < user_pet_tot; i++){
            cout << i << " - ";
            //cout << "Num:" << rbuf[j];
            cout << "Name:";
            pet[rbuf[j]].showname();
            char petinfo[20];
            pet[rbuf[j]].Qshowname(petinfo);
            j++;
            cout << " Level:" << (int)rbuf[j] << endl;
            strcat(petinfo, " 等级：");
            char templvC[3];
            if((int)rbuf[j] >= 10){
                templvC[0] = '1';
                templvC[1] = rbuf[j] + '0' - 10;
                templvC[2] = '\0';
            }
            else{
                templvC[0] = rbuf[j] + '0';
                templvC[1] = '\0';
            }
            strcat(petinfo, templvC);
            ui->comboBox_l->addItem(petinfo);
            j++;
        }
        sys_pet_tot = rbuf[j];
        j++;
        cout << "System owns " << sys_pet_tot << " pets:" << endl;
        for(int i = 0; i < sys_pet_tot; i++){
            cout << i << " - ";
            //cout << "Num:" << rbuf[j];
            cout << "Name:";
            pet[rbuf[j]].showname();
            char petinfo[20];
            pet[rbuf[j]].Qshowname(petinfo);
            j++;
            cout << " Level:" << (int)rbuf[j] << endl;
            strcat(petinfo, " 等级：");
            char templvC[3];
            if((int)rbuf[j] >= 10){
                templvC[0] = '1';
                templvC[1] = rbuf[j] + '0' - 10;
                templvC[2] = '\0';
            }
            else{
                templvC[0] = rbuf[j] + '0';
                templvC[1] = '\0';
            }
            strcat(petinfo, templvC);
            ui->comboBox_r->addItem(petinfo);
            j++;
        }
    }
}

void MainWindow::on_startbattleButton_clicked()//开始战斗
{
    int battle_type = 0, user_choose = -1, sys_choose = -1, battle_win = 0;
    user_choose = ui->comboBox_l->currentIndex();
    sys_choose = ui->comboBox_r->currentIndex();
    battle_type = ui->comboBox_type->currentIndex() + 1;
    //cout << user_choose << " " << sys_choose << " " << battle_type << endl;

    ui->battlechooseframe->setVisible(false);
    ui->battleframe->setVisible(true);
    ui->confirmButton->setVisible(false);
    ui->confirmButton_2->setVisible(false);
    //cout << ui->battlechooseframe->isHidden() << " " << ui->battleframe->isHidden();

    QBattle bat(rbuf[3 + 2 * user_choose], rbuf[4 + 2 * user_choose], rbuf[4 + 2 * user_pet_tot + 2 * sys_choose], rbuf[5 + 2 * user_pet_tot + 2 * sys_choose]);
    battle_win = bat.battle();

    sbuf[2] = battle_type;
    sbuf[3] = user_choose;
    sbuf[4] = sys_choose;
    sbuf[5] = battle_win;
    send(SocketClient, sbuf, sizeof(sbuf), 0);
    if(battle_type == 1 && battle_win == 1){//升级赛胜：加经验
        ui->confirmButton->setVisible(true);
        rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
        if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
            pet[rbuf[3]].showname();
            ui->textBrowser_2->setText("");
            ui->textBrowser_2->insertPlainText("您胜利了！");
            char name[20];
            pet[rbuf[3]].Qshowname(name);
            ui->textBrowser_2->insertPlainText(name);
            cout << " get exp:" << pet[sys_choose].getinfo(2) * 20 << endl;
            ui->textBrowser_2->insertPlainText(" 获得了经验值：");
            int tempexp = pet[sys_choose].getinfo(2) * 20;
            ui->textBrowser_2->insertPlainText(QString::number(tempexp));
            ui->textBrowser_2->insertPlainText("\n");
            int dlv = rbuf[9];
            if(dlv > 0){
                pet[rbuf[3]].showname();
                ui->textBrowser_2->insertPlainText(name);
                cout << " level up!" << endl;
                ui->textBrowser_2->insertPlainText(" 升级了！");
                ui->textBrowser_2->insertPlainText("\n");
                cout << "Level:" << (int)rbuf[4] << " + " << rbuf[11] - rbuf[4] << " -> " << (int)rbuf[11] << endl;
                ui->textBrowser_2->insertPlainText("等级：");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[4]));
                ui->textBrowser_2->insertPlainText(" + ");
                ui->textBrowser_2->insertPlainText(QString::number(rbuf[11] - rbuf[4]));
                ui->textBrowser_2->insertPlainText(" -> ");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[11]));
                ui->textBrowser_2->insertPlainText("\n");
                cout << "Attack:" << (int)rbuf[5] << " + " << rbuf[12] - rbuf[5] << " -> " << (int)rbuf[12] << endl;
                ui->textBrowser_2->insertPlainText("攻击：");;
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[5]));
                ui->textBrowser_2->insertPlainText(" + ");
                ui->textBrowser_2->insertPlainText(QString::number(rbuf[12] - rbuf[5]));
                ui->textBrowser_2->insertPlainText(" -> ");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[12]));
                ui->textBrowser_2->insertPlainText("\n");
                cout << "Defence:" << (int)rbuf[6] << " + " << rbuf[13] - rbuf[6] << " -> " << (int)rbuf[13] << endl;
                ui->textBrowser_2->insertPlainText("防御：");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[6]));
                ui->textBrowser_2->insertPlainText(" + ");
                ui->textBrowser_2->insertPlainText(QString::number(rbuf[13] - rbuf[6]));
                ui->textBrowser_2->insertPlainText(" -> ");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[13]));
                ui->textBrowser_2->insertPlainText("\n");
                cout << "Speed:" << (int)rbuf[7] << " + " << rbuf[14] - rbuf[7] << " -> " << (int)rbuf[14] << endl;
                ui->textBrowser_2->insertPlainText("敏捷：");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[7]));
                ui->textBrowser_2->insertPlainText(" + ");
                ui->textBrowser_2->insertPlainText(QString::number(rbuf[14] - rbuf[7]));
                ui->textBrowser_2->insertPlainText(" -> ");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[14]));
                ui->textBrowser_2->insertPlainText("\n");
                cout << "Power:" << (int)rbuf[8] << " + " << rbuf[15] - rbuf[8] << " -> " << (int)rbuf[15] << endl;
                ui->textBrowser_2->insertPlainText("生命：");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[8]));
                ui->textBrowser_2->insertPlainText(" + ");
                ui->textBrowser_2->insertPlainText(QString::number(rbuf[15] - rbuf[8]));
                ui->textBrowser_2->insertPlainText(" -> ");
                ui->textBrowser_2->insertPlainText(QString::number((int)rbuf[15]));
                ui->textBrowser_2->insertPlainText("\n");
            }
        }

    }
    else if(battle_type == 1 && battle_win == 2){//升级赛负：无事发生
        ui->confirmButton->setVisible(true);
        cout << "Your pet did not get experience" << endl;
        ui->textBrowser_2->setText("");
        ui->textBrowser_2->insertPlainText("您失败了，精灵未获得经验值");
    }
    else if(battle_type == 2 && battle_win == 1){//决斗赛胜：获得对方精灵
        ui->confirmButton->setVisible(true);
        rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
        if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
            cout << "Get pet:";
            ui->textBrowser_2->setText("");
            ui->textBrowser_2->insertPlainText("您胜利了，获得了对手的精灵：");
            pet[rbuf[2]].showname();
            char petname[20];
            pet[rbuf[2]].Qshowname(petname);
            ui->textBrowser_2->insertPlainText(petname);
            cout << "，等级：" << (int)rbuf[3] << endl;
        }
    }
    else if(battle_type == 2 && battle_win == 2){//决斗赛负：自己的三只随机精灵中，选择一只失去
        ui->confirmButton_2->setVisible(true);
        rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
        if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
            int lose_max = rbuf[2];
            cout << "您需送出以下 " << lose_max << " 只精灵中的一只，请选择：" << endl;
            ui->comboBox_lose->clear();
            for(int i = 0; i < lose_max; i++){
                cout << i << " - " ;
                pet[rbuf[3 + 2 * i]].showname();
                cout << "等级：" << (int)rbuf[4 + 2 * i] << endl;
                char petinfo[20];
                pet[rbuf[3 + 2 * i]].Qshowname(petinfo);
                strcat(petinfo, " 等级：");
                char templvC[3];
                if((int)rbuf[4 + 2 * i] >= 10){
                    templvC[0] = '1';
                    templvC[1] = rbuf[4 + 2 * i] + '0' - 10;
                    templvC[2] = '\0';
                }
                else{
                    templvC[0] = rbuf[4 + 2 * i] + '0';
                    templvC[1] = '\0';
                }
                strcat(petinfo, templvC);
                ui->comboBox_lose->addItem(petinfo);
            }
            user_choose = -1;         
        }
    }
}

void MainWindow::on_confirmButton_clicked()//战斗结束确认
{
    ui->battleframe->setVisible(false);        
    ui->onlineframe->setVisible(true);
}

void MainWindow::on_confirmButton_2_clicked()//战斗结束确认(仅当决斗赛负时）
{
    ui->battleframe->setVisible(false);
    ui->losepetframe->setVisible(true);
}

void MainWindow::on_loseButton_clicked()//失去精灵确认
{
    int user_choose = -1;
    user_choose = ui->comboBox_lose->currentIndex();
    sbuf[2] = user_choose;
    ui->losepetframe->setVisible(false);
    send(SocketClient, sbuf, sizeof(sbuf), 0);
    rbyte = recv(SocketClient, rbuf, sizeof(rbuf), 0);
    if(rbyte > 0 && rbuf[0] == 2 && rbuf[1] == 6){
        int lose = rbuf[2];
        cout << "You lose Pet:";
        ui->textBrowser_2->setText("");
        ui->textBrowser_2->insertPlainText("您失去了精灵：");
        pet[rbuf[3]].showname();
        char petname[20];
        pet[rbuf[3]].Qshowname(petname);
        ui->textBrowser_2->insertPlainText(petname);
        cout << endl;
        if(rbuf[5] >= 0){
            cout << "Since you have lost all your pets, the system offers you one pet:" << endl;
            ui->textBrowser_2->insertPlainText("\n");
            ui->textBrowser_2->insertPlainText("由于您失去了所有的精灵，系统已给您发放 1 只 1 级精灵：");
            pet[rbuf[5]].showname();
            pet[rbuf[5]].Qshowname(petname);
            ui->textBrowser_2->insertPlainText(petname);
            cout << endl;
        }
        ui->onlineframe->setVisible(true);
    }
}

char* Func::qs2ch(char* des, QString src, int len){
    QByteArray ba = src.toUtf8();
    int aclen = ba.length();
    char* P = ba.data();
    for(int i = 0; i < len; i++){
        *(des + i) = '\0';
    }
    memcpy(des, P, aclen);
    return des;
}

QString* Func::ch2qs(QString* des, char* src, int len){
    QString qstr = QString::fromUtf8(src);
    (*des) = qstr;
    return des;
}

int Func::getlen(int num){
    if(num == 0){
        return 1;
    }
    else{
        int count = 0;
        while(num != 0){
            count ++;
            num /= 10;
        }
        return count;
    }
}

char* Func::int2ch(char* ch, int num, int len){
    for(int i = 0; i < len; i++){
        *(ch + i) = (num % (int)pow(10, len - i)) / (int)pow(10, len - i - 1) + '0';
    }
    *(ch + len) = '\0';
}

QBattle::QBattle(int usr_id, int usr_lv, int sys_id, int sys_lv){//对战双方初始化
    userpet.init_p(usr_id, usr_lv);
    //userpet.lvup(usr_lv - 1, 0);
    syspet.init_p(sys_id, sys_lv);
    //syspet.lvup(sys_lv - 1, 0);
}

int QBattle::battle(){//双方开始对战；返回值为1表示我方胜，为2表示我方负
    int userhp = userpet.getinfo(6);
    int userhpmax = userpet.getinfo(6);
    int syshp = syspet.getinfo(6);
    int syshpmax = syspet.getinfo(6);
    char petname[20];
    userpet.showname();
    userpet.Qshowname(petname);
    wP->ui->namelabel_l->setText(petname);
    cout << "HP:" << userhp << " / " << userhpmax << endl;
    wP->ui->hplabel_l->setNum(userhp);
    wP->ui->hpmaxlabel_l->setNum(userhpmax);
    wP->ui->progressBar_l->setMaximum(userhpmax);
    wP->ui->progressBar_l->setValue(userhp);
    syspet.showname();
    syspet.Qshowname(petname);
    wP->ui->namelabel_r->setText(petname);
    cout << "HP:" << syshp << " / " << syshpmax << endl;
    wP->ui->hplabel_r->setNum(syshp);
    wP->ui->hpmaxlabel_r->setNum(syshpmax);
    wP->ui->progressBar_r->setMaximum(syshpmax);
    wP->ui->progressBar_r->setValue(syshp);
    wP->ui->lvnumlabel_l->setNum(userpet.getinfo(2));
    wP->ui->lvnumlabel_r->setNum(syspet.getinfo(2));
    wP->ui->lvlabel_l->repaint();
    wP->ui->lvlabel_r->repaint();
    wP->ui->lvnumlabel_l->repaint();
    wP->ui->lvnumlabel_r->repaint();
    wP->ui->namelabel_l->repaint();
    wP->ui->namelabel_r->repaint();
    wP->ui->textBrowser_sk->setText("");
    wP->ui->imgslabel_l->clear();
    wP->ui->imgllabel_l->clear();
    wP->ui->imgslabel_r->clear();
    wP->ui->imgllabel_r->clear();

    QImage* img=new QImage;
    QString filenamel;
    QString filenames;
    switch (userpet.getinfo(1)) {
        case 0:
            filenamel = "F:\\Qt\\client_pet\\image\\0l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\0s.jpg";
        break;
        case 1:
            filenamel = "F:\\Qt\\client_pet\\image\\1l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\1s.jpg";
        break;
        case 2:
            filenamel = "F:\\Qt\\client_pet\\image\\2l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\2s.jpg";
        break;
        case 3:
            filenamel = "F:\\Qt\\client_pet\\image\\3l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\3s.jpg";
        break;
        case 4:
            filenamel = "F:\\Qt\\client_pet\\image\\4l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\4s.jpg";
        break;
        case 5:
            filenamel = "F:\\Qt\\client_pet\\image\\5l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\5s.jpg";
        break;
        case 6:
            filenamel = "F:\\Qt\\client_pet\\image\\6l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\6s.jpg";
        break;
        case 7:
            filenamel = "F:\\Qt\\client_pet\\image\\7l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\7s.jpg";
        break;
        case 8:
            filenamel = "F:\\Qt\\client_pet\\image\\8l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\8s.jpg";
        break;
        case 9:
            filenamel = "F:\\Qt\\client_pet\\image\\9l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\9s.jpg";
        break;
        case 10:
            filenamel = "F:\\Qt\\client_pet\\image\\10l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\10s.jpg";
        break;
        case 11:
            filenamel = "F:\\Qt\\client_pet\\image\\11l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\11s.jpg";
        break;
        case 12:
            filenamel = "F:\\Qt\\client_pet\\image\\12l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\12s.jpg";
            break;
        case 13:
            filenamel = "F:\\Qt\\client_pet\\image\\13l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\13s.jpg";
        break;
        case 14:
            filenamel = "F:\\Qt\\client_pet\\image\\14l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\14s.jpg";
        break;
        case 15:
            filenamel = "F:\\Qt\\client_pet\\image\\15l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\15s.jpg";
        break;
        default:
            cout << "wrong pet number!" << endl;
    }
    if(! ( img->load(filenamel) ) ) {
        cout << "img open failure" << endl;
    }
    wP->ui->imgllabel_l->setPixmap(QPixmap::fromImage(*img));
    if(! ( img->load(filenames) ) ) {
        cout << "img open failure" << endl;
    }
    wP->ui->imgslabel_l->setPixmap(QPixmap::fromImage(*img));
    switch (syspet.getinfo(1)) {
        case 0:
            filenamel = "F:\\Qt\\client_pet\\image\\0l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\0s.jpg";
        break;
        case 1:
            filenamel = "F:\\Qt\\client_pet\\image\\1l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\1s.jpg";
        break;
        case 2:
            filenamel = "F:\\Qt\\client_pet\\image\\2l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\2s.jpg";
        break;
        case 3:
            filenamel = "F:\\Qt\\client_pet\\image\\3l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\3s.jpg";
        break;
        case 4:
            filenamel = "F:\\Qt\\client_pet\\image\\4l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\4s.jpg";
        break;
        case 5:
            filenamel = "F:\\Qt\\client_pet\\image\\5l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\5s.jpg";
        break;
        case 6:
            filenamel = "F:\\Qt\\client_pet\\image\\6l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\6s.jpg";
        break;
        case 7:
            filenamel = "F:\\Qt\\client_pet\\image\\7l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\7s.jpg";
        break;
        case 8:
            filenamel = "F:\\Qt\\client_pet\\image\\8l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\8s.jpg";
        break;
        case 9:
            filenamel = "F:\\Qt\\client_pet\\image\\9l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\9s.jpg";
        break;
        case 10:
            filenamel = "F:\\Qt\\client_pet\\image\\10l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\10s.jpg";
        break;
        case 11:
            filenamel = "F:\\Qt\\client_pet\\image\\11l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\11s.jpg";
        break;
        case 12:
            filenamel = "F:\\Qt\\client_pet\\image\\12l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\12s.jpg";
            break;
        case 13:
            filenamel = "F:\\Qt\\client_pet\\image\\13l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\13s.jpg";
        break;
        case 14:
            filenamel = "F:\\Qt\\client_pet\\image\\14l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\14s.jpg";
        break;
        case 15:
            filenamel = "F:\\Qt\\client_pet\\image\\15l.jpg";
            filenames = "F:\\Qt\\client_pet\\image\\15s.jpg";
        break;
        default:
            cout << "wrong pet number!" << endl;
    }
    if(! ( img->load(filenamel) ) ) {
        cout << "img open failure" << endl;
    }
    *img = img->mirrored(true, false);
    wP->ui->imgllabel_r->setPixmap(QPixmap::fromImage(*img));
    if(! ( img->load(filenames) ) ) {
        cout << "img open failure" << endl;
    }
    *img = img->mirrored(true, false);
    wP->ui->imgslabel_r->setPixmap(QPixmap::fromImage(*img));
    wP->ui->imgllabel_l->repaint();
    wP->ui->imgslabel_l->repaint();
    wP->ui->imgllabel_r->repaint();
    wP->ui->imgslabel_r->repaint();

    cout << "Let the battle begin!" << endl;
    wP->ui->textBrowser_sk->insertPlainText("战斗开始！");
    wP->ui->textBrowser_sk->insertPlainText("\n");

    srand(time(NULL));
    char name[20];
    for(int i = 0; ; i++){
        //cout << userpet.getinfo(5) << " " << syspet.getinfo(5) << endl;
        if(userpet.getinfo(5) >= syspet.getinfo(5)){
            Sleep(1500);
            userpet.skill();
            userpet.Qshowname(name);
            wP->ui->textBrowser_sk->insertPlainText(name);
            wP->ui->textBrowser_sk->insertPlainText(" 使用了 ");
            userpet.Qshowskname(name);
            wP->ui->textBrowser_sk->insertPlainText(name);
            syshp -= dmg(1);
            wP->ui->textBrowser_sk->insertPlainText("\n");
            wP->ui->textBrowser_sk->moveCursor(QTextCursor::End);
            wP->ui->hplabel_r->setNum(syshp);
            wP->ui->progressBar_r->setValue(syshp);
            cout << "HP:" << syshp << " / " << syshpmax << endl << endl;
            if(syshp <= 0){
                wP->ui->progressBar_r->setValue(0);
                cout << "You win!" << endl;
                wP->ui->textBrowser_sk->insertPlainText("恭喜您获得了胜利！");
                wP->ui->textBrowser_sk->moveCursor(QTextCursor::End);
                return 1;
            }
            Sleep(1500);
            syspet.skill();
            syspet.Qshowname(name);
            wP->ui->textBrowser_sk->insertPlainText(name);
            wP->ui->textBrowser_sk->insertPlainText(" 使用了 ");
            syspet.Qshowskname(name);
            wP->ui->textBrowser_sk->insertPlainText(name);
            userhp -= dmg(2);
            wP->ui->textBrowser_sk->insertPlainText("\n");
            wP->ui->textBrowser_sk->moveCursor(QTextCursor::End);
            wP->ui->hplabel_l->setNum(userhp);
            wP->ui->progressBar_l->setValue(userhp);
            cout << "HP:" << userhp << " / " << userhpmax << endl << endl;
            if(userhp <= 0){
                wP->ui->progressBar_l->setValue(0);
                cout << "You lose!" << endl;
                wP->ui->textBrowser_sk->insertPlainText("很遗憾，您失败了！");
                wP->ui->textBrowser_sk->moveCursor(QTextCursor::End);
                return 2;
            }
        }
        else{
            Sleep(1500);
            syspet.skill();
            syspet.Qshowname(name);
            wP->ui->textBrowser_sk->insertPlainText(name);
            wP->ui->textBrowser_sk->insertPlainText(" 使用了 ");
            syspet.Qshowskname(name);
            wP->ui->textBrowser_sk->insertPlainText(name);
            userhp -= dmg(2);
            wP->ui->textBrowser_sk->insertPlainText("\n");
            wP->ui->textBrowser_sk->moveCursor(QTextCursor::End);
            wP->ui->hplabel_l->setNum(userhp);
            wP->ui->progressBar_l->setValue(userhp);
            cout << "HP:" << userhp << " / " << userhpmax << endl << endl;
            if(userhp <= 0){
                wP->ui->progressBar_l->setValue(0);
                cout << "You lose!" << endl;
                wP->ui->textBrowser_sk->insertPlainText("很遗憾，您失败了！");
                wP->ui->textBrowser_sk->moveCursor(QTextCursor::End);
                return 2;
            }
            Sleep(1500);
            userpet.skill();
            userpet.Qshowname(name);
            wP->ui->textBrowser_sk->insertPlainText(name);
            wP->ui->textBrowser_sk->insertPlainText(" 使用了 ");
            userpet.Qshowskname(name);
            wP->ui->textBrowser_sk->insertPlainText(name);
            syshp -= dmg(1);
            wP->ui->textBrowser_sk->insertPlainText("\n");
            wP->ui->textBrowser_sk->moveCursor(QTextCursor::End);
            wP->ui->hplabel_r->setNum(syshp);
            wP->ui->progressBar_r->setValue(syshp);
            cout << "HP:" << syshp << " / " << syshpmax << endl << endl;
            if(syshp <= 0){
                wP->ui->progressBar_r->setValue(0);
                cout << "You win!" << endl;
                wP->ui->textBrowser_sk->insertPlainText("恭喜您获得了胜利！");
                wP->ui->textBrowser_sk->moveCursor(QTextCursor::End);
                return 1;
            }           
        }
    }
    return 0;
}

const int QBattle::dmg(int i){//一方对另一方造成伤害，参数为1表示我方造成伤害，2表示对方造成伤害；返回伤害值
    QElapsedTimer et;
    et.start();
    while(et.elapsed() < 25)
    {
        QCoreApplication::processEvents();
    }

    float at_lv, at_at, de_de;
    if(i == 1){
        at_lv = userpet.getinfo(2);
        at_at = userpet.getinfo(3);
        de_de = syspet.getinfo(4);
    }
    else if(i == 2){
        at_lv = syspet.getinfo(2);
        at_at = syspet.getinfo(3);
        de_de = userpet.getinfo(4);
    }
    int times = 1;
    switch(rand() % 16){
        case 1:
            times = 0;
            cout << "Miss！"<< endl;
            wP->ui->textBrowser_sk->insertPlainText("\n");
            wP->ui->textBrowser_sk->insertPlainText("Miss！");
            break;
        case 6:
        case 12:
            times = 2;
            cout << "打出了致命一击" << endl;
            wP->ui->textBrowser_sk->insertPlainText("\n");
            wP->ui->textBrowser_sk->insertPlainText("打出了致命一击！");
            break;
        default:
            break;
    }
    srand(time(NULL));
    int damage = ((at_lv * 0.4 + 2) * (at_lv * 10 + 10) * at_at / de_de / 50 + 2) * 1.5 * 1 * (255 - (rand() % 30)) / 255;
    damage *= times;
    if(times >= 0){
        if(i == 1){
            wP->ui->dmglabel_l->clear();
            wP->ui->dmglabel_r->setNum(- damage);
            syspet.showname();
        }
        else if(i == 2){
            wP->ui->dmglabel_r->clear();
            wP->ui->dmglabel_l->setNum(- damage);
            userpet.showname();
        }
        cout << "受到了 " << damage << " 点伤害 " ;
    }

    return damage;
}
