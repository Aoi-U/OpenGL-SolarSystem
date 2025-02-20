#include <glm/glm.hpp>

class Transformation {
public:
    Transformation() {};

    // set the x and y values
    void move(float x, float y);

    // set the rotation value
    void rotate(float rotation);

    // set the scale values
    void scale(float scaleX, float scaleY);

    // get the transformation matrix
    glm::mat4 getTransformationMatrix();

    // get the angle
    float getAngle();
    
    // get the scale
    glm::vec2 getScale();

    // get the position
    glm::vec2 getPosition();
    
    private:
    glm::mat4 transformationMatrix = glm::mat4(1.0f);
    float scaleX = 0.087f;
    float scaleY = 0.15f;
    float angle = 90;
};