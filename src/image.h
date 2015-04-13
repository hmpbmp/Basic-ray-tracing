#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED
#include "utils.h"

class Image {
public:
  Image ( int w, int h );
  void setPixelColor ( int x, int y, Color color );
  void saveImage ( char *filename );
private:
  int width,height;
  unsigned char* img;
  int filesize;

};

#endif