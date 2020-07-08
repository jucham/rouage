#include "VertexBufferObject.h"
#include <cassert>
#include <cstring>
#include <iostream>
#include <cstdio>

VertexBufferObject::VertexBufferObject(  int dataSize,
                           GLenum  accessMode,
                           void (*glPointer)(GLint, GLenum, GLsizei, const GLvoid*),
                           GLint numCoord ) :
   m_buffer(0),
   m_pData(0),
   m_iDataSize(dataSize),
   m_bHasChanged(false),
   m_accessMode(accessMode),
   m_glPointer(glPointer),
   m_iNumCoord(numCoord)
{
   Init();
}

VertexBufferObject::VertexBufferObject(const VertexBufferObject &vbo) :
		   m_buffer(0),
		   m_pData(0),
		   m_iDataSize(vbo.m_iDataSize),
		   m_bHasChanged(vbo.m_bHasChanged),
		   m_accessMode(vbo.m_accessMode),
		   m_glPointer(vbo.m_glPointer),
		   m_iNumCoord(vbo.m_iNumCoord)
{
	Init();
}

VertexBufferObject::~VertexBufferObject()
{
	//FIXME need to control the destroying explicitely because it could causes strange state of VBO in a VBOSprite
	glDeleteBuffersARB(1, &m_buffer);
	delete[] m_pData;
}


/*
void VertexBufferObject::Destroy() {
	//TODO delete data on GPU with unmapbuffer
	glDeleteBuffersARB(1, &m_buffer);
	delete[] m_pData;
}
*/

void VertexBufferObject::Init() {

	assert(m_iDataSize%2 == 0);

	int arraySize = m_iDataSize/sizeof(float);

	m_pData = new float[arraySize];
	for (int i=0; i < arraySize; ++i) {m_pData[i] = 0.0f;}  // initialize data
	
	glGenBuffersARB(1, &m_buffer); // create the gpu buffer
	glBindBufferARB(GL_ARRAY_BUFFER, m_buffer);
	glBufferDataARB(GL_ARRAY_BUFFER, m_iDataSize, 0, m_accessMode); // allocates memory
	glBufferSubDataARB(GL_ARRAY_BUFFER, 0, m_iDataSize, m_pData);    // set data
	UnbindBuffer();
}

void VertexBufferObject::setData(const float *pData)
{
   memcpy(m_pData, pData, m_iDataSize);
   m_bHasChanged = true;
}

void VertexBufferObject::update()
{
   glBindBufferARB(GL_ARRAY_BUFFER, m_buffer);

   // if data changed then update VBO
   if (m_bHasChanged)
   {
      float *pVBO = 0;
      pVBO = (float*) glMapBufferARB(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
      assert (pVBO != 0);
      memcpy(pVBO, m_pData, m_iDataSize);
      glUnmapBufferARB(GL_ARRAY_BUFFER);
      pVBO = 0;
      m_bHasChanged = false;
   }

   m_glPointer(m_iNumCoord, GL_FLOAT, 0, 0);
   UnbindBuffer();
}

void VertexBufferObject::UnbindBuffer()
{
   glBindBufferARB(GL_ARRAY_BUFFER, 0);
}


