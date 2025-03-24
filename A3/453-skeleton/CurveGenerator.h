#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Geometry.h"

// class to create bezier, b-spline and surface revolution geometries. 
class CurveGenerator
{
public:
	void GenerateBezierCurve(const CPU_Geometry& points, CPU_Geometry& curve, float step); // generates a bezier curve from user created control points
	void GenerateBSplineCurve(CPU_Geometry& curve); // generates a quadratic b-spline curve from user created control points
	void GenerateSurfaceOfRevolution(const CPU_Geometry& curve, CPU_Geometry& revolution, float step); // generates a surface of revolution from the control points

private:
	glm::vec3 deCasteljau(std::vector<glm::vec3> points, size_t d, float u); // de Casteljau algorithm for bezier curve taken from the lecture
};
