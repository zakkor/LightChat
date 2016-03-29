#include "Parser.h"

std::string Parser::copyFromCharToChar(char char1, char char2, const std::string &str)
{
    std::string newStr = "";

    unsigned const int start = str.find_first_of(char1) + 1;
    unsigned const int end = str.find_first_of(char2);

    for (unsigned int i = start; i < end; ++i)
    {
        newStr += str.at(i);
    }
    return newStr;
}

std::string Parser::copyFromCharToEnd(char character, const std::string &str)
{
    unsigned int position = str.find_first_of(character) + 1;
    std::string newStr = "";
    for (unsigned int i = position; i < str.size(); i++)
    {
        newStr += str.at(i);
    }
    return newStr;
}
