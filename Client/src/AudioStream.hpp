#include <SFML/System.hpp>
#include <string>
#include <cstdio>

class FileStream : public sf::InputStream
{
public :

    FileStream();

    ~FileStream();

    bool open();

    virtual sf::Int64 read(void* data, sf::Int64 size);

    virtual sf::Int64 seek(sf::Int64 position);

    virtual sf::Int64 tell();

    virtual sf::Int64 getSize();

private :
    std::FILE* m_file;
};
