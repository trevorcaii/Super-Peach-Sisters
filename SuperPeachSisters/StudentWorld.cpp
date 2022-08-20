#include "StudentWorld.h"
#include "GameConstants.h"
#include <string>
#include <iostream> // defines the overloads of the << operator
#include <sstream>  // defines the type std::ostringstream
#include <iomanip>  // defines the manipulator setw
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
    return new StudentWorld(assetPath);
}

// Students:  Add code to this file, StudentWorld.h, Actor.h, and Actor.cpp

StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
    m_peach = nullptr;
    m_stageCompleteStatus = 0;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

bool StudentWorld::overlap(int x, int y, Actor* actor1, Actor*& actor2)
{
    bool found = false;
    for(auto& element : m_objects)
        //make sure to not overlap with itself
        if(element != actor1)
        //see if the box that is both actors overlap with each other
            if(((x >= element->getX() && x <= element->getX()+SPRITE_WIDTH-1) || (x+SPRITE_WIDTH-1 >= element->getX() && x+SPRITE_WIDTH-1 <= element->getX()+SPRITE_WIDTH-1)) && ((y >= element->getY() && y <= element->getY()+SPRITE_HEIGHT-1) || (y+SPRITE_HEIGHT-1 >= element->getY() && y+SPRITE_HEIGHT-1 <= element->getY()+SPRITE_HEIGHT-1)))
            {
                if(found)
                {
        //if there is more than one object overlapping at one time, the one that is blocking takes importance
                    if(element->getBlocking())
                        actor2 = element;
                }
                else
        //give by reference which actors are overlapping
                    actor2 = element;
                found = true;
            }
    return found;
}

void StudentWorld::add(Actor* adder)
{
    m_objects.push_back(adder);
}

void StudentWorld::setStageCompleteStatus(int status)
{
    m_stageCompleteStatus = status;
}

bool StudentWorld::isPeach(Actor *actor)
{
    return actor == m_peach;
}

void StudentWorld::powerUpPeach(int power)
{
    switch (power) {
        case 1:
            m_peach->updateShootPower(true);
            break;
        case 2:
            m_peach->updateJumpPower(true);
            break;
        case 3:
            m_peach->updateStarPower(150);
            break;
        default:
            break;
    }
}

int StudentWorld::getPeachY()
{
    return m_peach->getY();
}

int StudentWorld::getPeachX()
{
    return m_peach->getX();
}

bool StudentWorld::starPeach()
{
    return m_peach->starPowerStatus();
}

int StudentWorld::init()
{
    Level lev(assetPath());
    int k = getLevel();
    ostringstream oss;
    oss << "level";
    oss.fill('0');
    oss << setw(2) << k << ".txt";
    string level_file = oss.str();
    Level::LoadResult result = lev.loadLevel(level_file);
    if(result == Level::load_fail_file_not_found || result == Level::load_fail_bad_format)
        return GWSTATUS_LEVEL_ERROR;
    else if(result == Level::load_success)
    {
        Level::GridEntry ge;
        for(int lx = 0; lx < GRID_WIDTH; lx++)
            for(int ly = 0; ly < GRID_HEIGHT; ly++)
            {
                //load in all obejcts in the game according to the level file
                ge = lev.getContentsOf(lx, ly);
                switch(ge)
                {
                    case Level::empty:
                        break;
                    case Level::block:
                        m_objects.push_back(new Block(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT, 0));
                        break;
                    case Level::star_goodie_block:
                        m_objects.push_back(new Block(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT, 1));
                        break;
                    case Level::flower_goodie_block:
                        m_objects.push_back(new Block(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT, 2));
                        break;
                    case Level::mushroom_goodie_block:
                        m_objects.push_back(new Block(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT, 3));
                        break;
                    case Level::pipe:
                        m_objects.push_back(new Pipe(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT));
                        break;
                    case Level::flag:
                        m_objects.push_back(new Flag(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT));
                        break;
                    case Level::mario:
                        m_objects.push_back(new Mario(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT));
                        break;
                    case Level::goomba:
                        m_objects.push_back(new Goomba(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT));
                        break;
                    case Level::koopa:
                        m_objects.push_back(new Koopa(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT));
                        break;
                    case Level::piranha:
                        m_objects.push_back(new Piranha(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT));
                        break;
                    case Level::peach:
                    {
                        m_peach = new Peach(this, lx*SPRITE_WIDTH, ly*SPRITE_HEIGHT);
                        m_objects.push_back(m_peach);
                        break;
                    }
                    default:
                        break;
                }
            }
    }
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    // This code is here merely to allow the game to build, run, and terminate after you hit enter.
    // Notice that the return value GWSTATUS_PLAYER_DIED will cause our framework to end the current level.
    //make all actors do their doSomething
    for(auto& element : m_objects)
        element->doSomething();
    
    if(m_stageCompleteStatus == 1)
    {
        playSound(SOUND_FINISHED_LEVEL);
        m_stageCompleteStatus = 0;
        return GWSTATUS_FINISHED_LEVEL;
    }
    
    if(m_stageCompleteStatus == 2)
    {
        playSound(SOUND_GAME_OVER);
        m_stageCompleteStatus = 0;
        return GWSTATUS_PLAYER_WON;
    }
    //delete all dead objects from the game
    for(vector<Actor*>::iterator it = m_objects.begin(); it != m_objects.end();)
    {
        if(!(*it)->isAlive())
        {
            //if peach is dead then lose life restart game
            if(*it == m_peach)
            {
                playSound(SOUND_PLAYER_DIE);
                decLives();
                return GWSTATUS_PLAYER_DIED;
            }
            delete *it;
            it = m_objects.erase(it);
        }
        else
            it++;
    }
    //display text
    ostringstream oss;
    int lives = getLives();
    int level = getLevel();
    int points = getScore();
    oss << "Lives " << lives << "  Level: " << level;
    oss.fill('0');
    oss << "  Points: " << setw(6) << points;
    if(m_peach != nullptr && m_peach->starPowerStatus())
        oss << " StarPower!";
    if(m_peach != nullptr && m_peach->shootPowerStatus())
        oss << " ShootPower!";
    if(m_peach != nullptr && m_peach->jumpPowerStatus())
        oss << " JumpPower!";
    setGameStatText(oss.str());
    
    return GWSTATUS_CONTINUE_GAME;
}

void StudentWorld::cleanUp()
{
    for(auto& element : m_objects)
        delete element;
    m_objects.clear();
}

