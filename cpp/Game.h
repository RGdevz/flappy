#include "Pipe.h"
#include "raylib.h"
#include <cassert>

#include <cstdio>
#include <functional>
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <memory>
#include "Entity.h"
#include "AssetLoader.h"
#include "Bird.h"

#define HEIGHT 850
#define WIDTH 650


#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

class Game {

 
  public: enum class State { MENU,PLAY,OVER };

  private:


      void realRestart(){

      score = 0;
      entities.clear();

      
      auto bird = createBird();

      addEntity(std::move(bird));

      setState(State::PLAY);

      restartNextFrame = false;
      

    }


  bool restartNextFrame = false;

  int score = 0;

   std::vector<std::function<void(State)>> stateChangeCallbacks;

 const float maxDeltaTime = 0.03f;
  const char* title;
  RenderTexture2D targetTexture;
  float scaleDivisor = 2.0f; // Make textures 2x bigger (screen/2)
  float renderWidth;
  float renderHeight; 
  float bgOffset = 0.0f;   // Horizontal offset for scrolling
  float bgSpeed = 100.0f;  // Pixels per second
  std::list<std::unique_ptr<Entity>> entities;
  std::function<void(float)> onUpdateCallback; 

  State state = State::PLAY;
  


public:


void doRestart(){

 restartNextFrame = true;

}


   void onStateChange(std::function<void(State)> cb) {
    stateChangeCallbacks.push_back(std::move(cb));
    }


    void setState(State s) {
     if (state != s) {
       state = s;
       for (auto& cb : stateChangeCallbacks) {
           cb(state);
       }
     }
    }


  State getState(){
    return this->state;
  }


 void setOnUpdate(std::function<void(float)> cb) {
   onUpdateCallback = std::move(cb);
  }



 std::unique_ptr<Entity> createBird(){
 return   std::make_unique<Bird>(this,50, 20);
 }


 

   void createPipe(float minGapPercent = 0.24f, float maxGapPercent = 0.32f) {
    // pick random gapPercent in range
    float gapPercent = minGapPercent + (float)GetRandomValue(0, 1000) / 1000.0f * (maxGapPercent - minGapPercent);
    const float gap = renderHeight * gapPercent;

    // center of screen
    const float mid = renderHeight * 0.5f;

    // max deviation from center
    const float maxOffset = renderHeight * 0.11f; // adjust for how much it can move up/down

    // pick center around middle of screen
    const float center = mid + (float)GetRandomValue(-(int)maxOffset, (int)maxOffset);

    const float gapTop    = center - gap * 0.5f;
    const float gapBottom = center + gap * 0.5f;

    const Texture2D& topTex = AssetLoader::getInstance().getTex("pipe-up");
    const Texture2D& bottomTex = AssetLoader::getInstance().getTex("pipe-down");

    // --- Top pipe ---
    float topPipeHeight = gapTop;
    float topTexHeight = (float)topTex.height;

    auto topPipe = std::make_unique<Pipe>(this, renderWidth, 0.0f, true);
    if (topPipeHeight > topTexHeight) {
        topPipe->setScale(1.0f, topPipeHeight / topTexHeight);
    } else {
        topPipe->setY(topPipeHeight - topTexHeight);
        topPipe->setScale(1.0f, 1.0f);
    }
    addEntity(std::move(topPipe));

    // --- Bottom pipe ---
    float bottomPipeHeight = renderHeight - gapBottom;
    float bottomTexHeight = (float)bottomTex.height;

    auto bottomPipe = std::make_unique<Pipe>(this, renderWidth, gapBottom, false);
    if (bottomPipeHeight > bottomTexHeight) {
        bottomPipe->setScale(1.0f, bottomPipeHeight / bottomTexHeight);
    } else {
        bottomPipe->setScale(1.0f, 1.0f);
    }
    addEntity(std::move(bottomPipe));
}




 float getRenderWidth(){
    return renderWidth;
 }

 float getRenderHeight(){
    return  renderHeight;
 }


   void addEntity(std::unique_ptr<Entity> entity) {
     entities.push_back(std::move(entity));
  }
    

  void removeDeadEntities() {
     for (auto it = entities.begin(); it != entities.end(); ) {
       if (!(*it)->isAlive()) {       
           it = entities.erase(it);   
       } else {
           ++it;
       }
     }
    }

 

    
    Game(const char* windowTitle)
        : title(windowTitle) {}

    void init() {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
        SetConfigFlags(FLAG_VSYNC_HINT);
        InitWindow(WIDTH, HEIGHT, title);

        // Create render texture based on scaled screen size
        renderWidth = GetScreenWidth() / scaleDivisor;
        renderHeight = GetScreenHeight() / scaleDivisor;

        targetTexture = LoadRenderTexture(renderWidth, renderHeight);

        AssetLoader::getInstance().load();

    }


    
  static void loop(void* arg) {
   Game* game = (Game*)arg;
   Game::update(game);
   }



   static void startGameLoop(Game* game) {
   #if defined(PLATFORM_WEB)
     emscripten_set_main_loop_arg(loop, game, 0, 1);
   #else
    while (!WindowShouldClose()) {
   Game::update(game);
    }
  #endif
  }





    void close() {
     AssetLoader::getInstance().unload();
      UnloadRenderTexture(targetTexture);
      CloseWindow();
    }


    float getClampedDeltaTime() const {
        float dt = GetFrameTime();
        if (dt > maxDeltaTime) dt = maxDeltaTime;
        return dt;
    }

    
private:

    static void update(Game* _game) {


    // Update all entities
    float deltaTime = _game->getClampedDeltaTime();

   Game& g = *_game;  // <-- reference, not copy


    if (g.restartNextFrame) {
    g.realRestart();
    return;
    }

    if (IsKeyPressed(KEY_R)) {
     g.doRestart();
    }

    if (g.state == State::OVER && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      g.doRestart();
    }

    if (g.state == State::PLAY) {
     g.bgOffset -= g.bgSpeed * deltaTime;
     if (g.bgOffset <= -g.renderWidth) {
      g.bgOffset += g.renderWidth; // add renderWidth instead of setting 0
     }
    }

    Bird* bird = nullptr;
    for (auto& entity : g.entities) {
     if (entity->getType() == Entity::Type::BIRD) {
        bird = static_cast<Bird*>(entity.get());
        break; // assume only one bird
     }
    }

    if (bird) {
        for (auto& entity : g.entities) {
            if (entity->getType() == Entity::Type::PIPE) {
                Pipe* pipe = static_cast<Pipe*>(entity.get());
                if (pipe->isBottomPipe() && !pipe->hasScored() &&
                    pipe->getX() + pipe->getWidth() < bird->getX()) {
                    g.score++;
                    pipe->setScored(true);
                }
            }
        }
    }

    for (auto it1 = g.entities.begin(); it1 != g.entities.end(); ++it1) {
      (*it1)->update(deltaTime);

      // Check collisions with other entities
      auto it2 = it1;
      ++it2; // avoid checking against itself and previous entities
      for (; it2 != g.entities.end(); ++it2) {
       (*it1)->checkCollision(it2->get());
      }
    }


    if (g.onUpdateCallback) {
      g.onUpdateCallback(deltaTime);
    }


    g.removeDeadEntities();


    if (IsWindowResized()) {
        // Save old target
        RenderTexture2D oldTarget = g.targetTexture;

        // Update render size
        g.renderWidth  = GetScreenWidth() / g.scaleDivisor;
        g.renderHeight = GetScreenHeight() / g.scaleDivisor;

        // Load new render texture
        g.targetTexture = LoadRenderTexture(g.renderWidth, g.renderHeight);

        BeginTextureMode(g.targetTexture);
        ClearBackground(RAYWHITE);
        DrawTexturePro(oldTarget.texture,
                       {0, 0, (float)oldTarget.texture.width, -(float)oldTarget.texture.height},
                       {0, 0, (float)g.renderWidth, (float)g.renderHeight},
                       {0, 0}, 0.0f, WHITE);
        EndTextureMode();

        // Unload old texture
        UnloadRenderTexture(oldTarget);
    }




    g.draw();
}



    void drawScaledTexture(Texture2D texture, float x, float y, int destWidth, int destHeight) {
        Rectangle srcRec = { 0, 0, (float)texture.width, (float)texture.height };
        Rectangle destRec = { x, y, (float)destWidth, (float)destHeight };
        DrawTexturePro(texture, srcRec, destRec, Vector2{0,0}, 0.0f, WHITE);
    }



   void draw() {


    renderWidth  = (float)GetScreenWidth() / scaleDivisor;
    renderHeight = (float)GetScreenHeight() / scaleDivisor;

    // Draw game to off-screen texture at scaled resolution
    BeginTextureMode(targetTexture);
    ClearBackground(RAYWHITE);

    Texture2D bg = AssetLoader::getInstance().getTex("background");


    int padding = 3;
    drawScaledTexture(bg, bgOffset, 0, (int)(renderWidth + padding), (int)renderHeight);
    drawScaledTexture(bg, bgOffset + renderWidth, 0, (int)(renderWidth + padding), (int)renderHeight);

    // Draw entities
    for (const auto& entity : entities) {
     entity->draw();
    }

    DrawText(TextFormat("Score: %d", score), 20+2, 20+2, 24, BLACK);
    DrawText(TextFormat("Score: %d", score), 20, 20, 24, WHITE);

    if (state == State::OVER){

      std::string text = "Game Over Press R to restart";
      int fontSize = 15;

      Vector2 textSize = MeasureTextEx(GetFontDefault(), text.c_str(), fontSize, 1);

       // Calculate centered position
       float x = (renderWidth - textSize.x) / 2.0f;
       float y = (renderHeight - textSize.y) / 2.0f;
       
       DrawText(text.c_str(), (int)x+2, (int)y+2, fontSize, BLACK);
       DrawText(text.c_str(), (int)x, (int)y, fontSize, WHITE);
    }

    EndTextureMode();

    // Draw render texture to fill entire window
    BeginDrawing();
    ClearBackground(BLACK);
    Rectangle srcRec = { 0, 0, renderWidth, -renderHeight };
    Rectangle destRec = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    DrawTexturePro(targetTexture.texture, srcRec, destRec, Vector2{0,0}, 0.0f, WHITE);
    EndDrawing();
}


};