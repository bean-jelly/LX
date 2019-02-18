#include "Subject.h"
#include "Observer.h"
#include <list>

SeekJobCenter::SeekJobCenter()
{
    mess = "";
    changed = false;
}

void SeekJobCenter::addObserver(Observer* o)
{
    list<Observer*>::iterator it;
    it = find(personList.begin(), personList.end(), 0);
    if(it == personList.end())
    {
        personList.push_back(o);
    }
    return;
}

void SeekJobCenter::deleteObserver(Observer* o)
{
    list<Observer*>::iterator it;
    it = find(personList.begin(), personList.end(), 0);
     if (it != personList.end())
     {
         personList.remove(*it);
         delete(*it);
     }
     return;
}

void SeekJobCenter::notifyObservers()
{
    list<Observer*>::iterator it;
    if(changed)
    {
        for(it = personList.begin(); it != personList.end(); it++)
        {
            (*it)->hearTelephone(mess);
        }
        changed = false;
    }
    return;
}

void SeekJobCenter::giveNewMessage(string str)
{
    if(o == strcmp(str.c_str(), mess.c_str()))
    {
        changed = false;
    }
    else
    {
        mess = str;
        changed = true;
    }
    return;
}