#ifndef _COMPONENT_H_
#define _COMPONENT_H_

class Component
{
public:
    Component();
    virtual ~Component();
    virtual void operation() = 0;
};

class ConcreteComponent : public Component
{
public:
    ConcreteComponent();
    ~ConcreteComponent();
    void operation();
};

#endif