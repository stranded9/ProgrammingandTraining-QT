/* -*- C++ -*- */
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <QMainWindow>
#include <QPushButton>
#include <QSignalMapper>
#include <QFont>
#include <QTime>
#include <QTimer>
#include <QSound>
#include <QPixmap>
#include <QIcon>
#include "game.h"

namespace Ui {
    class MainWindow;
}

struct Ope
{
    int pos, mask;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private:
    void clear();
    void operate(int pos, int mask);
    void update_board(int pos);
    void focus_board(int pos);
    int get_num(int pos);
    Ui::MainWindow *ui;
    QPushButton **board;
    QPushButton **num;
    Game *game;
    QSignalMapper *board_mapper, *num_mapper;
    int **guess;
    Ope *que;
    int top_q, cur_q;
    int *mark;
    
    int curButton;
    int curNum;

    QFont myfont1, myfont2;

    QTimer * timer;
    QTime * TimeRecord;
    QTime * Starttime;
    bool isStart;
    QSound *bgm;
    QIcon *pauseIcon, *continueIcon;
    QString *colorstyle;
    bool freemod;
private slots:
    void boardPressed(int pos);
    void numPressed(int pos);
    void check_board();
    void on_b_newgame_clicked();
    void on_b_solve_clicked();
    void on_b_undo_clicked();
    void on_b_redo_clicked();
    void on_b_delete_clicked();

    void updateTime();
    void on_b_pause_clicked();
    void on_b_continue_clicked();
    void on_b_restart_clicked();
    void on_b_mark_clicked();
    void on_actionplaymusic_triggered();
    void on_actionstopmusic_triggered();
    void on_actionNewGame_triggered();
};

#endif // MAINWINDOW_H
