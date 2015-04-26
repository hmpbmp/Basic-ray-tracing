#include "image.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <cfloat>

Image::Image ( int w, int h ) :width ( w ), height ( h ) {
  img = ( unsigned char * ) malloc ( 3  *w * h );
  memset ( img,0,sizeof ( img ) );
  int filesize = 54 + 3 * w * h;
  for ( int i = 0; i < w; i++ ) {
    for ( int j = 0; j < h; j++ ) {
      int x = i;
      int y = ( height-1 ) - j;
      img[ ( x + y * w ) * 3 + 2] = 0;
      img[ ( x + y * w ) * 3 + 1] = 0;
      img[ ( x + y * w ) * 3 + 0] = 0;
    }
  }

  f_img = ( float * ) malloc ( 3  *w * h * sizeof ( float ) );
  memset ( f_img,0,sizeof ( f_img ) );
  for ( int i = 0; i < w; i++ ) {
    for ( int j = 0; j < h; j++ ) {
      int x = i;
      int y = ( height-1 ) - j;
      f_img[ ( x + y * w ) * 3 + 2] = 0.0f;
      f_img[ ( x + y * w ) * 3 + 1] = 0.0f;
      f_img[ ( x + y * w ) * 3 + 0] = 0.0f;
    }
  }
}

void Image::setPixelColor ( int x, int y, Color &color ) {
  img[ ( x + y * width ) * 3 + 2] = color.red;
  img[ ( x + y * width ) * 3 + 1] = color.green;
  img[ ( x + y * width ) * 3 + 0] = color.blue;
}

void Image::setFloat ( int x, int y, float f ) {
  f_img[ ( x + y * width ) * 3 + 2] = f;
  f_img[ ( x + y * width ) * 3 + 1] = f;
  f_img[ ( x + y * width ) * 3 + 0] = f;
}

void Image::saveImage ( const char *filename ) {
  FILE *f;
  f = fopen ( filename,"wb" );



  unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
  unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
  unsigned char bmppad[3] = {0,0,0};

  bmpfileheader[ 2] = ( unsigned char ) ( filesize    );
  bmpfileheader[ 3] = ( unsigned char ) ( filesize>> 8 );
  bmpfileheader[ 4] = ( unsigned char ) ( filesize>>16 );
  bmpfileheader[ 5] = ( unsigned char ) ( filesize>>24 );

  bmpinfoheader[ 4] = ( unsigned char ) ( width    );
  bmpinfoheader[ 5] = ( unsigned char ) ( width >> 8 );
  bmpinfoheader[ 6] = ( unsigned char ) ( width >> 16 );
  bmpinfoheader[ 7] = ( unsigned char ) ( width >> 24 );
  bmpinfoheader[ 8] = ( unsigned char ) ( height    );
  bmpinfoheader[ 9] = ( unsigned char ) ( height >> 8 );
  bmpinfoheader[10] = ( unsigned char ) ( height >> 16 );
  bmpinfoheader[11] = ( unsigned char ) ( height >> 24 );

  fwrite ( bmpfileheader, 1, 14, f );
  fwrite ( bmpinfoheader, 1, 40, f );
  for ( int i = 0; i < height; i++ ) {
    fwrite ( img + ( width * ( height - i - 1 ) * 3 ),3,width,f );
    fwrite ( bmppad,1, ( 4 - ( width * 3 ) %4 ) %4,f );
  }
  fclose ( f );
}

void Image::normalize () {
  float min = FLT_MAX, max = 0.0f;
  for ( int i = 0; i < width; i++ ) {
    for ( int j = 0; j < height; j++ ) {
      int x = i;
      int y = ( height-1 ) - j;
      if ( f_img[ ( x + y * width ) * 3] < min && f_img[ ( x + y * width ) * 3] > FLT_EPSILON ) {
        min = f_img[ ( x + y * width ) * 3];
      }
      if ( f_img[ ( x + y * width ) * 3] > max ) {
        max = f_img[ ( x + y * width ) * 3];
      }
    }
  }


  for ( int i = 0; i < width; i++ ) {
    for ( int j = 0; j < height; j++ ) {
      int x = i;
      int y = ( height-1 ) - j;
      if (  f_img[ ( x + y * width ) * 3 + 2] > 0.0f ) {
        img[ ( x + y * width ) * 3 + 2] = ( unsigned char ) ( ( 1 - ( f_img[ ( x + y * width ) * 3 + 2] - min ) / ( max - min ) ) * 255.0f ) ;
        //img[ ( x + y * width ) * 3 + 2] = ( unsigned char ) ( ( ( f_img[ ( x + y * width ) * 3 + 2] - min ) / ( max - min ) ) * 255.0f ) ;
      }
      if (  f_img[ ( x + y * width ) * 3 + 1] > 0.0f ) {
        img[ ( x + y * width ) * 3 + 1] = ( unsigned char ) ( ( 1 - ( f_img[ ( x + y * width ) * 3 + 1] - min ) / ( max - min ) ) * 255.0f ) ;
        //img[ ( x + y * width ) * 3 + 1] = ( unsigned char ) ( ( ( f_img[ ( x + y * width ) * 3 + 1] - min ) / ( max - min ) ) * 255.0f ) ;
      }
      if (  f_img[ ( x + y * width ) * 3 + 0] > 0.0f ) {
        img[ ( x + y * width ) * 3 + 0] = ( unsigned char ) ( ( 1 - ( f_img[ ( x + y * width ) * 3 + 0] - min ) / ( max - min ) ) * 255.0f ) ;
        //img[ ( x + y * width ) * 3 + 0] = ( unsigned char ) ( ( ( f_img[ ( x + y * width ) * 3 + 0] - min ) / ( max - min ) ) * 255.0f ) ;
      }
    }
  }
}

