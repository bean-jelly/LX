#include "Decorator.h"

int main()
{
    Component* pComponent = new ConcreteComponent();
    ConcreteDecorator* pConDecorator = new ConcreteDecorator();
    pConDecorator->setComponent(pComponent);
    pConDecorator->operation();
    pConDecorator->addBehavior();
    return 0;
}