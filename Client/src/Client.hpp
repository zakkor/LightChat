#include "SFML/Network.hpp"
#include "SFML/Graphics.hpp"
#include "Person.h"
#include "TextureHandler.h"
#include "ChatBox.h"
#include "MainPane.h"
#include "Parser.h"
#include "MyRecorder.hpp"

class Client
{
public:
    Client();
    void run();

    sf::Vector2u screenSize;
private:
    void processEvents();
    void processNetworkEvents();
    void render();

    void startClient();


    //-------------------------------//

    sf::RenderWindow window;

    TextureHandler textureHandler;
    sf::Font font;

    sf::Music voice;
    MyRecorder recorder;

    sf::TcpSocket client;

    ChatBox chatBox;
    MainPane mainPane;


    short personCount;

    short personId;

    bool inFocus;

    bool currentlyTyping;

    std::map<short, Person> personMap;
};
