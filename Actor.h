#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "Board.h"

class StudentWorld;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject {
public:
    static const int WAITING_TO_ROLL = 0;
    static const int WALKING = 1;
    
    Actor(int imageID, double startX, double startY, StudentWorld* world, int dir = right, int depth = 0, double size = 1.0);
    virtual void doSomething() = 0;
    int getTicks() { return ticks_to_move; }
    void adjustTicks(int x) { ticks_to_move += x; }
    StudentWorld* getWorld() { return mWorld; }
    bool isAlive() { return alive; }
    void setAlive(bool x) { alive = x; }
    bool vortexable() { return canVortex; }
    void setVortexable(bool x) { canVortex = x; }
    bool gotVortexed() { return vortexed; }
    void setVortexed(bool x) { vortexed = x; }
    bool isSquare() { return square; }
    void setSquare(bool x) { square = x; }
    bool teleported() { return gotTeleported; }
    void setTeleport(bool x) { gotTeleported = x; }
    bool landedOn(Actor* actor);
    Board::GridEntry nextPos();
    void moveinDirection();
    bool atFork();
    void teleport();
    void adjustPos();
private:
    StudentWorld* mWorld;
    int ticks_to_move;
    bool alive;
    bool canVortex;
    bool vortexed;
    bool square;
    bool gotTeleported;
};

class Player : public Actor {
public:
    Player(int imageID, double startX, double startY, StudentWorld* world, int playerNum);
    virtual void doSomething();
    bool isWalking() { return state; }
    int getRoll() { return roll; }
    int getCoins() { return coins; }
    int getStars() { return stars; }
    bool hasVortex() { return vortex; }
    void setVortex(bool x) { vortex = x; }
    int getState() { return state; }
    void setState(bool x) { state = x; }
    void adjustCoins(int x) { coins += x; }
    void adjustStars(int x) { stars += x; }
    bool wasAffected() { return affected; }
    void setAffected(bool x) { affected = x; }
    void chooseDir();
private:
    int state;
    int mPlayer;
    int roll;
    int coins;
    int stars;
    bool vortex;
    bool affected;
};

class Interactable : public Actor {
public:
    Interactable(int imageID, double startX, double startY, StudentWorld* world, int dir = right, int depth = 1);
    virtual void doSomething() { };
    bool movingPast(Player* player);
    Player* peach();
    Player* yoshi();
    Player* player();
};

class CoinSquare : public Interactable {
public:
    CoinSquare(int imageID, double startX, double startY, StudentWorld* world, int change);
    virtual void doSomething();
private:
    int mChange;
};

class StarSquare: public Interactable {
public:
    StarSquare(int imageID, double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class DirSquare: public Interactable {
public:
    DirSquare(int imageID, double startX, double startY, StudentWorld* world, int dir);
    virtual void doSomething();
private:
    int direction;
};

class BankSquare: public Interactable {
public:
    BankSquare(int imageID, double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class EventSquare: public Interactable {
public:
    EventSquare(int imageID, double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class DroppingSquare: public Interactable {
public:
    DroppingSquare(int imageID, double startX, double startY, StudentWorld* world);
    virtual void doSomething();
};

class Enemy : public Interactable {
public:
    static const int PAUSED = 0;
    static const int WALKING = 1;
    
    Enemy(int imageID, double startX, double startY, StudentWorld* world);
    virtual void doSomething();
    virtual void activityOne() = 0;
    virtual void activityTwo() = 0;
    int getDistance() { return distance; }
    void setDistance(int x) { distance = x; }
    int getState() { return state; }
    void setState(int x) { state = x; }
    int getPause() { return pausecounter; }
    void setPause(int x) { pausecounter += x; }
    bool isNew() { return newObj; }
    void setNew(bool x) { newObj = x; }
    void randomDir();
    void impact();
private:
    int distance;
    int state;
    int pausecounter;
    bool newObj;
};

class Bowser : public Enemy {
public:
    Bowser(int imageID, double startX, double startY, StudentWorld* world);
    virtual void activityOne();
    virtual void activityTwo();
};

class Boo : public Enemy {
public:
    Boo(int imageID, double startX, double startY, StudentWorld* world);
    virtual void activityOne();
    virtual void activityTwo() { }
};

class Vortex : public Interactable {
public:
    Vortex(int imageID, double startX, double startY, StudentWorld* world, int dir);
    virtual void doSomething();
};
#endif // ACTOR_H_
