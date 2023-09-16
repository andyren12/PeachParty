#include <random>
#include "Actor.h"
#include "StudentWorld.h"
#include <string>
using namespace std;

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int imageID, double startX, double startY, StudentWorld* world, int dir, int depth, double size)
: GraphObject(imageID, startX, startY, dir, depth, size), mWorld(world), ticks_to_move(0), alive(true), canVortex(false), square(false) { }

Player::Player(int imageID, double startX, double startY, StudentWorld* world, int playerNum)
: Actor(imageID, startX, startY, world), state(WAITING_TO_ROLL), mPlayer(playerNum), roll(0), coins(0), stars(0), vortex(false), affected(false) { }

Interactable::Interactable(int imageID, double startX, double startY, StudentWorld* world, int dir, int depth)
: Actor(imageID, startX, startY, world, dir, depth) { }

CoinSquare::CoinSquare(int imageID, double startX, double startY, StudentWorld* world, int change)
: Interactable(imageID, startX, startY, world), mChange(change) { setSquare(true); }

StarSquare::StarSquare(int imageID, double startX, double startY, StudentWorld* world)
: Interactable(imageID, startX, startY, world) { setSquare(true); }

DirSquare::DirSquare(int imageID, double startX, double startY, StudentWorld* world, int dir)
: Interactable(imageID, startX, startY, world, dir), direction(dir) { setSquare(true); }

BankSquare::BankSquare(int imageID, double startX, double startY, StudentWorld* world)
: Interactable(imageID, startX, startY, world) { setSquare(true); }

EventSquare::EventSquare(int imageID, double startX, double startY, StudentWorld* world)
: Interactable(imageID, startX, startY, world) { setSquare(true); }

DroppingSquare::DroppingSquare(int imageID, double startX, double startY, StudentWorld* world)
: Interactable(imageID, startX, startY, world) { }

Enemy::Enemy(int imageID, double startX, double startY, StudentWorld* world)
: Interactable(imageID, startX, startY, world, right, 0), distance(0), state(PAUSED), pausecounter(180), newObj(false) { setVortexable(true); }

Bowser::Bowser(int imageID, double startX, double startY, StudentWorld* world)
: Enemy(imageID, startX, startY, world) { setDistance(10); }

Boo::Boo(int imageID, double startX, double startY, StudentWorld* world)
: Enemy(imageID, startX, startY, world) { setDistance(3); }

Vortex::Vortex(int imageID, double startX, double startY, StudentWorld* world, int dir)
: Interactable(imageID, startX, startY, world, dir, 0) { }

//Actor functions

//checks if actor will land on another actor (usually player on square)
bool Actor::landedOn(Actor* actor)
{
    if (actor->getTicks() == 1)
    {
        if (actor->getDirection() == right)
        {
            if ((actor->getX() + 2)/16 == getX()/16 && actor->getY()/16 == getY()/16) return true;
        } else if (actor->getDirection() == up)
        {
            if (actor->getX()/16 == getX()/16 && (actor->getY() + 2)/16 == getY()/16) return true;
        } else if (actor->getDirection() == left)
        {
            if ((actor->getX() - 2)/16 == getX()/16 && actor->getY()/16 == getY()/16) return true;
        } else if (actor->getDirection() == down)
        {
            if (actor->getX()/16 == getX()/16 && (actor->getY() - 2)/16 == getY()/16) return true;
        }
    }
    return false;
}

//returns identity of next grid square
Board::GridEntry Actor::nextPos()
{
    int x = 0;
    int y = 0;
    switch(getDirection())
    {
        case right:
            x = getX()/16 + 1;
            y = getY()/16;
            break;
        case up:
            x = getX()/16;
            y = getY()/16 + 1;
            break;
        case left:
            x = (getX() + 15)/16 - 1;
            y = getY()/16;
            break;
        case down:
            x = getX()/16;
            y = (getY() + 15)/16 - 1;
            break;
    }
    return getWorld()->getBoard()->getContentsOf(x, y);
}

//turns correctly at corners
void Actor::moveinDirection()
{
    if (getTicks() % 8 == 0 && nextPos() == Board::empty)
    {
        int dir = getDirection();
        if (dir == right || dir == left)
        {
            setDirection(up);
            if (nextPos() == Board::empty) setDirection(down);
        } else if (dir == up || dir == down)
        {
            setDirection(right);
            if (nextPos() == Board::empty) setDirection(left);
        }
    }
    moveAtAngle(getDirection(), 2);
    adjustTicks(-1);
}

//checks if actor is at fork
bool Actor::atFork()
{
    //if actor teleported to corner, not at fork
    if (teleported())
    {
        setTeleport(false);
        return false;
    }
    
    Board::GridEntry pos = getWorld()->getBoard()->getContentsOf(getX()/16,getY()/16);
    if (pos == Board::right_dir_square || pos == Board::up_dir_square || pos == Board::left_dir_square || pos == Board::down_dir_square || pos == Board::player) return false;
    
    int forkCount = 0;
    int originalDir = getDirection();
    
    //counts directions that can be moved
    if (getTicks() % 8 == 0)
    {
        if (nextPos() != Board::empty) forkCount++;
        
        if (getDirection() == up || getDirection() == down)
        {
            setDirection(left);
            if (nextPos() != Board::empty) forkCount++;
            setDirection(right);
            if (nextPos() != Board::empty) forkCount++;
        } else if (getDirection() == left || getDirection() == right)
        {
            setDirection(up);
            if (nextPos() != Board::empty) forkCount++;
            setDirection(down);
            if (nextPos() != Board::empty) forkCount++;
        }
    }
    setDirection(originalDir);
    return (forkCount > 1);
}

//teleports player to random location
void Actor::teleport()
{
    bool valid = false;
    while (!valid)
    {
        int x = randInt(0,16);
        int y = randInt(0,16);
        if (getWorld()->getBoard()->getContentsOf(x, y) != Board::empty)
        {
            valid = true;
            moveTo(x*16,y*16);
        }
    }
    //resets player's direction at new location
    switch (getDirection())
    {
        case right:
        case left:
            if (nextPos() == Board::empty) setDirection(up);
            if (nextPos() == Board::empty) setDirection(down);
            break;
        case up:
        case down:
            if (nextPos() == Board::empty) setDirection(right);
            if (nextPos() == Board::empty) setDirection(left);
            break;
    }
    //adjusts player position to allow teleported square to activate
    if (this == getWorld()->getPeach() || this == getWorld()->getYoshi()) adjustPos();
    setTeleport(true);
}

//moves player back 1 tick so they can land on square
void Actor::adjustPos()
{
    int dir = getDirection();
    if (dir <= 180) setDirection(dir + 180);
    else setDirection(dir - 180);
    adjustTicks(1);
    moveinDirection();
    setDirection(dir);
}

//takes player input to choose valid direction to move
void Player::chooseDir()
{
    int dir = getDirection();
    int action = getWorld()->getAction(mPlayer);
    
    if (action == ACTION_RIGHT && dir != left)
    {
        setDirection(right);
        if (nextPos() != Board::empty) moveinDirection();
        else setDirection(dir);
    } else if (action == ACTION_UP && dir != down)
    {
        setDirection(up);
        if (nextPos() != Board::empty) moveinDirection();
        else setDirection(dir);
    } else if (action == ACTION_LEFT && dir != right)
    {
        setDirection(left);
        if (nextPos() != Board::empty) moveinDirection();
        else setDirection(dir);
    } else if (action == ACTION_DOWN && dir != up)
    {
        setDirection(down);
        if (nextPos() != Board::empty) moveinDirection();
        else setDirection(dir);
    } else return;
}

void Player::doSomething()
{
    if (!isWalking())
    {
        switch(getWorld()->getAction(mPlayer))
        {
            case ACTION_ROLL:
                roll = randInt(1,10);
                adjustTicks(roll * 8);
                state = WALKING;
                break;
            case ACTION_FIRE:
                if (hasVortex())
                {
                    Vortex* v = new Vortex(IID_VORTEX, getX(), getY(), getWorld(), getDirection());
                    getWorld()->push(v);
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    setVortex(false);
                }
                break;
            default:
                break;
        }
    } else {
        setAffected(false);
        if (getTicks() == 0)
        {
            state = WAITING_TO_ROLL;
            return;
        }
        //if at fork, wait until player chooses direction before moving, otherwise move in valid direction
        if (atFork())
        {
            chooseDir();
        } else moveinDirection();
    }
}

//checks if player is moving past and not landing on square
bool Interactable::movingPast(Player* player)
{
    if (player->getTicks() != 0 && player->getTicks() != player->getRoll() * 8 && getWorld()->overlapping(this, player)) return true;
    return false;
}

Player* Interactable::peach()
{
    return getWorld()->getPeach();
}

Player* Interactable::yoshi()
{
    return getWorld()->getYoshi();
}

Player* Interactable::player()
{
    if (landedOn(peach()) || movingPast(peach())) return peach();
    if (landedOn(yoshi()) || movingPast(yoshi())) return yoshi();
    return peach();
}

void CoinSquare::doSomething()
{
    if (landedOn(player()))
    {
        if (mChange < 0 && player()->getCoins() < 3) player()->adjustCoins(-player()->getCoins());
        else player()->adjustCoins(mChange);
        if (mChange > 0) getWorld()->playSound(SOUND_GIVE_COIN);
        else getWorld()->playSound(SOUND_TAKE_COIN);
    }
}

void StarSquare::doSomething()
{
    if (landedOn(player()) || movingPast(player()))
    {
        if (player()->getCoins() < 20) return;
        player()->adjustCoins(-20);
        player()->adjustStars(1);
        getWorld()->playSound(SOUND_GIVE_STAR);
    }
}

void DirSquare::doSomething()
{
    if (getWorld()->overlapping(this, peach())) peach()->setDirection(direction);
    if (getWorld()->overlapping(this, yoshi())) yoshi()->setDirection(direction);
}

void BankSquare::doSomething()
{
    if (landedOn(player()))
    {
        int balance = getWorld()->getBank();
        player()->adjustCoins(balance);
        getWorld()->setBank(-balance);
        getWorld()->playSound(SOUND_WITHDRAW_BANK);
    } else if (movingPast(player()))
    {
        if (player() -> getCoins() >= 5)
        {
            player()->adjustCoins(-5);
            getWorld()->setBank(5);
        } else{
            getWorld()->setBank(player()->getCoins());
            player()->adjustCoins(-player()->getCoins());
        }
        getWorld()->playSound(SOUND_DEPOSIT_BANK);
    }
}

void EventSquare::doSomething()
{
    if (landedOn(player()))
    {
        switch(randInt(1,3))
        {
            case 1: {
                player()->teleport();
                getWorld()->playSound(SOUND_PLAYER_TELEPORT);
                break;
            }
            case 2: {
                Player* p2 = nullptr;
                player() == peach() ? p2 = yoshi() : p2 = peach();
                getWorld()->swapPlayers(player(), p2);
                break;
            }
            case 3: {
                if (!player()->hasVortex())
                {
                    player()->setVortex(true);
                    getWorld()->playSound(SOUND_GIVE_VORTEX);
                }
                break;
            }
        }
    }
}

void DroppingSquare::doSomething()
{
    if (landedOn(player()))
    {
        switch(randInt(1,2))
        {
            case 1:
                if (player()->getCoins() >= 10) player()->adjustCoins(-10);
                else player()->adjustCoins(-player()->getCoins());
                break;
            case 2:
                if (player()->getStars() >= 1) player()->adjustStars(-1);
                break;
        }
        getWorld()->playSound(SOUND_DROPPING_SQUARE_ACTIVATE);
    }
}

//chooses random valid direction to move in
void Enemy::randomDir()
{
    bool valid = false;
    while (!valid)
    {
        setDirection(randInt(0,3)*90);
        if (nextPos() != Board::empty) valid = true;
    }
}

//teleports enemy after being impacted by vortex
void Enemy::impact()
{
    teleport();
    setDirection(right);
    setState(PAUSED);
    setPause(-pausecounter + 180);
}

void Enemy::doSomething()
{
    if (gotVortexed())
    {
        impact();
        setVortexed(false);
    }
    
    Player* player = nullptr;
    if (getWorld()->overlapping(this, peach())) player = peach();
    if (getWorld()->overlapping(this, yoshi())) player = yoshi();
    
    if (getState() == PAUSED)
    {
        if (player != nullptr)
        {
            if (!player->wasAffected() && player->getState() == WAITING_TO_ROLL)
            {
                activityOne();
                cout << getState() << endl;
                player->setAffected(true);
            }

        }
        setPause(-1);
        if (getPause() == 0)
        {
            int squares_to_move = randInt(1,getDistance());
            adjustTicks(squares_to_move * 8);
            
            randomDir();
            setState(WALKING);
        }
    } else if (getState() == WALKING)
    {
        if (player != nullptr) player->setAffected(false);
        
        if (getTicks() == 0)
        {
            setState(PAUSED);
            setPause(180);
            activityTwo();
            return;
        }
        if (atFork()) randomDir();
        moveinDirection();
    }
}

void Bowser::activityOne()
{
    switch(randInt(1,2))
    {
        case 1:
            break;
        case 2:
            player()->adjustCoins(-player()->getCoins());
            player()->adjustStars(-player()->getStars());
            getWorld()->playSound(SOUND_BOWSER_ACTIVATE);
            break;
    }
}

void Bowser::activityTwo()
{
    switch(randInt(1,4))
    {
        case 1:
        case 2:
        case 3:
            break;
        case 4:
            getWorld()->placeDropSquare(this);
    }
}

void Boo::activityOne()
{
    switch(randInt(1,2))
    {
        case 1:
            getWorld()->swapCoins();
            break;
        case 2:
            getWorld()->swapStars();
            break;
    }
    getWorld()->playSound(SOUND_BOO_ACTIVATE);
}

void Vortex::doSomething()
{
    if(!isAlive())
    {
        return;
    }
    moveForward(2);
    if(getX()<0 || getX()>=VIEW_WIDTH || getY()<0 || getY()>=VIEW_HEIGHT)
    {
        setAlive(false);
    }
    
    Actor* actor = getWorld()->isHitByVortex(this);
    if (actor != nullptr)
    {
        actor->setVortexed(true);
        setAlive(false);
        getWorld()->playSound(SOUND_HIT_BY_VORTEX);
    }
}
