#include "MainPane.h"

MainPane::MainPane()
{
    box.setSize(sf::Vector2f(400, 300));
    box.setFillColor(sf::Color::White);
}


ChatMessage::ChatMessage()
    : belongsToSelf(false)
{
    name.setCharacterSize(14);
    name.setColor(sf::Color::Green);
    name.setPosition(0, 0);
    message.setCharacterSize(14);
    message.setColor(sf::Color::Black);
    message.setPosition(0, 0);
}

void MainPane::addNewMessage(ChatMessage chatMessage)
{
    int mess_xpos = 5, mess_ypos = 280;
    int name_xpos = 5, name_ypos = mess_ypos - 20;
    if (chatMessage.belongsToSelf)
    {
        name_xpos = box.getSize().x - 5 - chatMessage.name.getLocalBounds().width;
        mess_xpos = box.getSize().x - 5 - chatMessage.message.getLocalBounds().width;
    }

    chatMessage.message.setPosition(mess_xpos, mess_ypos);
    chatMessage.name.setPosition(name_xpos, name_ypos);

    for (unsigned int i = 0; i < messages.size(); ++i)
    {
        messages.at(i).message.setPosition(messages.at(i).message.getPosition().x,
                                                messages.at(i).message.getPosition().y - 40);
        messages.at(i).name.setPosition(messages.at(i).name.getPosition().x,
                messages.at(i).name.getPosition().y - 40);
    }
    messages.emplace_back(chatMessage);
}
