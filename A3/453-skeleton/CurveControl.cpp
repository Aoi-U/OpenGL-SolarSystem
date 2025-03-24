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
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			deleteMode = true;
		}
		else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
			deleteMode = false;
		}
	}

	virtual void mouseButtonCallback(int button, int action, int mods) override {
		if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS) {
			wasClicked = true;
		}
		else if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
			wasClicked = false;
		}
	}

	virtual void cursorPosCallback(double xpos, double ypos) override {
		this->xpos = xpos;
		this->ypos = ypos;
	}

	virtual void scrollCallback(double xoffset, double yoffset) override { }

	virtual void windowSizeCallback(int width, int height) override {
		// **Important**, calls glViewport(0, 0, width, height);
		CallbackInterface::windowSizeCallback(width, height);
	}

	bool wasClicked = false; // mouse click
	bool deleteMode = false; // delete mode enabled when holding space
	double xpos = 0, ypos = 0;  // mouse position
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
		}
		else if (button == GLFW_MOUSE_BUTTON_1 && action == GLFW_RELEASE) {
			wasClicked = false;
		}
	}
	virtual void cursorPosCallback(double xpos, double ypos) {
		this->xpos = (float)xpos;
		this->ypos = (float)ypos;
	}
	virtual void scrollCallback(double xoffset, double yoffset) {
		this->yoffset = (float)yoffset;
		scrolling = true;
	}
	virtual void windowSizeCallback(int width, int height) {

		// The CallbackInterface::windowSizeCallback will call glViewport for us
		CallbackInterface::windowSizeCallback(width, height);
	}

	float xpos = 0, ypos = 0; // mouse position
	float yoffset = 0; // scroll wheel movement
	bool wasClicked = false; // mouse click
	bool scrolling = false; // scroll wheel movement
};

class CurveEditorPanelRenderer : public PanelRendererInterface {
public:
	CurveEditorPanelRenderer()
		: reset(false), bezierMode(true), solidMode(true), perspectiveMode(true), pointSize(10.0f), curveRes(10), checkboxValue(false), comboSelection(0) {
		// Initialize options for the combo box
		options[0] = "2D Editor";
		options[1] = "3D View";
		options[2] = "3D Surface of revolution";
		options[3] = "Tensor Product Surfaces";

		// Initialize color (white by default)
		colorValue[0] = 1.0f; // R
		colorValue[1] = 1.0f; // G
		colorValue[2] = 1.0f; // B
	}

	virtual void render() override {
		// show fps
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		// Color selector
		ImGui::ColorEdit3("Select Background Color",
			colorValue); // RGB color selector
		ImGui::Text("Selected Color: R: %.3f, G: %.3f, B: %.3f", colorValue[0],
			colorValue[1], colorValue[2]);

		reset = ImGui::Button("Reset");

		// Combo box

		ImGui::Combo("View Mode", &comboSelection, options,
			IM_ARRAYSIZE(options));
		ImGui::Text("Selected: %s", options[comboSelection]);

		// check if the current mode is not 2d (i.e. it is in any 3d view mode)
		if (comboSelection != 0) {
			if (ImGui::Button(perspectiveMode ? "Switch to Orthographic" : "Switch to Perspective")) {
				perspectiveMode = !perspectiveMode;
			}
		}

		// check if the current mode is surface of revolution or tensor product
		if (comboSelection == 2 || comboSelection == 3) {
			// change between wireframe and solid mode
			if (ImGui::Button(solidMode ? "Switch to Wireframe Mode" : "Switch to solid mode")) {
				solidMode = !solidMode;
			}
		}

		// check if the current mode is the editor or regular 3d view
		if (comboSelection == 0 || comboSelection == 1) {
			// change curve modes
			if (ImGui::Button(bezierMode ? "Switch to B-Spline mode" : "Switch to Bezier mode")) {
				bezierMode = !bezierMode;
			}
		}

		// Float slider
		ImGui::SliderFloat("Float Slider", &pointSize, 5.0f, 100.0f,
			"Point Size: %.3f");

		ImGui::SliderInt("Curve Resolution", &curveRes, 0, 100, "Curve Resolution: %d");

		// Displaying current values
		ImGui::Text("Point Size: %.3f", pointSize);
	}

	glm::vec3 getColor() const {
		return glm::vec3(colorValue[0], colorValue[1], colorValue[2]);
	}

	float getPointSize() const { return pointSize; }
	int getCurveResolution() const { return curveRes; }
	bool isEditorMode() const { return comboSelection == 0; }
	bool isTensorMode() const { return comboSelection == 3; }
	ViewOption viewMode() const {
		switch (comboSelection) {
		case 0:
			return ViewOption::CurveEditor;
		case 1:
			return ViewOption::View;
		case 2:
			return ViewOption::SurfaceOfRevolution;
		case 3:
			return ViewOption::TensorSurface;
		default:
			return ViewOption::CurveEditor;
		}
	}
	bool isReset() const { return reset; }
	bool isBezierMode() const { return bezierMode; }
	bool isSolidMode() const { return solidMode; }
	bool isPerspectiveMode() const { return perspectiveMode; }

private:
	float colorValue[3];    // Array for RGB color values
	float pointSize;        // Value for float slider
	int curveRes; 			// number of points on the curve
	bool checkboxValue;     // Value for checkbox
	bool reset;			 	// resets the editor/view depending on the mode
	bool bezierMode;		// change between bezier and b-spline mode
	bool solidMode;			// change between wireframe and solid mode
	bool perspectiveMode; // change between perspective and orthographic mode
	int comboSelection;     // Index of selected option in combo box
	const char* options[4]; // Options for the combo box
};

CurveControl::CurveControl(Window& window)
	: mShader("shaders/test.vert", "shaders/test.frag"),
	mPanel(window.getGLFWwindow()), window(window) {
	camera = Camera(window.getWidth(), window.getHeight());
	curveGenerator = CurveGenerator();
	mCurveControls = std::make_shared<CurveEditorCallBack>();
	m3DCameraControls = std::make_shared<TurnTable3DViewerCallBack>();

	mPanelRenderer = std::make_shared<CurveEditorPanelRenderer>();

	// Set callback to window
	window.setCallbacks(mCurveControls);
	// Can swap the callback instead of maintaining a state machine
	// window.setCallbacks(turn_table_3D_viewer_callback);

	mPanel.setPanelRenderer(mPanelRenderer);

	mControlPointGeometry = GenerateInitialGeometry();
	mGPUGeometry.setVerts(mControlPointGeometry.verts);
	mGPUGeometry.setCols(mControlPointGeometry.cols);

	// generate the bezier curve
	curveGenerator.GenerateBezierCurve(mControlPointGeometry, mCurveGeometry, 1.0f / mPanelRenderer->getCurveResolution());

	// set the geometry for the bezier curve
	mCurveGPUGeometry.setVerts(mCurveGeometry.verts);
	mCurveGPUGeometry.setCols(mCurveGeometry.cols);

	// Setup OpenGL state that stays constant for most of the program
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}


void CurveControl::DrawGeometry() {
	glm::vec3 backgroundColor = mPanelRenderer->getColor();

	glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mShader.use();

	GLint editorLoc = glGetUniformLocation(mShader, "editor");
	glUniform1i(editorLoc, mPanelRenderer->isEditorMode());

	// set up the model, view, and projection matrices
	GLint modelLoc = glGetUniformLocation(mShader, "model");
	GLint viewLoc = glGetUniformLocation(mShader, "view");
	GLint projLoc = glGetUniformLocation(mShader, "proj");

	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(camera.getModel()));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera.getView()));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(camera.getProj()));


	// Render the control points
	ViewOption mode = mPanelRenderer->viewMode();
	if (mode == ViewOption::CurveEditor || mode == ViewOption::View) { 
		// render the control points
		glPointSize(mPanelRenderer->getPointSize());
		mGPUGeometry.bind();
		glDrawArrays(GL_POINTS, 0, (int)mControlPointGeometry.verts.size());

		// render the bezier/b-spline curve
		mCurveGPUGeometry.bind();
		glDrawArrays(GL_LINE_STRIP, 0, (int)mCurveGeometry.verts.size());
	}
	else if (mode == ViewOption::SurfaceOfRevolution) {
		// render the surface of revolution
		mRevolutionGPUGeometry.bind();
		glDrawArrays(GL_TRIANGLES, 0, (int)mRevolutionGeometry.verts.size());

		// set the polygon mode to wireframe or solid
		glPolygonMode(GL_FRONT_AND_BACK, mPanelRenderer->isSolidMode() ? GL_FILL : GL_LINE);

	}
	else { // mode is tensor product surface (NOT IMPLEMENTED)
		// 
	}

	// disable sRGB for things like imgui
	glDisable(GL_FRAMEBUFFER_SRGB);
	mPanel.render();
}

void CurveControl::DragPoint() {
	// calculate the world space position of where the mouse was clicked
	double xClick = (mCurveControls->xpos - 400.0) / 400.0;
	double yClick = (400.0 - mCurveControls->ypos) / 400.0;

	// update the position of the clicked control point
	mControlPointGeometry.verts[pointIndex] = { xClick, yClick, 0.f };
	mControlPointGeometry.cols[pointIndex] = { 0.f, 0.f, 1.f }; // coloring point blue just to make it easier to see
}

void CurveControl::CreatePoint() {
	// calculate the world space position of where the mouse was clicked
	double xClick = (mCurveControls->xpos - 400.0) / 400.0;
	double yClick = (400.0 - mCurveControls->ypos) / 400.0;

	// add a new control point at the clicked position
	mCurveControls->wasClicked = false;
	mControlPointGeometry.verts.emplace_back(xClick, yClick, 0.f);
	mControlPointGeometry.cols.emplace_back(0.f, 1.f, 0.f);
}

void CurveControl::DeletePoint() {
	if (mCurveControls->wasClicked) {
		// x and y position of where the mouse was clicked
		double xClick = (mCurveControls->xpos - 400.0) / 400.0;
		double yClick = (400.0 - mCurveControls->ypos) / 400.0;

		// size of points
		double pointSize = mPanelRenderer->getPointSize() / 1000.0;

		// check if the clicked position overlaps with any of the control points
		for (size_t i = 0; i < mControlPointGeometry.verts.size(); i++) {
			// calculate the distance between the clicked point and this point
			double distance = glm::distance(mControlPointGeometry.verts[i], glm::vec3(xClick, yClick, 0.f));
			// if the distance is less than the point size, point was clicked
			if (distance <= pointSize) {
				// remove this control point
				mControlPointGeometry.verts.erase(mControlPointGeometry.verts.begin() + i);
				mControlPointGeometry.cols.erase(mControlPointGeometry.cols.begin() + i);
				break;
			}
		}
	}
}

void CurveControl::ResetEditor() {
	mControlPointGeometry.verts.clear();
	mControlPointGeometry.cols.clear();
	mCurveGeometry.verts.clear();
	mCurveGeometry.cols.clear();
	mouseOnPoint = false;
	pointIndex = -1;
}

void CurveControl::UpdateEditorMode() {
	if (mPanelRenderer->isReset()) {
		ResetEditor();
	}
	else if (mCurveControls->deleteMode) {	// check if deleteMode is enabled by the user
		DeletePoint();
	}
	else if (mCurveControls->wasClicked) {
		// x and y positions of where the mouse was clicked
		double xClick = (mCurveControls->xpos - 400.0) / 400.0;
		double yClick = (400.0 - mCurveControls->ypos) / 400.0;

		// size of points
		double pointSize = mPanelRenderer->getPointSize() / 800.0;

		if (!mouseOnPoint) {
			// check if the clicked position overlaps with any of the control points
			for (size_t i = 0; i < mControlPointGeometry.verts.size(); i++) {
				// calculate the distance between the clicked point and this point
				double distance = glm::distance(mControlPointGeometry.verts[i], glm::vec3(xClick, yClick, 0.f));
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
		}
		else { // if no point is clicked, insert a new point at where the mouse was clicked
			CreatePoint();
		}
	}
	else {
		// if the mouse is released, reset the pointIndex and mouseOnPoint 
		if (pointIndex != -1) {
			mControlPointGeometry.cols[pointIndex] = { 0.f, 1.f, 0.f };
		}
		mouseOnPoint = false;
		pointIndex = -1;
	}
}

void CurveControl::UpdateViewMode() {
	if (mPanelRenderer->isReset()) {
		camera.Reset();
	}
	else if (m3DCameraControls->wasClicked) {
		// if the mouse is not being dragged, start dragging the camera
		if (!mouseDragging) {
			mouseDragging = true;
			// position of where the mouse was first clicked
			xStart = m3DCameraControls->xpos;
			yStart = m3DCameraControls->ypos;
		}

		// orbit the camera around the origin based on the dragged mouse movement
		if (mouseDragging) {
			float xEnd = m3DCameraControls->xpos;
			float yEnd = m3DCameraControls->ypos;

			// calculate the change in theta and phi based on the mouse movement
			float deltaTheta = xEnd - xStart;
			float deltaPhi = yEnd - yStart;

			camera.Move(deltaTheta, deltaPhi); // move the camera by theta and phi

			xStart = xEnd;
			yStart = yEnd;
		}
	}
	else if (m3DCameraControls->scrolling) {
		// zoom the camera based on the scroll wheel movement
		camera.Zoom(m3DCameraControls->yoffset);
		m3DCameraControls->scrolling = false;
	}
	else {
		mouseDragging = false;
	}
}

void CurveControl::Update() {
	// Use this function to process logic and update things based on user inputs
	//Example: generate a new control point

	// set the callbacks based on 2d or 3d mode
	if (mPanelRenderer->isEditorMode()) {
		window.setCallbacks(mCurveControls);
		UpdateEditorMode();
	}
	else {
		window.setCallbacks(m3DCameraControls);
		UpdateViewMode();
	}

	switch (mPanelRenderer->viewMode()) {
	case ViewOption::CurveEditor: // 2D editor
		// set geometry for the control points
		mGPUGeometry.setVerts(mControlPointGeometry.verts);
		mGPUGeometry.setCols(mControlPointGeometry.cols);

		// generate the bezier/b-spline curve
		mPanelRenderer->isBezierMode() ?
			curveGenerator.GenerateBezierCurve(mControlPointGeometry, mCurveGeometry, 1.0f / mPanelRenderer->getCurveResolution())
			: curveGenerator.GenerateBSplineCurve(mControlPointGeometry, mCurveGeometry);

		// set geometry for the bezier/b-spline curve
		mCurveGPUGeometry.setVerts(mCurveGeometry.verts);
		mCurveGPUGeometry.setCols(mCurveGeometry.cols);
		break;
	case ViewOption::View: // 3D view
		// set geometry for the control points
		mGPUGeometry.setVerts(mControlPointGeometry.verts);
		mGPUGeometry.setCols(mControlPointGeometry.cols);

		// generate the bezier/b-spline curve
		mPanelRenderer->isBezierMode() ?
			curveGenerator.GenerateBezierCurve(mControlPointGeometry, mCurveGeometry, 1.0f / mPanelRenderer->getCurveResolution())
			: curveGenerator.GenerateBSplineCurve(mControlPointGeometry, mCurveGeometry);

		// set geometry for the bezier/b-spline curve	
		mCurveGPUGeometry.setVerts(mCurveGeometry.verts);
		mCurveGPUGeometry.setCols(mCurveGeometry.cols);
		break;
	case ViewOption::SurfaceOfRevolution: // 3D surface of revolution
		// generate the b-spline curve
		curveGenerator.GenerateBSplineCurve(mControlPointGeometry, mCurveGeometry);

		// generate the surface of revolution
		curveGenerator.GenerateSurfaceOfRevolution(mCurveGeometry, mRevolutionGeometry, glm::two_pi<float>() / mPanelRenderer->getCurveResolution());

		// set geometry for the surface of revolution
		mRevolutionGPUGeometry.setVerts(mRevolutionGeometry.verts);
		mRevolutionGPUGeometry.setCols(mRevolutionGeometry.cols);

		// set the camera to perspective or orthographic mode
		if (mPanelRenderer->isPerspectiveMode() != perspectiveMode) {
			perspectiveMode = mPanelRenderer->isPerspectiveMode();
			camera.TogglePerspectiveMode();
		}

		break;
	case ViewOption::TensorSurface: // Tensor Product Surfaces (NOT IMPLEMENTED)
		break;
	}
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
