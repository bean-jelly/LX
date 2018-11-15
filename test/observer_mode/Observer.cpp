#include "Observer.h"

UniversityStudent::UniversityStudent(Subject* subject)
{
    if(subject != NULL)
    {
        this->subject = subject;
    }
    else
        return;
    subject->addObserver(this);
}

void UniversityStudent::hearTelephone(string heardMess)
{
    std::cout << "I am a university student" << std::endl;
    std::cout << "I hear message is " << heardMess << std::endl;
    return;
}

HaiGui::HaiGui(Subject* Subject)
{
    if(subject != NULL)
    {
        this->subject = subject;
    }
    else
        return;
    subject->addObserver(this);
}

void HaiGui::hearTelephone(string heardMess)
{
    std::cout << "I am a HaiGui" << std::endl;
    std::cout << "I hear message is " << heardMess << std::endl;
    return;
}