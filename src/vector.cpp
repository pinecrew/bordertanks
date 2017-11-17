#include "vector.hpp"

void vec2::operator()( const float x0, const float y0 ) {
    x = x0;
    y = y0;
}

bool vec2::operator==( const vec2 & v ) const { return x == v.x && y == v.y; }

bool vec2::operator!=( const vec2 & v ) const { return x != v.x || y != v.y; }

vec2 & vec2::operator=( const vec2 & v ) {
    x = v.x;
    y = v.y;
    return *this;
}

vec2 vec2::operator-( void ) { return vec2( -x, -y ); }

vec2 vec2::operator+( const vec2 & v ) { return vec2( x + v.x, y + v.y ); }

vec2 vec2::operator-( const vec2 & v ) { return vec2( x - v.x, y - v.y ); }

vec2 vec2::operator*( const float k ) {
    vec2 tmp( *this );
    return tmp *= k;
}

vec2 vec2::operator/( const float k ) {
    vec2 tmp( *this );
    return tmp /= k;
}

vec2 vec2::operator-=( const vec2 & v ) {
    x -= v.x;
    y -= v.y;
    return *this;
}

vec2 vec2::operator+=( const vec2 & v ) {
    x += v.x;
    y += v.y;
    return *this;
}

vec2 vec2::operator*=( const float k ) {
    x *= k;
    y *= k;
    return *this;
}

vec2 vec2::operator/=( const float k ) {
    x /= k;
    y /= k;
    return *this;
}

vec2 vec2::operator*( const vec2 & v ) { return vec2( x * v.x, y * v.y ); }

vec2 vec2::abs( void ) { return vec2( fabsf( x ), fabsf( y ) ); }

vec2 vec2::norm( void ) {
    float n = sqrtf( x * x + y * y );
    float a = 0.0f;

    if ( n > FLT_EPSILON ) {
        a = 1.0f / n;
    }
    return vec2( x * a, y * a );
}

vec2 vec2::rot( const float angle ) {
    float x1 = x * cos( angle ) - y * sin( angle );
    float y1 = x * sin( angle ) + y * cos( angle );
    return vec2( x1, y1 );
}

float vec2::length( void ) { return sqrtf( x * x + y * y ); }

float vec2::dot( const vec2 & v ) const { return x * v.x + y * v.y; }