#include <enet/enet.h>
#include <iostream>

#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <algorithm>

void SendPacket(const char *data, size_t s, ENetPeer *to) {
    ENetPacket *packet = enet_packet_create(data, s, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(to, 0, packet);
}

struct Ball {
    glm::vec2 pos;
    double points;
    glm::vec3 color;
    glm::vec2 velocity;
    bool isPlayer;
    bool isDead = false;
    int ID = 0;

    Ball(int ID): ID(ID) {};
    Ball(glm::vec2 position, double points, glm::vec3 color): pos(position), points(points),  color(color) {};
    Ball(glm::vec2 position, double points, glm::vec3 color, int ID): pos(position), points(points),  color(color), ID(ID) {};
};

void sendPlayer(std::vector<Ball> balls, int ID, ENetPeer *client) {
    std::string message = "";
  
    for(Ball &ball : balls) {
        if(ball.ID == ID) {
            ball.isDead = true;
            break;
        } else {
            message.append(std::to_string(ball.ID) + " " + std::to_string(ball.pos.x)+ " " + std::to_string(ball.pos.y));
        }
    }
    balls.erase(std::remove_if(balls.begin(), 
                              balls.end(),
                              [](auto ball) { return ball.isDead; }),
                balls.end());
    SendPacket(message.c_str(), message.size(), client);
}

int main(int argc, char **argv) {

    uint8_t max_clients_count = 32;
    int tickrate = 32;
    int port = 25566;

    std::vector<Ball>  players;

    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.find('=') == -1) {
          args.push_back(arg);
        } else {
          std::string key = arg.substr(0, arg.find('='));
          std::string value = arg.substr(arg.find('=') + 1);
          args.push_back(key);
          args.push_back(value);
        }
    }
  
    try {
      for (int i = 0; i < args.size(); ++i) {
        if (args[i] == "-c" || args[i] == "--max_clients_count") {
          max_clients_count = std::stoi(args.at(++i));
        } else if (args[i] == "-h" || args[i] == "--help") {
            printf("Usage:\n    Server [arguments]\n\nArguments:\n    -c or --max_clients_count    Sets max players count who can connect to a server.\n    -h or --help                 Print Help (This message) and exit\n    -p or --port                 Sets server port\n    -t or --tickrate             Sets how many times per second server update events");
            return 0;
        } else if (args[i] == "-p" || args[i] == "--port") {
            port = std::stoi(args.at(++i));
        } else if (args[i] == "-t" || args[i] == "--tickrate") {
            tickrate = std::stoi(args.at(++i));
        }
      }
    } catch (...) {
      throw std::invalid_argument("Invalid arguments");
    }

    if(enet_initialize() != 0) {
        std::runtime_error("Error: can't initialize enet");
    }
    printf("Info: enet initialized\n");
    

    ENetAddress addres = {};
    ENetHost *server = nullptr;
    addres.host = ENET_HOST_ANY;
    addres.port = port;

    server = enet_host_create(&addres,
        max_clients_count,
        0,
        0,
        0
    );

    if(server == NULL){
        std::runtime_error("Error: Can't create server\n");
    }

    bool run;

    ENetPeer *client = 0;

    tickrate *= 0.001f;
  
	while (true)
	{

		//recieve events

		ENetEvent event = {};
		/* Wait up to 10 milliseconds for an event. */
		//you probably don't want to wait here in a game and do other stuff,
		//or calculate how much you can wait each time
		while (enet_host_service(server, &event, 50) > 0)
		{
			switch (event.type)
			{
				case ENET_EVENT_TYPE_CONNECT:
				{
					printf("A new client connected from %x:%u.\n",
						event.peer->address.host,
						event.peer->address.port);

					client = event.peer;
          
          std::string temp = std::to_string(client->connectID);

          SendPacket(temp.c_str(), temp.size() + 1, client);

          players.push_back(Ball(client->connectID));
						
					break;
				}
				case ENET_EVENT_TYPE_RECEIVE:
				{
					printf("A packet of length %u containing %s was received on channel %u player %i.\n",
						event.packet->dataLength,
						event.packet->data,
						event.channelID,
            event.peer->connectID);
					/* Clean up the packet now that we're done using it. */
					enet_packet_destroy(event.packet);
          
				
					break;
				}

				case ENET_EVENT_TYPE_DISCONNECT:
				{
					printf("client disconnected.\n");

					client = 0;
					break;
				}
			}
		}
	}
		
	enet_host_destroy(server);
}