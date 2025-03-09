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
  Camera camera;
  size_t pointIndex = -1;
  bool mouseOnPoint = false;
  bool mouseDragging = false;
  float xStart, yStart;
  float yOffsetStart = 0;
  int moveSpeed = 1;
  

  // Geometry
  CPU_Geometry mCurveGeometry;
  GPU_Geometry mGPUGeometry;
  GPU_Geometry mPointGPUGeometry;

  // Private functions
  CPU_Geometry GenerateInitialGeometry();

  void DragPoint();
  void CreatePoint();
  void DeletePoint();
  void ResetPanel();
  void UpdateEditorMode(); // allow the user to edit/modify the curves 
  void UpdateViewMode(); // allow the user to view the curves from different angles
};
