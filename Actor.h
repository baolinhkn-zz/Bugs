#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include "StudentWorld.h"

class Compiler;

using namespace std;
// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp
class Actor : public GraphObject
{
public:
    Actor (int ID, int x, int y, StudentWorld* sw = nullptr, int d = 1);
    virtual void doSomething() = 0;
    virtual bool isDead() {return false;};
    bool blocked(int x, int y, StudentWorld* sw);
    StudentWorld* getStudentWorld();
    virtual void setMove() {return;} ;
    virtual bool moved() {return false;};
private:
    StudentWorld* m_studentWorld;
};

class Pebble : public Actor
{
public:
    Pebble(int ID_p = IID_ROCK, int x_p = 0, int y_p = 0, StudentWorld* sw = nullptr)
        : Actor(ID_p, x_p, y_p, sw) {}
    virtual void doSomething() {};

private:
};

class StunPoisonObj : public Actor
{
public:
    StunPoisonObj(int ID, int x, int y, StudentWorld* sw);
    virtual void doSomething() = 0;
    Insect* detect(int x, int y);
private:
    //pure virtual class containing waterpool and poison
    //activate when insects step on them
};

class WaterPool : public StunPoisonObj
{
public:
    WaterPool(int ID_w = IID_WATER_POOL, int x_w = 0, int y_w = 0, StudentWorld* sw = nullptr);
    virtual void doSomething();
};

class Poison : public StunPoisonObj
{
public:
    Poison(int ID_po = IID_POISON, int x_po = 0, int y_po = 0, StudentWorld* sw = nullptr);
    virtual void doSomething();

private:
    
};

class EnergyHolder : public Actor
{
public:
    //pure virtual class contianing ants, food, pheromes, anthills, adult grasshoppers, baby grasshoppers
    //objects hold energy that decay over time
    EnergyHolder(int ID, int x, int y, StudentWorld* sw, int hp = 0, int d = 1);
    virtual bool isDead();
    void setDead();
    int returnHP();
    int decreaseHP(int hp);     //returns how much hp was decreased by
    void increaseHP(int hp);
    virtual void doSomething() = 0;
    virtual bool eatFood(int x, int y, int amt);

    
private:
    int m_hitPoints;
    bool m_isDead;
};

class Food : public EnergyHolder
{
public:
    Food(int ID_f = IID_FOOD, int x_f = 0, int y_f = 0, StudentWorld* sw = nullptr, int hp = 6000);
    virtual void doSomething() {};
    virtual void setMove() {return;}
    virtual bool moved() {return false;};
    virtual bool eatFood(int x, int y, int amt) {return false;};

    
private:
};

class Pheromone: public EnergyHolder
{
public:
    Pheromone(int ID_p, int x_p, int y_p, StudentWorld* sw);
    virtual void doSomething();
    virtual bool eatFood(int x, int y, int amt){return false;};
private:
    
};

class Ant;

class Anthill : public EnergyHolder
{
public:
    Anthill(int ID_ah, int x_ah = 0, int y_ah = 0, StudentWorld* sw = nullptr, int colN = 5, Compiler* m_cah = nullptr);
    virtual void doSomething();
    void giveBirth();
    int getColonyNumber();
    void addAnt(Ant* newAnt);
    void cleanUpAnts();
    
private:
    vector<Ant*> m_ants;
    Compiler* m_compiler;
    int m_colonyNumber;

};

class Insect : public EnergyHolder
{
    //pure virtual class containing ants, baby grasshoppers, grasshoppers
    //spend time sleeping/stunned and can move
public:
    Insect(int ID, int x, int y, StudentWorld*sw, bool m = false, int hp = 0);
    virtual void doSomething() = 0;
    void setMove();
    bool moved();
    bool attemptMove(Direction dir);
    int getDistance();      //returns m_distance
    void decrementDistance();   //decrements m_distance
    void setDistance(int d);    //sets m_distance to d
    void setPoison();
    bool getPoisoned();
    int getSleeping();      //returns m_sleeping
    void decrementSleep();  //decrements m_sleeping
    virtual void resetSleep();      //resets m_sleeping to 2
    virtual void stun();
    virtual bool stunned();
    virtual void setStun();
    void setRandDir();
    virtual void bite(int x, int y, Insect* e) {};

    
private:
    bool m_flag;
    int m_distance;
    bool m_poisoned;
    int m_sleeping;
    bool m_stunned;

};

class Ant: public Insect
{
public:
    Ant(int ID_a, int x_a, int y_a, StudentWorld*  sw, bool mg = false, int hp = 1500, Compiler* m_ca = nullptr, int cNum = 5, Anthill* ah = nullptr);
    virtual void doSomething();
    bool interpreter(int& ic, Compiler* c);
    int getColonyNum();
    void pickUpFood(int x, int y, StudentWorld* sw);
    int getFood();
    void dropFood(int x, int y, StudentWorld* sw);
    void eatFood();
    void setBitten();
    bool bitten();
    Direction rotate(Direction dir, string type);
    int convert(string s);
    void lookForward(int& x, int& y, Direction dir);
    void setAHNull();
    void emitPheromone(int x, int y);
    bool ifCondition(int c);
    virtual void bite(int x, int y, Insect* e);
    virtual void resetSleep() {};
    
    
private:
    Anthill* m_anthill;
    bool m_blocked;
    bool m_bit;
    Compiler* m_compiler;
    int m_colonyNumber;
    int m_food;
    int m_lastRanNum;
    int m_iCounter;
};

class Grasshopper : public Insect
{
        //class that contains baby grasshoppers and adult grasshoppers, PURE VIRTUAL
public:
    Grasshopper(int ID, int x, int y, StudentWorld *sw, bool mg = false, int hp = 0);
    virtual void doSomething() = 0;
private:


};

class AdultGrasshopper : public Grasshopper
{
    //adult grasshoppers can bite, baby grasshoppers cannot
public:
    AdultGrasshopper(int ID_g = IID_ADULT_GRASSHOPPER, int x_ag = 0, int y_ag = 0, StudentWorld* sw = nullptr);
    virtual void doSomething();
    void jump(int x, int y);
    virtual void bite(int x, int y, Insect* e);
    virtual void stun() {return;};
    virtual bool stunned() {return false;};
    virtual void setStun() {return;} ;
private:
};


class BabyGrasshopper : public Grasshopper
{
public:
    BabyGrasshopper(int ID_bg = IID_BABY_GRASSHOPPER, int x_bg = 0, int y_bg = 0, StudentWorld* sw = nullptr);
    virtual void doSomething();
    void moult(int x, int y);
    void bite(int x, int y, Insect* e) {return;}
private:
};


#endif // ACTOR_H_
