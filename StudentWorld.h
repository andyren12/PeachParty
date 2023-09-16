#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "Actor.h"
#include "GameWorld.h"
#include "Board.h"
#include <string>
#include <list>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    Board* getBoard() { return board; }
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    void push(Actor* actor) { actors.push_back(actor); }
    Player* getPeach() { return peach; }
    Player* getYoshi() { return yoshi; }
    int getBank() { return bank; }
    void setBank(int x) { bank += x; }
    void swapPlayers(Player* p1, Player* p2);
    void swapCoins();
    void swapStars();
    void placeDropSquare(Actor* actor);
    bool overlapping(Actor* a, Actor* b);
    bool overlapsVortex(Actor* a, Actor* b);
    Actor* isHitByVortex(Actor* actor);
    int peachWins();
    int yoshiWins();
private:
    std::vector<Actor*> actors;
    Board* board;
    Player* peach;
    Player* yoshi;
    Bowser* bowser;
    Boo* boo;
    CoinSquare* bluecoinsquare;
    CoinSquare* redcoinsquare;
    StarSquare* starsquare;
    DirSquare* updirsquare;
    DirSquare* downdirsquare;
    DirSquare* leftdirsquare;
    DirSquare* rightdirsquare;
    DroppingSquare* droppingsquare;
    BankSquare* banksquare;
    EventSquare* eventsquare;
    int bank;
};

#endif // STUDENTWORLD_H_
