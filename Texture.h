/*
 * Texture.h
 *
 *  Created on: 2016-10-28
 *      Author: dannyphantom
 */

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "GL/glew.h"
#include "SOIL.h"
#include <string>

class Texture
{
public:
	Texture(GLenum TextureTarget, const std::string FileName);

	void bind(GLenum TextureUnit);
	void unBind(GLenum TextureUnit);

private:
	std::string fileName;
	GLenum textureTarget;
	GLuint textureObj;

	bool load();
};

#endif /* TEXTURE_H_ */
