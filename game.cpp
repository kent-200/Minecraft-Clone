#include "game.h"

// g++ -o run game.cpp text.cpp -lglfw3 -lkernel32 -lopengl32 -lglu32 -lglew32 


int main(void){
    Game game(800, 600);
    game.run();
    return 0;
}