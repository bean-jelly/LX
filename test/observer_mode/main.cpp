#include "Observer.h"
#include "Subject.h"

int main()
{
    SeekJobCenter *center = new SeekJobCenter();
    UniversityStudent *ly = new Observer(center);
    HaiGui *lx = new Observer(center);

    center->giveNewMessage("I need a cook");
    center->notifyObservers();
    center->giveNewMessage("I need a xxx");
    center->notifyObservers();

    delete center;
    delete lx;
    delete ly;
    return 0;
}