#include "Pipe.h"
#include "Entity.h"
#include "AssetLoader.h"
#include "Game.h"
#include "raylib.h"
#include <cstdio>

Pipe::Pipe(Game* g, float x, float y,bool up) : Entity(g,Entity::Type::PIPE, x, y) {

    if (up){
    setTexture(AssetLoader::getInstance().getTex("pipe-down"));

    } else {
     bottomPipe = true;
    setTexture(AssetLoader::getInstance().getTex("pipe-up"));
    
    }
}



void Pipe::update(float deltaTime) {


  if (game->getState() == Game::State::OVER){
    return;
  }
 

  x -= deltaTime * 150;

  if (x < -60){

   kill();
  }


}
