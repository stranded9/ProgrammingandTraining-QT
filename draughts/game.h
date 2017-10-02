/* -*- C++ -*- */
#ifndef GAME_H_
#define GAME_H_

#include <vector>
using std::vector;

class Move
{
public:
    Move(int _x, int _y, Move* _pre, int _dep);
    ~Move();
    void add(Move *mov);
    
    int x, y;
    vector<Move*> nxt;
    Move* pre;
    int len;
    int dep;
    int flag;
};

/*
  class Game:
  encoding:
  000 = 0
  100 = 4 white
  101 = 5 black
  110 = 6 white king
  111 = 7 black king
 */
class Game
{
public:
    Game();
    ~Game();
    void readinBoard();
    void printBoard();
    void clearBoard();
    void initBoard();
    void clearMov();
    bool checkpos(int x, int y);
    void printmov(Move *cur);
    void findMove(int x, int y);
    void searchMove(Move *cur, int dep, int c, bool isking);
    void dfsMov(Move *cur);
    void genMovboard(int c);
    void applyMov(Move *mov);
    void updateBoard();
    
    int **board;
    Move ***mov_board;
    int **vis;
    int maxdep;
    int **mark;
    int curx, cury;
};

#endif // GAME_H_
