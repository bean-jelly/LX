#include "Component.h"
#include <stdio.h>

Component::Component()
{

}

Component::~Component()
{

}

ConcreteComponent::ConcreteComponent()
{

}

ConcreteComponent::~ConcreteComponent()
{

}

void ConcreteComponent::operation()
{
    fprintf(stderr, "ConcreteComponent's operation!\n");
}