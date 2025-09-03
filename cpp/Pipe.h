#pragma once
#include "Entity.h"
#include "raylib.h"
#include <string>
#include <vector>

#include "AssetLoader.h"

class Game;
class Pipe : public Entity {
private:
 bool scored = false;

bool bottomPipe = false;

public:
 bool hasScored() const { return scored; }
    void setScored(bool s) {

     Sound flap = AssetLoader::getInstance().getSound("score");
     PlaySound(flap);

     scored = s;
     }

    bool isBottomPipe(){
        return  bottomPipe;
    }
    Pipe(Game* g, float x, float y,bool up);
    void update(float deltaTime) override;
};
