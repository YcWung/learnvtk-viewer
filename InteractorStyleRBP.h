#pragma once

#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkObjectFactory.h>

class InteractorStyleRBP : public vtkInteractorStyleRubberBandPick {
public:
  static InteractorStyleRBP* New();
  vtkTypeMacro(InteractorStyleRBP, vtkInteractorStyleRubberBandPick);
  void SetSelectionMode(bool flag) {
    this->CurrentMode = flag ? 1 : 0;
  }
protected:
  InteractorStyleRBP() : vtkInteractorStyleRubberBandPick() {}
  ~InteractorStyleRBP() override {}
private:
  InteractorStyleRBP(const InteractorStyleRBP&) = delete;
  void operator=(const InteractorStyleRBP&) = delete;
};

vtkStandardNewMacro(InteractorStyleRBP);
