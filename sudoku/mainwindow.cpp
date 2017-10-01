#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QFont>
#include <QTime>
#include <QTimer>
#include <QMessageBox>
#include <Qsound>
#include <QPainter>
#include "game.h"

#include <iostream>
#include <cstdio>
#include <cstdlib>
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    freemod = false;
    setWindowTitle(tr("sudoku"));
    bgm = new QSound(":/sound/bgm");
    game = new Game();
    guess = new int*[9];
    for (int i = 0; i < 9; ++i)
        guess[i] = new int[9];
    que = new Ope[15];
    mark = new int[81];
    clear();

    colorstyle = new QString[5];
    colorstyle[0] = "background-color:rgb(255,105,180)"; // mark
    colorstyle[1] = "background-color:rgb(255,140,0)"; // same number
    colorstyle[2] = "background-color:rgb(135,206,250)"; // same row/col
    colorstyle[3] = "background-color:rgb(255,250,240)"; // normal

    QPixmap pixPause(":/icon/pause.jpg");
    pauseIcon = new QIcon(pixPause);
    ui->b_pause->setIcon(*pauseIcon);
    ui->b_pause->show();
    QPixmap pixContinue(":/icon/continue.jpg");
    continueIcon = new QIcon(pixContinue);
    ui->b_continue->setIcon(*continueIcon);
    ui->b_continue->show();

    isStart = false;
    timer = new QTimer;
    TimeRecord = new QTime(0, 0, 0);
    Starttime = new QTime;
    timer->setInterval(20);
    timer->start();
    ui->LCDTimer->setDigitCount(12);
    ui->LCDTimer->setSegmentStyle(QLCDNumber::Flat);
    ui->LCDTimer->display(TimeRecord->toString("hh:mm:ss.zzz"));
    connect(timer,SIGNAL(timeout()),this,SLOT(updateTime()));

    ui->comboBox->addItem(tr("Level 0"));
    ui->comboBox->addItem(tr("Level 1"));
    ui->comboBox->addItem(tr("Level 2"));
    ui->comboBox->addItem(tr("Level 3"));
    ui->comboBox->addItem(tr("Level 4"));
    ui->comboBox->addItem(tr("Level 5"));
    ui->comboBox->addItem(tr("Level 6"));
    ui->comboBox->addItem(tr("Level 7"));
    ui->comboBox->addItem(tr("Level 8"));
    ui->comboBox->addItem(tr("Level 9"));
    ui->comboBox->addItem(tr("Level 10"));
    ui->comboBox->addItem(tr("Free"));

    myfont1 = QFont("consolas", 22);
    myfont2 = QFont("consolas", 9);


    QGridLayout **grid = new QGridLayout*[9];
    for (int i = 0; i < 9; ++i)
    {
        grid[i] = new QGridLayout();
        grid[i]->setSpacing(11);
        ui->boardLayout->addLayout(grid[i], i / 3, i % 3);
    }

    board = new QPushButton*[81];
    board_mapper = new QSignalMapper(this);
    
    for (int i = 0; i < 81; i++)
    {
        int x = i / 9, y = i % 9;
        QPushButton *b = new QPushButton(this);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        grid[x / 3 * 3 + y / 3]->addWidget(b, x % 3, y % 3);
        board[i] = b;
        board[i]->setFont(myfont1);
        board[i]->setCheckable(true);
        board[i]->setAutoExclusive(true);
        connect(board[i], SIGNAL(clicked(bool)),
                board_mapper, SLOT(map()));
        board_mapper->setMapping(board[i], i);
    }
    connect(board_mapper, SIGNAL(mapped(int)), this, SLOT(boardPressed(int)));
    focus_board(-1);
    num_mapper = new QSignalMapper(this);
    num = new QPushButton*[9];
    ui->numLayout->setSpacing(11);
    for (int i = 0; i < 9; ++i)
    {
        int x = i / 3, y = i % 3;
        QPushButton *b = new QPushButton(this);
        b->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        ui->numLayout->addWidget(b, x, y);
        num[i] = b;
        num[i]->setFont(myfont1);
        num[i]->setStyleSheet("background-color:rgb(225,255,255)");
        num[i]->setText(QString::number(i + 1));
        connect(num[i], SIGNAL(clicked(bool)), num_mapper, SLOT(map()));
        num_mapper->setMapping(num[i], i + 1);
    }
    connect(num_mapper, SIGNAL(mapped(int)), this, SLOT(numPressed(int)));


    for (int i = 0; i < 81; ++i)
    {
        board[i]->setEnabled(false);
        board[i]->setChecked(false);
    }
    ui->b_restart->setEnabled(false);
    ui->b_undo->setEnabled(false);
    ui->b_redo->setEnabled(false);
    ui->b_pause->setEnabled(false);
    ui->b_continue->setEnabled(false);
    ui->b_solve->setEnabled(false);
    ui->b_delete->setEnabled(false);
    ui->b_mark->setEnabled(false);
    for (int i = 0; i < 9; ++i)
        num[i]->setEnabled(false);
}

void MainWindow::clear()
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            if (game->board[i][j] > 0)
                guess[i][j] = 1 << (game->board[i][j] - 1);
            else
                guess[i][j] = 0;
    for (int i = 0; i < 81; ++i)
        mark[i] = 0;
    top_q = cur_q = 0;
    curButton = -1;
    curNum = 0;
}

void MainWindow::check_board()
{
    bool completed = true;
    for (int i = 0; i < 81; ++i)
    {
        int x = i / 9, y = i % 9;
        if (!(game->board[x][y] > 0 || get_num(i) > 0))
            completed = false;
    }
    if (!completed) return;
    int *tempboard = new int[81];
    for (int i = 0; i < 81; ++i)
    {
        int x = i / 9, y = i % 9;
        if (game->board[x][y] > 0)
            tempboard[i] = game->board[x][y];
        else
            tempboard[i] = get_num(i);
    }
    bool checked = true;
    for (int i = 0, x = 0, y = 0; i < 9; ++i)
    {
        int mask;
        mask = 0;
        for (int j = 0; j < 9; ++j)
            mask ^= 1 << (tempboard[i * 9 + j] - 1);
        if (mask != (1 << 9) - 1) checked = false;
        mask = 0;
        for (int j = 0; j < 9; ++j)
            mask ^= 1 << (tempboard[j * 9 + i] - 1);
        if (mask != (1 << 9) - 1) checked = false;
        mask = 0;
        for (int j = x; j < x + 3; ++j)
            for (int k = y; k < y + 3; ++k)
                mask ^= 1 << (tempboard[j * 9 + k] - 1);
        y += 3;
        if (y > 8) {x += 3; y = 0;}
        if (mask != (1 << 9) - 1) checked = false;
    }
    if (checked)
    {
        on_b_solve_clicked();
        QMessageBox::warning(this, tr("About"), tr("You Win!!!"), QMessageBox::Yes);
    }
    delete[] tempboard;
}

void MainWindow::operate(int pos, int mask)
{
    if (pos == -1 || game->board[pos / 9][pos % 9] > 0) return;
    if (cur_q >= 10)
    {
        for (int i = 0; i < 9; ++i)
            que[i] = que[i + 1];
        --cur_q;
    }
    que[cur_q].pos = pos;
    que[cur_q].mask = mask;
    int x = pos / 9, y = pos % 9;
    guess[x][y] ^= mask;
    update_board(pos);
    ++cur_q;
    top_q = cur_q;
    check_board();
}

void MainWindow::update_board(int pos)
{
    QString str = "_  ";
    int x = pos / 9, y = pos % 9;
    int mask = guess[x][y];
    int cntnum = 0, rec = 0;
    for (int i = 1; i <= 9; ++i, mask >>= 1)
    {
        if (mask & 1)
        {
            str += QString::number(i);
            ++cntnum;
            rec = i;
        }
        else str += " ";
        str += " ";
        if (i == 3 || i == 6) str += " _\r\n_  ";
    }
    str += " _";
    if (game->board[x][y] > 0) str = QString::number(game->board[x][y]);
    else if (cntnum == 0) str = "";
    else if (cntnum == 1) str = QString::number(rec);
    board[pos]->setText(str);
    if (cntnum <= 1) board[pos]->setFont(myfont1);
    else board[pos]->setFont(myfont2);
}

int MainWindow::get_num(int pos)
{
    int x = pos / 9, y = pos % 9;
    int cnt = 0, rec = -1;
    for (int i = 1, mask = guess[x][y]; i <= 9; ++i, mask >>= 1)
        if (mask & 1)
        {
            ++cnt;
            rec = i;
        }
    if (cnt > 1) return 0;
    return rec;
}

void MainWindow::focus_board(int pos)
{
    for (int i = 0; i < 81; ++i)
        update_board(i);
    int x = pos / 9, y = pos % 9, curnum = get_num(pos);
    if (pos == -1) x = y = curnum = -1;
    // maintain color
    for (int i = 0; i < 81; ++i)
    {
        int px = i / 9, py = i % 9;
        QString fontcolor = "color:";
        if (game->board[px][py] > 0)
            fontcolor += "gray";
        else
            fontcolor += "black";
        fontcolor += "; ";
        if (mark[i] == 1)
            board[i]->setStyleSheet(fontcolor + colorstyle[0]);
        else if (curnum > 0 && (game->board[px][py] == curnum || get_num(i) == curnum))
            board[i]->setStyleSheet(fontcolor + colorstyle[1]);
        else if (px == x || py == y)
            board[i]->setStyleSheet(fontcolor + colorstyle[2]);
        else
            board[i]->setStyleSheet(fontcolor + colorstyle[3]);
    }
    /*
    if (pos >= 0 && mark[pos] == 0)
        board[pos]->setStyleSheet(colorstyle[1]);
    */
}

void MainWindow::boardPressed(int pos)
{
    curButton = pos;
    //std::cout << "curbutton = " << pos << std::endl;
    update_board(pos);
    focus_board(pos);
}

void MainWindow::numPressed(int pos)
{
    curNum = pos;
    //std::cout << "curnum = " << pos << std::endl;
    //std::cout << "curButton = " << curButton << " curNum = " << curNum << std::endl;
    if (curButton != -1) operate(curButton, (1 << (curNum - 1)));
    //board[curButton]->setText(QString::number(curNum));
    focus_board(curButton);
}

MainWindow::~MainWindow()
{
    delete ui;
    for (int i = 0; i < 9; ++i)
        delete[] guess[i];
    delete[] guess;
    delete[] que;
    delete[] mark;
}

void MainWindow::on_b_newgame_clicked()
{
    if (ui->comboBox->currentIndex() <= 10)
    {
        game->gen(ui->comboBox->currentIndex());
        freemod = false;
    }
    else
    {
        for (int i = 0; i < 9; ++i)
            for (int j = 0; j < 9; ++j)
                game->board[i][j] = 0;
        freemod = true;
    }
    on_b_restart_clicked();
}

void MainWindow::on_b_restart_clicked()
{
    for (int i = 0; i < 81; ++i)
    {
        int x = i / 9, y = i % 9;
        if (game->board[x][y] != 0)
        {
            board[i]->setText(QString::number(game->board[x][y]));
            board[i]->setCheckable(true);
            board[i]->setEnabled(true);
        }
        else
        {
            board[i]->setText(tr(""));
            board[i]->setCheckable(true);
            board[i]->setEnabled(true);
        }
        board[i]->setAutoExclusive(false);
        board[i]->setChecked(false);
        board[i]->setAutoExclusive(true);
    }
    for (int i = 0; i < 81; ++i)
        board[i]->setVisible(true);
    clear();
    focus_board(-1);
    TimeRecord->setHMS(0, 0, 0);
    ui->LCDTimer->display(TimeRecord->toString("hh:mm:ss.zzz"));
    Starttime->start();
    isStart = true;
    ui->b_restart->setEnabled(true);
    ui->b_undo->setEnabled(true);
    ui->b_redo->setEnabled(true);
    ui->b_pause->setEnabled(true);
    ui->b_continue->setEnabled(false);
    ui->b_solve->setEnabled(true);
    ui->b_delete->setEnabled(true);
    ui->b_mark->setEnabled(true);
    for (int i = 0; i < 9; ++i)
        num[i]->setEnabled(true);
}

void MainWindow::on_b_solve_clicked()
{
    if (freemod)
    {
        bool completed = true;
        for (int i = 0; i < 81; ++i)
        {
            if (get_num(i) == 0)
                completed = false;
        }
        if (!completed) return;
        for (int i = 0; i < 81; ++i)
        {
            int x = i / 9, y = i % 9;
            if (get_num(i) > 0)
                game->board[x][y] = get_num(i);
        }
        bool checked = true;
        for (int i = 0, x = 0, y = 0; i < 9; ++i)
        {
            int mask;
            mask = 0;
            for (int j = 0; j < 9; ++j)
            {
                if (game->board[i][j] == 0) continue;
                if (mask & (1 << (game->board[i][j] - 1))) checked = false;
                mask ^= 1 << (game->board[i][j] - 1);
            }
            mask = 0;
            for (int j = 0; j < 9; ++j)
            {
                if (game->board[j][i] == 0) continue;
                if (mask & (1 << (game->board[j][i] - 1))) checked = false;
                mask ^= 1 << (game->board[j][i] - 1);
            }
            mask = 0;
            for (int j = x; j < x + 3; ++j)
                for (int k = y; k < y + 3; ++k)
                {
                    if (game->board[j][k] == 0) continue;
                    if (mask & (1 << (game->board[j][k] - 1))) checked = false;
                    mask ^= 1 << (game->board[j][k] - 1);
                }
            y += 3;
            if (y > 8) {x += 3; y = 0;}
        }
        if (!checked)
        {
            for (int i = 0; i < 9; ++i)
                for (int j = 0; j < 9; ++j)
                    game->board[i][j] = 0;
            return;
        }
    }
    game->solve(true, false);
    for (int i = 0; i < 81; ++i)
    {
        int x = i / 9, y = i % 9;
        board[i]->setText(QString::number(game->board[x][y]));
        board[i]->setCheckable(false);
        board[i]->setEnabled(false);
        board[i]->setChecked(false);
    }
    clear();
    focus_board(-1);
    *TimeRecord = TimeRecord->addMSecs(Starttime->elapsed());
    isStart = false;
    ui->b_restart->setEnabled(false);
    ui->b_undo->setEnabled(false);
    ui->b_redo->setEnabled(false);
    ui->b_pause->setEnabled(false);
    ui->b_continue->setEnabled(false);
    ui->b_solve->setEnabled(false);
    ui->b_delete->setEnabled(false);
    ui->b_mark->setEnabled(false);
    for (int i = 0; i < 9; ++i)
        num[i]->setEnabled(false);
}

void MainWindow::on_b_undo_clicked()
{
    if (cur_q > 0)
    {
        --cur_q;
        int x = que[cur_q].pos / 9, y = que[cur_q].pos % 9;
        guess[x][y] ^= que[cur_q].mask;
        update_board(que[cur_q].pos);
    }
    focus_board(curButton);
}

void MainWindow::on_b_redo_clicked()
{
    if (cur_q < top_q)
    {
        int x = que[cur_q].pos / 9, y = que[cur_q].pos % 9;
        guess[x][y] ^= que[cur_q].mask;
        update_board(que[cur_q].pos);
        ++cur_q;
    }
    focus_board(curButton);
}

void MainWindow::on_b_delete_clicked()
{
    if (curButton != -1 && get_num(curButton) != -1)
    {
        int x = curButton / 9, y = curButton % 9;
        operate(curButton, guess[x][y]);
    }
    focus_board(curButton);
}


void MainWindow::updateTime()
{
    QTime temptime;
    if (!isStart)
    {
        temptime = *TimeRecord;
    }
    else
    {
        temptime = TimeRecord->addMSecs(Starttime->elapsed());
    }
    ui->LCDTimer->display(temptime.toString("hh:mm:ss.zzz"));
}

void MainWindow::on_b_pause_clicked()
{
    if(isStart == true)
    {
        for (int i = 0; i < 81; ++i)
            board[i]->setVisible(false);
        *TimeRecord = TimeRecord->addMSecs(Starttime->elapsed());
        isStart = false;
        ui->b_undo->setEnabled(false);
        ui->b_redo->setEnabled(false);
        ui->b_pause->setEnabled(false);
        ui->b_continue->setEnabled(true);
        ui->b_solve->setEnabled(false);
        ui->b_delete->setEnabled(false);
        ui->b_mark->setEnabled(false);
        for (int i = 0; i < 9; ++i)
            num[i]->setEnabled(false);
    }
}

void MainWindow::on_b_continue_clicked()
{
    if (isStart == false)
    {
        for (int i = 0; i < 81; ++i)
            board[i]->setVisible(true);
        Starttime->start();
        isStart = true;
        ui->b_undo->setEnabled(true);
        ui->b_redo->setEnabled(true);
        ui->b_pause->setEnabled(true);
        ui->b_continue->setEnabled(false);
        ui->b_solve->setEnabled(true);
        ui->b_delete->setEnabled(true);
        ui->b_mark->setEnabled(true);
        for (int i = 0; i < 9; ++i)
            num[i]->setEnabled(true);
    }
}

void MainWindow::on_b_mark_clicked()
{
    mark[curButton] ^= 1;
    focus_board(curButton);
}

void MainWindow::on_actionplaymusic_triggered()
{
    bgm->play();
}

void MainWindow::on_actionstopmusic_triggered()
{
    bgm->stop();
}

void MainWindow::on_actionNewGame_triggered()
{
    on_b_newgame_clicked();
}
