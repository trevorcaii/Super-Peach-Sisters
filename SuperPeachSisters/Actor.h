#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cstdlib>
#include <cmath>
class StudentWorld;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor: public GraphObject
{
public:
    Actor(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection, int depth, double size);
    bool isAlive();                     //returns if the actor is alive or dead
    void setDead();                     //sets the actor alive status to false or dead
    StudentWorld* getWorld();           //returns the student world object that hold the actor
    virtual bool getBlocking();         //returns TRUE if actor should block movement and FALSE if not
    virtual bool isDamageable();        //returns TRUE if actor can be damaged and FALSE if not
    virtual void bonk(Actor* bonker) {} //action depending on the actor and the actor bonking it
    virtual void doSomething() = 0;     //action depending on the actor
private:
    StudentWorld* m_w;
    bool m_aliveStatus;
};

class Obstacle: public Actor
{
public:
    Obstacle(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection, int depth, double size);
    bool getBlocking();                 //obstacles block movement so set Blocking to TRUE
    void doSomething() {}
};

class Block: public Obstacle
{
public:
    Block(StudentWorld* w, int initialX, int initialY, int contains);
    void bonk(Actor* bonker);           //define the Block's bonk to deal with secret blocks and Goodies
private:
    int m_containsGoodie;               //tells which Goodie the Block contais:                                                                    0 - none, 1 - Star, 2 - Flower, 3 - Mushroom
};

class Pipe: public Obstacle
{
public:
    Pipe(StudentWorld* w, int initialX, int initialY);
};

class Peach: public Actor
{
public:
    Peach(StudentWorld* w, int initialX, int initialY);
    void doSomething();                 //move based on UP, LEFT, RIGHT keys as well as SPACE for shooting                                      Fireballs when she has Shooting Power.
    bool isDamageable();                //let the program know that peach is damageable
    bool shootPowerStatus();            //return TRUE if peach has shoot power and FALSE if not
    void updateShootPower(bool status); //change the status of the Shoot Power
    bool jumpPowerStatus();             //return TRUE if peach has jump power and FALSE if not
    void updateJumpPower(bool status);  //change the status of the Jump Power
    bool starPowerStatus();             //return TRUE if peach has star power and FALSE if not
    void updateStarPower(int time);     //add time to Star Power if she gets the Star Power Goodie
    void bonk(Actor* bonker);           //if she gets bonked by an enemy or a enemy projectile, check if                                        Peach is at 1 Hp, then kill her, else remove 1 Hp if she has jump                                    or shoot power or nothing if she has star power.
private:
    int m_Hp;
    int m_remaining_jump_distance;
    bool m_shootPowerStatus;
    bool m_jumpPowerStats;
    int m_starTimer;
    int m_tempInvincibility;
    int m_rechargeTime;
};


class PointAdders: public Actor
{
public:
    PointAdders(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection, int depth, double size, int pointsWorth);
    int getPoints();                    //Return the number of points a PointAdder is worth
private:
    int m_pointsWorth;
};

class Goodies: public PointAdders
{
public:
    Goodies(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection, int depth, double size, int pointsWorth);
    virtual void powerUp() = 0;         //tells peach which type of good she got and what Power to get
    void doSomething();                 //move left or right and switching direcitons when running into a                                       blocking object
};

class Flower: public Goodies
{
public:
    Flower(StudentWorld* w, int initialX, int initialY);
    void powerUp();                     //give peach the shooting poewr
};

class Mushroom: public Goodies
{
public:
    Mushroom(StudentWorld* w, int initialX, int initialY);
    void powerUp();                     //give peach the jump power
};

class Star: public Goodies
{
public:
    Star(StudentWorld* w, int initialX, int initialY);
    void powerUp();                     //give peach 10 seconds of star power
};

class Portals: public PointAdders
{
public:
    Portals(StudentWorld* w, int imageID, int initialX, int initialY);
    virtual int statusChanger() = 0;   //compete the level or the game depending on flag or Mario
    void doSomething();                 //check if overlaping with peach and change the status according to                                     the function above
};

class Flag: public Portals
{
public:
    Flag(StudentWorld* w, int initialX, int initialY);
    int statusChanger();                //change the statusChanger function to compete the level
};

class Mario: public Portals
{
public:
    Mario(StudentWorld* w, int initialX, int initialY);
    int statusChanger();                //change the statusChanger function to complete the game
};

class Projectiles: public Actor
{
public:
    Projectiles(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection);
    virtual void damage();              //damage damageable characters depending on if its friendly or not,                                     defaulted to damaging enemies(changed for Piranha Fire)
    void doSomething();                 //move down and to one direction until either hitting a blocking                                        object or a damageable object, setDead if this collision happens
};

class PeachFire: public Projectiles
{
public:
    PeachFire(StudentWorld* w, int initialX, int initialY, int initialDirection);
};

class Shell: public Projectiles
{
public:
    Shell(StudentWorld* w, int initialX, int initialY, int initialDirection);
};

class PiranhaFire: public Projectiles
{
public:
    PiranhaFire(StudentWorld* w, int initialX, int initialY, int initialDirection);
    void damage();                      //change what the projectile damages to only peach
};

class Enemies: public PointAdders
{
public:
    Enemies(StudentWorld* w, int imageID, int initialX, int initialY);
    bool isDamageable();                //enemies are damageable
    void bonk(Actor* bonker);           //add points to score and then take damage
    virtual void takeDamage();          //kill the enemy, added funtionablility for koopas to release a                                         shell as well
    virtual void doSomething();         //defaulted to walking back and forth, chaging direction from                                           blocking objects or open ledges, changing this for piranhas
};

class Goomba: public Enemies
{
public:
    Goomba(StudentWorld*w, int initialX, int initialY);
};

class Koopa: public Enemies
{
public:
    Koopa(StudentWorld*w, int initialX, int initialY);
    void takeDamage();                  //koopa will release a shell on death
};

class Piranha: public Enemies
{
public:
    Piranha(StudentWorld*w, int initialX, int initialY);
    void doSomething();                 //Piranhas will change animations and then will shoot at peach if                                       she gets within a certain distance from them, also changes                                           directions to face peach once she is the save y as them
private:
    int m_fireDelay;
};
#endif // ACTOR_H_
