#include "Panel.h"

Panel::Panel( float wCenter,
             float hCenter,
             float wCorner,
             float hCorner) :
   m_fWidth(wCenter + 2 * wCorner),
   m_fHeight(hCenter)
{

   float wSide = wCorner;
   float hSide = hCenter - 2*hCorner;


   // corner BL
   GLuint cornerBLList = glGenLists(1);
   glNewList(cornerBLList, GL_COMPILE);
   Quad(wCorner, hCorner, 0.0f, 0.5f, 0.5f, 1.0f);

   glTranslatef(0, hCorner, 0);  // translate forward
   glEndList();

   // left
   GLuint leftSideList = glGenLists(1);
   glNewList(leftSideList, GL_COMPILE);
   Quad(wSide, hSide, 0.5f, 0.5f, 0.75f, 0.75f);
   glTranslatef(0,hSide,0);  // translate forward
   glEndList();

   // corner TL
   GLuint cornerTLList = glGenLists(1);
   glNewList(cornerTLList, GL_COMPILE);
   Quad(wCorner, hCorner, 0.0f, 0.0f, 0.5f, 0.5f);
   glTranslatef(wCorner, hCorner-hCenter,0);  // translate forward
   glEndList();

   // center
   GLuint centerList = glGenLists(1);
   glNewList(centerList, GL_COMPILE);
   Quad(wCenter, hCenter, 0.5f, 0.5f, 0.75f, 0.75f);
   glTranslatef(wCenter,0,0);  // translate forward
   glEndList();

   // corner BR
   GLuint cornerBRList = glGenLists(1);
   glNewList(cornerBRList, GL_COMPILE);
   Quad(wCorner, hCorner, 0.5f, 0.5f, 1.0f, 1.0f);
   glTranslatef(0,hCorner,0);  // translate forward
   glEndList();

   // right
   GLuint rightSideList = glGenLists(1);
   glNewList(rightSideList, GL_COMPILE);
   Quad(wSide, hSide, 0.5f, 0.5f, 0.75f, 0.75f);
   glTranslatef(0,hSide,0);  // translate forward
   glEndList();

   // corner TR
   GLuint cornerTRList = glGenLists(1);
   glNewList(cornerTRList, GL_COMPILE);
   Quad(wCorner, hCorner, 0.5f, 0.0f, 1.0f, 0.5f);
   glEndList();


   m_displayList = glGenLists(1);
   glNewList(m_displayList, GL_COMPILE);
   glCallList(cornerBLList);
   glCallList(leftSideList);
   glCallList(cornerTLList);
   glCallList(centerList);
   glCallList(cornerBRList);
   glCallList(rightSideList);
   glCallList(cornerTRList);
   glEndList();


}

void Panel::Quad(float w, float h, float mx1, float my1, float mx2, float my2)
{
   glBegin(GL_QUADS);
   glTexCoord2f(mx2, my2);  glVertex2f(w, 0);
   glTexCoord2f(mx2, my1);  glVertex2f(w, h);
   glTexCoord2f(mx1, my1);  glVertex2f(0, h);
   glTexCoord2f(mx1, my2);  glVertex2f(0, 0);
   glEnd();
}

void Panel::Render(float x,
                  float y,
                  float r,
                  float g,
                  float b,
                  float a)
{
   glColor4f(r,g,b,a);
   glTranslatef( x, y, 0);
   glEnable(GL_TEXTURE_2D);
   GFXAsset::Instance().enableTex(TEX_UI_PANEL);
   glCallList(m_displayList);
   glDisable(GL_TEXTURE_2D);
   glLoadIdentity();
}
