#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

struct Color {
  Color();
  Color ( unsigned char r, unsigned char g, unsigned char b ) : red ( r ), green ( g ),blue ( b ) {};
  unsigned char red;
  unsigned char green;
  unsigned char blue;
};


#endif