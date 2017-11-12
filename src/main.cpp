#include <cstdlib>
#include <ctime>
#include <iostream>
#include <wchar.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "font.hpp"

const char game_name[] = "bordertanks";
const int screen_width = 640;
const int screen_height = 480;
const int tile_size = 37;
const int pole_size = 12;
const int two_pole_size = pole_size * pole_size;
const int tile_shift_x = ( screen_width - pole_size * tile_size ) / 2;
const int tile_shift_y = ( screen_height - pole_size * tile_size ) / 2;
bool quit_flag = false;
bool draw_game_info = false;
const short max_reload = 5;

enum {
    EMPTY = -1,
    BRICK,
    WALL,
    TANK
};

enum {
    MOVE_RIGHT = 0,
    MOVE_UP,
    MOVE_LEFT,
    MOVE_DOWN,
};

enum {
    GAME_IDLE = 0,
    GAME_ERROR,
    GAME_WIN
};

struct tank_t {
    int x, y;
    bool move;
    bool shoot;
    short direction;
    short reload;

    static const int anim_count = 3;
    char curr_frame = 0;

    void draw(SDL_Renderer *r, SDL_Texture *tex);
};

struct bullet_t {
    int x, y;
    short direction;
};

SDL_Window *window = NULL;
SDL_Renderer *render = NULL;
SDL_Event event;
SDL_Texture *tiles = NULL;

short pole[pole_size][pole_size];
short game_check_init = 0;

tank_t tank;

font_table_t ft;

void send_error( int code )
{
    std::cout << SDL_GetError() << std::endl;
    exit( code );
}

void tank_t::draw(SDL_Renderer *r, SDL_Texture *tex) {
    SDL_Rect wnd = { 0, 0, tile_size, tile_size };
    SDL_Rect pos = { 0, 0, tile_size, tile_size };

    pos.x = this->x + tile_shift_x; 
    pos.y = this->y + tile_shift_y;
    wnd.x = (this->direction * this->anim_count + this->curr_frame) * tile_size;
    if (this->move) {
        if (this->curr_frame >= anim_count - 1) {
            this->curr_frame = 0;
        } else {
            this->curr_frame++;
        }
    }
    SDL_RenderCopy( r, tex, &wnd, &pos );
}

void tile_draw( SDL_Renderer *r, SDL_Texture *tex, short id, int p )
{
    if (id == EMPTY) {return;}
    SDL_Rect wnd = { 0, 0, tile_size, tile_size };
    SDL_Rect pos = { 0, 0, tile_size, tile_size };
    short x = p % pole_size;
    short y = p / pole_size;

    pos.x = x * tile_size + tile_shift_x; 
    pos.y = y * tile_size + tile_shift_y;
    wnd.x = id * tile_size;
    wnd.y = tile_size;
    SDL_RenderCopy( r, tex, &wnd, &pos );
}

void game_restart( void )
{
    tank.x = 0;
    tank.y = 0;
    tank.shoot = false;
    tank.move = false;
    tank.direction = MOVE_RIGHT;
    for ( short i = 0; i < two_pole_size; i++ ) {
        pole[i%pole_size][i/pole_size] = rand() % 3 - 1;
    }
    pole[0][0] = EMPTY;
    pole[0][1] = EMPTY;
    pole[1][0] = EMPTY;
    pole[1][1] = EMPTY;
}


void game_event( SDL_Event *event )
{
    SDL_PollEvent( event );
    switch ( event->type ) {
        case SDL_QUIT:
            quit_flag = true;
            break;
        case SDL_KEYDOWN:
            switch ( event->key.keysym.sym ) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    quit_flag = true;
                    break;
                case SDLK_RIGHT:
                    tank.direction = MOVE_RIGHT;
                    tank.move = true;
                    break;
                case SDLK_LEFT:
                    tank.direction = MOVE_LEFT;
                    tank.move = true;
                    break;
                case SDLK_UP:
                    tank.direction = MOVE_UP;
                    tank.move = true;
                    break;
                case SDLK_DOWN:
                    tank.direction = MOVE_DOWN;
                    tank.move = true;
                    break;
                case SDLK_SPACE:
                    tank.shoot = true;
                    break;
                case SDLK_r:
                    // game_restart();
                    break;
            }
            break;
        case SDL_KEYUP:
        switch ( event->key.keysym.sym ) {
                case SDLK_RIGHT:
                case SDLK_LEFT:
                case SDLK_UP:
                case SDLK_DOWN:
                    tank.move = false;
                    break;
                case SDLK_SPACE:
                    tank.shoot = false;
                    break;
            }
            break;
        default:
            break;
    }
    // SDL2 button hack
    event->button.button = 0;
    event->key.keysym.sym = 0;
}

void game_loop( void )
{
    tank_t tank1 = tank;
    if (tank.move) {
        switch (tank.direction) {
            case MOVE_RIGHT:
                tank1.x++;
                break;
            case MOVE_LEFT:
                tank1.x--;
                break;
            case MOVE_UP:
                tank1.y--;
                break;
            case MOVE_DOWN:
                tank1.y++;
                break;
            default:
                break;
        }
    }
    if (pole[tank1.x / tile_size][tank1.y / tile_size] == EMPTY &&
        pole[(tank1.x + tile_size - 1)/ tile_size][tank1.y / tile_size] == EMPTY && 
        pole[tank1.x / tile_size][(tank1.y + tile_size - 1)/ tile_size] == EMPTY &&
        pole[(tank1.x + tile_size - 1) / tile_size][(tank1.y + tile_size - 1) / tile_size] == EMPTY) {
        tank = tank1;
    }
    if (tank.shoot) {
        if (tank.reload) {
            --tank.reload;
        } else {
            tank.reload = max_reload;
        }
    } else {
        tank.reload = 0;
    }
    // printf("%d %d %d\n", tank1.x / tile_size, tank1.y / tile_size, pole[tank1.x / tile_size][tank1.y / tile_size]);
}

void game_render( void )
{
    SDL_RenderClear( render );
    for ( short i = 0; i < two_pole_size; i++ ) {
        tile_draw( render, tiles, pole[i%pole_size][i/pole_size], i );
    }
    tank.draw( render, tiles );

    // debug log
    wchar_t buffer[32];
    swprintf(buffer, 32, L"%d %d %d", tank.x / tile_size, tank.y / tile_size, pole[tank.x / tile_size][tank.y / tile_size]);
    ft.draw(render, buffer, 0, 10);

    SDL_RenderPresent( render );
}

void game_destroy( void )
{
    ft.destroy();
    SDL_DestroyTexture( tiles );
    SDL_DestroyRenderer( render );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

void game_init( void )
{
    window = SDL_CreateWindow( game_name, SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, screen_width, screen_height, SDL_WINDOW_SHOWN );
    if ( window == NULL ) {
        send_error( EXIT_FAILURE );
    }
    render = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | 
        SDL_RENDERER_PRESENTVSYNC );
    if ( render == NULL ) {
        send_error( EXIT_FAILURE );
    }
    tiles = IMG_LoadTexture( render, "./images/tiles.png" );
    ft.load(render, "./configs/font.cfg");
    srand( time( NULL ) );
    game_restart();
}

int main( int argc, char *argv[] )
{
    Uint32 FPS_MAX = 1000 / 31; // 30 fps

    game_init();
    while ( quit_flag == false ) {
        game_event( &event );
        game_loop();
        game_render();
        SDL_Delay( FPS_MAX );
    }
    game_destroy();
    return EXIT_SUCCESS;
}