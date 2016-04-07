#include "SFML/Graphics.hpp"

class ChatMessage
{
public:
    ChatMessage();
    sf::Text name;
    sf::Text message;
    bool belongsToSelf;
};

class MainPane
{
public:
    MainPane();

    void addNewMessage(ChatMessage chatMessage);

    sf::RectangleShape box;
    std::vector <ChatMessage> messages;
};
