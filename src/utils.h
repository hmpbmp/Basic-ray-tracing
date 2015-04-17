#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

struct Color {
  Color() {};
  Color ( unsigned char r, unsigned char g, unsigned char b ) : red ( r ), green ( g ),blue ( b ) {};
  Color ( int d ) {
    red = ( d & 0xFF000000 ) >> 24;
    green = ( d & 0x00FF0000 ) >> 16;
    blue = ( d & 0x0000FF00 ) >> 8;
  }
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};


#endif