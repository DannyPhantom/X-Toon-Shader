/*
 * TexturedObject2D.h
 *
 *  Created on: 2016-10-28
 *      Author: dannyphantom
 */

#ifndef TEXTUREDOBJECT2D_H_
#define TEXTUREDOBJECT2D_H_

#include "Texture.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class TexturedObject2D {
public:
	TexturedObject2D(Texture *tex, glm::vec2 pos, float width, float height);
	virtual ~TexturedObject2D();
	void draw(GLuint program);
	void setTexture(Texture *tex);

	glm::vec2 getPos() {return pos;}
	float getHeight() {return height;}
	float getWidth() {return width;}

private:
	Texture *texture;
	glm::vec2 pos;
	float height, width;
	GLuint VAO;

	void generateVAO();
};

#endif /* TEXTUREDOBJECT2D_H_ */
