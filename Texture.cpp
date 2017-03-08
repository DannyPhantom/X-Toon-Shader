/*
 * Texture.cpp
 *
 *  Created on: 2016-10-28
 *      Author: dannyphantom
 */

#include "Texture.h"

Texture::Texture(GLenum TextureTarget, const std::string FileName)
{
	textureTarget = TextureTarget;
	fileName = FileName;
	load();
}

bool Texture::load()
{
	textureObj = SOIL_load_OGL_texture
		(
			fileName.c_str(),
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID,
			SOIL_FLAG_POWER_OF_TWO | SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_COMPRESS_TO_DXT
		);

	if (textureObj != 0) {
		//set texture parameters
		glBindTexture(textureTarget, textureObj);

		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glBindTexture(textureTarget, 0);

		return true;

	} else {
		printf( "SOIL loading error: '%s' %s\n ", SOIL_last_result(), fileName );
		return false;
	}
}

void Texture::bind(GLenum TextureUnit)
{
	glActiveTexture(TextureUnit);
	glBindTexture(textureTarget, textureObj);
}

void Texture::unBind(GLenum TextureUnit) {
	glActiveTexture(TextureUnit);
	glBindTexture(textureTarget, 0);
}

