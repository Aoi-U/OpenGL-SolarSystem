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
    
    // get the scale
    glm::vec2 getScale();
    
    // get the position
    glm::vec2 getPosition();
    
    // get the angle
    float getAngle();
    
    private:
    float scaleX = 0.088f;
    float scaleY = 0.15f;
    float posX = 0.0f;
    float posY = 0.0f;
    float angle = 90;
};