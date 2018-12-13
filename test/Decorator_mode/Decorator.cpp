#include "Decorator.h"
#include <stdio.h>

Decorator::Decorator()
{

}

Decorator::~Decorator()
{

}

void Decorator::operation()
{
    mComponent->operation();
}

void Decorator::setComponent(Component* pComponent)
{
    this->mComponent = pComponent;
}

ConcreteDecorator::ConcreteDecorator()
{

}

ConcreteDecorator::~ConcreteDecorator()
{

}

void ConcreteDecorator::addBehavior()
{
    fprintf(stderr, "ConcreteDecorator's addBehavior!\n");
}