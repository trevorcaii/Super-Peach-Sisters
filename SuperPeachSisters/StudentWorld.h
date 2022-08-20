#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Actor.h"
#include "Level.h"
#include <string>
#include <vector>

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetPath);
    ~StudentWorld();
    bool overlap(int x, int y, Actor* actor1, Actor*& actor2);  //check if an actor at a specific location                                                              will overlap with another actor at                                                                   location
    void add(Actor*);                                           //add an actor to the actors vector
    void setStageCompleteStatus(int status);                    //set stage to complete if stage is complete                                                            or set the game is compelte if it is
    bool isPeach(Actor* actor);                                 //check if an actor is peach
    void powerUpPeach(int power);                               //give peach the appropriate power up
    bool starPeach();                                           //check if peach has star power
    int getPeachY();
    int getPeachX();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
private:
    std::vector<Actor*> m_objects;
    Peach* m_peach;
    int m_stageCompleteStatus;
};

#endif // STUDENTWORLD_H_
