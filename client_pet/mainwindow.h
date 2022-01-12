#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    friend class QBattle;

private slots:
    void on_loginButton_clicked();

    void on_signinButton_clicked();

    void on_logoutButton_clicked();

    void on_checkinfoButton_clicked();

    void on_checkuserButton_clicked();

    void on_battleButton_clicked();

    void on_startbattleButton_clicked();

    void on_confirmButton_clicked();

    void on_loseButton_clicked();

    void on_confirmButton_2_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
