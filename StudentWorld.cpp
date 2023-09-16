#include "StudentWorld.h"
#include "GameConstants.h"
#include "Board.h"
#include "Actor.h"
#include "SpriteManager.h"
#include <string>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath), bank(0)
{
    actors.clear();
    peach = nullptr;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

int StudentWorld::init()
{
    board = new Board();
    int board_num = getBoardNumber();
    string board_file = assetPath() + "board0" + to_string(board_num) + ".txt";
    Board::LoadResult result = board->loadBoard(board_file);
    if (result == Board::load_fail_file_not_found || result == Board::load_fail_bad_format)
    {
        return GWSTATUS_BOARD_ERROR;
    } else if (result == Board::load_success)
    {
        for (int i = 0; i < BOARD_WIDTH; i++)
        {
            for (int j = 0; j < BOARD_HEIGHT; j++)
            {
                switch(board->getContentsOf(i, j)) {
                    case Board::empty:
                        break;
                    case Board::player:
                        peach = new Player(IID_PEACH, i*16, j*16, this, 1);
                        yoshi = new Player(IID_YOSHI, i*16, j*16, this, 2);
                        bluecoinsquare = new CoinSquare(IID_BLUE_COIN_SQUARE, i*16, j*16, this, 3);
                        push(peach);
                        push(yoshi);
                        push(bluecoinsquare);
                        break;
                    case Board::blue_coin_square:
                        bluecoinsquare = new CoinSquare(IID_BLUE_COIN_SQUARE, i*16, j*16, this, 3);
                        push(bluecoinsquare);
                        break;
                    case Board::red_coin_square:
                        redcoinsquare = new CoinSquare(IID_RED_COIN_SQUARE, i*16, j*16, this, -3);
                        push(redcoinsquare);
                        break;
                    case Board::up_dir_square:
                        updirsquare = new DirSquare(IID_DIR_SQUARE, i*16, j*16, this, Actor::up);
                        push(updirsquare);
                        break;
                    case Board::down_dir_square:
                        downdirsquare = new DirSquare(IID_DIR_SQUARE, i*16, j*16, this, Actor::down);
                        push(downdirsquare);
                        break;
                    case Board::left_dir_square:
                        leftdirsquare = new DirSquare(IID_DIR_SQUARE, i*16, j*16, this, Actor::left);
                        push(leftdirsquare);
                        break;
                    case Board::right_dir_square:
                        rightdirsquare = new DirSquare(IID_DIR_SQUARE, i*16, j*16, this, Actor::right);
                        push(rightdirsquare);
                        break;
                    case Board::event_square:
                        eventsquare = new EventSquare(IID_EVENT_SQUARE, i*16, j*16, this);
                        push(eventsquare);
                        break;
                    case Board::bank_square:
                        banksquare = new BankSquare(IID_BANK_SQUARE, i*16, j*16, this);
                        push(banksquare);
                        break;
                    case Board::star_square:
                        starsquare = new StarSquare(IID_STAR_SQUARE, i*16, j*16, this);
                        push(starsquare);
                        break;
                    case Board::bowser:
                        bowser = new Bowser(IID_BOWSER, i*16, j*16, this);
                        bluecoinsquare = new CoinSquare(IID_BLUE_COIN_SQUARE, i*16, j*16, this, 3);
                        push(bowser);
                        push(bluecoinsquare);
                        break;
                    case Board::boo:
                        boo = new Boo(IID_BOO, i*16, j*16, this);
                        bluecoinsquare = new CoinSquare(IID_BLUE_COIN_SQUARE, i*16, j*16, this, 3);
                        push(boo);
                        push(bluecoinsquare);
                        break;
                }
            }
        }
    }
    startCountdownTimer(99);
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit ESC.
    // Notice that the return value GWSTATUS_NOT_IMPLEMENTED will cause our framework to end the game.

    if (timeRemaining() <= 0)
    {
        if (peach->getStars() > yoshi->getStars()) return peachWins();
        else if (yoshi->getStars() > peach->getStars()) return yoshiWins();
        else
        {
            if (peach->getCoins() > yoshi->getCoins()) return peachWins();
            else if (yoshi->getCoins() > peach->getCoins()) return yoshiWins();
            else
            {
                switch(randInt(1,2))
                {
                    case 1:
                        return peachWins();
                    case 2:
                        return yoshiWins();
                }
            }
        }
    }
    
    for (vector<Actor*>::iterator p = actors.begin(); p != actors.end(); p++)
    {
        if ((*p)->isAlive()) (*p)->doSomething();
        else
        {
            delete *p;
            p = actors.erase(p);
            p--;
        }
    }
    
    string p1VOR = "";
    string p2VOR = "";
    
    if (peach->hasVortex()) p1VOR = " VOR";
    if (yoshi->hasVortex()) p2VOR = " VOR";
    
    setGameStatText("P1: Roll: " + to_string(peach->getRoll()) + " Stars: " + to_string(peach->getStars()) + " $$: " + to_string(peach->getCoins()) + p1VOR + " | Time: " + to_string(timeRemaining()) + " | Bank: " + to_string(getBank()) + " | " + "P2: Roll: " + to_string(yoshi->getRoll()) + " Stars: " + to_string(yoshi->getStars()) + " $$: " + to_string(yoshi->getCoins()) + p2VOR);
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for (vector<Actor*>::iterator p = actors.begin(); p != actors.end();)
    {
        delete *p;
        p = actors.erase(p);
    }
}

//swaps player locations
void StudentWorld::swapPlayers(Player* p1, Player* p2)
{
    int tempX = p1->getX();
    int tempY = p1->getY();
    int tempDir = p1->getDirection();
    
    p1->setDirection(p2->getDirection());
    p2->setDirection(tempDir);
    p1->moveTo(p2->getX(), p2->getY());
    p2->moveTo(tempX, tempY);
    p2->moveAtAngle(p2->getDirection(), 2);
    p1->adjustTicks(p2->getTicks());
    p2->adjustTicks(-p2->getTicks());
    p1->adjustPos();
    playSound(SOUND_PLAYER_TELEPORT);
}

//swaps player coins
void StudentWorld::swapCoins()
{
    int temp = peach->getCoins();
    peach->adjustCoins(-peach->getCoins() + yoshi->getCoins());
    yoshi->adjustCoins(-yoshi->getCoins() + temp);
}

//swaps player stars
void StudentWorld::swapStars()
{
    int temp = peach->getStars();
    peach->adjustStars(-peach->getStars() + yoshi->getStars());
    yoshi->adjustStars(-yoshi->getStars() + temp);
}

//places dropping square and deletes square below it
void StudentWorld::placeDropSquare(Actor* actor)
{
    for(vector<Actor*>::iterator p = actors.begin(); p!=actors.end();p++)
    {
        if(overlapping(actor,(*p)) && (*p)->isSquare())
        {
            playSound(SOUND_DROPPING_SQUARE_CREATED);
            droppingsquare = new DroppingSquare(IID_DROPPING_SQUARE, (*p)->getX(),(*p)->getY(), this);
            actors.push_back(droppingsquare);
            delete *p;
            p = actors.erase(p);
            p--;
        }
    }
}

//checks if two actors are overlapping
bool StudentWorld::overlapping(Actor* a, Actor* b)
{
    if (a->getX() == b->getX() && a->getY() == b->getY()) return true;
    return false;
}

//checks if vortex overlaps even 1 pixel with another actor
bool StudentWorld::overlapsVortex(Actor *a, Actor *b)
{
    
    if(a->getX() + SPRITE_WIDTH > b->getX() && a->getX() < b->getX() + SPRITE_WIDTH && a->getY() + SPRITE_HEIGHT > b->getY() && a->getY() < b->getY() + SPRITE_HEIGHT) return true;
    return false;
}

//returns if an impactible actor is hit by vortex
Actor* StudentWorld::isHitByVortex(Actor* actor)
{
    for (vector<Actor*>::iterator p = actors.begin(); p != actors.end(); p++)
    {
        if (overlapsVortex((*p), actor) && (*p)->vortexable())
        {
            return (*p);
        }
    }
    return nullptr;
}

int StudentWorld::peachWins()
{
    setFinalScore(peach->getStars(), peach->getCoins());
    return GWSTATUS_PEACH_WON;
}

int StudentWorld::yoshiWins()
{
    setFinalScore(yoshi->getStars(), yoshi->getCoins());
    return GWSTATUS_YOSHI_WON;
}
