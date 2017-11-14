#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include <cstring>
#include <wchar.h>

enum font_error {
    A_NULL_OBJECT = 0,
    A_SUCCESS,
    A_ERROR_OPEN_FILE,
    A_ERROR_LOAD_TEXTURE,
};

struct font_table {
    int f_height;
    int f_width;
    int t_height;
    int t_width;
    int * table;
    char * tex_name;
    SDL_Texture * font;
};
typedef struct font_table font_table_t;

class FontTable {
  public:
    FontTable( void ) : render( nullptr ), ft( nullptr ) {}
    ~FontTable( void );
    int load( SDL_Renderer * r, const char * font );
    void draw( int x, int y, const wchar_t * text );
    void reload( SDL_Renderer * r );
    void set_coloru( Uint32 color );
    int get_error( void ) const { return last_error; }
    int get_height( void ) const { return ft->t_height; }
    int get_width( void ) const { return ft->t_width; }

  private:
    int last_error = A_SUCCESS;
    SDL_Renderer * render;
    font_table_t * ft;
};

// структура файла конфигурации
// заголовок:
//     unsigned int size;
//     unsigned int abc_size;
//     char * filename[size];
//     int width;
//     int height;
// размер заголовка = sizeof(int) * 4 + size + 1;
// далее алфавит до конца файла в формате UTF-8
