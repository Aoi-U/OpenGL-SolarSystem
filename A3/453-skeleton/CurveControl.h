#pragma once

#include <memory>

#include "Geometry.h"
#include "Panel.h"
#include "ShaderProgram.h"
#include "Window.h"
#include "Camera.h"
#include "CurveGenerator.h"

// forward declare classes that only exist in the cpp file
class CurveEditorCallBack;
class TurnTable3DViewerCallBack;

class CurveEditorPanelRenderer;

enum class ViewOption { CurveEditor, View, SurfaceOfRevolution, TensorSurface };

class CurveControl {
public:
	explicit CurveControl(Window& window);

	void Update();
	void DrawGeometry();

private:
	// State variables
	std::shared_ptr<CurveEditorCallBack> mCurveControls;
	std::shared_ptr<TurnTable3DViewerCallBack> m3DCameraControls;
	ShaderProgram mShader;
	Panel mPanel;
	std::shared_ptr<CurveEditorPanelRenderer> mPanelRenderer;
	Window& window;
	Camera camera; // camera for 3D view
	CurveGenerator curveGenerator;
	size_t pointIndex = -1; // index of the control point that is being dragged
	bool mouseOnPoint = false; // tracks if the mouse is on a control point
	bool mouseDragging = false; // tracks if the mouse is being dragged
	bool perspectiveMode = true; // perspective mode
	float xStart, yStart; // tracks the current x and y position of the mouse

	// Geometry
	CPU_Geometry mControlPointGeometry; // CPU geometry for control points
	CPU_Geometry mCurveGeometry; // CPU geometry for the bezier/b-spline curve
	CPU_Geometry mRevolutionGeometry; // CPU geometry for the surface of revolution

	GPU_Geometry mGPUGeometry; // GPU geometry for control points
	GPU_Geometry mCurveGPUGeometry; // GPU geometry for the bezier/b-spline curve
	GPU_Geometry mRevolutionGPUGeometry; // GPU geometry for the surface of revolution

	// Private functions
	CPU_Geometry GenerateInitialGeometry();

	void DragPoint(); // drags a control point based on the mouse position
	void CreatePoint(); // creates a control point at the clicked position
	void DeletePoint(); // deletes a clicked control point
	void ResetEditor(); // resets the editor by clearing all control points
	void UpdateEditorMode(); // allow the user to edit/modify the curves 
	void UpdateViewMode(); // allow the user to view the curves in 3D
};
