#include "Transformation.h"
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>

// set the x and y values
void Transformation::move(float x, float y) 
{
    posX += x;
    posY += y;
}

// set the rotation value
void Transformation::rotate(float rotation) 
{
    angle += rotation;
}

// set the scale values
void Transformation::scale(float x, float y) 
{
    scaleX = x;
    scaleY = y;
}

// get the transformation matrix
glm::mat4 Transformation::getTransformationMatrix() 
{
    // construct the transformation matrix 
    glm::mat4 t = glm::mat4(1.0f);
    t = glm::translate(t, glm::vec3(posX, posY, 0.0f));
    t = glm::rotate(t, glm::radians(angle - 90), glm::vec3(0.0f, 0.0f, 1.0f));
    t = glm::scale(t, glm::vec3(scaleX, scaleY, 1.0f));
    return t;
}

// get the scale
glm::vec2 Transformation::getScale() 
{
    return glm::vec2(scaleX, scaleY);
}

// get the position
glm::vec2 Transformation::getPosition() 
{
    return glm::vec2(posX, posY);
}

// get the angle
float Transformation::getAngle() 
{
    return angle;
}