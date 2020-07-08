#include "VBO.h"

VBO::VBO(int dataSize, GLenum accessMode) :
   m_bufObjName(0),
   m_iDataSize(dataSize),
   m_accessMode(accessMode)
{
   glGenBuffersARB(1, &m_bufObjName);
   glBindBufferARB(GL_ARRAY_BUFFER, m_bufObjName);
   glBufferDataARB(GL_ARRAY_BUFFER, m_iDataSize, 0, m_accessMode);
   bindZeroBuffer();
}

VBO::~VBO()
{
   glDeleteBuffersARB(1, &m_bufObjName);
}


void VBO::update(const float* pData)
{
   float *pVBO = 0;
   pVBO = static_cast<float*> (glMapBufferARB(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
   assert (pVBO != 0);
   memcpy(pVBO, pData, m_iDataSize);
   glUnmapBufferARB(GL_ARRAY_BUFFER);
   pVBO = 0;
}






