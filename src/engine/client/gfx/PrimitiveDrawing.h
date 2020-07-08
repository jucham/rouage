#ifndef PRIMITIVEDRAWING_H
#define PRIMITIVEDRAWING_H

#include <GL/glew.h>



///////////////////////////////////////////////////////////////////////////////
///
/// \class PrimitiveDrawing PrimitiveDrawing.h
/// \brief This class implements primitive drawings using opengl
/// \author Julien Champalbert
/// \version 0.1
///
///////////////////////////////////////////////////////////////////////////////
class PrimitiveDrawing
{
   public:


      /// \fn static void drawQuad(  GLfloat w,
      ///                              GLfloat h,
      ///                              GLubyte r,
      ///                              GLubyte g,
      ///                              GLubyte b,
      ///                              GLubyte a)
      /// \brief Draw a quad
      ///
      /// \param w width of quad
      /// \param h height of quad
      /// \param r red level of color
      /// \param g green level of color
      /// \param b blue level of color
      /// \param a alpha level
      ///
      /// Draw a quad starting with bottom left point and continuing to place
      /// others points in anti-clockwise direction. The first placed point
      /// (0,0) is coordinate of current opengl modelview matrix
      /// (ignoring the z component).
      /// Each color component (r,g,b) can have value 0 to 255.
      /// The component a is alpha level (0-255). Default value is 255 that is
      /// completely opaque.
      ///
      static void drawQuad( GLfloat w,
                              GLfloat h,
                              GLubyte r,
                              GLubyte g,
                              GLubyte b,
                              GLubyte a = 255);


      static void drawVoidQuad(GLfloat w,
                                    GLfloat h,
                                    GLfloat lw,
                                    GLubyte r,
                                    GLubyte g,
                                    GLubyte b,
                                    GLubyte a = 255);

};

#endif // PRIMITIVEDRAWING_H
