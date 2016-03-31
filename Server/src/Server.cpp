#include <iostream>
#include "Server.hpp"
#include "SFML/Network.hpp"
#include "SFML/Audio.hpp"
#include "../../shared/src/NetEventTypes.hpp"
#include <string.h>

#define RELEASE

using std::cin;
using std::cout;
using std::map;

Server::Server()
    : personCount(0)
{ }

void Server::initializePlayers()
{
    //Initialize players:
    cout << "Sending data to all clients... \n";

    short personId = 0;
    for (auto &client : clients)
    {
	std::cout << "Data sent for client #" << personId << std::endl;
        sf::Packet packet;
        packet << NET::PersonConnected;
        packet << personCount;
        packet << personId;

	//add all the names
	for (auto p : personMap)
	{
	    std::cout << p.second.name.c_str() << std::endl;
	    packet << p.second.name.c_str();
	}

        client->send(packet);
        personId++;
    }

    cout << "done!\n";
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

		//Add one new person to the personMap
                Person newPerson;
                newPerson.name = "unnamed";

                personMap.emplace(personCount, newPerson);
                personCount++;

                initializePlayers();
            }
        }
    }
}

void Server::sendPacketToAllClients(sf::Packet packet)
{
    for (auto &return_client : clients)
    {
	return_client->send(packet);
    }
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
		selector.remove(*clients.at(tempId));
		it = clients.erase(clients.begin() + tempId);

                personCount--;

                initializePlayers();

		needToSkipIncrement = true;

                cout << "User with ID " << tempId << " has left your channel.\n";
            }


            if (aux == NET::PersonTalked)
            {
                //Unravel the packet and set the new coordinates to the personId
                short tempId;
                char message[100];
                tempId = -1;

                packet >> tempId;
                packet >> message;

                if (tempId != -1)
                {
                    //Now we tell all the clients that one of them moved
		    sendPacketToAllClients(return_packet);
                }
            }

	    if (aux == NET::ServerCommand)
	    {
                short tempId;
                char command[100];
		char arg[100];
		bool hasArg;


                tempId = -1;

                packet >> tempId;
                packet >> command;
		packet >> hasArg;

		if (hasArg)
		{
		    packet >> arg;
		    // check commands
		    if (strncmp(command,"rename", 100) == 0)
		    {

			personMap.at(tempId).name = arg;
			return_packet.clear();
			return_packet << NET::PersonRenamed << tempId
				      << personMap.at(tempId).name.c_str();
		    }
		}

		std::cout << command << std::endl;



		if (tempId != -1)
		{
		    sendPacketToAllClients(return_packet);
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
        processNetworkEvents();
    }
}
