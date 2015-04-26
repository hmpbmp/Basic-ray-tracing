#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED
#include "utils.h"

class Image {
public:
  Image ( int w, int h );
  void setPixelColor ( int x, int y, Color &color );
  void setFloat ( int x, int y, float f );
  void saveImage ( const char *filename );
  void normalize ( );
private:
  int width,height;
  unsigned char* img;
  float *f_img;
  int filesize;

};

#endif