#include "StudentWorld.h"
#include <string>
#include "Field.h"
#include <list>
#include "Actor.h"
#include <cassert>
#include "Compiler.h"
#include <iostream>
#include <sstream>
#include <iomanip>

using namespace std;

GameWorld* createStudentWorld(string assetDir)
{
	return new StudentWorld(assetDir);
}

// Students:  Add code to this file (if you wish), StudentWorld.h, Actor.h and Actor.cpp

StudentWorld::StudentWorld(std::string assetDir)
: GameWorld(assetDir)
{
    for (int i = 0; i < 4; i++)
    {
        antCount[i] = -1;
    }
    m_ticks = 2000;
}

StudentWorld::~StudentWorld()
{
    cleanUp();
}

Field StudentWorld::getField()
{
    return m_field;
}

int StudentWorld::init()
{    //intialize data structure
    //load current field details from data file
    
    string fieldFileName;
    Field f;
   
    //attempt to load file
    std::string fieldFile = getFieldFilename();
    string error;
    if (f.loadField(fieldFile, error) != Field::LoadResult::load_success)
    {
        setError(fieldFile + " " + error);
        return false;
    }
    
    m_field = f;

    Compiler *compilerForEntrant0, *compilerForEntrant1,
            *compilerForEntrant2, *compilerForEntrant3;
    

    antFiles = getFilenamesOfAntPrograms();


    if (antFiles.size() > 0)
    {
        compilerForEntrant0 = new Compiler;
        if ( ! compilerForEntrant0->compile(antFiles[0], error) )
        {
            setError(antFiles[0] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        m_compilers.push_back(compilerForEntrant0);
        colonyNames.push_back(compilerForEntrant0->getColonyName());
    }
    if (antFiles.size() > 1)
    {
        compilerForEntrant1 = new Compiler;
        if (!compilerForEntrant1->compile(antFiles.at(1), error))
        {
            setError(antFiles[1] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        m_compilers.push_back(compilerForEntrant1);

        colonyNames.push_back(compilerForEntrant1->getColonyName());
    }
    if (antFiles.size() > 2)
    {
        compilerForEntrant2 = new Compiler;
        if (!compilerForEntrant2->compile(antFiles[2], error))
        {
            setError(antFiles[3] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        m_compilers.push_back(compilerForEntrant2);

        colonyNames.push_back(compilerForEntrant2->getColonyName());
    }
    if (antFiles.size() > 3)
    {
        compilerForEntrant3 = new Compiler;
        if (!compilerForEntrant3->compile(antFiles[3], error))
        {
            setError(antFiles[3] + " " + error);
            return GWSTATUS_LEVEL_ERROR;
        }
        m_compilers.push_back(compilerForEntrant3);

        colonyNames.push_back(compilerForEntrant3->getColonyName());
    }
 
    
    // add ah0 to the simulation with
        //m_data[y][x].push_back(ah0)
 
    //allocate and insert all objects into data structure
    
    for (int y = 0; y < VIEW_HEIGHT; y++)           //y coordinates
    {
        for (int x = 0; x < VIEW_WIDTH; x++)        //x coordinates
        {
            Field::FieldItem item = f.getContentsOf(x, y);
            switch(item)
            {
                case (Field::FieldItem::rock):
                    m_data[y][x].push_back(new Pebble(IID_ROCK, x, y, this));
                    break;
                case (Field::FieldItem::grasshopper):
                    m_data[y][x].push_back(new BabyGrasshopper(IID_BABY_GRASSHOPPER, x, y, this));
                    break;
                case (Field::FieldItem::food):
                    m_data[y][x].push_back(new Food(IID_FOOD, x, y, this, 6000));
                    break;
                case (Field::FieldItem::poison):
                    m_data[y][x].push_back(new Poison(IID_POISON, x, y, this));
                    break;
                case (Field::FieldItem::water):
                    m_data[y][x].push_back(new WaterPool(IID_WATER_POOL, x, y, this));
                    break;
                case (Field::FieldItem::anthill0):
                    if (antFiles.size() > 0)
                        m_data[y][x].push_back(new Anthill(IID_ANT_HILL, x, y, this, 0, compilerForEntrant0));
                    break;
                case (Field::FieldItem::anthill1):
                    if (antFiles.size() > 1)
                        m_data[y][x].push_back(new Anthill(IID_ANT_HILL, x, y, this, 1, compilerForEntrant1));
                    break;
                case (Field::FieldItem::anthill2):
                    if (antFiles.size() > 2)
                        m_data[y][x].push_back(new Anthill(IID_ANT_HILL, x, y, this, 2, compilerForEntrant2));
                    break;
                case (Field::FieldItem::anthill3):
                    if (antFiles.size() > 3)
                        m_data[y][x].push_back(new Anthill(IID_ANT_HILL, x, y, this, 3, compilerForEntrant3));
                    break;
                default:
                    break;
                    
            }
        }
    }
    
    return GWSTATUS_CONTINUE_GAME;
}

int StudentWorld::move()
{
    //decrement ticks
    m_ticks--;

		  // This code is here merely to allow the game to build, run, and terminate after you hit enter.
		  // Notice that the return value GWSTATUS_NO_WINNER will cause our framework to end the simulation.
    if (m_ticks > 0)
    {
        for (int y = 0; y < VIEW_HEIGHT; y++)
        {
            for (int x = 0; x < VIEW_WIDTH; x++)
            {
                if (!m_data[y][x].empty())
                {
                    list<Actor*>::iterator p = m_data[y][x].begin();
                    while(p != m_data[y][x].end())
                    {
                        if ((*p)->moved() == false)
                        {
                            int prevX = (*p)->getX();
                            int prevY = (*p)->getY();
                            (*p)->doSomething();
                            int currX = (*p)->getX();
                            int currY = (*p)->getY();
                            if (prevX != currX || prevY != currY)   //check if Actor object has been moved
                            {
                                (*p)->setMove();            //if so, set moved to true
                                m_data[currY][currX].push_back(*p);     //move the objects to the correct m_data slot
                                p = m_data[y][x].erase(p);
                            }
                            else
                            {
                                p++;
                            }
                        }
                        else
                        {
                            p++;
                        }
                    }
                }
            }
        }
        //loop through all Actors, moving
        for (int y = 0; y < VIEW_HEIGHT; y++)
        {
            for (int x = 0; x < VIEW_WIDTH; x++)
            {
                if (!m_data[y][x].empty())
                {
                    list<Actor*>::iterator q = m_data[y][x].begin();
                    while(q != m_data[y][x].end())
                    {
                        if ((*q)->moved())
                        {
                            (*q)->setMove();
                            int ID = (*q)->getID();
                            if (ID == IID_BABY_GRASSHOPPER ||ID == IID_ANT_TYPE0 || ID == IID_ANT_TYPE1 || ID == IID_ANT_TYPE2 || ID == IID_ANT_TYPE3)
                            {
                                Insect* p = dynamic_cast<Insect*>(*q);
                                if (p->stunned())
                                    p->setStun();
                                assert(p->stunned() == false);
                                if (ID == IID_ANT_TYPE0 || ID == IID_ANT_TYPE1 || ID == IID_ANT_TYPE2 || ID == IID_ANT_TYPE3)
                                {
                                    Ant* a = dynamic_cast<Ant*>(*q);
                                    if (a->bitten())
                                        a->setBitten();
                                    assert(a->bitten() == false);
                                }
                            }
                        }
                        else if ((*q)->isDead())
                        {
                            Actor* temp = *q;
                            if ((*q)->getID() == IID_ANT_HILL)
                            {
                                
                            }
                            q = m_data[y][x].erase(q);
                            delete temp;
                        }
                        else
                        {
                            q++;
                        }
                    }
                }
            }
        }


        int winningAnt = getWinningAnt();
        string s = formatDisplayText(m_ticks, antCount[0], antCount[1], antCount[2], antCount[3], getWinningAnt());
        setGameStatText(s);
        
        return GWSTATUS_CONTINUE_GAME;
    }
    if (getWinningAnt() != -1)
    {
        setWinner(getWinningAnt(getWinningAnt()));
        return GWSTATUS_PLAYER_WON;
    }
    else
        return GWSTATUS_NO_WINNER;
}

string StudentWorld::getWinningAnt(int i)
{
    return colonyNames[i];
}

int StudentWorld::getWinningAnt()
{
    int maxIndex = 0;
    for (int i = 0; i < 4; i++)
        if ((antCount[i] > antCount[maxIndex]) && antCount[i] >= 6)
            maxIndex = i;
    if (antCount[maxIndex] <= 5)
        return -1;
    return maxIndex;
}


string StudentWorld::formatDisplayText(int ticks, int a0, int a1, int a2, int a3, int winningAnt)
{
    string s = "";
    ostringstream t;
    t << setw(5) << ticks;
    string antCounts[4];
    ostringstream ant0;
    ant0 << setw(2) << a0;
    ant0.fill('0');
    antCounts[0] = ant0.str();
    ostringstream ant1;
    ant1 << setw(2) << a1;
    ant1.fill('0');
    antCounts[1] = ant1.str();
    ostringstream ant2;
    ant2 << setw(2) << a2;
    ant2.fill('0');
    antCounts[2] = ant2.str();
    ostringstream ant3;
    ant3 << setw(2) << a3;
    ant3.fill('0');
    antCounts[3] = ant2.str();
    s = "Ticks:" + t.str() + " - ";
    vector<string>::iterator p = colonyNames.begin();
    int i = 0;
    while (p!= colonyNames.end())
    {
        s = s + *p;
        if (winningAnt == i)
        {
            s = s + "*";
        }
        s = s + ": " + antCounts[i] + " ants  ";
        p++;
        i++;
    }
    return s;
}

void StudentWorld::addFood(int x, int y, int f_hp)
{

    Food* p = foodCheck(x, y);
    if (p == nullptr)
    {
        m_data[y][x].push_back(new Food(IID_FOOD, x, y, this, f_hp));
    }
    else
    {
        p->increaseHP(100);
    }

}

Food* StudentWorld::foodCheck(int x, int y)
{
    list<Actor*>::iterator p = m_data[y][x].begin();
    while (p != m_data[y][x].end())
    {
        if ((*p)->getID() == IID_FOOD)
        {
            Food* fp = dynamic_cast<Food*>(*p);
            if (fp->returnHP() >= 1)
                return fp;
        }
        p++;
    }
    return nullptr;
}

void StudentWorld::addAdultGrasshopper(int x, int y)
{
    m_data[y][x].push_back(new AdultGrasshopper(IID_ADULT_GRASSHOPPER, x, y, this));
}

Insect* StudentWorld::findEnemy(int x, int y, Insect* curr)
{
    list<Actor*>::iterator p = m_data[y][x].begin();
    vector<Insect*> enemyList;
    bool ant = true;
    AdultGrasshopper *gp;
    Ant* ap;
    if (curr->getID() == IID_ADULT_GRASSHOPPER)
    {
        gp = dynamic_cast<AdultGrasshopper*>(curr);
        ant = false;
    }
    else
    {
        ap = dynamic_cast<Ant*>(curr);

    }
    while (p!= m_data[y][x].end())
    {
        int ID = (*p)->getID();
        if (!ant)
        {
            if (ID == IID_ADULT_GRASSHOPPER)
            {
                AdultGrasshopper* othergp = dynamic_cast<AdultGrasshopper*>(*p);
                if (gp != othergp)
                {
                    Insect* ip = dynamic_cast<Insect*>(*p);
                    enemyList.push_back(ip);
                }
            }
            if (ID == IID_BABY_GRASSHOPPER || ID == IID_ANT_TYPE0 || ID == IID_ANT_TYPE1 || ID == IID_ANT_TYPE2 || ID == IID_ANT_TYPE3)
            {
                Insect* ip = dynamic_cast<Insect*>(*p);
                enemyList.push_back(ip);
            }
        }
        if (ant)
        {
            if (ID == IID_ADULT_GRASSHOPPER || ID == IID_BABY_GRASSHOPPER)
            {
                Insect* ip = dynamic_cast<Insect*>(*p);
                enemyList.push_back(ip);
            }
            if (ID == IID_ANT_TYPE0 || ID == IID_ANT_TYPE1 || ID == IID_ANT_TYPE2 || ID == IID_ANT_TYPE3)
            {
                Ant* otherap = dynamic_cast<Ant*>(*p);
                if (otherap->getColonyNum() != ap->getColonyNum())
                {
                    Insect* ip = dynamic_cast<Insect*>(*p);
                    enemyList.push_back(ip);
                }
            }
        }
        p++;
    }
    if (enemyList.size() == 0)
        return nullptr;
    else
    {
        int enemy = randInt(0, enemyList.size()-1);
        return enemyList.at(enemy);
    }
}

Insect* StudentWorld::detectUnpoisonedInsect(int x, int y)
{
    list<Actor*>::iterator p = m_data[y][x].begin();
    while (p!= m_data[y][x].end())
    {
        int ID = (*p)->getID();
        if (ID == IID_BABY_GRASSHOPPER || ID == IID_ANT_TYPE0 || ID == IID_ANT_TYPE1 || ID == IID_ANT_TYPE2 || ID == IID_ANT_TYPE3)
        {
            Insect* ip = dynamic_cast<Insect*>(*p);
            if ((ip)->getPoisoned()==false)
                return ip;
            else
                p++;
        }
        else
        {
            p++;
        }
    }
    return nullptr;
}


Insect* StudentWorld::detectUnstunnedInsect(int x, int y)
{
    list<Actor*>::iterator p = m_data[y][x].begin();
    while (p!= m_data[y][x].end())
    {
        int ID = (*p)->getID();
        if (ID == IID_BABY_GRASSHOPPER || ID == IID_ANT_TYPE0 || ID == IID_ANT_TYPE1 || ID == IID_ANT_TYPE2 || ID == IID_ANT_TYPE3)
        {
            Insect* ip = dynamic_cast<Insect*>(*p);
            if ((ip)->stunned()==false)
                return ip;
            else
                p++;
        }
        else
        {
            p++;
        }
    }
    return nullptr;
}

void StudentWorld::addAnt(int x, int y, Compiler* c, Anthill* currentAntHill)
{
    int antID = 0;
    switch (currentAntHill->getColonyNumber())
    {
        case 0:
            antID = IID_ANT_TYPE0;
            break;
        case 1:
            antID = IID_ANT_TYPE1;
            break;
        case 2:
            antID = IID_ANT_TYPE2;
            break;
        case 3:
            antID = IID_ANT_TYPE3;
            break;
    }
    if (antCount[currentAntHill->getColonyNumber()] == -1)
        antCount[currentAntHill->getColonyNumber()] = 0;
    antCount[currentAntHill->getColonyNumber()]++;
    Ant* ant = new Ant(antID, x, y, this, false, 1500, c, currentAntHill->getColonyNumber(), currentAntHill);
    m_data[y][x].push_back(ant);
    currentAntHill->addAnt(ant);
}

Pheromone* StudentWorld::findMyPheromone(int x, int y, Ant* ant)
{
    int colNum = ant->getColonyNum();
    int pID;
    switch (colNum)
    {
        case 0:
            pID = IID_PHEROMONE_TYPE0;
            break;
        case 1:
            pID = IID_PHEROMONE_TYPE1;
            break;
        case 2:
            pID = IID_PHEROMONE_TYPE2;
            break;
        case 3:
            pID = IID_PHEROMONE_TYPE3;
            break;
    }
    list<Actor*>::iterator p = m_data[y][x].begin();
    while (p != m_data[y][x].end())
    {
        if ((*p)->getID() == pID)
        {
            Pheromone* pheromone = dynamic_cast<Pheromone*>(*p);
            return pheromone;
        }
        else
            p++;
    }
    return nullptr;
}

bool StudentWorld::findPheromone(int x, int y)
{
    list<Actor*>::iterator p = m_data[y][x].begin();
    while (p != m_data[y][x].end())
    {
        int ID = (*p)->getID();
        if (ID == IID_PHEROMONE_TYPE0 || ID == IID_PHEROMONE_TYPE1 || ID == IID_PHEROMONE_TYPE2 || ID == IID_PHEROMONE_TYPE3)
        {
            return true;
        }
        else
            p++;
    }
    return false;
}

void StudentWorld::addPheromone(int x, int y, Ant* ant)
{
    Pheromone* p = findMyPheromone(x, y, ant);
    if (p != nullptr)
    {
        int amt = 256;
        if (p->returnHP() + amt >= 768)
        {
            amt = 768 - p->returnHP();
        }
        p->increaseHP(amt);
        return;
    }
    else
    {
        int colNum = ant->getColonyNum();
        int pID;
        switch (colNum)
        {
            case 0:
                pID = IID_PHEROMONE_TYPE0;
                break;
            case 1:
                pID = IID_PHEROMONE_TYPE1;
                break;
            case 2:
                pID = IID_PHEROMONE_TYPE2;
                break;
            case 3:
                pID = IID_PHEROMONE_TYPE3;
                break;
        }
        m_data[y][x].push_back(new Pheromone(pID, x, y, this));
        return;
    }
}

void StudentWorld::cleanUp()
{
    for (int y = 0; y < VIEW_HEIGHT; y++)
    {
        for (int x = 0; x < VIEW_WIDTH; x++)
        {
            if (!m_data[y][x].empty())      //if there are actors at that position
            {
                list<Actor*>::iterator p = m_data[y][x].begin();
                while (p != m_data[y][x].end())
                {
                    Actor* temp = *p;
                    p = m_data[y][x].erase(p);      //empty the list and delete dynamically allocated actor objects
                    delete temp;
                }
            }
        }
    }
    list<Compiler*>::iterator p = m_compilers.begin();
    while (p!= m_compilers.end())
    {
        Compiler* temp = *p;
        p = m_compilers.erase(p);
        delete temp;
    }
}

