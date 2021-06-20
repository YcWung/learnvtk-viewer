#pragma once

#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkObjectFactory.h>
#include <vtkPointPicker.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkNew.h>
#include <vtkCamera.h>
#include <vtkTransform.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>

class InteractorStyleRBP : public vtkInteractorStyleRubberBandPick {
public:
  static InteractorStyleRBP* New();
  vtkTypeMacro(InteractorStyleRBP, vtkInteractorStyleRubberBandPick);
  void SetSelectionMode(bool flag) {
    this->CurrentMode = flag ? 1 : 0;
  }
  void SetPickFocalPoint(bool flag) {
    this->pick_focal_point_flag = flag;
  }
  void OnLeftButtonUp() override {
    if (pick_focal_point_flag) {
      PickFocalPoint();
      pick_focal_point_flag = false;
    } 
    this->Superclass::OnLeftButtonUp();
  }
protected:
  InteractorStyleRBP() : vtkInteractorStyleRubberBandPick(), pick_focal_point_flag(false) {}
  ~InteractorStyleRBP() override {}
  bool pick_focal_point_flag;
  void PickFocalPoint() {
    vtkNew<vtkPointPicker> picker;
    int pt[2];
    for (int i = 0; i < 2; ++i) pt[i] = this->Interactor->GetEventPosition()[i];
    picker->Pick(pt[0], pt[1], 0, this->CurrentRenderer);
    double pick_pt[3];
    picker->GetPickPosition(pick_pt);
    std::cout << "picked point id: " << picker->GetPointId() << std::endl;
    std::cout << "picked position: " << pick_pt[0] << " " << pick_pt[1] << " " << pick_pt[2] << std::endl;
    vtkCamera* cam = this->CurrentRenderer->GetActiveCamera();
    vtkNew<vtkTransform> t;
    t->Identity();
    double old_fp[3];
    cam->GetFocalPoint(old_fp);
    double t_fp[3];
    for (int i = 0; i < 3; ++i)
      t_fp[i] = pick_pt[i] - old_fp[i];
    t->Translate(t_fp);
    cam->ApplyTransform(t);
    this->Interactor->GetRenderWindow()->Render();
  }
private:
  InteractorStyleRBP(const InteractorStyleRBP&) = delete;
  void operator=(const InteractorStyleRBP&) = delete;
};

vtkStandardNewMacro(InteractorStyleRBP);
