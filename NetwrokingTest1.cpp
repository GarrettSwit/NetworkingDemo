#include <enet/enet.h>
#include <iostream>
#include <string>
#include <conio.h>
#include <thread>
#include <stdlib.h>
#include "PacketHeaderTypes.h"
#include "Message.h"
#include "User.h"

ENetHost* server = nullptr;
ENetHost* client = nullptr;
ENetAddress address;
std::thread* PacketThread = nullptr;
User thisUser;
bool isDone = false;
Message message;

std::string userName;
void ClientInput(User& thisUser, ENetHost* person) {
    while (isDone == false) {

        std::string text = "";
        thisUser.name = userName;
         {
            std::getline(std::cin, text);
            if (text == "q") {
                isDone = true;
            }
            text = thisUser.name + ": " + text;
        }
        //system("CLS");
        message.message = text;
        Buffer* msgBuffer = message.Serialize();
        ENetPacket* packet = enet_packet_create(msgBuffer->data,
            msgBuffer->dataSize,
            ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(person, 0, packet);
        enet_host_flush(person);
    }
}
void HostInput(User& thisUser, ENetHost* person) {
    while (isDone == false) {

        std::string text = "";
        thisUser.name = userName;
        {
            std::getline(std::cin, text);
            if (text == "q") {
                isDone = true;
            }
            text = thisUser.name + ": " + text;
        }
        //system("CLS");
        message.message = text;
        Buffer* msgBuffer = message.Serialize();
        ENetPacket* packet = enet_packet_create(msgBuffer->data,
            msgBuffer->dataSize,
            ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast(person, 0, packet);
        enet_host_flush(person);
    }
}
void ServerOuput() {
    while (isDone == false) {
        ENetEvent event;
        /* Wait up to 1000 milliseconds for an event. */
 
            switch (event.type)
            {
            case ENET_EVENT_TYPE_RECEIVE:
                std::cout << (char*)event.packet->data
                    << std::endl;
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
                break;
            }
        
    }
}
void GetUserName() {
    std::cout << " What is your name?" << std::endl;
    std::cin >> userName;
    system("CLS");
}
bool CreateServer() {
    /* Bind the server to the default localhost.     */
    /* A specific host address can be specified by   */
    /* enet_address_set_host (& address, "x.x.x.x"); */
    address.host = ENET_HOST_ANY;
    /* Bind the server to port 1234. */
    address.port = 1234;
    server = enet_host_create(&address /* the address to bind the server host to */,
        32      /* allow up to 32 clients and/or outgoing connections */,
        2      /* allow up to 2 channels to be used, 0 and 1 */,
        0      /* assume any amount of incoming bandwidth */,
        0      /* assume any amount of outgoing bandwidth */);

    return server != nullptr;
}
bool CreateClient() {

    client = enet_host_create(NULL /* create a client host */,
        1 /* only allow 1 outgoing connection */,
        2 /* allow up 2 channels to be used, 0 and 1 */,
        0 /* assume any amount of incoming bandwidth */,
        0 /* assume any amount of outgoing bandwidth */);

    return client != nullptr;


}
void CheckClient() {
    if (!CreateClient())
    {
        std::cout << (stderr,
            "An error occurred while trying to create an ENet client host.\n");
        exit(EXIT_FAILURE);
    }
}
void CheckServer() {
    if (!CreateServer())
    {
        std::cout << (stderr,
            "An error occurred while trying to create an ENet server host.\n");
        exit(EXIT_FAILURE);
    }
    
}
void SendPacket(ENetHost* person) {
    std::string prefix = "["+ userName + "] ";
    std::string message;
    std::getline(std::cin, message);
    message.insert(0, prefix);
    std::cin.clear();

    /* Create a reliable packet of size 7 containing "packet\0" */
    ENetPacket* packet = enet_packet_create(message.c_str(),
        message.length() + 1,
        ENET_PACKET_FLAG_RELIABLE);
    /* Send the packet to the peer over channel id 0. */
    /* One could also broadcast the packet by         */
    enet_host_broadcast(person, 0, packet);
    //enet_peer_send(event.peer, 0, packet);
        /* One could just use enet_host_service() instead. */
    enet_host_flush(person);
}  //not used, only for demonstation of how to effectivly send a packet
void ServerInstatiate() {
   
    /* Wait up to 1000 milliseconds for an event. */
    while (isDone == false) {
        
        ENetEvent event;
        while (enet_host_service(server, &event, 1000) > 0)
        {
           
            switch (event.type)
            {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "A new client connected from "
                    << event.peer->address.host
                    << event.peer->address.port
                    << std::endl;
                /* Store any relevant client information here. */
                event.peer->data = (void*)"Client information";

                break;
            case ENET_EVENT_TYPE_RECEIVE:
                std::cout << (char*)event.packet->data
                    << std::endl;
                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout << ("%s disconnected.\n", (char*)event.peer->data) << std::endl;
                /* Reset the peer's client information. */
                event.peer->data = NULL;
            }
           /* */

        }
       
        
    }
    
    
    
}

void HostInstatiate() {
    
    thisUser.name = "";
    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;
    /* Connect to some.server.net:1234. */
    enet_address_set_host(&address, "127.0.0.1");
    address.port = 1234;

    /* Initiate the connection, allocating the two channels 0 and 1. */
    peer = enet_host_connect(client, &address, 2, 0);
    if (peer == NULL)
    {
        fprintf(stderr,
            "No available peers for initiating an ENet connection.\n");
        exit(EXIT_FAILURE);
    }
    /* Wait up to 5 seconds for the connection attempt to succeed. */
    if (enet_host_service(client, &event, 1000) > 0 &&
        event.type == ENET_EVENT_TYPE_CONNECT)
    {
        std::cout << "Connection to 127.0.0.1:1234 succeeded." << std::endl;
    }
    else
    {
        /* Either the 5 seconds are up or a disconnect event was */
        /* received. Reset the peer in the event the 5 seconds   */
        /* had run out without any significant event.            */
        enet_peer_reset(peer);
        std::cout << "Connection to 127.0.0.1:1234 failed." <<std:: endl;
    }

    std::thread outputThread(ServerOuput);
    std::thread inputThread(ClientInput, std::ref(thisUser), client);
    outputThread.join();
    inputThread.join();
}

void Cleanup() {
    if (server != nullptr) {
        enet_host_destroy(server);
    }
    if (client != nullptr) {
        enet_host_destroy(client);
    }
}

int main(int argc, char** argv)
{
    if (enet_initialize() != 0)
    {
        std::cout << (stderr, "An error occurred while initializing ENet.\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);
  
    GetUserName();

    std::cout << "1) Create Server" << std::endl;
    std::cout << "2) Create Client" << std::endl;
    int UserInput;
    std::cin >> UserInput;

    switch (UserInput) {
    case 1:
        std::cout << "Creating chat room..." << std::endl;
        system("CLS");
        CheckServer();
        {
            std::thread ServerIntatiate(ServerInstatiate); //infinite loop
            std::thread inputThread(ClientInput, std::ref(thisUser), server);
            inputThread.join();
            ServerIntatiate.join();
        }
            break;
    case 2:
        std::cout << "Connecting to chat room..." << std::endl;
        system("CLS");
        CheckClient();
        HostInstatiate();
            break;
    default:
        std::cout << "Bad!" << std::endl;
            break;

    }
    
  



    Cleanup();
    return EXIT_SUCCESS;
}