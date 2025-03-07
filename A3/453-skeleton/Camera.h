#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Window.h"

class Camera
{
public:
    Camera(Window& window);

    void Move(float theta, float phi); // pans the camera about the origin
    void Zoom(float zoom); // zooms the camera towards/away from the camera
    glm::mat4 getModel(); // returns the model matrix
    glm::mat4 getView(); // returns the view matrix
    glm::mat4 getProj(); // returns the projection matrix

private:
    Window& window;
    glm::mat4 proj; 
    glm::mat4 view;
    glm::mat4 model;
};