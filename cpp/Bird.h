#pragma once
#include "Entity.h"
#include <string>
#include <vector>

class Game;

class Bird : public Entity {
private:
    float velocityY = 0.0f;
    const float gravity = 500.0f;
    const float jumpForce = 230.0f;
    std::vector<std::string> textures;
    int currentTexture;
    float frameTimer;

public:
    Bird(Game* g, float x, float y);
    void update(float deltaTime) override;
    void onCollision(Entity* other) override;
};
