/*
 * SceneShader.cpp
 *
 *  Created on: Nov 17, 2015
 *      Author: acarocha
 */

#include "SceneShader.h"
#include <dirent.h>
#include <stdio.h>

static float PI = 3.14159265359;

SceneShader::SceneShader() :
		Shader() {
	_programPlane = 0;
	_programMesh = 0;
	_program2D = 0;
	_planeVertexArray = -1;
	_planeVertexArray = -1;
	_mvUniform = -1;
	_projUniform = -1;
	_zTranslation = 1.0;
	_aspectRatio = 1.0;
	_xRot = 0.0;
	_yRot = 0.0;
	lightPosition = glm::vec3(0.5, 0.5, 0.5);

}

void SceneShader::loadTextures(std::string pathToTextures) {
	DIR *dp;
	struct dirent *dirp;

	//open the directory
	if ((dp = opendir(pathToTextures.c_str())) == NULL) {
		printf("Could not open directory with textures: %s", pathToTextures.c_str());
		exit(1);
	}

	//scan it and load textures one by one
	//(assume that every file in the directory is a texture)
	while ((dirp = readdir(dp)) != NULL) {
		if (dirp->d_name[0] != '.') {
			Texture *tex = new Texture(GL_TEXTURE_2D, pathToTextures + std::string(dirp->d_name));
			allTextures.push_back(tex);
		}
	}

	//check that there's at least one texture
	if (allTextures.size() == 0) {
		printf("No textures found in the specified directory: %s", pathToTextures.c_str());
		exit(1);
	}

	closedir(dp);
}

void SceneShader::readMesh(std::string filename) {
	_mesh = trimesh::TriMesh::read(filename.c_str());

	_mesh->need_bbox();
	_mesh->need_faces();
	_mesh->need_normals();
	_mesh->need_bsphere();

	for (unsigned int i = 0; i < _mesh->faces.size(); i++) {
		_triangleIndices.push_back(_mesh->faces[i][0]);
		_triangleIndices.push_back(_mesh->faces[i][1]);
		_triangleIndices.push_back(_mesh->faces[i][2]);
	}
}

void SceneShader::createVertexBuffer() {
	//create plane geometry
	static const GLfloat quadData[] = { -1.0f, 0.0f, -1.0f, -1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, };

	//passing model attributes to the GPU
	//plane
	glGenVertexArrays(1, &_planeVertexArray);
	glBindVertexArray(_planeVertexArray);

	glGenBuffers(1, &_planeVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _planeVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadData), quadData, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);

	//read and create mesh geometry
	readMesh("./models/bunny.ply"); //normalized vertices coordinates

	//triangle mesh
	glGenVertexArrays(1, &_meshVertexArray);
	glBindVertexArray(_meshVertexArray);

	glGenBuffers(1, &_meshVertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshVertexBuffer);
	glBufferData(GL_ARRAY_BUFFER,
			_mesh->vertices.size() * sizeof(trimesh::point),
			_mesh->vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glGenBuffers(1, &_meshNormalBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, _meshNormalBuffer);
	glBufferData(GL_ARRAY_BUFFER, _mesh->normals.size() * sizeof(trimesh::vec),
			_mesh->normals.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &_meshIndicesBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _meshIndicesBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			_triangleIndices.size() * sizeof(unsigned int),
			_triangleIndices.data(), GL_STATIC_DRAW);

	glBindVertexArray(0);

}

//Creates trackbars for every changable value
void SceneShader::createTrackbars() {
	//create the trackbar
	rTrackbar = new TrackBar(0.0, 15.0, 0.4, 0.03, _program2D,
			"./textures/R.png");
	//adjust its position
	rTrackbar->setCenter(glm::vec2(-0.525, 0.95));
	//set the color
	rTrackbar->setColor(glm::vec3(0.0, 0.0, 0.8));
	//and the default value
	rTrackbar->setCurrentValue(2);
}

//Creates TexturedObject2D's that represent the GUI
//that allows the user to change the texture
void SceneShader::createGUI() {

	//create the right side texture display
	Texture *currentTextureWrapperTex = new Texture(GL_TEXTURE_2D, "./textures/currentTextureWrapper.png");
	currentTextureWrapper = new TexturedObject2D(currentTextureWrapperTex, glm::vec2(0.8, 0.8), 0.19, 0.19);
	currentTexture = new TexturedObject2D(allTextures[currentTextureIndex], glm::vec2(0.8, 0.8), 0.15, 0.15);

	//create textures of buttons
	Texture *prevButtonClickedTex = new Texture(GL_TEXTURE_2D, "./textures/prev_clicked.png");
	Texture *prevButtonNormalTex = new Texture(GL_TEXTURE_2D, "./textures/prev_normal.png");
	Texture *nextButtonClickedTex = new Texture(GL_TEXTURE_2D, "./textures/next_clicked.png");
	Texture *nextButtonNormalTex = new Texture(GL_TEXTURE_2D, "./textures/next_normal.png");

	//create button states
	TexturedObject2D *prevButtonClicked = new TexturedObject2D(prevButtonClickedTex, glm::vec2(0.72, 0.57), 0.07, 0.03);
	TexturedObject2D *prevButtonNormal = new TexturedObject2D(prevButtonNormalTex, glm::vec2(0.72, 0.57), 0.07, 0.03);
	TexturedObject2D *nextButtonClicked = new TexturedObject2D(nextButtonClickedTex, glm::vec2(0.88, 0.57), 0.07, 0.03);
	TexturedObject2D *nextButtonNormal = new TexturedObject2D(nextButtonNormalTex, glm::vec2(0.88, 0.57), 0.07, 0.03);

	//and create buttons
	prevButton = new Button(prevButtonNormal, prevButtonClicked, this);
	nextButton = new Button(nextButtonNormal, nextButtonClicked, this);
}

void SceneShader::startup() {
	_programPlane = compile_shaders("./shaders/plane.vert",
			"./shaders/plane.frag");

	_programMesh = compile_shaders("./shaders/mesh.vert",
			"./shaders/mesh.frag");

	_programLight = compile_shaders("./shaders/light.vert",
			"./shaders/light.frag");

	_program2D = compile_shaders("./shaders/2dshader.vert",
			"./shaders/2dshader.frag");

	createVertexBuffer();
	createTrackbars();
	loadTextures("./assignment_textures/");
	createGUI();
}

void SceneShader::renderPlane() {
	glBindVertexArray(_planeVertexArray);

	glUseProgram(_programPlane);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt(eye, center, up);

	glm::mat4 identity(1.0f);
	_projection = glm::perspective(45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot * PI / 180.0f,
			glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot * PI / 180.0f,
			glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translationZ = glm::translate(identity,
			glm::vec3(0.0f, 0.0f, _zTranslation));
	_modelview *= translationZ * rotationY * rotationX;

	//Uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "modelview"), 1,
			GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programPlane, "projection"), 1,
			GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programPlane, "lightPosition"), 1,
			glm::value_ptr(lightPosition));

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
}

void SceneShader::renderMesh() {
	glBindVertexArray(_meshVertexArray);

	glUseProgram(_programMesh);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt(eye, center, up);

	_projection = glm::perspective(45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot * PI / 180.0f,
			glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot * PI / 180.0f,
			glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translationZ = glm::translate(identity,
			glm::vec3(0.0f, 0.0f, _zTranslation));
	_modelview *= translationZ * rotationY * rotationX;

	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "modelview"), 1,
			GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programMesh, "projection"), 1,
			GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programMesh, "lightPosition"), 1,
			glm::value_ptr(lightPosition));

	//x-toon shading info
	glUniform1f(glGetUniformLocation(_programMesh, "r"),
			rTrackbar->getCurrentValue());
	allTextures[currentTextureIndex] -> bind(GL_TEXTURE0);

	glDrawElements( GL_TRIANGLES, _mesh->faces.size() * 3, GL_UNSIGNED_INT, 0);

	allTextures[currentTextureIndex] -> unBind(GL_TEXTURE0);
	glBindVertexArray(0);
}

void SceneShader::renderLight() {
	glUseProgram(_programLight);

	//scene matrices and camera setup
	glm::vec3 eye(0.0f, 0.3f, 2.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	glm::vec3 center(0.0f, 0.0f, 0.0f);

	_modelview = glm::lookAt(eye, center, up);

	_projection = glm::perspective(45.0f, _aspectRatio, 0.01f, 100.0f);

	glm::mat4 identity(1.0f);

	glm::mat4 rotationX = glm::rotate(identity, _yRot * PI / 180.0f,
			glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 rotationY = glm::rotate(identity, _xRot * PI / 180.0f,
			glm::vec3(1.0f, 0.0f, 0.0f));
	glm::mat4 translationZ = glm::translate(identity,
			glm::vec3(0.0f, 0.0f, _zTranslation));
	_modelview *= translationZ * rotationY * rotationX;

	//uniform variables
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "modelview"), 1,
			GL_FALSE, glm::value_ptr(_modelview));
	glUniformMatrix4fv(glGetUniformLocation(_programLight, "projection"), 1,
			GL_FALSE, glm::value_ptr(_projection));

	glUniform3fv(glGetUniformLocation(_programLight, "lightPosition"), 1,
			glm::value_ptr(lightPosition));

	glPointSize(30.0f);
	glDrawArrays( GL_POINTS, 0, 1);

}

void SceneShader::render() {
	renderPlane();
	renderMesh();
	renderLight();

	rTrackbar->render();
	currentTextureWrapper->draw(_program2D);
	currentTexture->draw(_program2D);
	prevButton->render(_program2D);
	nextButton->render(_program2D);
}

void SceneShader::setZTranslation(float z) {
	_zTranslation = z;
}

void SceneShader::setAspectRatio(float ratio) {
	_aspectRatio = ratio;
}

void SceneShader::setRotationX(float x) {
	_xRot = x;
}

void SceneShader::setRotationY(float y) {
	_yRot = y;
}

void SceneShader::shutdown() {
	glDeleteBuffers(1, &_meshVertexBuffer);
	glDeleteBuffers(1, &_meshNormalBuffer);
	glDeleteBuffers(1, &_meshIndicesBuffer);
	glDeleteVertexArrays(1, &_meshVertexArray);
	glDeleteVertexArrays(1, &_planeVertexArray);
}

void SceneShader::updateLightPositionX(float x) {
	lightPosition.x += x;
}

void SceneShader::updateLightPositionY(float y) {
	lightPosition.y += y;
}

void SceneShader::updateLightPositionZ(float z) {
	lightPosition.z += z;
}

SceneShader::~SceneShader() {
	shutdown();

	delete rTrackbar;
}

bool SceneShader::mouseStuck() {
	return mouseStuckToButton() || mouseStuckToTrackBar();
}

bool SceneShader::mouseStuckToButton() {
	return getButtonMouseStuckTo() != NULL;
}

bool SceneShader::mouseStuckToTrackBar() {
	return getBarMouseStuckTo() != NULL;
}

void SceneShader::mousePressed(float x, float y) {
	//when the mouse is pressed we have to tell each trackbar about the event.
	//We stop as soon as one of the trackbars says that they've captured the event.
	rTrackbar->mousePressed(x, y) ||
			prevButton->mousePressed(x, y) ||
			nextButton->mousePressed(x, y);

}

void SceneShader::mouseReleased() {
	//tell every trackbar about the event
	rTrackbar->mouseReleased();
	prevButton->mouseReleased();
	nextButton->mouseReleased();
}

//get the trackbar we are adjust values for
TrackBar* SceneShader::getBarMouseStuckTo() {
	if (rTrackbar->getMouseStuck())
		return rTrackbar;

	return NULL;
}

//get the trackbar we are adjust values for
Button* SceneShader::getButtonMouseStuckTo() {
	if (prevButton->getMouseStuck()) return prevButton;
	if (nextButton->getMouseStuck()) return nextButton;

	return NULL;
}

void SceneShader::onButtonRelease(Button *button) {
	if (button == nextButton) {
		currentTextureIndex++;
		//clamp
		if (currentTextureIndex > allTextures.size() - 1) {
			currentTextureIndex = 0;
		}
	} else if (button == prevButton){
		currentTextureIndex--;
		//clamp
		if (currentTextureIndex < 0) {
			currentTextureIndex = allTextures.size() - 1;
		}
	}

	//set to the GUI
	currentTexture->setTexture(allTextures[currentTextureIndex]);
}
