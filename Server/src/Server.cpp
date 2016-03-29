#include <iostream>
#include "Server.hpp"
#include "SFML/Network.hpp"
#include "SFML/Audio.hpp"
#include "NetEventTypes.h"

#define RELEASE

using std::cin;
using std::cout;
using std::map;

Server::Server()
    : personCount(0)
{

}

void Server::checkForNewConnections()
{
    if (selector.wait())
    {
        if (selector.isReady(listener))
        {
            //We have a new connection!
	    std::unique_ptr<sf::TcpSocket> newClient (new sf::TcpSocket);
	    
            newClient->setBlocking(false);
            if (listener.accept(*newClient) == sf::Socket::Done)
            {
                cout << "Added new client!\n";
		selector.add(*newClient);
                clients.push_back(std::move(newClient));
		// !!! newClient is unusable from here!

//                Add new person to the personMap
                Person newPerson;
                newPerson.name = "unnamed";

                personMap.emplace(personCount, newPerson);
                personCount++;

                initializePlayers();
            }
        }
    }
}

void Server::initializePlayers()
{
    //Initialize players:
    cout << "Sending data to all clients... \n";


    short playerId = 0;
    for (auto &client : clients)
    {
	std::cout << "Client #" << playerId << std::endl;
        sf::Packet packet;
        packet << NET::PersonConnected;
        packet << personCount;
        packet << playerId;
        client->send(packet);
        playerId++;
    }

    cout << "done!\n";
}

void Server::processEvents()
{
}

void Server::processNetworkEvents()
{    
    for (auto it = clients.begin(); it != clients.end(); /* !!! */)
    {
	bool needToSkipIncrement = false;
        sf::TcpSocket &client = **it;
	
        // The client has sent some data, we can receive it
        sf::Packet packet;
        if (client.receive(packet) == sf::Socket::Done)
        {
            //Make a copy of the packet to send to the rest of the clients
            sf::Packet return_packet = packet;

            int aux = -1; //error code
            packet >> aux;


            if (aux == NET::PersonDisconnected)
            {
                short tempId;
                tempId = -1;

                packet >> tempId;

                personMap.erase(tempId);
//		delete (&clients.at(tempId));
		selector.remove(*clients.at(tempId));
		it = clients.erase(clients.begin() + tempId);
		

                personCount--;

                initializePlayers();
		
		needToSkipIncrement = true;
		
                cout << "User with ID " << tempId << " has left your channel.\n";
            }


            if (aux == NET::PersonTalked)
            {
                //Unravel the packet and set the new coordinates to the playerId
                short tempId;
                char message[100];
                tempId = -1;


                packet >> tempId;
                packet >> message;


                //Just in case

                string messageString = message;
                cout << messageString << '\n';

                if (messageString[0] == '/')
                {
                    //this means its a command
                    string commandName = "";

                    if (messageString.find_first_of(' ') != string::npos)
                    {
                        commandName = parser.copyFromCharToChar('/', ' ', messageString);
                    }
                    else
                    {
                        commandName = parser.copyFromCharToEnd('/', messageString);
                    }

                    // check commands
                    if (commandName == "rename")
                    {
                       string newName = parser.copyFromCharToEnd(' ', messageString);
		       return_packet.clear();
		       return_packet << NET::PersonRenamed << tempId << newName.c_str();
                    }
                }

                if (tempId != -1)
                {
                    //Now we tell all the clients that one of them moved
                    for (auto &return_client : clients)
                    {
                        return_client->send(return_packet);
                    }
                }
            }
        }
	if (!needToSkipIncrement)
	{
	    ++it;
	}
    }
}

void Server::run()
{
    cout << "Server is listening to port 53000...\n";

    // bind the listener to a port
    listener.listen(53005);
    selector.add(listener);


    for (;;)
    {
        checkForNewConnections();

        processEvents();

        processNetworkEvents();
    }
}
