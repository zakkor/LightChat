#include <string>

class Person
{
public:
    Person();

    void addPerson(const Person &p);
    
    std::string name;
};
