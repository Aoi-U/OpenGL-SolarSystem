#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Window.h"

// Camera class that keeps track and controls the camera
class Camera
{
public:
    Camera(Window& window);

    void Move(float dTheta, float dPhi); // pans the camera about the origin
    void Zoom(float zoom); // zooms the camera towards/away from the origin
    void Reset(); // resets the camera to its initial position
    glm::mat4 getModel(); // returns the model matrix
    glm::mat4 getView(); // returns the view matrix
    glm::mat4 getProj(); // returns the projection matrix

private:
    static constexpr float sensitivity = 0.3f; // sensitivity for camera movement
    static constexpr float defaultRadius = 2.0f; // default distance from the origin
    static constexpr float defaultTheta = 45.0f; // default angle in the xz plane
    static constexpr float defaultPhi = 45.0f; // default angle in the xy plane

    Window& window;
    glm::mat4 proj; // projection matrix
    glm::mat4 view; // view matrix
    glm::mat4 model; // model matrix

    float radius; // distance from the origin
    float phi; // angle in the xz plane
    float theta; // angle in the xy plane

    void UpdateView(); // updates the view matrix based on the current radius, phi, and theta
};