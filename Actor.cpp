#include "Actor.h"
#include "StudentWorld.h"
#include "Compiler.h"
#include <cmath>
#include <cassert>

// Students:  Add code to this file (if you wish), Actor.h, StudentWorld.h, and StudentWorld.cpp

Actor::Actor(int ID, int x, int y, StudentWorld* sw, int d)
: GraphObject(ID, x, y, right, d)         //ID, x, y, depth,
{
    m_studentWorld = sw;
}

StudentWorld* Actor::getStudentWorld()
{
    return m_studentWorld;
}

bool Actor::blocked(int x, int y, StudentWorld* sw)
{
    Field f = sw->getField();
    if (f.getContentsOf(x, y) == Field::FieldItem::rock)       //later only check if blocked by insects or rocks
        return true;
    return false;
}

////////////////////////////////////////////////////////////////////////////////
///    STUNPOISON OBJ IMPLEMENTATION           /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
StunPoisonObj::StunPoisonObj(int ID, int x, int y, StudentWorld* sw)
: Actor(ID, x, y, sw)
{
}

Insect* detect(int x, int y)
{
    return nullptr;
}

////////////////////////////////////////////////////////////////////////////////
///    POISON OBJ IMPLEMENTATION           /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
Poison::Poison(int ID_po, int x_po, int y_po, StudentWorld* sw)
: StunPoisonObj(ID_po, x_po, y_po, sw)
{
}

void Poison::doSomething()
{
    Insect* p = getStudentWorld()->detectUnpoisonedInsect(getX(), getY());
    while (p!= nullptr)
    {
        p->setPoison();
        if ((p)->getID() != IID_BABY_GRASSHOPPER)
            p->decreaseHP(150);
        else
            p->decreaseHP(100);
        p = getStudentWorld()->detectUnpoisonedInsect(getX(), getY());
    }
}

////////////////////////////////////////////////////////////////////////////////
///    WATERPOOL OBJ IMPLEMENTATION           //////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

WaterPool::WaterPool(int ID_w, int x_w, int y_w, StudentWorld* sw)
: StunPoisonObj(ID_w, x_w, y_w, sw)
{
}

void WaterPool::doSomething()
{
    Insect* p = getStudentWorld()->detectUnstunnedInsect(getX(), getY());
    while (p!= nullptr)
    {
        p->setStun();
        p->stun();
        p = getStudentWorld()->detectUnstunnedInsect(getX(), getY());
    }
}

////////////////////////////////////////////////////////////////////////////////
///  ENERGY HOLDER IMPLEMENTATION    ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

EnergyHolder::EnergyHolder(int ID, int x, int y, StudentWorld* sw, int hp, int d)
: Actor(ID, x, y, sw, d)
{
    m_hitPoints = hp;
    m_isDead = false;
}

bool EnergyHolder::isDead()
{
    return m_isDead;
}

void EnergyHolder::setDead()
{
    m_isDead = true;
}

int EnergyHolder::returnHP()
{
    return m_hitPoints;
}

int EnergyHolder::decreaseHP(int hp)
{
    if (hp >= m_hitPoints)
    {
        hp = m_hitPoints;
        m_hitPoints = 0;
        setDead();
    }
    else
        m_hitPoints = m_hitPoints - hp;
    return hp;
}

void EnergyHolder::increaseHP(int hp)
{
    m_hitPoints += hp;
}


bool EnergyHolder::eatFood(int x, int y, int amt)
{
    Food* f = getStudentWorld()->foodCheck(x, y);
    if ( f != nullptr)
    {
        int eaten = amt;
        if (f->returnHP() <= eaten)
        {
            eaten = f->returnHP();
        }
        f->decreaseHP(eaten);
        increaseHP(eaten);
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////
///    FOOD IMPLEMENTATION           ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Food::Food(int ID_f, int x, int y, StudentWorld* sw, int hp)        //hp can change depending on how the food was created
: EnergyHolder(ID_f, x, y, sw, hp, 2)
{

}

////////////////////////////////////////////////////////////////////////////////
///    PHEROMONE IMPLEMENTATION           //////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Pheromone::Pheromone(int ID_p, int x_p, int y_p, StudentWorld* sw)
: EnergyHolder(ID_p, x_p, y_p, sw, 256, 2)
{

}

void Pheromone::doSomething()
{
    decreaseHP(1);
    if (returnHP() == 0)
    {
        setDead();
    }
}

////////////////////////////////////////////////////////////////////////////////
///  ANTHILL IMPLEMENTATION    /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Anthill::Anthill(int ID_ah, int x_ah, int y_ah, StudentWorld* sw, int colN, Compiler* cah)
: EnergyHolder(ID_ah, x_ah, y_ah, sw, 8999, 2), m_compiler(cah), m_colonyNumber(colN)
{
}

void Anthill::doSomething()
{
    decreaseHP(1);
    if (returnHP() == 0)
    {
        setDead();
        return;
    }
    if (eatFood(getX(), getY(), 10000))
    {
        return;
    }
    cerr << m_colonyNumber << ":" << returnHP() << endl;
    if (returnHP() >= 2000)
    {
        giveBirth();
    }
}

void Anthill::giveBirth()
{
    getStudentWorld()->addAnt(getX(), getY(), m_compiler, this);
    decreaseHP(1500);
    //allocate a new ant
    //pass it a pointer to the copmiler object
    //Ant* newAnt = new Ant(.....);
    //add it to the simulation

}


int Anthill::getColonyNumber()
{
    return m_colonyNumber;
}

void Anthill::addAnt(Ant* newAnt)
{
    m_ants.push_back(newAnt);
}

void Anthill::cleanUpAnts()
{
    vector<Ant*>::iterator p = m_ants.begin();
    while (p!=m_ants.end())
    {
        (*p)->setAHNull();
        p++;
    }

}
////////////////////////////////////////////////////////////////////////////////
///    INSECT IMPLEMENTATION         ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Insect::Insect(int ID, int x, int y, StudentWorld* sw, bool m, int hp)
: EnergyHolder(ID, x, y, sw, hp), m_flag(m)
{
    m_sleeping = 0;
    m_flag = false;
    m_poisoned = false;
    m_stunned = false;
}

void Insect::setMove()
{
    if (m_flag)
        m_flag = false;
    else
        m_flag = true;
}

bool Insect::moved()
{
    return m_flag;
}

bool Insect::attemptMove(Direction dir)
{
    switch (dir)
    {
        case up:
            if(!blocked(getX(), getY()+1, getStudentWorld()))
            {
                moveTo(getX(), getY()+1);
                return true;
            }
            break;
        case down:
            if (!blocked(getX(), getY()-1, getStudentWorld()))
            {
                moveTo(getX(), getY()-1);
                return true;
            }
            break;
        case left:
            if (!blocked(getX()-1, getY(), getStudentWorld()))
            {
                moveTo(getX()-1, getY());
                return true;
            }
            break;
        case right:
            if (!blocked(getX()+1, getY(), getStudentWorld()))
            {
                moveTo(getX()+1, getY());
                return true;
            }
            break;
    }
    return false;

}
int Insect::getSleeping()
{
    return m_sleeping;
}

void Insect::decrementSleep()
{
    m_sleeping--;
}

void Insect::resetSleep()
{
    m_sleeping = 2;
}

void Insect::stun()
{
    m_sleeping = m_sleeping + 2;
}

bool Insect::stunned()
{
    return m_stunned;
}

void Insect::setStun()
{
    if (m_stunned)
        m_stunned = false;
    else
        m_stunned = true;
}

int Insect::getDistance()
{
    return m_distance;
}

void Insect::decrementDistance()
{
    m_distance--;
}

void Insect::setDistance(int d)
{
    m_distance = d;
}

void Insect::setPoison()
{
    if (m_poisoned)
        m_poisoned = false;
    else
        m_poisoned = true;
}

bool Insect::getPoisoned()
{
    return m_poisoned;
}

void Insect::setRandDir()
{
    int randDir = randInt(0, 3);
    switch (randDir)
    {
        case 0:
            setDirection(right);
            break;
        case 1:
            setDirection(up);
            break;
        case 2:
            setDirection(down);
            break;
        case 3:
            setDirection(left);
            break;
    }
}

////////////////////////////////////////////////////////////////////////////////
///    ANT IMPLEMENTATION           ////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Ant::Ant(int ID_a, int x_a, int y_a, StudentWorld*  sw, bool mg, int hp, Compiler* m_ca, int cNum, Anthill* ah)
: Insect(ID_a, x_a, y_a, sw, mg, hp), m_compiler(m_ca), m_colonyNumber(cNum), m_anthill(ah)
{
    setRandDir();
    m_food = 0;
    m_lastRanNum = 0;
    m_iCounter = 0;
    m_blocked = false;
    m_bit = false;
}

void Ant::doSomething()
{
    decreaseHP(1);
    if (returnHP() <= 0)
    {
        getStudentWorld()->addFood(getX(), getY(), 100);
        setDead();
        cerr << "death";
        return;
    }
    if (getSleeping() != 0)
    {
        decrementSleep();
        return;
    }
    if (!interpreter(m_iCounter, m_compiler))
    {
        setDead();
        getStudentWorld()->addFood(getX(), getY(), 100);
        return;
    }
    
}

bool Ant::interpreter(int& ic, Compiler* c)
{
    Compiler::Command cmd;
    int commands = 0;
    for (;;)
    {
        if (!(c->getCommand(ic, cmd)))
            return false;
        commands++;
        switch(cmd.opcode)
        {
            case Compiler::moveForward:
                //cerr << "moveforward" << endl;
                if (attemptMove(getDirection()))
                {
                    m_blocked = false;
                    m_bit = false;
                }
                else
                {
                    m_blocked = true;
                }
                ic++;
                return true;
                break;
            case Compiler::eatFood:
               // cerr << "eat" << endl;
                eatFood();
                ic++;
                return true;
                break;
            case Compiler::dropFood:
                //cerr << "drop food" << endl;
                if (m_anthill != nullptr && m_food != 0)
                    dropFood(getX(), getY(), getStudentWorld());
                ic++;
                return true;
                break;
            case Compiler::bite:
            {
                //cerr << "bite" << endl;
                Insect* enemy = getStudentWorld()->findEnemy(getX(), getY(), this);
                if (enemy!= nullptr)
                {
                    bite(getX(), getY(), enemy);
                }
                ic++;
                return true;
                break;
            }
            case Compiler::pickupFood:
                //cerr << "pick up food" << endl;
                pickUpFood(getX(), getY(), getStudentWorld());
                ic++;
                return true;
                break;
            case Compiler::emitPheromone:
                //cerr << "emit pheromone" << endl;
                emitPheromone(getX(), getY());
                ic++;
                return true;
                break;
            case Compiler::faceRandomDirection:
               // cerr << "face random direction" << endl;
                setRandDir();
                ic++;
                return true;
                break;
            case Compiler::rotateClockwise:
                //cerr << "rotate c" << endl;
                setDirection(rotate(getDirection(), "c"));
                ic++;
                return true;
                break;
            case Compiler::rotateCounterClockwise:
                //cerr << "rotate cc" <<endl;
                setDirection(rotate(getDirection(), "cc"));
                ic++;
                return true;
                break;
            case Compiler::generateRandomNumber:
                //cerr << "generate random number" << endl;
                if (convert(cmd.operand1) == 0)
                    m_lastRanNum = 0;
                else
                {
                    int digit = convert(cmd.operand1);
                    m_lastRanNum = randInt(0, digit-1);
                }
                break;
            case Compiler::goto_command:
                ic = convert(cmd.operand1);
                break;
            case Compiler::if_command:
            {
                //cerr << "if command" << endl;
                int condition = convert(cmd.operand1);
                if (ifCondition(condition))
                {
                    ic = convert(cmd.operand2);
                }
                else
                    ic++;
                break;
                //cerr << condition << endl;
            }
        }
        if (commands == 10)
        {
            return true;
        }
    }
}

void Ant::bite(int x, int y, Insect* e)
{
    if (e->getID() != IID_ADULT_GRASSHOPPER && e->getID() != IID_BABY_GRASSHOPPER)
    {
        Ant* a = dynamic_cast<Ant*>(e);
        a->setBitten();
    }
    e->decreaseHP(15);
    if (e->isDead())
    {
        getStudentWorld()->addFood(getX(), getY(), 100);
    }
}

bool Ant::ifCondition(int c)
{
    switch (c)
    {
        case Compiler::last_random_number_was_zero:
            //cerr << "     last ran num was zero" << endl;
            if (m_lastRanNum == 0)
                return true;
            break;
        case Compiler::i_am_carrying_food:
            if (m_food > 0)
                return true;
            break;
        case Compiler::i_am_hungry:
            //cerr << "     hungry" << endl;
            if (returnHP() <= 25)
                return true;
            break;
        case Compiler::i_am_standing_with_an_enemy:
        {
            //cerr <<"     standing with enemy" << endl;
            Insect* enemy = getStudentWorld()->findEnemy(getX(), getY(), this);
            if (enemy != nullptr)
                return true;
            break;
        }
        case Compiler::i_am_standing_on_food:
            //cerr << "     standing on food" << endl;
            if (getStudentWorld()->foodCheck(getX(), getY())!=nullptr)
                return true;
            break;
        case Compiler::i_am_standing_on_my_anthill:
            if (m_anthill != nullptr && m_anthill->getX() == getX() && m_anthill->getY() == getY())
                return true;
            break;
            
        case Compiler::i_smell_pheromone_in_front_of_me:
        {
            //cerr << "     smelling pheromone" << endl;
            int xToLook = getX();
            int yToLook = getY();
            lookForward(xToLook, yToLook, getDirection());
            if (getStudentWorld()->findPheromone(xToLook, yToLook))
                return true;
            break;
        }
        case Compiler::i_smell_danger_in_front_of_me:
        {
            //cerr <<     "smell danger" << endl;
            int xToLook = getX();
            int yToLook = getY();
            lookForward(xToLook, yToLook, getDirection());
            Insect* enemy = getStudentWorld()->findEnemy(xToLook, yToLook, this);
            if (enemy != nullptr)
                return true;
            break;
        }
        case Compiler::i_was_bit:
            if (m_bit)
                return true;
            break;
        case Compiler::i_was_blocked_from_moving:
            //cerr << "     blocked from moving" << endl;
            if (m_blocked)
                return true;
            break;
    }
    return false;
}

int Ant::getColonyNum()
{
    return m_colonyNumber;
}

void Ant::pickUpFood(int x, int y, StudentWorld* sw)
{
    Food* f = getStudentWorld()->foodCheck(x, y);
    if ( f != nullptr)
    {
        int pickUpAmt = 400;
        if (f->returnHP() <= pickUpAmt)
        {
            pickUpAmt = f->returnHP();
        }
        if (m_food + pickUpAmt > 1800)
        {
            pickUpAmt = 1800-m_food;
        }
        f->decreaseHP(pickUpAmt);
        m_food = m_food + pickUpAmt;
    }
}
void Ant::dropFood(int x, int y, StudentWorld* sw)
{
    sw->addFood(x, y, m_food);
    m_food = 0;
}

void Ant::eatFood()
{
    int eaten = 100;
    if (getFood() < 100)
    {
        eaten = getFood();
    }
    m_food = m_food - eaten;
    increaseHP(eaten);
}

int Ant::getFood()
{
    return m_food;
}

void Ant::setBitten()
{
    if (m_bit)
        m_bit = false;
    else
        m_bit = true;
}

bool Ant::bitten()
{
    return m_bit;
}

GraphObject::Direction Ant::rotate(Direction dir, string type)
{
    if (type == "cc")
    {
        switch (dir)
        {
            case up:
                return left;
            case down:
                return right;
            case right:
                return up;
            case left:
                return down;
        }
    }
    else if (type == "cw")
    {
        switch (dir)
        {
            case up:
                return right;
            case down:
                return left;
            case left:
                return up;
            case right:
                return down;
        }
    }
    return none;
}

int Ant::convert(string s)
{
    int digit = 0;
    for (int i = 0; i < s.size(); i++)
    {
        digit = digit * 10;
        digit = digit + (s[i] - '0');
    }
   // cerr << endl << "convert result" << digit << " !!!" << endl;
    return digit;
}

void Ant::lookForward(int& x, int& y, Direction dir)
{
    switch (dir)
    {
        case up:
            y = y + 1;
            return;
        case down:
            y = y - 1;
            return;
        case right:
            x = x + 1;
            return;
        case left:
            y = y + 1;
            return;
        default:
            return;
    }
}

void Ant::setAHNull()
{
    m_anthill = nullptr;
}

void Ant::emitPheromone(int x, int y)
{
    getStudentWorld()->addPheromone(x, y, this);
}
////////////////////////////////////////////////////////////////////////////////
///    GRASSHOPPER IMPLEMENTATION    ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Grasshopper::Grasshopper(int ID, int x, int y, StudentWorld* sw, bool m, int hp)
: Insect(ID, x, y, sw, m, hp)
{
    setDistance(randInt(2, 100));
    setRandDir();
}


////////////////////////////////////////////////////////////////////////////////
/// ADULT GRASSHOPPER IMPLEMENTATION ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

AdultGrasshopper::AdultGrasshopper(int ID_ag, int x_ag, int y_ag, StudentWorld* sw)
: Grasshopper(ID_ag, x_ag, y_ag, sw, true, 1600)
{
}

void AdultGrasshopper::doSomething()
{
    decreaseHP(1);
    if (returnHP() == 0)
    {
        getStudentWorld()->addFood(getX(), getY(), 100);
        setDead();
        return;
    }
    if (getSleeping() == 0)
    {
        //IF there is an enemy
        Insect* enemy = getStudentWorld()->findEnemy(getX(), getY(), this);
        if (enemy!= nullptr)
        {
            if (randInt(0,2) == 0)  //1 in 2 chance it will bite
            {
                bite(getX(), getY(), enemy);
                resetSleep();
                return;
            }
        }
        //ELSE IF 1/10 chance it will jump
        if (randInt(0, 9) == 0)
        {
            //1 in 10 chance it will jump
            jump(getX(), getY());
            resetSleep();
            return;
            //if jumps - go to step 13
        }
        //attempt to eat food
        if (eatFood(getX(), getY(), 200))
        {
            if (randInt(0, 1) == 0)
            {
                resetSleep();
                return;
            }
        }

        //check if finished walking
        if (getDistance() == 0)
        {
            //new rand direction
            setDistance(randInt(2, 10));
            //new rand distance
            setRandDir();
        }
        //attempt to move one square
        if (getDistance() != 0)
        {
            if (attemptMove(getDirection()))
            {
                decrementDistance();
            }
            setDistance(0);
        }
            //if blocked set distance to 0 and go to step 13
        resetSleep();
        return;
    }
    else
    {
        decrementSleep();
        return;
    }
}

void AdultGrasshopper::bite(int x, int y, Insect* e)
{
    if (e->getID() != IID_ADULT_GRASSHOPPER && e->getID() != IID_BABY_GRASSHOPPER)
    {
        Ant* a = dynamic_cast<Ant*>(e);
        a->setBitten();
    }
    e->decreaseHP(50);
    if (e->isDead())
    {
        getStudentWorld()->addFood(getX(), getY(), 100);
    }
}

void AdultGrasshopper::jump(int x, int y)
{
    //choose a random open square within a circular radius of 10 to itself and use MoveTo on that square
    double PI = 4 * atan(1.0);
    int degree = randInt(0, 360);
    double angle = degree * PI / 180;
    int xComp = 10*cos(angle);
    int yComp = 10*sin(angle);
    int i = 0;
    while (i < 10)
    {
        angle = randInt(0, 6);
        xComp = 10*cos(angle);
        yComp = 10*cos(angle);
        if (x + xComp >= 0 &&
            x + xComp < VIEW_WIDTH &&
            y + yComp >= 0 &&
            y + yComp < VIEW_HEIGHT &&
            (!blocked(x + xComp, y + yComp, getStudentWorld()))
            )
        {
            moveTo(x + xComp, y + yComp);
            setMove();
            return;
        }
        i++;

    }

}
////////////////////////////////////////////////////////////////////////////////
/// BABY GRASSHOPPER IMPLEMENTATION  ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

BabyGrasshopper::BabyGrasshopper(int ID_bg, int x_bg, int y_bg, StudentWorld* sw)
: Grasshopper(ID_bg, x_bg, y_bg, sw, true, 500)
{
}

void BabyGrasshopper::doSomething()
{
    decreaseHP(1);
    if (returnHP() == 0)
    {
            //add 100 units of food to current x,y location
        getStudentWorld()->addFood(getX(), getY(), 100);
            //set its state to dead
        setDead();
        return;
    }
    if (getSleeping() == 0)     //if not stunned, then the baby grasshopper can do something
    {

        if (returnHP() >= 1600)
        {
            //create an adult grasshopper at this cell
            moult(getX(), getY());
            getStudentWorld()->addFood(getX(), getY(), 100);
            return;
        }
        //attempt to eat up to 200 units of food at a time
        if (eatFood(getX(), getY(), 200))
        {
            if (randInt(0, 1) == 0)
            {
                resetSleep();
                return;
            }
        }
        
        //check if the grasshopper has finished moving
        if (getDistance()== 0)
        {
            setDistance(randInt(2, 10));
            setRandDir();
        }
        //attempt a move
        //loop through linked list at that coordinate to find a rock, if there is a rock don't move
        //else move forward
        if (getDistance() != 0)
        {
            if (attemptMove(getDirection()))
            {
                decrementDistance();
            }
            setDistance(0);
        }
        resetSleep();
        return;
    }
    else
    {
        //stunned 2/3 ticks
        //decrement amount of ticks left to be sleeping
        decrementSleep();
        return;
    }
}

void BabyGrasshopper::moult(int x, int y)
{
    getStudentWorld()->addAdultGrasshopper(getX(), getY());
    setDead();
}
