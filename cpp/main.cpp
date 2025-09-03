#include "Game.h"
#include <cstdio>


float frametime = 0;

Game* g = nullptr; // global pointer

int main() {
    Game game("birb");

    g = &game;
    
    game.init();


    game.doRestart();


    game.setOnUpdate([&game](float dt){

     if (game.getState() == Game::State::OVER){
         return;
     }

     if (frametime > 1){
       frametime = 0;
       game.createPipe();
     }


     frametime += dt;

    }
    );


    game.startGameLoop(g);
    game.close();

    return 0;
}

