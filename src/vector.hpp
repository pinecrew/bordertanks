#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <cmath>
#include <float.h>

struct vec2 {
    float x;
    float y;

    vec2() : x( 0.0f ), y( 0.0f ) {}
    vec2( const float x0, const float y0 ) : x( x0 ), y( y0 ) {}
    void operator()( const float x0, const float y0 );
    bool operator==( const vec2 & v ) const;
    bool operator!=( const vec2 & v ) const;
    vec2 & operator=( const vec2 & v );
    vec2 operator-( void );
    vec2 operator+( const vec2 & v );
    vec2 operator-( const vec2 & v );
    vec2 operator*( const float k );
    vec2 operator/( const float k );
    vec2 operator-=( const vec2 & v );
    vec2 operator+=( const vec2 & v );
    vec2 operator*=( const float k );
    vec2 operator/=( const float k );
    vec2 operator*( const vec2 & v );

    vec2 abs();
    vec2 norm();
    vec2 rot( const float angle );
    float length();
    float dot( const vec2 & v ) const;
};

#endif