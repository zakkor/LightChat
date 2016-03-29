#include "SFML/Network.hpp"
#include "Person.h"
#include <deque>

class Server
{
public:
    Server();
    void run();
private:
    void checkForNewConnections();
    void processNetworkEvents();

    void initializePlayers();

    void sendPacketToAllClients(sf::Packet packet);

    sf::TcpListener listener;
    std::deque<std::unique_ptr<sf::TcpSocket>> clients;
    sf::SocketSelector selector;

    short personCount;
    std::map<short, Person> personMap;
};
