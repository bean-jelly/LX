#ifndef _SUBJECT_H_
#define _SUBJECT_H_
#include "Observer.h"
#include <list>
#include <string>

using namespace std;
class Observer;
typedef list<Obsever*> myList;

class Subject
{
public:
    Subject(){}
    virtual ~Subject(){}
    virtual void addObserver(Observer* o) = 0;
    virtual void deleteObserver(Observer* o) = 0;
    virtual void notifyObservers() = 0;
};

class SeekJobCenter : public Subject
{
public:
    SeekJobCenter();
    ~SeekJobCenter();
    void addObserver(Observer* o);
    void deleteObserver(Observer* o);

    void notifyObservers();
    void giveNewMessage(string str);
private:
    string mess;
    bool changed;
    myList personList;
};

#endif