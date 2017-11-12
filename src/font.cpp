#include "font.hpp"

const char NULL_STR = '\0';

FontTable::~FontTable( void ) {
    // удаление загруженной текстуры
    SDL_DestroyTexture( ft->font );
    // освобождение параметров текстуры
    delete[] ft->tex_name;
    delete[] ft->table;
    delete ft;
}

// функция загрузки шрифтовой текстуры
int FontTable::load( SDL_Renderer * r, const char * font ) {
    unsigned int text_size = 0, abc_size = 0;
    SDL_Texture *tex = nullptr;
    wint_t current = 0;
    size_t load = 1;
    int id = 0;
    FILE * f;

    // записываем рендер
    render = r;
    // создаём таблицу с символами
    ft = new font_table_t;
    // открываем файл с конфигурацией
    f = fopen( font, "rb" );
    if ( f == nullptr ) {
        return ( last_error = A_ERROR_OPEN_FILE );
    }
    // читаем размер имени файла с текстурой
    fread( &( text_size ), sizeof(int), 1, f );
    // читаем размер алфавита
    fread( &( abc_size ), sizeof(int), 1, f );
    // выделяем данные под название текстуры
    ft->tex_name = new char [text_size];
    // под таблицу для хранения алфавита 
    ft->table = new int [abc_size];
    // читаем имя файла текстуры
    fread( ft->tex_name, text_size, 1, f );
    // ширина текстурной буквы
    fread( &( ft->t_width ), sizeof(int), 1, f );
    // высота текстурной буквы
    fread( &( ft->t_height ), sizeof(int), 1, f );
    // загружаем текстуру
    tex = IMG_LoadTexture( render, ft->tex_name );
    ft->font = tex;
    // обработка ошибок
    if ( tex == nullptr ) {
        fclose( f );
        return ( last_error = A_ERROR_LOAD_TEXTURE );
    }
    // получаем размер текстуры
    SDL_QueryTexture( tex, nullptr, nullptr, &( ft->f_width ), &( ft->f_height ) );
    // устанавливаем точку начал чтения алфавита
    fseek( f, sizeof(int) * 4 + text_size + 1, SEEK_SET );
    do {
        // читаем букву
        load = fread( &current, 2, 1, f );
        // добавляем в таблицу
        if ( current != L'\n' && current < 0xFFFF && load != 0 ) {
            ft->table[current] = id++;
        }
    } while ( load != 0 );
    // закрываем файл конфигурации
    fclose( f );
    return ( last_error = A_SUCCESS );
}

// функция отрисовки текста
void FontTable::draw( int x, int y, const wchar_t * text ) {
    // размер текстурной единицы (блок с буквой)
    SDL_Rect wnd = { 0, 0, ft->t_width, ft->t_height };
    // координаты положения
    SDL_Rect pos = { 0, 0, ft->t_width, ft->t_height };
    int dy = 0, i = 0, id = 0, old_x = x - ft->t_width;
    wint_t current;

    pos.x = old_x; pos.y = y;
    // цикл по всей строке с текстом
    while ( ( current = text[i++] ) != NULL_STR ) {
        switch ( current ) {
            // обработка переноса строки
            case '\n':
                pos.y += ft->t_height;
                pos.x = old_x;
                continue;
            // обработка символа пробел
            case ' ':
                pos.x += ft->t_width;
                continue;
            // обработка символа табуляции
            case '\t':
                pos.x += 4 * ft->t_width;
                continue;
            // перевод из строчных в заглавные буквы
            case 'a'...'z':
            case L'а'...L'я':
                current -= 0x20;
                break;
        }
        // находим номер буквы
        id = ft->table[current];
        // находим позицию в текстуре
        while ( id * ft->t_width >= ft->f_width ) {
            id -= ft->f_width / ft->t_width;
            dy++;
        }
        // сдвигаем область отрисовки
        wnd.x = id * ft->t_width; wnd.y = dy * ft->t_height;
        pos.x += ft->t_width;
        // рисуем
        SDL_RenderCopy( render, ft->font, &wnd, &pos );
        dy = 0;
    }
}

// функция перезагрузки
void FontTable::reload( SDL_Renderer * r ) {
    SDL_DestroyTexture( ft->font );
    ft->font = IMG_LoadTexture( r, ft->tex_name );
    render = r;
}

// функция установки цвета
void FontTable::set_coloru( Uint32 color ) {
    SDL_SetTextureColorMod( ft->font, color >> 16, ( color >> 8 ) & 0xFF, color & 0xFF );
}