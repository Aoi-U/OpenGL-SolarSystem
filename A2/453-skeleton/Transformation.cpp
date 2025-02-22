#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

void Transformation::move(float x, float y) 
{
    this->posX += x;
    this->posY += y;
}

void Transformation::rotate(float rotation) 
{
    this->angle += rotation;
    // normalize the angle to be between -180 and 180
    while (this->angle > 180) {
        this->angle -= 360;
    }
    while (this->angle < -180) {
        this->angle += 360;
    }
}

void Transformation::scale(float scaleX, float scaleY) 
{
    this->scaleX = scaleX;
    this->scaleY = scaleY;
}

glm::mat4 Transformation::getTransformationMatrix() 
{
    glm::mat4 t = glm::mat4(1.0f);
    t = glm::translate(t, glm::vec3(posX, posY, 0.0f));
    t = glm::rotate(t, glm::radians(angle - 90), glm::vec3(0.0f, 0.0f, 1.0f));
    t = glm::scale(t, glm::vec3(scaleX, scaleY, 1.0f));
    return t;
}

glm::vec2 Transformation::getScale() 
{
    return glm::vec2(scaleX, scaleY);
}

glm::vec2 Transformation::getPosition() 
{
    return glm::vec2(posX, posY);
}

float Transformation::getAngle() 
{
    return angle;
}