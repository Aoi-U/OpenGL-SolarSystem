#include "CurveControl.h"

#include "Geometry.h"
#include "Log.h"
#include "Panel.h"
#include "ShaderProgram.h"
#include "Window.h"

#include <imgui.h>
#include <memory>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class CurveEditorCallBack : public CallbackInterface {
public:
	CurveEditorCallBack() {}

	virtual void keyCallback(int key, int scancode, int action, int mods) override {
		Log::info("KeyCallback: key={}, action={}", key, action);

		if (key == GLFW_KEY_R && action == GLFW_PRESS) {
			reset = true;
		} else if (key == GLFW_KEY_R && action == GLFW_RELEASE) {
			reset = false;
		}
		
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			deleteMode = true;
		} else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
			deleteMode = false;
		}
	}

	virtual void mouseButtonCallback(int button, int action, int mods) override {
		Log::info("MouseButtonCallback: button={}, action={}", button, action);

		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
			wasClicked = true;
		} else if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
			wasClicked = false;
		}
	}

	virtual void cursorPosCallback(double xpos, double ypos) override {
		Log::info("CursorPosCallback: xpos={}, ypos={}", xpos, ypos);
		this->xpos = xpos;
		this->ypos = ypos;
	}

	virtual void scrollCallback(double xoffset, double yoffset) override {
		Log::info("ScrollCallback: xoffset={}, yoffset={}", xoffset, yoffset);
	}

	virtual void windowSizeCallback(int width, int height) override {
		Log::info("WindowSizeCallback: width={}, height={}", width, height);

		// **Important**, calls glViewport(0, 0, width, height);
		CallbackInterface::windowSizeCallback(width, height);
	}

	bool wasClicked = false;
	bool deleteMode = false;
	bool reset = false;
	double xpos = 0, ypos = 0;
	double xstart = 0, ystart = 0;
	int samples = 10;
private:
};

// Can swap the callback instead of maintaining a state machine

class TurnTable3DViewerCallBack : public CallbackInterface {
public:
	TurnTable3DViewerCallBack() {}

	virtual void keyCallback(int key, int scancode, int action, int mods) {}
	virtual void mouseButtonCallback(int button, int action, int mods) {
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
			wasClicked = true;
		} else if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
			wasClicked = false;
		}
	}
	virtual void cursorPosCallback(double xpos, double ypos) {
		this->xpos = (float)xpos;
		this->ypos = (float)ypos;
	}
	virtual void scrollCallback(double xoffset, double yoffset) {
		this->yoffset = (float)yoffset;
	}
	virtual void windowSizeCallback(int width, int height) {

		// The CallbackInterface::windowSizeCallback will call glViewport for us
		CallbackInterface::windowSizeCallback(width, height);
	}

	int forward;
	int backward;
	int left;
	int right;
	float xpos = 0, ypos = 0;
	float yoffset = 0;
	bool wasClicked = false;
};

class CurveEditorPanelRenderer : public PanelRendererInterface {
public:
	CurveEditorPanelRenderer()
		: inputText(""), editorMode(true), pointSize(5.0f), dragValue(0.0f),
		inputValue(0.0f), checkboxValue(false), comboSelection(0) {
		// Initialize options for the combo box
		options[0] = "Option 1";
		options[1] = "Option 2";
		options[2] = "Option 3";

		// Initialize color (white by default)
		colorValue[0] = 1.0f; // R
		colorValue[1] = 1.0f; // G
		colorValue[2] = 1.0f; // B
	}

	virtual void render() override {
		// Color selector
		ImGui::ColorEdit3("Select Background Color",
			colorValue); // RGB color selector
		ImGui::Text("Selected Color: R: %.3f, G: %.3f, B: %.3f", colorValue[0],
			colorValue[1], colorValue[2]);

		// Text input
		ImGui::InputText("Input Text", inputText, IM_ARRAYSIZE(inputText));

		// Display the input text
		ImGui::Text("You entered: %s", inputText);

		// Button
		if (ImGui::Button("Change Editor Mode")) {
			editorMode = !editorMode;
		}

		// Scrollable block
		ImGui::TextWrapped("Scrollable Block:");
		ImGui::BeginChild("ScrollableChild", ImVec2(0, 100),
			true); // Create a scrollable child
		for (int i = 0; i < 20; i++) {
			ImGui::Text("Item %d", i);
		}
		ImGui::EndChild();

		// Float slider
		ImGui::SliderFloat("Float Slider", &pointSize, 5.0f, 100.0f,
			"Point Size: %.3f");

		// Float drag
		ImGui::DragFloat("Float Drag", &dragValue, 0.1f, 0.0f, 100.0f,
			"Drag Value: %.3f");

		// Float input
		ImGui::InputFloat("Float Input", &inputValue, 0.1f, 1.0f,
			"Input Value: %.3f");

		// Checkbox
		ImGui::Checkbox("Enable Feature", &checkboxValue);
		ImGui::Text("Feature Enabled: %s", checkboxValue ? "Yes" : "No");

		// Combo box
		ImGui::Combo("Select an Option", &comboSelection, options,
			IM_ARRAYSIZE(options));
		ImGui::Text("Selected: %s", options[comboSelection]);

		// Displaying current values
		ImGui::Text("Point Size: %.3f", pointSize);
		ImGui::Text("Drag Value: %.3f", dragValue);
		ImGui::Text("Input Value: %.3f", inputValue);
	}

	glm::vec3 getColor() const {
		return glm::vec3(colorValue[0], colorValue[1], colorValue[2]);
	}

	float getPointSize() const { return pointSize; }
	bool getEditorMode() const { return editorMode;}

private:
	float colorValue[3];    // Array for RGB color values
	char inputText[256];    // Buffer for input text
	float pointSize;        // Value for float slider
	float dragValue;        // Value for drag input
	float inputValue;       // Value for float input
	bool checkboxValue;     // Value for checkbox
	bool editorMode;        // Value for editor mode
	int comboSelection;     // Index of selected option in combo box
	const char* options[3]; // Options for the combo box
};

CurveControl::CurveControl(Window& window)
	: mShader("shaders/test.vert", "shaders/test.frag"),
	mPanel(window.getGLFWwindow()), window(window), camera(Camera(window)) {
	mCurveControls = std::make_shared<CurveEditorCallBack>();
	m3DCameraControls = std::make_shared<TurnTable3DViewerCallBack>();

	mPanelRenderer = std::make_shared<CurveEditorPanelRenderer>();

	// Set callback to window
	window.setCallbacks(mCurveControls);
	// Can swap the callback instead of maintaining a state machine
	// window.setCallbacks(turn_table_3D_viewer_callback);

	mPanel.setPanelRenderer(mPanelRenderer);

	mCurveGeometry = GenerateInitialGeometry();
	mGPUGeometry.setVerts(mCurveGeometry.verts);
	mGPUGeometry.setCols(mCurveGeometry.cols);

	// Using two different buffers for the control points and the lines themselves
	// makes it easier to highlight the selected point
	mPointGPUGeometry.setVerts(mCurveGeometry.verts);
	mPointGPUGeometry.setCols(
		std::vector<glm::vec3>(mCurveGeometry.verts.size(), { 1.f, 0.f, 0.f }));

	// Setup OpenGL state that stays constant for most of the program
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

double prev = glfwGetTime();
float rotation = 0.0f;

void CurveControl::DrawGeometry() {
	glm::vec3 backgroundColor = mPanelRenderer->getColor();

	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mShader.use();

	GLint editorLoc = glGetUniformLocation(mShader, "editor");
	if (mPanelRenderer->getEditorMode()) {
		glUniform1i(editorLoc, true);
	} else {
		glUniform1i(editorLoc, false);
	}

	GLint modelLoc = glGetUniformLocation(mShader, "model");
	GLint viewLoc = glGetUniformLocation(mShader, "view");
	GLint projLoc = glGetUniformLocation(mShader, "proj");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(camera.getModel()));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getView()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera.getProj()));


	// Render the control points
	glPointSize(mPanelRenderer->getPointSize());
	mGPUGeometry.bind();
	glDrawArrays(GL_POINTS, 0, (int)mCurveGeometry.verts.size());

	// Render the curve that connects the control points
	mPointGPUGeometry.bind();
	glDrawArrays(GL_LINE_STRIP, 0, (int)mCurveGeometry.verts.size());

	// disable sRGB for things like imgui
	glDisable(GL_FRAMEBUFFER_SRGB);
	mPanel.render();
}

void CurveControl::DragPoint() {
	double xClick = (mCurveControls->xpos - 400.0) / 400.0;
	double yClick = (400.0 - mCurveControls->ypos) / 400.0;

	Log::debug("Dragging control point based on mouse position");
	mCurveGeometry.verts[pointIndex] = { xClick, yClick, 0.f };
	mCurveGeometry.cols[pointIndex] = { 0.f, 0.f, 1.f }; // coloring point blue for debugging
}

void CurveControl::CreatePoint() {
	double xClick = (mCurveControls->xpos - 400.0) / 400.0;
	double yClick = (400.0 - mCurveControls->ypos) / 400.0;

	Log::debug("Inserting control point based on the position clicked");
	mCurveControls->wasClicked = false;
	mCurveGeometry.verts.emplace_back(xClick, yClick, 0.f);
	mCurveGeometry.cols.emplace_back(0.f, 1.f, 0.f);
}

void CurveControl::DeletePoint() {
	if (mCurveControls->wasClicked) {
		// x and y position of where the mouse was clicked
		double xClick = (mCurveControls->xpos - 400.0) / 400.0;
		double yClick = (400.0 - mCurveControls->ypos) / 400.0;
		
		// size of points
		double pointSize = mPanelRenderer->getPointSize() / 1000.0;

		// check if the clicked position overlaps with any of the control points
		for (size_t i = 0; i < mCurveGeometry.verts.size(); i++) {
			// calculate the distance between the clicked point and this point
			double distance = glm::distance(mCurveGeometry.verts[i], glm::vec3(xClick, yClick, 0.f));
			// if the distance is less than the point size, point was clicked
			if (distance < pointSize) {
				// remove this control point
				mCurveGeometry.verts.erase(mCurveGeometry.verts.begin() + i);
				mCurveGeometry.cols.erase(mCurveGeometry.cols.begin() + i);
				break;
			}
		}
	}
}

void CurveControl::ResetPanel() {
	mCurveGeometry.verts.clear();
	mCurveGeometry.cols.clear();
	mouseOnPoint = false;
	pointIndex = -1;
}

static const float two_pi = glm::pi<float>() * 2.f;
glm::vec3 c(float t) {
	return 0.5f * glm::vec3(glm::cos(two_pi * t), glm::sin(two_pi * t), 0.f);
}

void CurveControl::UpdateEditorMode() {
	if (mCurveControls->reset) {
		ResetPanel();
	} else if (mCurveControls->deleteMode) {	// check if deleteMode is enabled by the user
		DeletePoint();
	} else if (mCurveControls->wasClicked) { 
		// x and y positions of where the mouse was clicked
		double xClick = (mCurveControls->xpos - 400.0) / 400.0;
		double yClick = (400.0 - mCurveControls->ypos) / 400.0;
		
		// size of points
		double pointSize = mPanelRenderer->getPointSize() / 1000.0; 

		if (!mouseOnPoint) {
			// check if the clicked position overlaps with any of the control points
			for (size_t i = 0; i < mCurveGeometry.verts.size(); i++) {
				// calculate the distance between the clicked point and this point
				double distance = glm::distance(mCurveGeometry.verts[i], glm::vec3(xClick, yClick, 0.f));
				// if the distance is less than the point size, then the point is clicked
				if (distance < pointSize) {
					pointIndex = i; // save the index of the clicked point
					mouseOnPoint = true;
					break;
				}	
			}
		}
		
		if (mouseOnPoint) { // if a point is clicked, drag the point with the mouse position
			DragPoint();
		} else { // if no point is clicked, insert a new point at where the mouse was clicked
			CreatePoint();
		}
	} else {
		// if the mouse is released, reset the pointIndex and mouseOnPoint 
		if (pointIndex != -1) {
			mCurveGeometry.cols[pointIndex] = { 0.f, 1.f, 0.f };
		}
		mouseOnPoint = false;
		pointIndex = -1;
	}
}

void CurveControl::UpdateViewMode() {
	// panning around origin
	if (m3DCameraControls->wasClicked) {
		if (!mouseDragging) {
			mouseDragging = true;
			xStart = m3DCameraControls->xpos;
			yStart = m3DCameraControls->ypos;
		} else if (mouseDragging) {
			float xEnd = m3DCameraControls->xpos;
			float yEnd = m3DCameraControls->ypos;

			float dx = xEnd - xStart;
			float dy = yEnd - yStart;
			std::cout << "dx: " << dx << " dy: " << dy << std::endl;
			

			camera.Move(dx, dy);

			xStart = xEnd;
			yStart = yEnd;
		}
	} else if (m3DCameraControls->yoffset != yOffsetStart) {
		yOffsetStart = m3DCameraControls->yoffset;
		std::cout << "yoffset: " << m3DCameraControls->yoffset << std::endl;
		camera.Zoom(m3DCameraControls->yoffset);
	} else {
		mouseDragging = false;
	}

	
}

void CurveControl::Update() {
	// Use this function to process logic and update things based on user inputs
	//Example: generate a new control point
	if (mPanelRenderer->getEditorMode()) {
		window.setCallbacks(mCurveControls);
		UpdateEditorMode();
	} else {
		window.setCallbacks(m3DCameraControls);
		UpdateViewMode();
	}

	mGPUGeometry.setVerts(mCurveGeometry.verts);
	mGPUGeometry.setCols(mCurveGeometry.cols);
	mPointGPUGeometry.setVerts(mCurveGeometry.verts);
	mPointGPUGeometry.setCols(
		std::vector<glm::vec3>(mCurveGeometry.verts.size(), { 1.f, 0.f, 0.f }));

	// CPU_Geometry mCurve;
	
	// float dt = 1.f / static_cast<float>(mCurveControls->samples - 1);
	// for (int i = 0; i < mCurveControls->samples; i++) {
	// 	mCurve.verts.push_back(c(i * dt));
	// 	mCurve.cols.push_back({ 1.f, 0.f, 0.f });
	// }
	// mPointGPUGeometry.setVerts(mCurve.verts);
	// mPointGPUGeometry.setCols(mCurve.cols);
}

// Generate some initial points to show what the rendering system is doing at
// the moment
CPU_Geometry CurveControl::GenerateInitialGeometry() {
	std::vector<glm::vec3> cp_positions_vector = {
	  {-.5f, -.5f, 0.f}, {.5f, -.5f, 0.f}, {.5f, .5f, 0.f}, {-.5f, .5f, 0.f} };
	glm::vec3 cp_line_colour = { 0.f, 1.f, 0.f };

	CPU_Geometry cp_point_cpu;
	cp_point_cpu.verts = cp_positions_vector;
	cp_point_cpu.cols =
		std::vector<glm::vec3>(cp_point_cpu.verts.size(), cp_line_colour);

	return cp_point_cpu;
}
