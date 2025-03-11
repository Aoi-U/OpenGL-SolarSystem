#pragma once

#include <memory>

#include "Geometry.h"
#include "Panel.h"
#include "ShaderProgram.h"
#include "Window.h"
#include "Camera.h"

// forward declare classes that only exist in the cpp file
class CurveEditorCallBack;
class TurnTable3DViewerCallBack;

class CurveEditorPanelRenderer;

enum class ViewOption { CurveEditor, SurfaceOfRevolution, TensorSurface };

class CurveControl {
public:
  explicit CurveControl(Window &window);

  void Update();
  void DrawGeometry();

private:
  // State variables
  ViewOption mCurrentViewOption = ViewOption::CurveEditor;
  std::shared_ptr<CurveEditorCallBack> mCurveControls;
  std::shared_ptr<TurnTable3DViewerCallBack> m3DCameraControls;
  ShaderProgram mShader;
  Panel mPanel;
  std::shared_ptr<CurveEditorPanelRenderer> mPanelRenderer;
  Window &window;
  Camera camera; // camera for 3D view
  size_t pointIndex = -1; // index of the control point that is being dragged
  bool mouseOnPoint = false; // tracks if the mouse is on a control point
  bool mouseDragging = false; // tracks if the mouse is being dragged
  float xStart, yStart; // tracks the current x and y position of the mouse
  float yOffsetStart = 0; // tracks the current y offset of the scroll wheel

  // Geometry
  CPU_Geometry mControlPointGeometry; // CPU geometry for control points
  CPU_Geometry mCurveGeometry; // CPU geometry for the bezier/b-spline curve
  GPU_Geometry mGPUGeometry; // GPU geometry for control points
  GPU_Geometry mPointGPUGeometry; // GPU geometry for lines connecting control points
  GPU_Geometry mCurveGPUGeometry; // GPU geometry for the bezier/b-spline curve

  // Private functions
  CPU_Geometry GenerateInitialGeometry();
  void GenerateBezierCurve(); // generates a bezier curve from user created control points
  void  GenerateBSplineCurve(); // generates a quadratic b-spline curve from user created control points
  void GenerateSurfaceOfRevolution(); // generates a surface of revolution from the control points
  void GenerateTensorProduct(); // generates a tensor product surface 
  glm::vec3 deCasteljau(std::vector<glm::vec3> points, size_t d, float u); // de Casteljau algorithm for bezier curve

  void DragPoint(); // drags a control point based on the mouse position
  void CreatePoint(); // creates a control point at the clicked position
  void DeletePoint(); // deletes a clicked control point
  void ResetEditor(); // resets the editor by clearing all control points
  void UpdateEditorMode(); // allow the user to edit/modify the curves 
  void UpdateViewMode(); // allow the user to view the curves in 3D
};
