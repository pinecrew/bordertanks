#include "font.hpp"

std::vector<std::string> tokenize( std::string data, std::string delimeters ) {
    size_t start = 0, counter = 0, length;
    size_t i, del_length = delimeters.length() + 1;
    size_t d_length = data.length();
    std::vector<std::string> a;

    do {
        for ( i = 0; i < del_length; i++ ) {
            if ( data[counter] == delimeters[i] ) {
                length = counter - start;
                if ( length > 0 ) {
                    a.push_back( std::string( data, start, length ) );
                    start = counter + 1;
                    break;
                }
                start = counter + 1;
            }
        }
    } while ( counter++ <= d_length ); 
    return a;
}

int font_table::load(SDL_Renderer *r, const char *font) {
    SDL_Texture *tex = NULL;
    std::string tex_name, data, buf;
    std::vector<std::string> tok;
    char buffer[128];
    wint_t current = 0;
    size_t load = 1, i = 0;
    FILE *f;
    int id = 0;

    f = fopen( font, "rb" );
    if ( f == NULL ) {
        return -2;
    }

    do {
        load = fread( &buffer[i], 1, 1, f );
    } while ( buffer[i++] != '\0' );
    data = buffer;
    tok = tokenize( data, " " );
    buf = tok[0];
    this->t_width = atoi( tok[1].c_str() );
    this->t_height = atoi( tok[2].c_str() );
    tok.clear();
    tok = tokenize( font, "/" );
    for ( size_t i = 0; i < tok.size() - 1; i++ ) {
        tex_name += tok[i] + "/";
    }
    tex_name += buf;

    tex = IMG_LoadTexture( r, tex_name.c_str() );
    this->font = tex;
    if ( tex == NULL ) {
        fclose( f );
        return -1;
    }
    SDL_QueryTexture( tex, NULL, NULL, &( this->f_width ), &( this->f_height ) );
    do {
        load = fread( &current, 2, 1, f );
        if ( current != L'\n' && current < 0xFFFF && load != 0 ) {
            this->table[current] = id++;
        }
    } while ( load != 0 );
    fclose( f );
    return 0;
}

void font_table::draw(SDL_Renderer *r, const wchar_t *text, int x, int y) {
    SDL_Rect wnd = { 0, 0, this->t_width, this->t_height };
    SDL_Rect pos = { 0, 0, this->t_width, this->t_height };
    int dy = 0, i = 0, id = 0, old_x = x;
    wint_t current;

    pos.x = x; pos.y = y;
    while ( ( current = text[i++] ) != '\0' ) {
        switch ( current ) {
            case '\n':
                pos.y += this->t_height;
                pos.x = old_x;
                continue;
            case '\t':
                pos.x += 2 * this->t_width;
                continue;
            // to upper
            // case 'a'...'z':
            case L'а'...L'я':
                current -= 0x20;
                break;
        }
        id = this->table[current];
        while ( id * this->t_width >= this->f_width ) {
            id -= this->f_width / this->t_width;
            dy++;
        }
        wnd.x = id * this->t_width; wnd.y = dy * this->t_height;
        pos.x += this->t_width;
        SDL_RenderCopy( r, this->font, &wnd, &pos );
        dy = 0;
    }
}

void font_table::destroy() {
    SDL_DestroyTexture( this->font );
}

void font_table::set_color(Uint32 color) {
    SDL_SetTextureColorMod( this->font, color >> 16, ( color >> 8 ) & 0xFF, color & 0xFF );
}
