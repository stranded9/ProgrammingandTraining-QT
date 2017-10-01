/* -*- C++ -*- */
#ifndef GAME_H_
#define GAME_H_

#include <QObject>
#include <vector>
using std::vector;

class Game //: public QObject
{
    //Q_OBJECT
public:
    //Game(QObject *parent = 0);
    Game();
    ~Game();
    void print_board();
    void init();
    void solve(bool getsol, bool printboard);
    void dfs(int cur, int lim, bool getsol, bool printboard);
    void gen(int level);

    void readinboard();
    
    int** board;
    int cnt_ans;
private:
    vector<int> seq, row, col, blk;
};

#endif // GAME_H_
