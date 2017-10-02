#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QMouseEvent>
#include <QLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QLabel>
#include <QtNetwork>
#include <QSignalMapper>
#include <QGroupBox>
#include <QMessageBox>
#include <QSound>
#include <QPixmap>

#include <QTextEdit>

#include "game.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent* ev);
    void mousePressEvent(QMouseEvent* ev);

    Game* game;
    int turn;
    Move* cur;
    int pressx, pressy;


    QTabWidget *tabwidget;
    QLineEdit *showIP, *inputIP, *inputPort_server, *inputPort_client;
    QPushButton *b_setserver, *b_setclient;

    QSignalMapper *mapper;
    QPushButton *b_newgame;
    QGroupBox *freegroup;
    QPushButton *b_freemod, *b_freedone, *b_black, *b_white, *b_delete;
    QPushButton *b_bfirst, *b_wfirst;
    QLabel *first;

    QGroupBox *opergroup;
    QPushButton *b_draw, *b_resign;

    int freemod;
    int ishost;

    QTcpServer *listenSocket;
    QTcpSocket *readWriteSocket;

    QTextEdit *infoEdit;
    QPushButton *sendButton;

    bool connected;

    QSound *sound, *winsound, *losesound;

public slots:
    void starts();
    void pressBoard(int x, int y);
    void setserver();
    void setclient();
    void acceptConnection();

    void sendInfo(int val);
    void recvInfo();

    void newgame();
    void free_start();
    void free_end();
    void blackchess();
    void whitechess();
    void deletechess();
    void bfirst();
    void wfirst();
    void offerdraw();
    void resign();

    void check();
    void result(int win);

private:
    int window_width, window_height;
    int board_x, board_y, grid_size;
};

#endif // MAINWINDOW_H
