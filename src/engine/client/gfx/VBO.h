#ifndef VBO_H
#define VBO_H

#include <cassert>
#include <cstring>
#include <GL/glew.h>

// that class lets manipulation of float VBO (Vertex Buffer Object)

class VBO
{
   public:
      VBO(int dataSize, GLenum accessMode);
      ~VBO();

      void        bindBuffer();
      static void bindZeroBuffer();
      void        update(const float* pData);

   protected:
      GLuint   m_bufObjName;
      int      m_iDataSize;
      GLenum   m_accessMode;
};

inline void VBO::bindBuffer()
{
   glBindBufferARB(GL_ARRAY_BUFFER, m_bufObjName);
}

inline void VBO::bindZeroBuffer()
{
   glBindBufferARB(GL_ARRAY_BUFFER, 0);
}


#endif // VBO_H
