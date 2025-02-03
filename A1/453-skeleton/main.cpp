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

// #include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"

const double PI = 3.14159265358979323846;

// EXAMPLE CALLBACKS

class MyCallbacks : public CallbackInterface {

public:
	MyCallbacks(ShaderProgram& shader) : shader(shader) {}

	virtual void keyCallback(int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_R && action == GLFW_PRESS) { // On a positive edge press (when FIRST clicked)
			shader.recompile();
		}
	}

// 	// Other callbacks we implemented that you may use
// 	//virtual void mouseButtonCallback(int button, int action, int mods) {}
// 	//virtual void cursorPosCallback(double xpos, double ypos) {}
// 	//virtual void scrollCallback(double xoffset, double yoffset) {}
// 	//virtual void windowSizeCallback(int width, int height) { CallbackInterface::windowSizeCallback(width, height);/*Should be called*/ }

private:
	ShaderProgram& shader;
};

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
// END EXAMPLES

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
			return 3 * int(pow(3, iteration));
		case 1:
			return 2 * int(pow(2, iteration));
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
std::vector<triangle> calculate_triangle(cord left, double length, double iteration) {
	// base case, return a single triangle
	if (iteration == 0) {
		return {
			{
				left, // bottom left corner
				{left.x + length, left.y}, // bottom right corner 
				{left.x + length / 2, left.y + length * sin(PI / 3)} // top corner
			}
		};
	}

	// calculates points for the left subtriangle
	triangle triangle_left {
		left,
		{left.x + length / 2, left.y},
		{left.x + length / 4, left.y + length / 2 * sin(PI / 3)}
	};

	// calculates points for the right subtriangle
	triangle triangle_right {
		triangle_left.right,
		{triangle_left.right.x + length, triangle_left.right.y},
		{triangle_left.right.x + length / 2, triangle_left.right.y + length * sin(PI / 3)}
	};

	// calculates points for the top subtriangle
	triangle triangle_top {
		triangle_left.top,
		triangle_right.top,
		{triangle_left.top.x + length / 2, triangle_left.top.y + length * sin(PI / 3)}
	};

	iteration--;
	length /= 2;

	// recursively call calculate_triangle for each subtriangle
	std::vector<triangle> result_left = calculate_triangle(triangle_left.left, length, iteration);
	std::vector<triangle> result_right = calculate_triangle(triangle_left.right, length, iteration);
	std::vector<triangle> result_top = calculate_triangle(triangle_left.top, length, iteration);

	// combine all triangles into a single vector and return 
	std::vector<triangle> result {};
	result.insert(result.end(), result_left.begin(), result_left.end());
	result.insert(result.end(), result_right.begin(), result_right.end());
	result.insert(result.end(), result_top.begin(), result_top.end());

	return result;
}

std::vector<levy> calculate_levy(cord start, double length, double alpha, double iteration) {
	// base case, return a single line from points start to end
	if (iteration == 0) {
		return {
			{
				start,
				{
					start.x + length * cos(alpha),
					start.y + length * sin(alpha)
				}
			}
		};
	}

	length *= sqrt(2) / 2;

	// calculates points for the first subline
	levy first {
		start,
		{
			start.x + length * cos(alpha + PI / 4),
			start.y + length * sin(alpha + PI / 4)
		}
	};

	// calculates points for the second subline
	levy second {
		first.end,
		{
			first.end.x + length * cos(alpha + PI / 4),
			first.end.y + length * sin(alpha + PI / 4)
		}
	};
	
	iteration--;

	// recursively call calculate_levy for each subline
	std::vector<levy> result_first = calculate_levy(first.start, length, alpha + PI / 4, iteration);
	std::vector<levy> result_second = calculate_levy(second.start, length, alpha - PI / 4, iteration);

	// combine all line segments into a single vector and return
	std::vector<levy> result {};
	result.insert(result.end(), result_first.begin(), result_first.end());
	result.insert(result.end(), result_second.begin(), result_second.end());

	return result;
}

std::vector<tree> calculate_tree(cord start, double length, double alpha, double iteration, int max_iteration) {
	// if before 4 iterations, color the tree brown, otherwise color the tree green
	color col;
	if (iteration < max_iteration - 3) {
		col = {0.1, 0.5, 0.1};
	} else {
		col = {0.5, 0.3, 0.1};
	}
	// base case, return a single branch from start to end
	if (iteration == 0) {
		return {
			{
				start,
				{
					start.x + length * cos(alpha),
					start.y + length * sin(alpha)
				},
				col
			}
		};
	}

	tree branch {
		start,
		{
			start.x + length * cos(alpha),
			start.y + length * sin(alpha)
		},
		col
	};

	length /= 2;

	cord mid {
		(branch.start.x + branch.end.x) / 2,
		(branch.start.y + branch.end.y) / 2
	};

	tree left_branch {
		mid, 
		{
			mid.x + length * cos(alpha + 25.7 * (PI/180)),
			mid.y + length * sin(alpha + 25.7 * (PI/180))
		},
		col
	};

	tree right_branch {
		mid,
		{
			mid.x + length * cos(alpha - 25.7 * (PI/180)),
			mid.y + length * sin(alpha - 25.7 * (PI/180))
		},
		col
	};

	tree top_branch {
		branch.end,
		{
			branch.end.x + length * cos(alpha),
			branch.end.y + length * sin(alpha)
		},
		col
	};

	iteration--;

	std::vector<tree> result_left = calculate_tree(left_branch.start, length, alpha + 25.7 * (PI/180), iteration, max_iteration);
	std::vector<tree> result_right = calculate_tree(right_branch.start, length, alpha - 25.7 * (PI/180), iteration, max_iteration);
	std::vector<tree> result_top = calculate_tree(top_branch.start, length, alpha, iteration, max_iteration);

	std::vector<tree> result {branch};
	result.insert(result.end(), result_left.begin(), result_left.end());
	result.insert(result.end(), result_right.begin(), result_right.end());
	result.insert(result.end(), result_top.begin(), result_top.end());

	return result;
}

// render serpinski triangle
void render_serpinski(CPU_Geometry& cpuGeom, int iterations) {
	// call calculate_triangle to get all vertices of triangles
	std::vector<triangle> triangles = calculate_triangle({-0.8f, -0.8f}, 1.6f, iterations);

	double step = 1.f / triangles.size(); // color increment value between each triangle 
	double current = 0.f; // starting value

	// for each triangle, add its vertices and color to cpuGeom
	for (triangle triangle : triangles) {
		cpuGeom.verts.push_back(glm::vec3(triangle.left.x, triangle.left.y, 0.f));
		cpuGeom.verts.push_back(glm::vec3(triangle.right.x, triangle.right.y, 0.f));
		cpuGeom.verts.push_back(glm::vec3(triangle.top.x, triangle.top.y, 0.f));

		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
		current += step;		
	}
}

void render_levy(CPU_Geometry& cpuGeom, int iterations) {
	// call calculate_levy to get all vertices of the levy c curve
	std::vector<levy> levys = calculate_levy({0.f, -0.4f}, 0.8f, PI / 2, iterations);

	double step = 1.f / levys.size(); // color increment value for each line segment
	double current = 0.f; // starting value

	// for each line segment, add its vertices and color to cpuGeom
	for (levy levy : levys) {
		cpuGeom.verts.push_back(glm::vec3(levy.start.x, levy.start.y, 0.f));
		cpuGeom.verts.push_back(glm::vec3(levy.end.x, levy.end.y, 0.f));

		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
		current += step;
		cpuGeom.cols.push_back(glm::vec3(1.f - current, current, 0.f));
	}
}

void render_tree(CPU_Geometry& cpuGeom, int iterations) {
	// call calculate_tree to get all vertices of the tree
	std::vector<tree> trees = calculate_tree({0.f, -0.8f}, 0.8f, PI / 2, iterations, iterations);

	for (tree branch : trees) {
		cpuGeom.verts.push_back(glm::vec3(branch.start.x, branch.start.y, 0.f));
		cpuGeom.verts.push_back(glm::vec3(branch.end.x, branch.end.y, 0.f));

		cpuGeom.cols.push_back(glm::vec3(branch.col.r, branch.col.g, branch.col.b));
		cpuGeom.cols.push_back(glm::vec3(branch.col.r, branch.col.g, branch.col.b));
	}
}

int main() {
	Log::debug("Starting main");

	// WINDOW
	glfwInit();//MUST call this first to set up environment (There is a terminate pair after the loop)
	Window window(800, 800, "CPSC 453 Assignment 1: Fractals"); // Can set callbacks at construction if desired

	GLDebug::enable(); // ON Submission you may comments this out to avoid unnecessary prints to the console

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

	// CALLBACKS
	// std::shared_ptr<MyCallbacks> callback_ptr = std::make_shared<MyCallbacks>(shader); // Class To capture input events
	// //std::shared_ptr<MyCallbacks2> callback2_ptr = std::make_shared<MyCallbacks2>();
	// window.setCallbacks(callback_ptr); // Can also update callbacks to new ones as needed (create more than one instance)

	std::shared_ptr<KeyPresses> callback_ptr = std::make_shared<KeyPresses>(iterations, shape, max_iterations);
	window.setCallbacks(callback_ptr);

	// GEOMETRY
	CPU_Geometry cpuGeom; // Just a collection of vectors
	GPU_Geometry gpuGeom; // Wrapper managing VAO and VBOs, in a TIGHTLY packed format
	//https://www.khronos.org/opengl/wiki/Vertex_Specification_Best_Practices#Attribute_sizes

	// IMGUI_CHECKVERSION();
	// ImGui::CreateContext();
	// ImGuiIO& io = ImGui::GetIO(); (void)io;
	// ImGui::StyleColorsDark();
	// ImGui_ImplGlfw_InitForOpenGL(window.getGLFWwindow(), true);
	// ImGui_ImplOpenGL3_Init("#version 330");

	while (!window.shouldClose()) {
		glfwPollEvents(); // Propagate events to the callback class

		int nVerts = 0;
		switch (shape) {
			case 0:
				render_serpinski(cpuGeom, iterations);
				render_mode = GL_TRIANGLES;
				nVerts = calculate_num_vertices(iterations, shape);
				max_iterations = 10;
				break;
			case 1:
				render_levy(cpuGeom, iterations);
				render_mode = GL_LINES;
				nVerts = calculate_num_vertices(iterations, shape);
				max_iterations = 15;
				break;
			case 2:
				render_tree(cpuGeom, iterations);
				render_mode = GL_LINES;
				nVerts = calculate_num_vertices(iterations, shape);
				max_iterations = 10;
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
		cpuGeom.verts.clear();
		cpuGeom.cols.clear();

		window.swapBuffers(); //Swap the buffers while displaying the previous 
	}

	glfwTerminate(); // Clean up GLFW
	return 0;
}
