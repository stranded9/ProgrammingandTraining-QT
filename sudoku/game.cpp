#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <cstring>
#include <ctime>

#include <QObject>
#include "game.h"
using namespace std;

//Game::Game(QObject *parent) : QObject(parent)
Game::Game()
{
    srand(time(0));
    board = new int*[9];
    for (int i = 0; i < 9; ++i)
        board[i] = new int[9];
    init();

    seq.reserve(100);
    row.reserve(10); col.reserve(10); blk.reserve(10);
}

Game::~Game()
{
    for (int i = 0; i < 9; ++i)
        delete[] board[i];
    delete[] board;

}

void Game::print_board()
{
    cout << "===========================" << endl;
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
        {
            if (j % 3 == 0) cout << " | "; else cout << " ";
            cout << board[i][j];
        }
        cout << " | " << endl;
        if (i == 2 || i == 5)
            cout << " -------------------------" << endl;
    }
    cout << "===========================" << endl;
}

void Game::init()
{
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            board[i][j] = 0;
}

void Game::solve(bool getsol, bool printboard)
{
    for (int i = 0; i < 9; ++i)
        row[i] = col[i] = blk[i] = (1 << 9) - 1;
    for (int i = 0; i < 81; ++i)
    {
        int x = i / 9, y = i % 9;
        if (board[x][y] == 0) continue;
        int b = x / 3 * 3 + y / 3;
        int bin = 1 << (board[x][y] - 1);
        row[x] ^= bin;
        col[y] ^= bin;
        blk[b] ^= bin;
    }
    
    int cnt = 0;
    for (int i = 0; i < 81; ++i) seq[i] = -1;
    for (int i = 0; i < 81; ++i)
    {
        int x = i / 9, y = i % 9;
        if (board[x][y] == 0) seq[cnt++] = i;
    }
    if (cnt == 0) return;

    for (int i = 0; i < 100; ++i)
        swap(seq[rand() % cnt], seq[rand() % cnt]);
    int* tmp = new int[90];
    for (int i = 0; i < 81; ++i) tmp[i] = 0;
    for (int i = 0; i < 81; ++i)
    {
        int x = i / 9, y = i % 9, b = x / 3 * 3 + y / 3;
        if (board[x][y] > 0) continue;
        int mask = row[x] & col[y] & blk[b];
        tmp[i] = 0;
        for (; mask; mask -= mask & (-mask)) ++tmp[i];
    }

    for (int i = 0; i < cnt - 1; ++i)
    {
        for (int j = cnt - 1; j > i; --j)
            if (tmp[seq[j - 1]] > tmp[seq[j]])
                swap(seq[j - 1], seq[j]);
        int p1 = seq[i], x1 = p1 / 9, y1 = p1 % 9;
        for (int j = i + 1; j < cnt; ++j)
        {
            int p2 = seq[j], x2 = p2 / 9, y2 = p2 % 9;
            if (x1 == x2 || y1 == y2 ||
                x1 / 3 * 3 + y1 / 3 == x2 / 3 * 3 + y2 / 3)
                --tmp[p2];
        }
    }
    delete[] tmp;
    /*
    for (int i = 0; i < 81; ++i)
    {
        if (i % 9 == 0) cout << endl;
        int pos;
        for (int j = 0; j < 81; ++j)
            if (seq[j] == i) pos = j;
        cout << pos << "\t";
    }
    cout << endl;
    */
    
    //cout << "cnt = " << cnt << endl << "seq: ";
    //for (int i = 0; i < cnt; ++i) cout << seq[i] << " ";
    //cout << endl;
    
    cnt_ans = 0;
    dfs(0, cnt, getsol, printboard);
}

void Game::dfs(int cur, int lim, bool getsol, bool printboard)
{
    if (cur >= lim)
    {
        ++cnt_ans;
        if (printboard) print_board();
        return;
    }
    int x = seq[cur] / 9, y = seq[cur] % 9;
    int b = x / 3 * 3 + y / 3;
    int mask = row[x] & col[y] & blk[b];
    for (; mask; mask -= mask & (-mask))
    {
        int bit = mask & (-mask), num = 0;
        for (int tmp = bit; tmp; tmp >>= 1) ++num;
        board[x][y] = num;
        row[x] ^= bit; col[y] ^= bit; blk[b] ^= bit;
        dfs(cur + 1, lim, getsol, printboard);
        if ((cnt_ans == 1 && getsol) || cnt_ans >= 2) return;
        board[x][y] = 0;
        row[x] ^= bit; col[y] ^= bit; blk[b] ^= bit;
    }
}

void Game::readinboard()
{
    init();
    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 9; ++j)
            cin >> board[i][j];
    }
}

void Game::gen(int level)
{
    srand(time(0));
    init();
    solve(true, false);
    int** tmparr;
    tmparr = new int*[9];
    for (int i = 0; i < 9; ++i)
    {
        tmparr[i] = new int[9];
        for (int j = 0; j < 9; ++j)
            tmparr[i][j] = board[i][j];
    }
    int* order = new int[81];
    for (int i = 0; i < 81; ++i) order[i] = i;
    int cnt_hole = level * 5 + (rand() % 5);
    for (int i = 0; i < 100; ++i)
        swap(order[rand() % 81], order[rand() % 81]);
    for (int k = 0; k < 81; ++k)
    {
        int x = order[k] / 9, y = order[k] % 9;
        for (int i = 0; i < 9; ++i)
            for (int j = 0; j < 9; ++j)
                board[i][j] = tmparr[i][j];
        board[x][y] = 0;
        solve(false, false);
        if (cnt_ans == 1)
        {
            tmparr[x][y] = 0;
            --cnt_hole;
        }
        if (cnt_hole <= 0) break;
    }
    for (int i = 0; i < 9; ++i)
        for (int j = 0; j < 9; ++j)
            board[i][j] = tmparr[i][j];
    for (int i = 0; i < 9; ++i)
        delete[] tmparr[i];
    delete[] tmparr;
    //print_board();
}

/*
int main()
{
    Game* game = new Game();
    //game->readinboard();
    game->gen();
    game->solve(true);
    return 0;
}
*/
