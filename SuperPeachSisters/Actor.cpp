#include "Actor.h"
#include "StudentWorld.h"

// Students:  Add code to this file, Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection, int depth, double size) :GraphObject(imageID, initialX, initialY, initialDirection, depth, size)
{
    m_w = w;                    //all actors will have a pointer to the StudentWorld so they get access to                              the GameWorld functions
    m_aliveStatus = true;       //all actors will be alive until specified and then will be removed from the                            game
}

bool Actor::isAlive()
{
    return m_aliveStatus;       
}

void Actor::setDead()
{
    m_aliveStatus = false;
}

StudentWorld* Actor::getWorld()
{
    return m_w;
}

bool Actor::getBlocking()
{
    return false;
}

bool Actor::isDamageable()
{
    return false;
}

Obstacle::Obstacle(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection, int depth, double size)
:Actor(w, imageID, initialX, initialY, initialDirection, depth, size) {}

bool Obstacle::getBlocking()
{
    return true;
}

Block::Block(StudentWorld* w, int initialX, int initialY, int contains)
:Obstacle(w, IID_BLOCK, initialX, initialY, 0, 2, 1.0)
{
    m_containsGoodie = contains;
}

void Block::bonk(Actor* bonker)
{
    switch (m_containsGoodie) {
        case 0:                 //0 means normal block and thus getting bonked just plays the sound
            getWorld()->playSound(SOUND_PLAYER_BONK);
            break;
        case 1:                 //1 means to pop out a star ontop of the block and playing the appropriate                         sound
        {
            getWorld()->add(new Star(getWorld(), getX(), getY()+8));
            getWorld()->playSound(SOUND_POWERUP_APPEARS);
            m_containsGoodie = 0;
            break;
        }
        case 2:                 //2 means pop out a flower ontop of the block and play sound
        {
            getWorld()->add(new Flower(getWorld(), getX(), getY()+8));
            getWorld()->playSound(SOUND_POWERUP_APPEARS);
            m_containsGoodie = 0;
            break;
        }
        case 3:                 //3 means pop out a mushroom ontop of the block and play sound
        {
            getWorld()->add(new Mushroom(getWorld(), getX(), getY()+8));
            getWorld()->playSound(SOUND_POWERUP_APPEARS);
            m_containsGoodie = 0;
            break;
        }
        default:
            break;
    }
}

Pipe::Pipe(StudentWorld* w, int initialX, int initialY)
:Obstacle(w, IID_PIPE, initialX, initialY, 0, 2, 1.0) {}

PointAdders::PointAdders(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection, int depth, double size, int pointsWorth)
:Actor(w, imageID, initialX, initialY, initialDirection, depth, size)
{
    m_pointsWorth = pointsWorth;
}

int PointAdders::getPoints()
{
    return m_pointsWorth;
}

Goodies::Goodies(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection, int depth, double size, int pointsWorth)
:PointAdders(w, imageID, initialX, initialY, initialDirection, depth, size, pointsWorth) {}

void Goodies::doSomething()
{
    Actor* collision;
    if(!isAlive())
        return;
    //move downwards if there isn't a Block or Pipe under it
    if(!getWorld()->overlap(getX(), getY()-2, this, collision) || (getWorld()->overlap(getX(), getY()-2, this, collision) && !collision->getBlocking()))
    {
        moveTo(getX(), getY()-2);
    }
    //Move to the right or the left if there isnt something blocking it, if there is then switch directions
    if(getDirection() == 180)
    {
        if(!getWorld()->overlap(getX()-2, getY(), this, collision) || (getWorld()->overlap(getX()-2, getY(), this, collision) && !collision->getBlocking()))
        {
            setDirection(180);
            moveTo(getX()-2, getY());
        }
        else
            setDirection(0);
    }
    if(getDirection() == 0)
    {
        if(!getWorld()->overlap(getX()+2, getY(), this, collision) || (getWorld()->overlap(getX()+2, getY(), this, collision) && !collision->getBlocking()))
        {
            setDirection(0);
            moveTo(getX()+2, getY());
        }
        else
            setDirection(180);
    }
    //if overlapping with peach, update points, play sound, give peach power and remove from game
    if(getWorld()->overlap(getX(), getY(), this, collision))
    {
        if(getWorld()->isPeach(collision))
        {
            powerUp();
            getWorld()->increaseScore(getPoints());
            getWorld()->playSound(SOUND_PLAYER_POWERUP);
            setDead();
        }
    }
}

//Flower, Mushroom and Star's powerUp function is used to tell peach which power she is suppoed to get
Flower::Flower(StudentWorld* w, int initialX, int initialY)
:Goodies(w, IID_FLOWER, initialX, initialY, 0, 1, 1.0, 50) {}

void Flower::powerUp()
{
    getWorld()->powerUpPeach(1);
}

Mushroom::Mushroom(StudentWorld* w, int initialX, int initialY)
:Goodies(w, IID_MUSHROOM, initialX, initialY, 0, 1, 1.0, 75) {}

void Mushroom::powerUp()
{
    getWorld()->powerUpPeach(2);
}

Star::Star(StudentWorld* w, int initialX, int initialY)
:Goodies(w, IID_STAR, initialX, initialY, 0, 1, 1.0, 100) {}

void Star::powerUp()
{
    getWorld()->powerUpPeach(3);
}

Portals::Portals(StudentWorld* w, int imageID, int initialX, int initialY)
:PointAdders(w, imageID, initialX, initialY, 0, 1, 1.0, 1000) {}

void Portals::doSomething()
{
    Actor* collision;
    if(!isAlive())
        return;
    if(getWorld()->overlap(getX(), getY(), this, collision) && getWorld()->isPeach(collision))
    {
        getWorld()->increaseScore(getPoints());
        getWorld()->setStageCompleteStatus(statusChanger());
        setDead();
    }
}

Flag::Flag(StudentWorld* w, int initialX, int initialY)
:Portals(w, IID_FLAG, initialX, initialY) {}

int Flag::statusChanger()
{
    return 1;
}

Mario::Mario(StudentWorld* w, int initialX, int initialY)
:Portals(w, IID_MARIO, initialX, initialY) {}

int Mario::statusChanger()
{
    return 2;
}

Enemies::Enemies(StudentWorld* w, int imageID, int initialX, int initialY)
:PointAdders(w, imageID, initialX, initialY, (rand()%2)*180, 0, 1.0, 100) {}

bool Enemies::isDamageable()
{
    return true;
}

void Enemies::takeDamage()
{
    getWorld()->increaseScore(getPoints());
    setDead();
}

void Enemies::bonk(Actor* bonker)
{
    if(getWorld()->isPeach(bonker))
    {
        if(getWorld()->starPeach())
        {
            getWorld()->playSound(SOUND_PLAYER_KICK);
            takeDamage();
        }
    }
    else
        takeDamage();
}

void Enemies::doSomething()
{
    Actor* collision;
    if(!isAlive())
        return;
    //check if is overlapping with peach, if so bonk her and bonk myself
    if(getWorld()->overlap(getX(), getY(), this, collision) && getWorld()->isPeach(collision))
    {
        collision->bonk(this);
        bonk(collision);
        return;
    }
    //move left or right one space endless there is a blocking object or an open ledge, if so turn around
    if(getDirection() == 180)
    {
        if((!getWorld()->overlap(getX()-1, getY(), this, collision) || (getWorld()->overlap(getX()-1, getY(), this, collision) && !collision->getBlocking())) && (getWorld()->overlap(getX()-SPRITE_WIDTH, getY()-1, this, collision) && collision->getBlocking()))
        {
            moveTo(getX()-1, getY());
        }
        else
            setDirection(0);
    }
    else if(getDirection() == 0)
    {
        if((!getWorld()->overlap(getX()+1, getY(), this, collision) || (getWorld()->overlap(getX()+1, getY(), this, collision) && !collision->getBlocking())) && (getWorld()->overlap(getX()+SPRITE_WIDTH, getY()-1, this, collision)) && collision->getBlocking())
        {
            moveTo(getX()+1, getY());
        }
        else
            setDirection(180);
    }
}

Goomba::Goomba(StudentWorld* w, int initialX, int initialY)
:Enemies(w, IID_GOOMBA, initialX, initialY) {}

Koopa::Koopa(StudentWorld* w, int initialX, int initialY)
:Enemies(w, IID_KOOPA, initialX, initialY) {}
//koopa spawns a shell when it dies
void Koopa::takeDamage()
{
    getWorld()->increaseScore(getPoints());
    setDead();
    getWorld()->add(new Shell(getWorld(), getX(), getY(), getDirection()));
}

Piranha::Piranha(StudentWorld* w, int initialX, int initialY)
:Enemies(w, IID_PIRANHA, initialX, initialY)
{
    m_fireDelay = 0;
}

void Piranha::doSomething()
{
    Actor* collision;
    if(!isAlive())
        return;
    increaseAnimationNumber();
    //check if is overlapping with peach, if so bonk her and bonk myself
    if(getWorld()->overlap(getX(), getY(), this, collision) && getWorld()->isPeach(collision))
    {
        collision->bonk(this);
        bonk(collision);
        return;
    }
    //if peach is in the same y as piranha, switch to face her
    if(abs(getWorld()->getPeachY() - getY()) <= 1.5*SPRITE_HEIGHT)
    {
        if(getWorld()->getPeachX() <= getX())
            setDirection(180);
        else
            setDirection(0);
        if(m_fireDelay > 0)
        {
            m_fireDelay--;
            return;
        }
        else
    //if peach is within a certain x from piranha, shoot fireball at her
            if(abs(getWorld()->getPeachX() - getX()) < 8*SPRITE_WIDTH)
            {
                getWorld()->add(new PiranhaFire(getWorld(), getX(), getY(), getDirection()));
                getWorld()->playSound(SOUND_PIRANHA_FIRE);
                m_fireDelay = 40;
            }
    }
}

Projectiles::Projectiles(StudentWorld* w, int imageID, int initialX, int initialY, int initialDirection)
:Actor(w, imageID, initialX, initialY, initialDirection, 1, 1.0) {}
//sets to dead if collids with peach
void Projectiles::damage()
{
    Actor* collision;
    if(getWorld()->overlap(getX(), getY(), this, collision))
    {
        if(collision->isDamageable() && !getWorld()->isPeach(collision))
        {
            collision->bonk(this);
            setDead();
        }
    }
}

void Projectiles::doSomething()
{
    Actor* collision;
    if(!isAlive())
        return;
    //move downwards if there isn't a Block or Pipe under it
    if(!getWorld()->overlap(getX(), getY()-2, this, collision) || (getWorld()->overlap(getX(), getY()-2, this, collision) && !collision->getBlocking()))
    {
        moveTo(getX(), getY()-2);
    }
    //if there is nothing blocking the path of the then move 2 pixels, else die
    if(getDirection() == 180)
    {
        if(!getWorld()->overlap(getX()-2, getY(), this, collision) || (getWorld()->overlap(getX()-2, getY(), this, collision) && !collision->getBlocking()))
            moveTo(getX()-2, getY());
        else
            setDead();
    }
    if(getDirection() == 0)
    {
        if(!getWorld()->overlap(getX()+2, getY(), this, collision) || (getWorld()->overlap(getX()+2, getY(), this, collision) && !collision->getBlocking()))
            moveTo(getX()+2, getY());
        else
            setDead();
    }
    damage();
}

PeachFire::PeachFire(StudentWorld* w, int initialX, int initialY, int initialDirection)
:Projectiles(w, IID_PEACH_FIRE, initialX, initialY, initialDirection) {}

Shell::Shell(StudentWorld* w, int initialX, int initialY, int initialDirection)
:Projectiles(w, IID_SHELL, initialX, initialY, initialDirection) {}

PiranhaFire::PiranhaFire(StudentWorld* w, int initialX, int initialY, int initialDirection)
:Projectiles(w, IID_PIRANHA_FIRE, initialX, initialY, initialDirection) {}
//piranha will only damage peach
void PiranhaFire::damage()
{
    Actor* collision;
    if(getWorld()->overlap(getX(), getY(), this, collision))
    {
        if(collision->isDamageable() && getWorld()->isPeach(collision))
        {
            collision->bonk(this);
            setDead();
        }
    }
}

Peach::Peach(StudentWorld* w, int initialX, int initialY)
:Actor(w, IID_PEACH, initialX, initialY, 0, 0, 1.0)
{
    m_Hp = 1;
    m_remaining_jump_distance = 0;
    m_shootPowerStatus = false;
    m_jumpPowerStats = false;
    m_starTimer = 0;
    m_rechargeTime = 0;
    m_tempInvincibility = 0;
}

void Peach::doSomething()
{
    int ui;
    Actor* collision;
    if(!isAlive())
        return;
    //decrement all timers
    if(m_starTimer > 0)
        m_starTimer--;
    if(m_tempInvincibility > 0)
        m_tempInvincibility--;
    if(m_rechargeTime > 0)
        m_rechargeTime--;
    //if there is still more to jump, then move up units if there is no object blocking it
    if(m_remaining_jump_distance > 0)
    {
        if(getWorld()->overlap(getX(), getY()+4, this, collision))
        {
            collision->bonk(this);
            if(collision->getBlocking())
                m_remaining_jump_distance = 0;
        }
        else
        {
            moveTo(getX(), getY()+4);
            m_remaining_jump_distance--;
        }
    }
    //fall if there is no blocking object under
    else
    {
        if(!getWorld()->overlap(getX(), getY()-4, this, collision) || (getWorld()->overlap(getX(), getY()-4, this, collision) && !collision->getBlocking()))
            moveTo(getX(), getY()-4);
    }
    //get user input for UP, LEFT, RIGHT or SPACE and act accordingly
    if(getWorld()->getKey(ui))
    {
        switch (ui) {
            case KEY_PRESS_LEFT:
            {
                if(!getWorld()->overlap(getX()-4, getY(), this, collision) || (getWorld()->overlap(getX()-4, getY(), this, collision) && !collision->getBlocking()))
                {
                    setDirection(180);
                    moveTo(getX()-4, getY());
                }
                break;
            }
            case KEY_PRESS_RIGHT:
            {
                if(!getWorld()->overlap(getX()+4, getY(), this, collision) || (getWorld()->overlap(getX()+4, getY(), this, collision) && !collision->getBlocking()))
                {
                    setDirection(0);
                    moveTo(getX()+4, getY());
                }
                break;
            }
            //only be able jump if there is a block under it as "Support"
            case KEY_PRESS_UP:
            {
                if(getWorld()->overlap(getX(), getY()-1, this, collision))
                {
                    if(m_jumpPowerStats)
                        m_remaining_jump_distance = 12;
                    else
                        m_remaining_jump_distance = 8;
                    getWorld()->playSound(SOUND_PLAYER_JUMP);
                }
                break;
            }
            case KEY_PRESS_SPACE:
            {
                if(m_shootPowerStatus && m_rechargeTime == 0)
                {
                    if(getDirection() == 0)
                        getWorld()->add(new PeachFire(getWorld(), getX()+4, getY(), 0));
                    else
                        getWorld()->add(new PeachFire(getWorld(), getX()-4, getY(), 180));
                    getWorld()->playSound(SOUND_PLAYER_FIRE);
                    m_rechargeTime = 8;
                }
                break;
            }
            default:
                break;
        }
    }
}

bool Peach::isDamageable()
{
    return true;
}

bool Peach::shootPowerStatus()
{
    return m_shootPowerStatus;
}

void Peach::updateShootPower(bool status)
{
    m_shootPowerStatus = status;
    if(status)
        m_Hp = 2;
}

bool Peach::jumpPowerStatus()
{
    return m_jumpPowerStats;
}

void Peach::updateJumpPower(bool status)
{
    m_jumpPowerStats = status;
    if(status)
        m_Hp = 2;
}

bool Peach::starPowerStatus()
{
    return m_starTimer > 0;
}

void Peach::updateStarPower(int time)
{
    m_starTimer = time;
}

//if invincible, do nothing, else if peach has a powerup, give her temperary invincibility, else she dies
void Peach::bonk(Actor* bonker)
{
    if(m_starTimer > 0 || m_tempInvincibility > 0)
        return;
    m_Hp--;
    if(m_Hp == 1)
    {
        m_tempInvincibility = 10;
        getWorld()->playSound(SOUND_PLAYER_HURT);
        updateJumpPower(false);
        updateShootPower(false);
    }
    else
    {
        setDead();
    }
}
