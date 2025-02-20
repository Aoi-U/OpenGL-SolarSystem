#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

Transformation::Transformation(float x, float y, float rotation, float scaleX, float scaleY) 
{
    transformationMatrix = glm::mat4(1.0f);
    move(x, y);
    rotate(rotation);
    scale(scaleX, scaleY);
}

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

float Transformation::getAngle() 
{
    return this->angle;
}

glm::vec2 Transformation::getScale() 
{
    return glm::vec2(scaleX, scaleY);
}

glm::vec2 Transformation::getPosition() 
{
    return glm::vec2(transformationMatrix[3][0], transformationMatrix[3][1]);
}
