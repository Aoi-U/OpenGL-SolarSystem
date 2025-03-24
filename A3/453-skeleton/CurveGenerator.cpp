#include "CurveGenerator.h"

void CurveGenerator::GenerateBezierCurve(const CPU_Geometry& points, CPU_Geometry& curve, float step) {
	if (points.verts.size() > 1) {
		// generates a bezier curve from the control points
		curve.verts.clear();
		curve.cols.clear();
		// run de Casteljau algorithm for each segment of the curve
		for (float u = 0; u <= 1; u += step) {
			glm::vec3 point = deCasteljau(points.verts, points.verts.size(), u);
			curve.verts.emplace_back(point);
			curve.cols.emplace_back(0.f, 0.f, 1.f);
		}

		// floating point loop may pass 1 so guarantee a point at 1
		glm::vec3 point = deCasteljau(points.verts, points.verts.size(), 1);
		curve.verts.emplace_back(point);
		curve.cols.emplace_back(0.f, 0.f, 1.f);
	}
}

// de Casteljau algorithm referenced from the lecture
glm::vec3 CurveGenerator::deCasteljau(std::vector<glm::vec3> points, size_t d, float u) {
	for (size_t i = 1; i < d; i++) {
		for (int j = 0; j < d - i; j++) {
			points[j] = (1 - u) * points[j] + u * points[j + 1];
		}
	}
	return points[0];
}

void CurveGenerator::GenerateBSplineCurve(const CPU_Geometry& points, CPU_Geometry& curve) {
	// generates a quadratic b-spline curve from the control points
	curve.verts.clear();
	curve.cols.clear();

	// run chaikins algorithm on the control points
	if (points.verts.size() > 1) {
		std::vector<glm::vec3> newPoints = points.verts;

		// run chaikins algorithm 4 times for a sufficiently smooth curve
		for (int i = 0; i < 4; i++) {
			newPoints = chaikin(newPoints);
		}
		// add the new points to the curve
		curve.verts = newPoints;
		curve.cols = std::vector<glm::vec3>(newPoints.size(), glm::vec3(0.f, 0.f, 1.f));
	}
}

// chaikin's algorithm referenced from the lecture 
std::vector<glm::vec3> CurveGenerator::chaikin(const std::vector<glm::vec3>& points) {
	std::vector<glm::vec3> newPoints;

	newPoints.push_back(points[0]);

	for (size_t i = 0; i < points.size() - 1; i++) {
		newPoints.push_back(0.75f * points[i] + 0.25f * points[i + 1]);
		newPoints.push_back(0.25f * points[i] + 0.75f * points[i + 1]);
	}

	newPoints.push_back(points[points.size() - 1]);

	return newPoints;
}

void CurveGenerator::GenerateSurfaceOfRevolution(const CPU_Geometry& curve, CPU_Geometry& revolution, float step) {
	revolution.verts.clear();
	revolution.cols.clear();

	// stores a vector of sample points for each section of the revolution
	std::vector<std::vector<glm::vec3>> points;

	// generate a surface of revolution from the sample points
	for (float u = 0; u < glm::two_pi<float>(); u += step) {
		// generates sample points for section u of the revolution
		std::vector<glm::vec3> curveSection;
		for (auto point : curve.verts) {
			curveSection.emplace_back(point.x * glm::cos(u), point.y, point.x * glm::sin(u));
		}
		points.push_back(curveSection);
	}

	// floating point loop may pass 2pi so guarantee a section at 2pi
	std::vector<glm::vec3> endSection;
	for (auto point : curve.verts) {
		endSection.emplace_back(point.x * glm::cos(glm::two_pi<float>()), point.y, point.x * glm::sin(glm::two_pi<float>()));
	}
	points.push_back(endSection);

	// triangulate each section and add it to the VBO
	for (size_t i = 0; i < points.size() - 1; i++) {
		for (size_t j = 0; j < points[i].size() - 1; j++) {
			glm::vec3 pOne = points[i][j];
			glm::vec3 pTwo = points[i + 1][j];
			glm::vec3 pThree = points[i][j + 1];
			glm::vec3 pFour = points[i + 1][j + 1];

			revolution.verts.push_back(pOne);
			revolution.verts.push_back(pTwo);
			revolution.verts.push_back(pThree);
			revolution.cols.emplace_back(0.f, 1.f, 1.f);
			revolution.cols.emplace_back(0.f, 1.f, 1.f);
			revolution.cols.emplace_back(0.f, 1.f, 1.f);

			revolution.verts.push_back(pTwo);
			revolution.verts.push_back(pThree);
			revolution.verts.push_back(pFour);
			revolution.cols.emplace_back(0.f, 1.f, 1.f);
			revolution.cols.emplace_back(0.f, 1.f, 1.f);
			revolution.cols.emplace_back(0.f, 1.f, 1.f);
		}
	}
}

