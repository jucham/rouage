#include "PrimitiveDrawing.h"

#include <GL/glew.h>
#include <cassert>


void PrimitiveDrawing::drawQuad(GLfloat w,
                                  GLfloat h,
                                  GLubyte r,
                                  GLubyte g,
                                  GLubyte b,
                                  GLubyte a)
{
   assert (w > 0 && h > 0);

   glBegin(GL_QUADS);

            glColor4ub(r,g,b,a);
            glVertex2d(0, 0);
            glVertex2d(w, 0);
            glVertex2d(w, h);
            glVertex2d(0, h);
   glEnd();

}

void PrimitiveDrawing::drawVoidQuad(GLfloat w,
                                    GLfloat h,
                                    GLfloat lw,
                                    GLubyte r,
                                    GLubyte g,
                                    GLubyte b,
                                    GLubyte a)
{


   glLineWidth(lw);

   glBegin(GL_LINE_LOOP);
      glColor4ub(r,g,b,a);
      glVertex2d(0, 0);
      glVertex2d(w, 0);
      glVertex2d(w, h);
      glVertex2d(0, h);
   glEnd();
}
