#ifndef TEXTCHAR_H
#define TEXTCHAR_H

struct Vertex
{
   float pos[2];
   float tex[2];
   float color[4];
};


class TextChar
{
   public:
      TextChar();
      ~TextChar();

      void setChar(const char c);

   protected:
      Vertex m_quad[4];
};

#endif // TEXTCHAR_H
