#pragma once
#include "raylib.h"
#include "string"

class Game; // forward declaration of Game

class Entity {
protected:
    float x, y;              // Position
    float width, height;     // Dimensions
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    
    Texture2D texture;       // Entity texture
    Color tint;              // Texture tint
    bool hasTexture;         // Flag to check if texture is set
    Game* game;
    bool alive = true;



public:
    enum class Type { BIRD, PIPE, OTHER };

    Type getType() const { return type; }


    std::string getTypeName() const {
    switch (type) {
        case Type::BIRD:  return "BIRD";
        case Type::PIPE:  return "PIPE";
        case Type::OTHER: return "OTHER";
    }
    return "UNKNOWN";
}

    void kill() { alive = false; }
    bool isAlive() const { return alive; }

    // Constructor
    Entity(Game* game, Type type, float x = 0, float y = 0, float width = 0, float height = 0)
        : x(x), y(y), width(width), height(height), scaleX(1.0f), scaleY(1.0f),
          tint(WHITE), hasTexture(false), type(type), game(game) {}

    virtual ~Entity() = default;

    virtual void update(float deltaTime) = 0;

    // Collision
    virtual void onCollision(Entity* other) {
        // Default behavior: do nothing
    }

    bool checkCollision(Entity* other) {
        // Axis-Aligned Bounding Box collision
        if (!other) return false;

        float ax = x, ay = y;
        float aw = width * scaleX, ah = height * scaleY;
        float bx = other->x, by = other->y;
        float bw = other->width * other->scaleX, bh = other->height * other->scaleY;

        bool collided = (ax < bx + bw) && (ax + aw > bx) &&
                        (ay < by + bh) && (ay + ah > by);

        if (collided) {
            this->onCollision(other);
            other->onCollision(this);
        }

        return collided;
    }

    // Setters
    void setScale(float sx, float sy) { scaleX = sx; scaleY = sy; }
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }
    void setTexture(Texture2D newTexture) {
        texture = newTexture;
        hasTexture = true;
        if (width == 0 && height == 0) {
            width = texture.width;
            height = texture.height;
        }
    }
    void setSize(float newWidth, float newHeight) { width = newWidth; height = newHeight; }
    void setTint(Color newTint) { tint = newTint; }

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    float getScaleX() const { return scaleX; }
    float getScaleY() const { return scaleY; }

    // Draw
    virtual void draw() {
        if (hasTexture) {
            Rectangle srcRec = { 0, 0, (float)texture.width, (float)texture.height };
            Rectangle destRec = { x, y, width * scaleX, height * scaleY };
            DrawTexturePro(texture, srcRec, destRec, Vector2{0, 0}, 0.0f, tint);
        } else {
            DrawRectangle((int)x, (int)y, (int)(width * scaleX), (int)(height * scaleY), tint);
        }
    }


    private:
    Type type;

};
