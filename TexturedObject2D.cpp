/*
 * TexturedObject2D.cpp
 *
 *  Created on: 2016-10-28
 *      Author: dannyphantom
 */

#include "TexturedObject2D.h"

TexturedObject2D::TexturedObject2D(Texture *tex, glm::vec2 pos, float width, float height) :
		texture(tex), pos(pos), height(height), width(width) {
	generateVAO();
}

TexturedObject2D::~TexturedObject2D() {

}

void TexturedObject2D::generateVAO() {
	//create a rectangle with 4 vertices
	float vertices[] = { -1, 1,		//BL
			-1, -1, 	//TL
			1, 1,		//BR
			1, -1,		//TR
			};

	// and create the uv's for the vertices
	float uvs[] = { 0, 0,		//BL
			0, 1, 	//TL
			1, 0,		//BR
			1, 1,		//TR
			};

	GLuint VBO_vertices, VBO_uvs;

	//passing model attributes to the GPU
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//vertices
	glGenBuffers(1, &VBO_vertices);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	//uv's
	glGenBuffers(1, &VBO_uvs);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_uvs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TexturedObject2D::draw(GLuint program) {
	glUseProgram(program);

	glBindVertexArray(VAO);

	//indicate that we use a texture
	glUniform1i(glGetUniformLocation(program, "textureValid"), true);

	glm::mat4 identity(1.0f);
	glm::mat4 translation;
	glm::mat4 scale;
	glm::mat4 modelMatrix;

	//perform some setup
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//pass in the model matrix
	translation = glm::translate(identity,
			glm::vec3(pos.x, pos.y, 0.0f));
	scale = glm::scale(identity, glm::vec3(width, height, 1.0));
	modelMatrix = translation * scale;
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1,
			GL_FALSE, glm::value_ptr(modelMatrix));

	//attach the texture
	texture->bind(GL_TEXTURE_2D);

	//draw the box
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	texture->unBind(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);

	glBindVertexArray(0);

	glUseProgram(0);
}

void TexturedObject2D::setTexture(Texture *tex) {
	texture = tex;
}

