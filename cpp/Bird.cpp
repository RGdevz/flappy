#include "Bird.h"
#include "Entity.h"
#include "AssetLoader.h"
#include "Game.h"
#include "raylib.h"

Bird::Bird(Game* g, float x, float y) : Entity(g,Entity::Type::BIRD, x, y) {
    textures = {"bird-1", "bird-2", "bird-3"};
    currentTexture = 0;
    frameTimer = 0.0f;

    setTexture(AssetLoader::getInstance().getTex(textures[currentTexture]));

     game->onStateChange([](Game::State s){
      
    if (s == Game::State::OVER){
     Sound flap = AssetLoader::getInstance().getSound("hit");
     PlaySound(flap);
     // don't touch 'this' here!
    }
   }
   );

}




void Bird::onCollision(Entity* e) {
   
   if (e->getType() == Entity::Type::PIPE){
       game->setState(Game::State::OVER);
   }
    
}



void Bird::update(float deltaTime) {
    float h = game->getRenderHeight();

    // Input & gravity
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_SPACE)) {
    if (game->getState() == Game::State::OVER){
      return;
    }

    Sound flap = AssetLoader::getInstance().getSound("flap");
    PlaySound(flap);

    velocityY = -jumpForce;
    }


    velocityY += gravity * deltaTime;
    y += velocityY * deltaTime;

    // Clamp position
    if (y + height  > h) {
      game->setState(Game::State::OVER);
      y = h - height ;
      velocityY = 0;
    }
    if (y < 0) {
        y = 0;
        velocityY = 0;
    }

    
    if (game->getState() == Game::State::OVER){
      return;
    }
    
    frameTimer += deltaTime;
    if (frameTimer >= 0.2f) {
      frameTimer = 0.0f;
      currentTexture = (currentTexture + 1) % textures.size();
      setTexture(AssetLoader::getInstance().getTex(textures[currentTexture]));
    }

}
