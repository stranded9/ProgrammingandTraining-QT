#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>

#include "game.h"
using namespace std;

// class: Move

Move::Move(int _x, int _y, Move* _pre, int _dep)
    : x(_x), y(_y), pre(_pre), len(0), dep(_dep), flag(-1)
{
    nxt.clear();
}

Move::~Move()
{
    for (int i = 0; i < len; ++i)
        delete nxt[i];
}

void Move::add(Move *mov)
{
    nxt.push_back(mov);
    ++len;
}

// class: Game

Game::Game()
{
    board = new int*[10];
    for (int i = 0; i < 10; ++i)
        board[i] = new int[10];
    clearBoard();

    mov_board = new Move**[10];
    for (int i = 0; i < 10; ++i)
    {
        mov_board[i] = new Move*[10];
        for (int j = 0; j < 10; ++j)
            mov_board[i][j] = NULL;
    }

    vis = new int*[10];
    for (int i = 0; i < 10; ++i)
        vis[i] = new int[10];
    mark = new int*[10];
    for (int i = 0; i < 10; ++i)
        mark[i] = new int[10];
}

Game::~Game()
{
    for (int i = 0; i < 10; ++i)
        delete[] board[i];
    delete[] board;
    clearMov();
    for (int i = 0; i < 10; ++i)
        delete[] mov_board[i];
    delete[] mov_board;
    for (int i = 0; i < 10; ++i)
        delete[] vis[i];
    delete[] vis;
    for (int i = 0; i < 10; ++i)
        delete[] mark[i];
    delete[] mark;
}

void Game::readinBoard()
{
    ifstream fin("/Users/clbq/Documents/program/qt/draughts/board.txt");
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            fin >> board[i][j];
}

void Game::printBoard()
{
    cout << "=======================" << endl;
    cout << "   ";
    for (int i = 0; i < 10; ++i) cout << i << " ";
    cout << endl;
    for (int i = 0; i < 10; ++i)
    {
        cout << " " << i << " ";
        for (int j = 0; j < 10; ++j)
        {
            if (board[i][j] == 0) cout << ".";
            else if (board[i][j] == 4) cout << "w";
            else if (board[i][j] == 5) cout << "b";
            else if (board[i][j] == 6) cout << "W";
            else if (board[i][j] == 7) cout << "B";
            cout << " ";
        }
        cout << endl;
    }
    cout << "=======================" << endl;
}

void Game::clearBoard()
{
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            board[i][j] = 0;
}

void Game::initBoard()
{
    clearBoard();
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            if ((i ^ j) & 1)
            {
                if (i < 4) board[i][j] = 5;
                if (i > 5) board[i][j] = 4;
            }
    clearMov();
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            vis[i][j] = mark[i][j] = 0;
}

void Game::clearMov()
{
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            if (mov_board[i][j] != NULL) delete mov_board[i][j];
            mov_board[i][j] = NULL;
        }
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            mark[i][j] = 0;
}

inline bool Game::checkpos(int x, int y)
{
    if (x >= 0 && x <= 9 && y >= 0 && y <= 9)
        return true;
    else
        return false;
}

void Game::printmov(Move *cur)
{
    if (cur == NULL) return;
    // output info
    cout << "(" << cur->x << ", " << cur->y << ") dep = "<< cur->dep << " flag = " << cur->flag;
    if (cur->pre != NULL) cout << " pre = (" << cur->pre->x << ", " << cur->pre->y << ")" << endl;
    else cout << endl;
    
    for (int i = 0; i < cur->len; ++i)
        printmov(cur->nxt[i]);
}


void Game::findMove(int x, int y)
{
    mov_board[x][y] = new Move(x, y, NULL, -1);

    int c = board[x][y] & 1;
    bool isking = ((board[x][y] & 2) > 0);
    int delx = 0, dely = 0;
    if (!isking) // not king
    {
        delx = (c ? +1 : -1);
        dely = -1;
        for (int i = 1; i <= 2; ++i, dely = -dely)
        {
            int xx = x + delx, yy = y + dely;
            if (checkpos(xx, yy) && board[xx][yy] == 0)
            {
                mov_board[x][y]->add(new Move(xx, yy, mov_board[x][y], 0));
                maxdep = max(maxdep, 0);
            }
        }
    }
    else // king
    {
        delx = -1; dely = -1;
        for (int i = 1; i <= 2; ++i, delx = -delx)
            for (int j = 1; j <= 2; ++j, dely = -dely)
            {
                int xx = x + delx, yy = y + dely;
                while (checkpos(xx, yy) && board[xx][yy] == 0)
                {
                    mov_board[x][y]->add(new Move(xx, yy, mov_board[x][y], 0));
                    maxdep = max(maxdep, 0);
                    xx += delx;
                    yy += dely;
                }
            }
    }
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            vis[i][j] = 0;
    curx = x; cury = y;
    searchMove(mov_board[x][y], 1, c, isking);
}

void Game::searchMove(Move *cur, int dep, int c, bool isking)
{
    int x = cur->x, y = cur->y;
    int delx = 0, dely = 0;
    if (!isking)
    {
        delx = -1; dely = -1;
        for (int i = 1; i <= 2; ++i, delx = -delx)
            for (int j = 1; j <= 2; ++j, dely = -dely)
            {
                int xx = x + delx * 2, yy = y + dely * 2;
                if (checkpos(xx, yy)
                    && board[xx][yy] == 0
                    && !vis[x + delx][y + dely]
                    && (board[x + delx][y + dely] & 4)
                    && ((board[x + delx][y + dely] & 1) ^ c) )
                {
                    vis[x + delx][y + dely] = 1;
                    cur->add(new Move(xx, yy, cur, dep));
                    maxdep = max(maxdep, dep);
                    searchMove(cur->nxt[cur->len - 1], dep + 1, c, isking);
                    vis[x + delx][y + dely] = 0;
                }
            }
    }
    else
    {
        delx = -1; dely = -1;
        for (int i = 1; i <= 2; ++i, delx = -delx)
            for (int j = 1; j <= 2; ++j, dely = -dely)
            {
                int xx = x + delx, yy = y + dely;
                int xpos = -1, ypos = -1;
                while(checkpos(xx, yy))
                {
                    if ((board[xx][yy] & 4) && (board[xx][yy] & 1) == c &&
                            (curx != xx || cury != yy))
                    {
                        break;
                    }
                    else if ((board[xx][yy] & 4) && (board[xx][yy] & 1) != c)
                    {
                        if (xpos != -1) break;
                        xpos = xx; ypos = yy;
                        if (vis[xpos][ypos]) break;
                    }
                    else if (board[xx][yy] == 0 && xpos != -1)
                    {
                        vis[xpos][ypos] = 1;
                        cur->add(new Move(xx, yy, cur, dep));
                        maxdep = max(maxdep, dep);
                        searchMove(cur->nxt[cur->len - 1], dep + 1, c, isking);
                        vis[xpos][ypos] = 0;
                    }
                    xx += delx;
                    yy += dely;
                }
            }
    }
}

void Game::dfsMov(Move* cur)
{
    if (cur == NULL) return;
    if (cur->dep == maxdep)
    {
        for (Move* pointer = cur; pointer != NULL; pointer = pointer->pre)
            pointer->flag = 0;
    }
    for (int i = 0; i < cur->len; ++i)
        dfsMov(cur->nxt[i]);
}

void Game::genMovboard(int c)
{
    //cout << "c = " << c << endl;
    clearMov();
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            vis[i][j] = mark[i][j] = 0;
    maxdep = -1;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            if ((board[i][j] & 4) && ((board[i][j] & 1) == c))
                findMove(i, j);
    /*
    //cout << "maxdep = " << maxdep << endl;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            printmov(mov_board[i][j]);
            */
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            dfsMov(mov_board[i][j]);
    /*
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
            printmov(mov_board[i][j]);
            */
}

void Game::applyMov(Move *mov)
{
    if (mov == NULL || mov->pre == NULL) return;
    Move *pre = mov->pre;
    if (pre->flag != 1 && pre->dep != -1) return;
    if (mov->flag != 0) return;
    int delx, dely;
    delx = (mov->x > pre->x) ? +1 : -1;
    dely = (mov->y > pre->y) ? +1 : -1;
    int xx = pre->x + delx, yy = pre->y + dely;
    while (xx != mov->x || yy != mov->y)
    {
        if (board[xx][yy] > 0) mark[xx][yy] = 1;
        xx += delx;
        yy += dely;
    }
    mark[pre->x][pre->y] = 2;
    board[mov->x][mov->y] = board[pre->x][pre->y];
    board[pre->x][pre->y] = 0;
    mov->flag = 1;
    if (mov->dep != -1 && mov->len == 0)
        updateBoard();
    //printBoard();
}

void Game::updateBoard()
{
    for (int j = 0; j < 10; ++j)
        if ((board[0][j] & 4) && (board[0][j] & 1) == 0)
            board[0][j] |= 2;
    for (int j = 0; j < 10; ++j)
        if ((board[9][j] & 4) && (board[9][j] & 1) == 1)
            board[9][j] |= 2;
    for (int i = 0; i < 10; ++i)
        for (int j = 0; j < 10; ++j)
        {
            if (mark[i][j] == 1) board[i][j] = 0;
            mark[i][j] = 0;
        }
}

/*

bool ffff = false;

void test(Move* cur, Game* game)
{
    game->applyMov(cur);
    if (cur->dep == game->maxdep) {ffff = true; return;}
    for (int i = 0; i < cur->len; ++i)
    {
        if (cur->nxt[i]->flag == 0) test(cur->nxt[i], game);
        if (ffff) break;
    }
}

int main()
{
    Game *game = new Game();
    //game->printBoard();
    //game->initBoard();
    //game->printBoard();
    game->readinBoard();
    game->printBoard();
    game->genMovboard(0);

    //test(game->mov_board[0][3], game);
    return 0;
}

*/
