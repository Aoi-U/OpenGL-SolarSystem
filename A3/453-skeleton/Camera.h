#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Window.h"

// Camera class that keeps track and controls the camera
class Camera
{
public:
    Camera(int width = 800, int height = 800);

    void Move(const float& dTheta, const float& dPhi); // pans the camera about the origin
    void Zoom(const float& zoom); // zooms the camera towards/away from the origin
    void Reset(); // resets the camera to its initial position
		void TogglePerspectiveMode(); // toggles between perspective and orthographic mode
    glm::mat4 getModel(); // returns the model matrix
    glm::mat4 getView(); // returns the view matrix
    glm::mat4 getProj(); // returns the projection matrix

private:
    static constexpr float sensitivity = 0.3f; // sensitivity for camera movement
    static constexpr float defaultRadius = 2.0f; // default distance from the origin
    static constexpr float defaultTheta = 45.0f; // default angle in the xz plane
    static constexpr float defaultPhi = 45.0f; // default angle in the xy plane

    glm::mat4 proj; // projection matrix
    glm::mat4 view; // view matrix
    glm::mat4 model; // model matrix

    int width = 800; // width of the window
    int height = 800; // height of the window

    float radius; // distance from the origin
    float phi; // angle in the xz plane
    float theta; // angle in the xy plane

		bool isPerspective = true; // perspective mode

    void UpdateView(); // updates the view matrix based on the current radius, phi, and theta
};
