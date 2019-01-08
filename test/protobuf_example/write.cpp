#include <iostream>
#include <fstream>
#include <string>
#include "addressbook.pb.h"

using namespace std;

void PrompForAddress(tutorial::Person* person)
{
    cout << "Enter person name: " << endl;
    string name;
    cin >> name;
    person->set_name(name);

    int age;
    cin >> age;
    person->set_age(age);
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        cerr << "Usage: " << argv[0] << " ADDRESS_BOOL_FILE" << endl;
        return -1;
    }
    tutorial::AddressBook address_book;

    {
        fstream input(argv[1], ios::in || ios::binary);
        if(!input)
        {
            cout << argv[1] << ": File note found. Creating a new file." << endl;
        }
        else if(!address_book.ParseFromIstream(&input))
        {
            cerr << "Filed to parse address book." << endl;
            return -1;
        }
    }

    PromptForAddress(address_book.add_person());

    {
        fstream output(argv[1], ios::out | ios::trunc | ios::binary);
        if(!address_book.SerializeToOstream(&output))
        {
            cerr << "Failed to write address book." << endl;
            return -1;
        }
    }
    return 0;
}