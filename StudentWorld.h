#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "GameConstants.h"
#include <string>
#include <list>
#include "Field.h"

class Actor;
class Food;
class AdultGrasshopper;
class Insect;
class Compiler;
class Anthill;
class Ant;
class Pheromone;

using namespace std;

// Students:  Add code to this file, StudentWorld.cpp, Actor.h, and Actor.cpp

class StudentWorld : public GameWorld
{
public:
    StudentWorld(std::string assetDir);
    ~StudentWorld();
    virtual int init();
    virtual int move();
    virtual void cleanUp();
    Field getField();
    void addFood(int x, int y, int f_hp);
    Food* foodCheck(int x, int y);
    void addAdultGrasshopper(int x, int y);
    Insect* findEnemy(int x, int y, Insect* curr);
    Insect* detectUnpoisonedInsect(int x, int y);
    Insect* detectUnstunnedInsect(int x, int y);
    void addAnt(int x, int y, Compiler* c, Anthill* currentAntHill);
    Pheromone* findMyPheromone(int x, int y, Ant* ant);
    bool findPheromone(int x, int y);
    void addPheromone(int x, int y, Ant* ant);
    int getWinningAnt();
    string getWinningAnt(int i);
    string formatDisplayText(int ticks, int a0, int a1, int a2, int a3, int winningAnt);
    
private:
    std::vector<std::string> antFiles;
    std::vector<std::string> colonyNames;
    list<Actor*> m_data[VIEW_HEIGHT][VIEW_WIDTH];
    int m_ticks;
    Field m_field;
    int antCount[4];
    list<Compiler*> m_compilers;
};

#endif // STUDENTWORLD_H_
