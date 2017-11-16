#include "font.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <wchar.h>

// place for enum, struct & etc.
enum { EMPTY = -1, BRICK, WALL, CONCRETE, WATER, TANK };

enum {
    MOVE_RIGHT = 0,
    MOVE_UP,
    MOVE_LEFT,
    MOVE_DOWN,
};

enum { GAME_IDLE = 0, GAME_ERROR, GAME_WIN };

struct color_t {
    Uint8 r;
    Uint8 g;
    Uint8 b;
};

struct tank_t {
    int x, y;
    bool move;
    bool shoot;
    short direction;
    short reload;
    color_t color = {0, 200, 0};

    static const int anim_count = 3;
    char curr_frame = 0;

    void draw( SDL_Renderer * r, SDL_Texture * tex );
};

struct bullet_t {
    int x, y;
    short direction;
    short lifetime;

    bool operator<( const bullet_t & a ) { return lifetime > a.lifetime; }
};

// place for variables and consts
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
const short max_reload = 15;

// RIGH UP LEFT DOWN
const int bullet_correct[][ 2 ] = {
    {tile_size, tile_size / 2}, {tile_size / 2, 0}, {0, tile_size / 2}, {tile_size / 2, tile_size}};
const int direction_vector[][ 2 ] = {{+1, 0}, {0, -1}, {-1, 0}, {0, +1}};

SDL_Window * window = NULL;
SDL_Renderer * render = NULL;
SDL_Event event;
SDL_Texture * tiles = NULL;

short pole[ pole_size ][ pole_size ];
short game_check_init = 0;

tank_t tank;

std::vector<bullet_t> bullets;

FontTable ft;

void send_error( int code ) {
    std::cout << SDL_GetError() << std::endl;
    exit( code );
}

void tank_t::draw( SDL_Renderer * r, SDL_Texture * tex ) {
    // tile (x, y, w, h) on image
    SDL_Rect wnd = {0, 0, tile_size, tile_size};
    // position on screen
    SDL_Rect pos = {0, 0, tile_size, tile_size};

    pos.x = this->x + tile_shift_x;
    pos.y = this->y + tile_shift_y;
    wnd.x = ( this->direction * this->anim_count + this->curr_frame ) * tile_size;
    if ( this->move ) {
        if ( this->curr_frame >= anim_count - 1 ) {
            this->curr_frame = 0;
        } else {
            this->curr_frame++;
        }
    }
    // dirty color changer hack
    color_t def_color;
    SDL_GetTextureColorMod( tex, &def_color.r, &def_color.g, &def_color.b );
    SDL_SetTextureColorMod( tex, this->color.r, this->color.g, this->color.b );
    SDL_RenderCopy( r, tex, &wnd, &pos );
    SDL_SetTextureColorMod( tex, def_color.r, def_color.g, def_color.b );
}

void tile_draw( SDL_Renderer * r, SDL_Texture * tex, short id, int p ) {
    if ( id == EMPTY ) {
        return;
    }
    SDL_Rect wnd = {0, 0, tile_size, tile_size};
    SDL_Rect pos = {0, 0, tile_size, tile_size};
    short x = p % pole_size;
    short y = p / pole_size;

    pos.x = x * tile_size + tile_shift_x;
    pos.y = y * tile_size + tile_shift_y;
    wnd.x = id * tile_size;
    wnd.y = tile_size;
    SDL_RenderCopy( r, tex, &wnd, &pos );
}

void load_map( const char * filename ) {
    // +1 for \n char
    const int map_size = ( pole_size + 1 ) * pole_size;
    char buffer[ 256 ];
    FILE * f = fopen( filename, "r" );
    fread( buffer, 1, map_size, f );
    fclose( f );

    int new_line_count = 0;
    for ( short i = 0; i < map_size; i++ ) {
        short x = ( i - new_line_count ) % pole_size;
        short y = ( i - new_line_count ) / pole_size;
        switch ( buffer[ i ] ) {
        case 'P':
            tank = {x * tile_size, y * tile_size, false, false, MOVE_RIGHT};
            pole[ x ][ y ] = EMPTY;
            break;
        case '#':
            pole[ x ][ y ] = WALL;
            break;
        case '=':
            pole[ x ][ y ] = BRICK;
            break;
        case '.':
            pole[ x ][ y ] = EMPTY;
            break;
        case '+':
            pole[ x ][ y ] = CONCRETE;
            break;
        case '~':
            pole[ x ][ y ] = WATER;
            break;
        case '\n':
            new_line_count++;
            break;
        default:
            break;
        }
    }
}

void game_restart( void ) {
    load_map( "./maps/map01.txt" );
    // auto-generated map
    // tank = { 0, 0, false, false, MOVE_RIGHT };
    // for ( short i = 0; i < two_pole_size; i++ ) {
    //     pole[i%pole_size][i/pole_size] = rand() % 5 - 1;
    // }
    // pole[0][0] = pole[0][1] = pole[1][0] = pole[1][1] = EMPTY;
}

void game_event( SDL_Event * event ) {
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
            game_restart();
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

void game_loop( void ) {
    tank_t tank1 = tank;
    if ( tank.move ) {
        tank1.x += direction_vector[ tank1.direction ][ 0 ];
        tank1.y += direction_vector[ tank1.direction ][ 1 ];
    }
    if ( pole[ tank1.x / tile_size ][ tank1.y / tile_size ] == EMPTY &&
         pole[ ( tank1.x + tile_size - 1 ) / tile_size ][ tank1.y / tile_size ] == EMPTY &&
         pole[ tank1.x / tile_size ][ ( tank1.y + tile_size - 1 ) / tile_size ] == EMPTY &&
         pole[ ( tank1.x + tile_size - 1 ) / tile_size ]
             [ ( tank1.y + tile_size - 1 ) / tile_size ] == EMPTY ) {
        tank = tank1;
    }
    if ( tank.reload ) {
        --tank.reload;
    } else {
        if ( tank.shoot ) {
            tank.reload = max_reload;
            bullets.push_back( {tank.x + bullet_correct[ tank.direction ][ 0 ],
                                tank.y + bullet_correct[ tank.direction ][ 1 ], tank.direction,
                                500} );
        }
    }

    // experimental code
    for ( auto & it : bullets ) {
        it.x += 2 * direction_vector[ it.direction ][ 0 ];
        it.y += 2 * direction_vector[ it.direction ][ 1 ];
        // collide with pole
        short * curr_tile = &pole[ it.x / tile_size ][ it.y / tile_size ];
        if ( *curr_tile != EMPTY ) {
            // simple code for destroy tile (please update this code later)
            if ( *curr_tile == BRICK ) {
                *curr_tile = EMPTY;
            }
            it.lifetime = 0;
        }
        it.lifetime--;
    }
    std::sort( bullets.begin(), bullets.end() );
    auto index = bullets.size();
    for ( auto it = bullets.rbegin(); it != bullets.rend(); it++ ) {
        if ( it->lifetime >= 0 ) {
            break;
        }
        index--;
    }
    bullets.resize( index );
}

void game_render( void ) {
    SDL_RenderClear( render );
    // render game pole
    for ( short i = 0; i < two_pole_size; i++ ) {
        tile_draw( render, tiles, pole[ i % pole_size ][ i / pole_size ], i );
    }
    tank.draw( render, tiles );

    // render bullets
    SDL_SetRenderDrawColor( render, 255, 255, 255, 255 );
    for ( auto & it : bullets ) {
        SDL_Rect rect = {it.x - 3 + tile_shift_x, it.y - 3 + tile_shift_y, 6, 6};
        SDL_RenderFillRect( render, &rect );
    }
    SDL_SetRenderDrawColor( render, 0, 0, 0, 255 );

    // debug log
    wchar_t buffer[ 64 ];
    swprintf( buffer, 64, L"ix: %d\niy: %d\nid: %d\npx: %d\npy: %d", tank.x / tile_size,
              tank.y / tile_size, pole[ tank.x / tile_size ][ tank.y / tile_size ], tank.x,
              tank.y );
    ft.draw( 5, 5, buffer );

    SDL_RenderPresent( render );
}

void game_destroy( void ) {
    SDL_DestroyTexture( tiles );
    SDL_DestroyRenderer( render );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

void game_init( void ) {
    window = SDL_CreateWindow( game_name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               screen_width, screen_height, SDL_WINDOW_SHOWN );
    if ( window == NULL ) {
        send_error( EXIT_FAILURE );
    }
    render = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if ( render == NULL ) {
        send_error( EXIT_FAILURE );
    }
    tiles = IMG_LoadTexture( render, "./images/tiles.png" );
    ft.load( render, "./configs/font.cfg" );
    srand( time( NULL ) );
    game_restart();
}

int main( int argc, char * argv[] ) {
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