#include "Client.hpp"
#include <iostream>
#include "../../shared/src/NetEventTypes.hpp"

using std::cin;
using std::cout;

Client::Client()
    : screenSize(400, 400)
    , window(sf::VideoMode(screenSize.x, screenSize.y), "Client", sf::Style::Close)
    , personCount(0)
    , inFocus(false)
    , currentlyTyping(false)
{
    font.loadFromFile("res/arial.ttf");

    window.setKeyRepeatEnabled(true);
    window.setVerticalSyncEnabled(false);

    chatBox.text.setFont(font);
}

void Client::run()
{
    sf::IpAddress ipAddress;
    //    cout << "Enter server ip to connect to:\n";
    //    cin >> ipAddress;

    ipAddress = "localhost";

    client.setBlocking(false);
    sf::Socket::Status status = client.connect(ipAddress, 53005);
    while (status != sf::Socket::Done)
    {
        cout << "Attempting to connect...\n";
        status = client.connect(ipAddress, 53005);
    }

    cout << "Connected succesfully!\n";

    recorder.start();

    startClient();
}

void Client::render()
{
    window.clear(sf::Color::Black);

    window.draw(chatBox.bgBox);
    window.draw(chatBox.box);
    window.draw(chatBox.text);

    window.draw(mainPane.box);

    if (mainPane.messages.size() > 0)
    {
        for (unsigned int i = 0; i < mainPane.messages.size(); ++i)
        {
            window.draw(mainPane.messages.at(i).name);
            window.draw(mainPane.messages.at(i).message);
        }
    }

    window.display();
}

void Client::processEvents()
{

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
	    // person disconnected
            sf::Packet packet;
            packet << NET::PersonDisconnected << personId;
            client.send(packet);
            window.close();
        }

        if (event.type == sf::Event::KeyPressed)
        {
            if (event.key.code == sf::Keyboard::Return)
            {
//                if (currentlyTyping == false)
//                {
//                    currentlyTyping = true;
//                }
//                else
//                {
//                    currentlyTyping = false;
//                }


                sf::Packet packet;
		std::string message = chatBox.text.getString().toAnsiString();

		// check if it's a command
		if (message.at(0) == '/')
		{
		    bool hasArg = false;
		    string commandName = ""; std::string arg = "";
                    if (message.find_first_of(' ') != string::npos)
                    {
                        commandName = Parser::copyFromCharToChar('/', ' ', message);
			arg = Parser::copyFromCharToEnd(' ', message);
			hasArg = true;
                    }
                    else
                    {
                        commandName = Parser::copyFromCharToEnd('/', message);
                    }

		    // check if command is valid
		    if (commandName != "")
		    {
			std::cout << commandName << std::endl;
			packet << NET::ServerCommand << personId
			       << commandName.c_str() << hasArg;
			if (hasArg)
			{
			    packet << arg.c_str();
			}

			client.send(packet);
		    }

		}
		// it's a regular message
		else
		{
		    packet << NET::PersonTalked << personId << message.c_str();
		    client.send(packet);
		}

		// reset chat box
		chatBox.text.setString("");
            }

        }
        if (event.type == sf::Event::TextEntered)
        {
            std::string textHolder = chatBox.text.getString();
            if (event.text.unicode >= 32 && event.text.unicode <= 126)
            {
                textHolder += (char) event.text.unicode;
            }
            else if (event.text.unicode == 8 && textHolder.size() > 0)
            {
                textHolder.erase(textHolder.size() - 1, textHolder.size());
            }
            chatBox.text.setString(textHolder);

        }
        if (event.type == sf::Event::GainedFocus)
        {
            inFocus = true;
        }
        if (event.type == sf::Event::LostFocus)
        {
            inFocus = false;
        }
    }
}

void Client::processNetworkEvents()
{
    sf::Packet packet;
    if (client.receive(packet) != sf::Socket::Done)
    {
//            cout << "Couldn't recieve data from server.\n";
    } else
    {
        int aux;
        packet >> aux;

        if (aux == NET::PersonConnected)
        {
            personMap.clear();

            packet >> personCount;
            packet >> personId;

            cout << "People connected that I know of are:\n";
            for (int i = 0; i < personCount; ++i)
            {
                Person newPerson;
		char newName[] = "YA FUCKED UP!!!!";
		packet >> newName;
		std::cout << "PERSON NAME: " << newName << std::endl;

		newPerson.name = newName;

                cout << newPerson.name << " ";

                personMap.emplace(i, newPerson);
            }
            cout << std::endl;
        }
        if (aux == NET::PersonTalked)
        {
            //Unravel the packet and set the new coordinates to the playerId
            short tempId;
            char message[100];

            packet >> tempId;
            packet >> message;

            ChatMessage newChatMessage;
            newChatMessage.message.setString(message);
            newChatMessage.message.setFont(font);

	    newChatMessage.name.setString(personMap.at(tempId).name + ":");
            newChatMessage.name.setFont(font);

            if (personId == tempId)
            {
                newChatMessage.belongsToSelf = true;
            }

            mainPane.addNewMessage(newChatMessage);
        }
        if (aux == NET::PersonRenamed)
        {
            short tempId;
            char newName[100];

            packet >> tempId;
            packet >> newName;

            ChatMessage newChatMessage;
            newChatMessage.message.setString(personMap.at(tempId).name + " changed his name to " + newName);
            newChatMessage.message.setFont(font);
            newChatMessage.message.setColor(sf::Color::Red);

            newChatMessage.name.setString("SERVER:");
            newChatMessage.name.setColor(sf::Color::Red);
            newChatMessage.name.setFont(font);

            mainPane.addNewMessage(newChatMessage);

            personMap.at(tempId).name = newName;
        }
    }
}

void Client::startClient()
{
    while (window.isOpen())
    {
        voice.openFromSamples(dataPtr, recorder.getSampleRate());
        voice.loadFromSamples(recorder.get, samples.size(), 2, 44100);


        processEvents();

        processNetworkEvents();

        if (personCount != 0)
        {
            render();
        }
    }
}
