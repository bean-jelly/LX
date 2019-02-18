#ifndef _OBSERVER_H_
#define _OBSERVER_H_
#include <string>
#include <iostream>
#include <list>
#include "Subject.h"
using namespace std;
class Subject;

class Observer
{
public:
    Observer(){};
    virtual ~Observer(){};
    virtual void hearTelephone(string& hearMess) = 0;
};

class UniversityStudent : public Observer
{
public:
    UniversityStudent(Subject* subject);
    virtual ~UniversityStudent(){};
    void hearTelephone(string heardMess);
private:
    Subject* subject;
};

class HaiGui:public Observer
{
public:
    HaiGui(Subject* subject);
    virtual ~HaiGui(){};
    void hearTelephone(string heardMess);
private:
    Subject* subject;
};

#endif 
