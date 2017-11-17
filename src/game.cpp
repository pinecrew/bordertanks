#include "vector.hpp"
#include <map>
#include <string>
#include <vector>

class Bullet {};
class Tank {};
class Tile {};

class BulletFactory {
    BulletFactory( std::string config_file, std::string tiles_file ){};
    Bullet create( std::string type, vec2 position, float speed, vec2 direction );
};

class TankFactory {
    TankFactory( std::string config_file, std::string tiles_file ){};
    Tank create( std::string type, vec2 position, vec2 direction );
};

class TileFactory {
    TileFactory( std::string config_file, std::string tiles_file ){};
    Tile create( std::string type, vec2 position );
};

class Game {
    std::vector<Bullet> bullets;
    std::vector<Tank> tanks;
    std::map<std::pair<int, int>, Tile> tiles;
    int height;
    int width;
    void next();
    Game(){};
};