#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

#include <QDebug>

using std::swap;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    window_width = 870;
    window_height = 600;
    board_x = 50; board_y = 50;
    grid_size = 50;
    setFixedSize(window_width, window_height);

    sound = new QSound(":/sound/sound/sound.wav");
    winsound = new QSound(":/sound/sound/win.wav");
    losesound = new QSound(":/sound/sound/lose.wav");

    /* // failed
    QString localHostName = QHostInfo::localHostName();
    qDebug() << "localHostName = " << localHostName;
    QHostInfo info = QHostInfo::fromName(localHostName);
    qDebug() << "IP address = " << info.addresses();
    QString local;
    foreach(QHostAddress address,info.addresses())
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            //qDebug() << address.toString();
            local = address.toString();
        }
    }
    qDebug() << "local = " << local;
    */
    QString local_ip;
    QList<QHostAddress> list = QNetworkInterface::allAddresses();
    foreach (QHostAddress address, list)
    {
        if(address.protocol() == QAbstractSocket::IPv4Protocol)
        {
            if (address.toString().contains("127.0."))
            {
                continue;
            }
            local_ip = address.toString();
        }
    }
    if (local_ip == "127.0.0.1")
    {
        qDebug() << "get local ip fail";
        return;
    }
    else
    {
        qDebug() << local_ip;
    }

    ishost = -1;

    mapper = new QSignalMapper();
    connect(mapper, SIGNAL(mapped(int)), this, SLOT(sendInfo(int)));


    QWidget *widget = new QWidget();
    setCentralWidget(widget);
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(600, 50, 50, 50);

    tabwidget = new QTabWidget();
    QWidget *tab1 = new QWidget(), *tab2 = new QWidget();
    QVBoxLayout *tablayout1 = new QVBoxLayout(), *tablayout2 = new QVBoxLayout();
    QLabel *templabel;
    QHBoxLayout *temphlayout;
    showIP = new QLineEdit(); showIP->setEnabled(false);
    //
    showIP->setText(local_ip);
    //
    inputIP = new QLineEdit(); inputIP->setEnabled(true);
    templabel = new QLabel("IP:");
    temphlayout = new QHBoxLayout();
    temphlayout->addWidget(templabel); temphlayout->addWidget(showIP);
    tablayout1->addLayout(temphlayout);
    templabel = new QLabel("IP:");
    temphlayout = new QHBoxLayout();
    temphlayout->addWidget(templabel); temphlayout->addWidget(inputIP);
    tablayout2->addLayout(temphlayout);
    inputPort_server = new QLineEdit();
    inputPort_client = new QLineEdit();
    templabel = new QLabel("Port:");
    temphlayout = new QHBoxLayout();
    temphlayout->addWidget(templabel); temphlayout->addWidget(inputPort_server);
    tablayout1->addLayout(temphlayout);
    templabel = new QLabel("Port:");
    temphlayout = new QHBoxLayout();
    temphlayout->addWidget(templabel); temphlayout->addWidget(inputPort_client);
    tablayout2->addLayout(temphlayout);
    b_setserver = new QPushButton("Set server");
    b_setclient = new QPushButton("Find server");
    connect(b_setserver, SIGNAL(clicked(bool)), this, SLOT(setserver()));
    connect(b_setclient, SIGNAL(clicked(bool)), this, SLOT(setclient()));
    tablayout1->addWidget(b_setserver);
    tablayout2->addWidget(b_setclient);
    tab1->setLayout(tablayout1);
    tab2->setLayout(tablayout2);
    tabwidget->addTab(tab1, "Server");
    tabwidget->addTab(tab2, "Client");
    layout->addWidget(tabwidget);

    b_newgame = new QPushButton("NewGame");
    connect(b_newgame, SIGNAL(clicked(bool)), this, SLOT(newgame()));
    connect(b_newgame, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_newgame, 1);
    layout->addWidget(b_newgame);

    b_freemod = new QPushButton("Free");
    connect(b_freemod, SIGNAL(clicked(bool)), this, SLOT(free_start()));
    connect(b_freemod, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_freemod, 2);
    layout->addWidget(b_freemod);

    freegroup = new QGroupBox();
    QVBoxLayout *grouplayout = new QVBoxLayout();

    QHBoxLayout *chesslayout = new QHBoxLayout();
    b_black = new QPushButton("Black");
    b_white = new QPushButton("White");
    b_delete = new QPushButton("Delete");
    connect(b_black, SIGNAL(clicked(bool)), this, SLOT(blackchess()));
    connect(b_black, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_black, 3);
    connect(b_white, SIGNAL(clicked(bool)), this, SLOT(whitechess()));
    connect(b_white, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_white, 4);
    connect(b_delete, SIGNAL(clicked(bool)), this, SLOT(deletechess()));
    connect(b_delete, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_delete, 5);
    chesslayout->addWidget(b_black);
    chesslayout->addWidget(b_white);
    chesslayout->addWidget(b_delete);
    grouplayout->addLayout(chesslayout);

    QHBoxLayout *whosfirstlayout = new QHBoxLayout();
    b_bfirst = new QPushButton("Black first");
    b_wfirst = new QPushButton("White first");
    connect(b_bfirst, SIGNAL(clicked(bool)), this, SLOT(bfirst()));
    connect(b_bfirst, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_bfirst, 6);
    connect(b_wfirst, SIGNAL(clicked(bool)), this, SLOT(wfirst()));
    connect(b_wfirst, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_wfirst, 7);
    whosfirstlayout->addWidget(b_bfirst);
    whosfirstlayout->addWidget(b_wfirst);
    grouplayout->addLayout(whosfirstlayout);
    first = new QLabel("");
    first->setAlignment(Qt::AlignCenter);
    grouplayout->addWidget(first);

    b_freedone = new QPushButton("Done");
    connect(b_freedone, SIGNAL(clicked(bool)), this, SLOT(free_end()));
    connect(b_freedone, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_freedone, 8);
    grouplayout->addWidget(b_freedone);

    freegroup->setLayout(grouplayout);
    layout->addWidget(freegroup);

    opergroup = new QGroupBox();
    QHBoxLayout *operatelayout = new QHBoxLayout();
    b_draw = new QPushButton("Draw");
    connect(b_draw, SIGNAL(clicked(bool)), this, SLOT(offerdraw()));
    /*//出现粘包
    connect(b_draw, SIGNAL(clicked(bool)), mapper, SLOT(map()));
    mapper->setMapping(b_draw, 9);
    */
    b_resign = new QPushButton("Resign");
    connect(b_resign, SIGNAL(clicked(bool)), this, SLOT(resign()));
    operatelayout->addWidget(b_draw);
    operatelayout->addWidget(b_resign);
    opergroup->setLayout(operatelayout);
    layout->addWidget(opergroup);

    widget->setLayout(layout);

    b_newgame->setEnabled(false);
    b_freemod->setEnabled(false);
    freegroup->setEnabled(false);
    opergroup->setEnabled(false);
    connected = false;

    game = new Game();
    game->initBoard();
    game->clearBoard();
    //game->readinBoard();
//    turn = 0;
//    game->genMovboard(turn);
//    cur = NULL;

    update();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::starts()
{
    tabwidget->setEnabled(false);
    b_newgame->setEnabled(true);
    b_freemod->setEnabled(true);
    freegroup->setEnabled(false);
    opergroup->setEnabled(true);
    connected = true;
    newgame();
}

void MainWindow::paintEvent(QPaintEvent* ev)
{
    QPainter p(this);
    QPen pen1(Qt::black, 5);
    QPen pen2(Qt::black, 1);
    QPixmap lightpixmap(":/pic/pic/light.jpg");
    QPixmap darkpixmap(":/pic/pic/dark.jpg");
    //QBrush light(QColor(222,184,135));
    //QBrush dark(QColor(160,82,45));
    //QBrush light(lightpixmap), dark(darkpixmap);
    QBrush hlight(Qt::green);
    p.translate(board_x, board_y);

    // board
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            /*
            if ((i ^ j) & 1)
                p.setBrush(dark);
            else
                p.setBrush(light);
            p.drawRect(i * grid_size, j * grid_size, grid_size, grid_size);
            //*/
            if ((i ^ j) & 1)
                p.drawPixmap(i * grid_size, j * grid_size, grid_size, grid_size, darkpixmap);
            else
                p.drawPixmap(i * grid_size, j * grid_size, grid_size, grid_size, lightpixmap);
        }
    // chess & mark
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            int board = game->board[j][i];
            int x = i, y = j;
            if (!ishost) {x = 9 - x; y = 9 - y;}
            qreal xx = x * grid_size, yy = y * grid_size;
            if (board & 4)
            {
                QRadialGradient radial_d(xx + 25, yy + 25, 25, xx + 10, yy + 10);
                radial_d.setColorAt(0.0, Qt::white);
                if (board & 1)
                    radial_d.setColorAt(0.5, Qt::black);
                else
                    radial_d.setColorAt(0.5, Qt::lightGray);
                p.setBrush(radial_d);
                p.drawEllipse(xx + 2, yy + 2, grid_size - 4, grid_size - 4);
                if (board & 2)
                {
                    p.setBrush(Qt::yellow);
                    p.drawEllipse(xx + 15, yy + 15, 20, 20);
                }
            }

            if (game->mark[j][i] == 2)
            {
                p.setPen(pen1);
                p.setBrush(Qt::NoBrush);
                p.drawRect(xx, yy, grid_size, grid_size);
                p.setPen(pen2);
            }
        }
    // highlight next move
    //if ((freemod == 0) && (ishost ^ turn) == 0) return;
    if (cur != NULL && cur->flag > -1)
    {
        for (int i = 0; i < cur->len; ++i)
            if (cur->nxt[i]->flag == 0)
            {
                int x = cur->nxt[i]->y, y = cur->nxt[i]->x;
                if (!ishost) {x = 9 - x; y = 9 - y;}
                p.setBrush(hlight);
                p.drawEllipse(QPoint(x * grid_size + 25, y * grid_size + 25), 5, 5);
            }
    }
    else
    {
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 10; ++j)
                if (game->mov_board[i][j] != NULL && game->mov_board[i][j]->flag == 0)
                {
                    int x = j, y = i;
                    if (!ishost) {x = 9 - x; y = 9 - y;}
                    p.setBrush(hlight);
                    p.drawEllipse(QPoint(x * grid_size + 25, y * grid_size + 25), 5, 5);
                }
    }
}

void MainWindow::mousePressEvent(QMouseEvent* ev)
{
    if (!connected) return;
    if ((freemod == 0) && (ishost ^ turn) == 0) return;
    int xpos = ev->x(), ypos = ev->y();
    xpos -= board_x; ypos -= board_y;
    int x = ypos / grid_size;
    int y = xpos / grid_size;
    if (!(xpos >= 0 && ypos >= 0 && x >= 0 && x <= 9 && y >= 0 && y <= 9))
        return;
    if (((x ^ y) & 1) == 0) return;
    if (!ishost) {x = 9 - x; y = 9 - y;}

    QByteArray *info = new QByteArray();
    info->clear();
    info->append((char)(x + 1));
    info->append((char)(y + 1));
    readWriteSocket->write(info->data());

    pressBoard(x, y);

}

void MainWindow::pressBoard(int x, int y)
{
    if (freemod == 1)
        if (pressx != -1 && pressy != -1)
            game->mark[pressx][pressy] = 0;
    pressx = x; pressy = y;
    if (freemod == 1)
    {
        game->mark[pressx][pressy] = 2;
        update();
        return;
    }
    if (cur == NULL)
    {
        cur = game->mov_board[pressx][pressy];
    }
    else
    {
        bool check = false;
        for (int i = 0; i < cur->len; ++i)
        {
            if (cur->nxt[i]->flag == 0 &&
                    cur->nxt[i]->x == pressx && cur->nxt[i]->y == pressy)
            {
                check = true;
                cur = cur->nxt[i];
                //qDebug() << "*****" << cur->x << " " << cur->y;
                game->applyMov(cur);
                sound->play();
                break;
            }
        }
        if (!check && cur->dep == -1)
            cur = game->mov_board[pressx][pressy];
    }
    update();
    //if (cur != NULL) qDebug() << "cur = " << cur->x << " " << cur->y;
    if (cur != NULL && cur->dep != -1 && cur->len == 0)
    {
        turn ^= 1;
        game->genMovboard(turn);
        cur = NULL;
        //sendInfo(14);
        check();
    }
    update();
}

void MainWindow::setserver()
{
    tabwidget->setEnabled(false);
    qDebug() << "Im server";
    ishost = 1;
    listenSocket = new QTcpServer;
    listenSocket->listen(QHostAddress::Any, (quint16)inputPort_server->text().toUInt());
    connect(listenSocket, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void MainWindow::setclient()
{
    tabwidget->setEnabled(false);
    qDebug() << "Im client";
    ishost = 0;
    readWriteSocket = new QTcpSocket;
    connect(readWriteSocket, SIGNAL(connected()), this, SLOT(starts()));
    readWriteSocket->connectToHost(QHostAddress(inputIP->text()), (quint16)inputPort_client->text().toUInt());
    connect(readWriteSocket, SIGNAL(readyRead()), this, SLOT(recvInfo()));
}

void MainWindow::acceptConnection()
{
    readWriteSocket = listenSocket->nextPendingConnection();
    starts();
    qDebug() << "connect success";
    connect(readWriteSocket, SIGNAL(readyRead()), this, SLOT(recvInfo()));
}

/*
void MainWindow::recvMessage()
{
    QByteArray info = readWriteSocket->readAll();
    if ((int)info[0] == 1) newgame();
//    QString info;
//    info += readWriteSocket->readAll();
//    infoEdit->setText("Client: "+info);
}

void MainWindow::on_sendButton()
{
    QByteArray *array =new QByteArray;
    array->clear();
    array->append(infoEdit->toPlainText());
    readWriteSocket->write(array->data());
}
*/



void MainWindow::sendInfo(int val)
{
    QByteArray *info = new QByteArray();
    info->clear();
    info->append((char)(val));
    readWriteSocket->write(info->data());
}

void MainWindow::recvInfo()
{
    QByteArray info = readWriteSocket->readAll();
    //qDebug() << "size = " << info.size();
    if (info.size() == 2)
    {
        int x = (int)info[0] - 1, y = (int)info[1] - 1;
        pressBoard(x, y);
        return;
    }
    int val = info[0];
    if (val == 1) newgame();
    else if (val == 2) free_start();
    else if (val == 3) blackchess();
    else if (val == 4) whitechess();
    else if (val == 5) deletechess();
    else if (val == 6) bfirst();
    else if (val == 7) wfirst();
    else if (val == 8) free_end();
    else if (val == 9)
    {
        int ret = QMessageBox::warning(this, "Offers Draw", "Opponent offers draw", QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No)
        {
            sendInfo(10);
        }
        else
        {
            sendInfo(11);
            result(-1);
        }
    }
    else if (val == 10)
    {
        QMessageBox::warning(this, "Declined", "Draw offer declined");
    }
    else if (val == 11) result(-1);
    else if (val == 12) result(0);
    else if (val == 13) result(1);
    //else if (val == 14) check();
}

void MainWindow::newgame()
{
    freemod = 0;
    game->initBoard();
    turn = 1;
    game->genMovboard(turn);
    cur = NULL;
    first->setText("");
    freegroup->setEnabled(false);
    opergroup->setEnabled(true);
    update();
}

void MainWindow::free_start()
{
    freemod = 1;
    game->initBoard();
    game->clearBoard();
    pressx = pressy = -1;
    bfirst();
    cur = NULL;
    freegroup->setEnabled(true);
    opergroup->setEnabled(false);
    update();
}

void MainWindow::free_end()
{
    game->genMovboard(turn);
    cur = NULL;
    freemod = 0;
    freegroup->setEnabled(false);
    opergroup->setEnabled(true);
    game->updateBoard();
    update();
}

void MainWindow::blackchess()
{
    if (pressx == -1 || pressy == -1) return;
    if (game->board[pressx][pressy] == 5)
        game->board[pressx][pressy] |= 2;
    else
        game->board[pressx][pressy] = 5;
    update();
}

void MainWindow::whitechess()
{
    if (pressx == -1 || pressy == -1) return;
    if (game->board[pressx][pressy] == 4)
        game->board[pressx][pressy] |= 2;
    else
        game->board[pressx][pressy] = 4;
    update();
}

void MainWindow::deletechess()
{
    if (pressx == -1 || pressy == -1) return;
    game->board[pressx][pressy] = 0;
    update();
}

void MainWindow::bfirst()
{
    turn = 1;
    first->setText("Black first");
}

void MainWindow::wfirst()
{
    turn = 0;
    first->setText("White first");
}

void MainWindow::offerdraw()
{
    sendInfo(9);
}

void MainWindow::resign()
{
    if (!ishost)
    {
        sendInfo(12);
        result(0);
    }
    else
    {
        sendInfo(13);
        result(1);
    }
}

void MainWindow::check()
{
    bool flag = true;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            if (game->mov_board[i][j] != NULL &&
                    game->mov_board[i][j]->len > 0)
            {
                flag = false;
                break;
            }
    if (flag)
    {
        result(turn ^ 1);
    }
}

void MainWindow::result(int win)
{
    QString res;
    if (win == 1) res = "Black wins";
    else if (win == 0) res = "White wins";
    else if (win == -1) res = "Draw";
    if (win != -1)
    {
        if (win ^ ishost)
            winsound->play();
        else
            losesound->play();
    }
    QMessageBox::warning(this, "result", res);
    newgame();
}
