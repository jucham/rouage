#ifndef VERTEXBUFFEROBJECT_H
#define VERTEXBUFFEROBJECT_H

#include <GL/glew.h>
#include <cstring>

//TODO could be generic
class VertexBufferObject
{
   public:
      VertexBufferObject(  int dataSize,
                           GLenum  accessMode,
                           void (*glPointer)(GLint, GLenum, GLsizei, const GLvoid*),
                           GLint numCoord );
      VertexBufferObject(const VertexBufferObject &vbo);
      ~VertexBufferObject();
      void Destroy();

      void Init();

      void setData(const float* pData);
      void update();

      static void UnbindBuffer();

      static void vertexPointer(GLint, GLenum, GLsizei, const GLvoid*);
      static void texCoordPointer(GLint, GLenum, GLsizei, const GLvoid*);
      static void colorPointer(GLint, GLenum, GLsizei, const GLvoid*);

      GLenum GetAccessMode() const {return m_accessMode;}

   protected:
      GLuint   m_buffer;
      float    *m_pData;
      int      m_iDataSize;
      bool     m_bHasChanged;
      GLenum   m_accessMode;

      void (*m_glPointer)(GLint, GLenum, GLsizei, const GLvoid*);
      GLint m_iNumCoord;


};


inline void VertexBufferObject::vertexPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
   glVertexPointer(size, type, stride, pointer);
}

inline void VertexBufferObject::texCoordPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
   glTexCoordPointer(size, type, stride, pointer);
}

inline void VertexBufferObject::colorPointer(GLint size, GLenum type, GLsizei stride, const GLvoid* pointer)
{
   glColorPointer(size, type, stride, pointer);
}

#endif // VERTEXBUFFEROBJECT_H
