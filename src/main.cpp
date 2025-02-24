#include "engine/engine.h"
#include <enet/enet.h>
#include <vector>
#include <iostream>
#include <cassert>
#include <thread>

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


    Ball(glm::vec2 position, double points, glm::vec3 color): pos(position), points(points),  color(color) {};
    Ball(glm::vec2 position, double points, glm::vec3 color, int ID): pos(position), points(points),  color(color), ID(ID) {};

    float getRadius() {
        return static_cast<float>(points) * 0.004;
    }

    bool checkCollision(Ball *second) {
        if (glm::distance(pos, second->pos) - glm::max(this->getRadius(), second->getRadius()) > 0) {
            return false;
        }
        return true;
    }
    

    void update(BS::Timer time) {
        float mouseX = (BS::Window::getMouseX() / BS::Window::getWidth()) * 2.0f - 1.0f;
        float mouseY = (BS::Window::getMouseY() / BS::Window::getHeight()) * 2.0f - 1.0f;
    
        mouseY *= -1;
    
        mouseX *= BS::Window::getAspect();
        
        float mouseRadius = 0.1;

        velocity = glm::vec2(mouseX, mouseY) / mouseRadius;
        
        float length = glm::length(velocity);

        if (length > 1 - glm::max(0.5f, this->getRadius())) {
            velocity /= length;
        }

        pos += velocity * time.getDelta();
    }
};

void Networking(std::vector<Ball> &balls) {
    ENetHost *client = nullptr;
	ENetPeer *server = nullptr;

	client = enet_host_create(NULL,
		1,
		0,
		0,
		0);

	if (client == NULL)
	{
		std::cout << "An error occurred while trying to create an ENet client host.\n";
		return;
	}

	ENetAddress address = {};
	//enet_address_set_host(&address, "127.0.0.1");
	enet_address_set_host(&address, "localhost");
	address.port = 25566;

    bool firstPacket = true;

	server = enet_host_connect(client, &address, 0, 0);

	if (server == nullptr)
	{
		std::cout << "Wasn't able to initialize connection\n";
		enet_host_destroy(client);
	}
	else
	{
		ENetEvent event = {};
		if (enet_host_service(client, &event, 15000) > 0 &&
			event.type == ENET_EVENT_TYPE_CONNECT && BS::Window::isRunning())
		{
			std::cout << "Got a connection!\n";

			while (BS::Window::isRunning()) {
				ENetEvent event;

				while (enet_host_service(client, &event, 50) > 0) {
					switch (event.type)
					{
					case ENET_EVENT_TYPE_CONNECT:
					break;

					case ENET_EVENT_TYPE_RECEIVE:
                    if(!firstPacket) {
                        printf("A packet of length %u containing %s was received on channel %u.\n",
                            event.packet->dataLength,
                            event.packet->data,
                            event.channelID);
    
                        enet_packet_destroy(event.packet);
                    } else {
                        for(Ball &ball : balls) {
                            ball.ID = *event.packet->data;
                            printf("%i\n", ball.ID);
                        }
                        enet_packet_destroy(event.packet);
                        firstPacket = !firstPacket;
                    }
					

					break;

					case ENET_EVENT_TYPE_DISCONNECT:
					std::cout << "Server Disconected\n";
                    firstPacket = !firstPacket;
					break;
					}
				}

                std::string ballPos = "";

                for(Ball &ball : balls) {
                    ballPos.append(std::to_string(ball.ID) + " " + std::to_string(ball.pos.x) + " " + std::to_string(ball.pos.y));
                }
                SendPacket(ballPos.c_str(), ballPos.size() + 1, server);
			}
		}
		else
		{
			std::cout << "Wasn't able to connect\n";
		}

		enet_peer_reset(server);
		enet_host_destroy(client);
	}
}

int main() {
    BS::Window::create(1920, 1080, "Agar");
    glfwSwapInterval( 0 );

    std::vector<Ball> balls = {};

    std::vector<Ball> player_balls = {Ball(glm::vec2(0, 0), 20, glm::vec3(0.7, 0.0 , 0.0))};

    BS::Mesh square = BS::Mesh(BS::VertexBuffer({0,0,1,0,1,1,0,1}, 2), {}, GL_TRIANGLE_FAN);
    BS::Mesh background = BS::Mesh(BS::VertexBuffer({-1000,-1000,1000,-1000,1000,1000,-1000,1000}, 2), {}, GL_TRIANGLE_FAN);

    float zoom = 0.3;

    BS::ShaderProgram worldShader = BS::ShaderProgram("./assets/shaders/world.vert", "./assets/shaders/world.frag", nullptr);

    GLuint ballTexture = BS::Texture::loadFromFile("./assets/textures/Ball.png", GL_LINEAR, GL_REPEAT);
    GLuint backGround = BS::Texture::loadFromFile("./assets/textures/BackGround.png", GL_NEAREST, GL_REPEAT);

    BS::Timer time;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glm::vec2 ball_direction;

    glm::vec2 cameraPosition;

    std::thread networking(Networking, std::ref(player_balls));

    int fps = 0;

    double fpstimer;

    while(BS::Window::isRunning()) {
        BS::Window::pollEvents();
        worldShader.use();
        time.update();

        fps ++;

        fpstimer += time.getDelta();

        if(fpstimer >= 1) {
            printf("%i\n", fps);
            fpstimer = 0;
            fps = 0;
        }

        BS::Texture::use(backGround);
        
        worldShader.setVector2("position", glm::vec2(0, 0));
        worldShader.setFloat("size", 1);
        worldShader.setVector4("hue", glm::vec4(0.7, 0.7, 0.7, 1.0));

        background.render();

        worldShader.setFloat("aspect", BS::Window::getAspect());
        
        BS::Texture::use(ballTexture);

        for(auto &ball : balls) {
            worldShader.setVector4("hue", glm::vec4(ball.color, 1.0));
            worldShader.setVector2("position", glm::vec2(ball.pos.x - static_cast<float>(ball.getRadius()), ball.pos.y - static_cast<float>(ball.getRadius())));
            worldShader.setVector2("cameraPosition", cameraPosition);
            worldShader.setFloat("size", ball.getRadius() * 2);

            square.render();
            
            for(Ball &player_ball : player_balls) {
                if(player_ball.checkCollision(&ball)) {
                    if (player_ball.points > ball.points) {
                        player_ball.points += ball.points;
                        ball.isDead = true;
                        break;
                    } else if (player_ball.points < ball.points) {
                        
                    } else {}
                }
            }
        }
        balls.erase(std::remove_if(balls.begin(), 
                              balls.end(),
                              [](auto ball) { return ball.isDead; }),
                balls.end());

        for(Ball &ball : player_balls) {
            ball.update(time);
            worldShader.setVector4("hue", glm::vec4(ball.color, 1.0));
            worldShader.setVector2("position", glm::vec2(ball.pos.x - static_cast<float>(ball.getRadius()), ball.pos.y - static_cast<float>(ball.getRadius())));
            worldShader.setFloat("size", ball.getRadius() * 2);
            worldShader.setFloat("zoom", glm::max(glm::min(20.0 ,1 / ball.getRadius() * 0.5 - 4), 1.0));
            cameraPosition = glm::vec2(ball.pos.x, ball.pos.y);
            worldShader.setVector2("cameraPosition", cameraPosition);
            
            square.render();
        }

        BS::Window::swapBuffers();
    }

    square.destroy();
    worldShader.destroy();
    BS::Texture::destroy(ballTexture);

    networking.join();

    BS::Window::close();

    return 0;
}