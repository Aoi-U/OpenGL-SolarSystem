#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void Transformation::move(float x, float y) 
{
    transformationMatrix = glm::translate(transformationMatrix, glm::vec3(x, y, 0.0f));
}

void Transformation::rotate(float rotation) 
{
    this->angle += rotation;
    transformationMatrix = glm::rotate(transformationMatrix, rotation, glm::vec3(0.0f, 0.0f, 1.0f));
}

void Transformation::scale(float scaleX, float scaleY) 
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;
}

glm::mat4 Transformation::getTransformationMatrix() 
{
    return this->transformationMatrix;
}

glm::vec2 Transformation::getScale() 
{
    return glm::vec2(scaleX, scaleY);
}

glm::vec2 Transformation::getPosition() 
{
    return glm::vec2(transformationMatrix[3][0], transformationMatrix[3][1]);
}

float Transformation::getAngle() 
{
    return this->angle;
}