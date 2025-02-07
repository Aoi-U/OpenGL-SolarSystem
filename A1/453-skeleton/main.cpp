#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <math.h>

#include "Geometry.h"
#include "GLDebug.h"
#include "Log.h"
#include "ShaderProgram.h"
#include "Shader.h"
#include "Window.h"
#include "AssetPath.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


// Callback class incase ImGui fails
class KeyPresses : public CallbackInterface {

public:
	KeyPresses(int& iterations, int& shape, int& max_iterations) : iterations(iterations), shape(shape), max_iterations(max_iterations) {}	

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
			if (iterations < max_iterations) {
				iterations++;
				std::cout << "Iterations: " << iterations << std::endl;
			}
		} else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
			if (iterations > 0) {
				iterations--;
				std::cout << "Iterations: " << iterations << std::endl;
			}
		} else if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			if (shape > 0) {
				shape--;
				iterations = 0;
			}
		} else if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
			if (shape < 2) {
				shape++;
				iterations = 0;
			}
		}
	}
private:
	int& iterations;
	int& shape;
	int& max_iterations;
};

// defines a point with x and y values
struct cord {
	double x;
	double y;
};

// defines a color with r, g, and b values
struct color {
	double r;
	double g;
	double b;
};

// defines a triangle with 3 vertices and the color of its triangle
struct triangle {
	cord left; // bottom left corner
	cord right; // bottom right corner
	cord top; // top corner
};

struct levy {
	cord start; // starting point of a line segment
	cord end; // end point of a line segment
};

struct tree {
	cord start;
	cord end;
	color col;
};

// calculates the number of vertices for x iterations
int calculate_num_vertices(int iteration, int shape) {
	switch (shape) {
		case 0:
			return 3 * glm::pow(3, iteration);
		case 1:
			return 2 * glm::pow(2, iteration);
		case 2:
			int previous = 2;
			for (int i = 0; i < iteration; i++) {
				previous = 3 * previous + 2;
			}
			return previous;
	}

	return 0;
}

// recursive function to calculate all vertices of triangles
std::vector<glm::vec3> calculate_triangle(glm::vec3 left, double length, double iteration) {
	// base case, return a single triangle with vertices left, right, and top
	glm::vec3 right = left + glm::vec3(length, 0.f, 0.f);
	glm::vec3 top = left + glm::vec3(length / 2, length * sqrt(3) / 2, 0.f);
	if (iteration == 0) {
		std::vector<glm::vec3> result {left, right, top};
		return result;
	}

	// calculate the midpoints of each side of the triangle
	glm::vec3 left_mid = (left + top) / 2.f;
	glm::vec3 right_mid = (right + top) / 2.f;
	glm::vec3 top_mid = (left + right) / 2.f;

	iteration--;

	// recursively call calculate_triangle for each subtriangle
	std::vector<glm::vec3> result_left = calculate_triangle(left, length / 2, iteration);
	std::vector<glm::vec3> result_right = calculate_triangle(left_mid, length / 2, iteration);
	std::vector<glm::vec3> result_top = calculate_triangle(top_mid, length / 2, iteration);

	// combine all vertices into a single vector and return
	std::vector<glm::vec3> result {};
	result.insert(result.end(), result_left.begin(), result_left.end());
	result.insert(result.end(), result_right.begin(), result_right.end());
	result.insert(result.end(), result_top.begin(), result_top.end());

	return result;	
}

std::vector<glm::vec3> calculate_levy(glm::vec3 start, double length, double alpha, double iteration) {
	// base case, return a single line from points start to end
	if (iteration == 0) {
		return {start, glm::vec3(start.x + length * glm::cos(alpha), start.y + length * glm::sin(alpha), 0.f)};
	}

	length *= glm::sqrt(2) / 2;
	// calculate the mid point of the line segment
	glm::vec3 mid {
		start.x + length * glm::cos(alpha + glm::pi<double>() / 4),
		start.y + length * glm::sin(alpha + glm::pi<double>() / 4),
		0.f
	};

	iteration--;
	// recursively call calculate_levy for each subline segment
	std::vector<glm::vec3> result_first = calculate_levy(start, length, alpha + glm::pi<double>() / 4, iteration);
	std::vector<glm::vec3> result_second = calculate_levy(mid, length, alpha - glm::pi<double>() / 4, iteration);

	// combine all line segments into a single vector and return
	std::vector<glm::vec3> result {};
	result.insert(result.end(), result_first.begin(), result_first.end());
	result.insert(result.end(), result_second.begin(), result_second.end());

	return result;
}

// contains only parameters necessary for the bonus marks from the assignment
struct bonus_params {
	float angle = 25.7; // Lateral branch angle
	float scale = 0.5; // branch scaling factor
	float interpolation_factor = 0.5; // lateral branch interpolation factor
	int color_depth = 4; // leaf color depth
};

std::vector<std::pair<glm::vec3, color>> calculate_tree(glm::vec3 start, double length, double alpha, int iteration, int max_iteration, bonus_params bonus) {	
	color col;
	if (iteration < max_iteration - (bonus.color_depth - 1)) {
		col = {0.1, 0.5, 0.1};
	} else {
		col = {0.5, 0.3, 0.1};
	}

	// calculate the end point of the branch
	glm::vec3 end {
		start.x + length * glm::cos(alpha),
		start.y + length * glm::sin(alpha),
		0.f
	};

	// base case, return a single branch from start to end
	if (iteration == 0) {
		return {{start, col}, {end, col}};
	}

	// calculate the mid point (or user defined point by interpolation_factor) of a line segment
	glm::vec3 mid {
		start.x + bonus.interpolation_factor * (end.x - start.x),
		start.y + bonus.interpolation_factor * (end.y - start.y),
		0.f
	};

	iteration--;
	// scale the length of the branch by 0.5 (or user defined scale)
	length *= bonus.scale;
	// recursively call calculate_tree for each subbranch
	std::vector<std::pair<glm::vec3, color>> result_left = calculate_tree(mid, length, alpha + glm::radians(bonus.angle), iteration, max_iteration, bonus);
	std::vector<std::pair<glm::vec3, color>> result_right = calculate_tree(mid, length, alpha - glm::radians(bonus.angle), iteration, max_iteration, bonus);
	std::vector<std::pair<glm::vec3, color>> result_top = calculate_tree(end, length, alpha, iteration, max_iteration, bonus);

	// combine all branches into a single vector and return
	std::vector<std::pair<glm::vec3, color>> result {{start, col}, {end, col}};
	result.insert(result.end(), result_left.begin(), result_left.end());
	result.insert(result.end(), result_right.begin(), result_right.end());
	result.insert(result.end(), result_top.begin(), result_top.end());

	return result;
}

// render serpinski triangle
void render_serpinski(CPU_Geometry& cpuGeom, int iterations) {
	// call calculate_triangle to get all vertices of triangles
	std::vector<glm::vec3> triangles = calculate_triangle(glm::vec3(-0.8f, -0.8f, 0.f), 1.6f, iterations);

	double step = 1.f / (triangles.size() / 3); // color increment value for each triangle
	double current = 0.f; // starting value

	// for each triangle, add its vertices and color to cpuGeom
	for (int i = 0; i < triangles.size(); i++) {
		cpuGeom.verts.push_back(triangles[i]);
		cpuGeom.verts.push_back(triangles[++i]);
		cpuGeom.verts.push_back(triangles[++i]);
		
		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
		current += step;		
	}
}

void render_levy(CPU_Geometry& cpuGeom, int iterations) {
	// call calculate_levy to get all vertices of the levy c curve
	std::vector<glm::vec3> levys = calculate_levy({0.f, -0.4f, 0.f}, 0.8f, glm::pi<double>() / 2, iterations);

	double step = 1.f / (levys.size() / 2); // color increment value for each line segment
	double current = 0.f; // starting value

	// for each line segment, add its vertices and color to cpuGeom
	for (int i = 0; i < levys.size(); i++) {
		cpuGeom.verts.push_back(levys[i]);
		cpuGeom.verts.push_back(levys[++i]);

		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
		current += step;
		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
	}
}

void render_tree(CPU_Geometry& cpuGeom, int iterations, bonus_params bonus) {
	// call calculate_tree to get all vertices of the tree
	std::vector<std::pair<glm::vec3, color>> trees = calculate_tree({0.f, -0.8f, 0.f}, 0.8f, glm::pi<double>() / 2, iterations, iterations, bonus);

	// calculate the vertex number where the color changes

	// for each branch, add its vertices and color to cpuGeom
	for (int i = 0; i < trees.size(); i++) {
		cpuGeom.verts.push_back(trees[i].first);
		cpuGeom.cols.push_back(glm::vec3(trees[i].second.r, trees[i].second.g, trees[i].second.b));

		cpuGeom.verts.push_back(trees[++i].first);
		cpuGeom.cols.push_back(glm::vec3(trees[i].second.r, trees[i].second.g, trees[i].second.b));
	}
	
}

int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();//MUST call this first to set up environment (There is a terminate pair after the loop)
	Window window(800, 800, "CPSC 453 Assignment 1: Fractals"); // Can set callbacks at construction if desired

	GLDebug::enable(); // ON Submission you may comments this out to avoid unnecessary prints to the console

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);          // Second param install_callback=true will install GLFW callbacks and chain to existing ones.
	ImGui_ImplOpenGL3_Init();

	// SHADERS
	ShaderProgram shader(
		AssetPath::Instance()->Get("shaders/basic.vert"), 
		AssetPath::Instance()->Get("shaders/basic.frag")
	); // Render pipeline we will use (You can use more than one!)

	// default values for render mode, iterations, and shape
	int render_mode = GL_TRIANGLES; // Default to triangle
	int max_iterations = 10;
	int iterations = 0;
	int shape = 0;
	int prevShape = shape;
	bonus_params bonus;

	std::string shapeName = "Serpinski's Triangle";

	// UNCOMMENT THE BELOW 2 LINES FOR KEYCALLBACKS
	//std::shared_ptr<KeyPresses> callback_ptr = std::make_shared<KeyPresses>(iterations, shape, max_iterations);
	//window.setCallbacks(callback_ptr);

	// GEOMETRY
	CPU_Geometry cpuGeom; // Just a collection of vectors
	GPU_Geometry gpuGeom; // Wrapper managing VAO and VBOs, in a TIGHTLY packed format
	//https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices#Attribute_sizes

	while (!window.shouldClose()) {
		glfwPollEvents(); // Propagate events to the callback class
		ImGui_ImplOpenGL3_NewFrame();

		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		cpuGeom.verts.clear();
		cpuGeom.cols.clear();
		int nVerts = 0;

		if (prevShape != shape) {
			iterations = 0;
			prevShape = shape;
			bonus = {};
		}
		switch (shape) {
			case 0:
				render_mode = GL_TRIANGLES;
				nVerts = calculate_num_vertices(iterations, shape);
				max_iterations = 9;
				shapeName = "Serpinski's Triangle";
				render_serpinski(cpuGeom, iterations);
				break;
			case 1:
				render_mode = GL_LINES;
				nVerts = calculate_num_vertices(iterations, shape);
				max_iterations = 15;
				shapeName = "C Levy Curve";
				render_levy(cpuGeom, iterations);
				break;
			case 2:
				render_mode = GL_LINES;
				nVerts = calculate_num_vertices(iterations, shape);
				max_iterations = 10;
				shapeName = "Tree";
				render_tree(cpuGeom, iterations, bonus);
				break;
		}

		gpuGeom.setVerts(cpuGeom.verts);
		gpuGeom.setCols(cpuGeom.cols);

		shader.use(); // Use "this" shader to render
		gpuGeom.bind(); // USe "this" VAO (Geometry) on render call

		glEnable(GL_FRAMEBUFFER_SRGB); // Expect Colour to be encoded in sRGB standard (as opposed to RGB) 
		// https://www.viewsonic.com/library/creative-work/srgb-vs-adobe-rgb-which-one-to-use/
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear render screen (all zero) and depth (all max depth)
		glDrawArrays(render_mode, 0, nVerts); // Render Triangle primatives, starting at index 0 (first) with a total of nVerts
		glDisable(GL_FRAMEBUFFER_SRGB); // disable sRGB for things like imgui (if used)

		ImGui::Begin("Bonus Points");
		// show fps
		ImGui::Text("FPS: %.1f", ImGui::GetIO().Framerate);
		if (ImGui::Button("Reset Parameters")) {
			iterations = 0;
			bonus = {};
		}
		ImGui::SliderInt("Shape", &shape, 0, 2, shapeName.c_str());
		ImGui::SliderInt("Iteration", &iterations, 0, max_iterations, std::to_string(iterations).c_str());
		// enable extra tree customization values if on the tree fractal
		if (shape == 2) {
			ImGui::SliderFloat("Branch Angle", &bonus.angle, 0, 180, (std::to_string(bonus.angle) + "Degrees").c_str());
			ImGui::SliderFloat("Branch Scale", &bonus.scale, 0, 1, std::to_string(bonus.scale).c_str());
			ImGui::SliderFloat("Anchor Factor", &bonus.interpolation_factor, 0, 1, std::to_string(bonus.interpolation_factor).c_str());
			ImGui::SliderInt("Color Depth", &bonus.color_depth, 0, 11, std::to_string(bonus.color_depth).c_str());
		}
		ImGui::End();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		window.swapBuffers(); //Swap the buffers while displaying the previous 
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate(); // Clean up GLFW
	return 0;
}
