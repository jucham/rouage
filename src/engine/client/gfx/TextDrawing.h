#ifndef TEXTDRAWING_H
#define TEXTDRAWING_H
#include <GL/glew.h>

// For debug build target compile correctly
#include "windows.h"
#define GLUT_DISABLE_ATEXIT_HACK
//
#include "../gfx/glut.h"

class TextDrawing
{
   public:

      static inline void printString(const GLfloat x,
                                                   const GLfloat y,
                                                   const char* str)
      {
         glColor3ub(255,255,255);
         glRasterPos2d(x,y);

         char* str2 = "AZERTY";
         while(*str2)
         {
            glutBitmapCharacter( GLUT_BITMAP_HELVETICA_12, *str2 );
            str2++;
         }
      }

};

#endif // TEXTDRAWING_H
