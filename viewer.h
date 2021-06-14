#pragma once

#include <vtkActor.h>
#include <vtkBoxWidget.h>
#include <vtkCamera.h>
#include <vtkCommand.h>
#include <vtkConeSource.h>
#include <vtkInteractorStyleRubberBandPick.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSmartPointerBase.h>
#include <vtkTransform.h>
#include <vtkRenderedAreaPicker.h>
#include <vtkCallbackCommand.h>
#include <vtkExtractSelectedFrustum.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkHardwareSelector.h>
#include <vtkSelection.h>
#include <vtkSelectionNode.h>
#include <vtkExtractSelectedPolyDataIds.h>
#include <vtkDataSetAttributes.h>
#include <vtkNamedColors.h>
#include <vtkProperty.h>
#include <vtkIdtypeArray.h>
#include "InteractorStyleRBP.h"

vtkIdTypeArray *IdArray(vtkSelection *sel);
void AddSelection(vtkSelection *total, vtkSelection *cur);
void AddSelection(vtkIdTypeArray *old, vtkIdTypeArray *cur,
                  vtkIdTypeArray *out);
void SubtractSelection(vtkIdTypeArray *total, vtkIdTypeArray *cur);
void SubtractSelection(vtkSelection *total, vtkSelection *cur);

class PointCloudViewer {
public:
  PointCloudViewer(vtkRenderWindowInteractor *iren, vtkRenderWindow *rwin)
      : iren_(iren), iren_style_rb_pick_(InteractorStyleRBP::New()),
        area_picker_(vtkRenderedAreaPicker::New()),
        frustum_extractor_(vtkExtractSelectedFrustum::New()),
        end_pick_cbc_(vtkCallbackCommand::New()), rwin_(rwin),
        renderer_(vtkRenderer::New()), actor_(vtkActor::New()),
        mapper_(vtkPolyDataMapper::New()), actor_sel_(vtkActor::New()),
        mapper_sel_(vtkPolyDataMapper::New()),
        area_selection_(vtkSelection::New()) {
    iren_->SetInteractorStyle(iren_style_rb_pick_);
    iren_->SetPicker(area_picker_);
    rwin_->AddRenderer(renderer_);
    renderer_->AddActor(actor_);
    actor_->SetMapper(mapper_);

    // selection actor and picker callback
    renderer_->AddActor(actor_sel_);
    actor_sel_->SetMapper(mapper_sel_);
    actor_sel_->GetProperty()->SetColor(colors_->GetColor4d("red").GetData());
    actor_sel_->SetPickable(false);
    vtkNew<vtkTransform> t;
    t->Identity();
    actor_sel_->SetUserTransform(t);
    actor_->SetUserTransform(t);
    end_pick_cbc_->SetClientData(this);
    end_pick_cbc_->SetCallback(&EndPickCallback);
    iren_->AddObserver(vtkCommand::EndPickEvent, end_pick_cbc_);

    // selection member variable 
    vtkNew<vtkIdTypeArray> sel_ids;
    sel_ids->SetName("SelectedIds");
    sel_ids->SetNumberOfComponents(1);
    //for (vtkIdType i = 0; i < 10; ++i) sel_ids->InsertNextValue(i);
    vtkNew<vtkSelectionNode> sel_node;
    sel_node->SetFieldType(vtkSelectionNode::CELL);
    sel_node->SetContentType(vtkSelectionNode::INDICES);
    sel_node->GetSelectionData()->AddArray(sel_ids);
    area_selection_->AddNode(sel_node);
  }
  PointCloudViewer()
      : PointCloudViewer(vtkRenderWindowInteractor::New(),
                         vtkRenderWindow::New()) {
    iren_->SetRenderWindow(rwin_);
  }
  PointCloudViewer(vtkRenderWindow *rwin)
      : PointCloudViewer(rwin->GetInteractor() == nullptr ?
                         vtkRenderWindowInteractor::New() :
                         rwin->GetInteractor(), rwin) {
    if (rwin->GetInteractor() == nullptr) {
      iren_->SetRenderWindow(rwin_);
    }
  }

  //vtkPolyDataMapper* mapper() { return mapper_; }
  InteractorStyleRBP* interactor_style() { return iren_style_rb_pick_; }
  vtkRenderWindowInteractor* interactor() { return iren_; }
  vtkRenderWindow* render_window() { return rwin_; }
  vtkRenderer* renderer() { return renderer_; }
  vtkActor* actor() { return actor_; }
  vtkPolyData* data() { return poly_data_; }
  void SetData(vtkPolyData* data) {
    poly_data_ = data;
    mapper_->AddInputDataObject(poly_data_);
  }
  void Start() {
    iren_->Initialize();
    iren_->Start();
  }
  static void EndPickCallback(vtkObject* caller, unsigned long event_id, void* client_data, void* call_data) {
    auto self = reinterpret_cast<PointCloudViewer*>(client_data);
    vtkNew<vtkHardwareSelector> selr;
    selr->SetRenderer(self->renderer_);
    selr->SetArea(self->renderer_->GetPickX1(), self->renderer_->GetPickY1(), self->renderer_->GetPickX2(), self->renderer_->GetPickY2());
    vtkSelection* sel = selr->Select();
    if (self->iren_->GetControlKey()) {
      AddSelection(self->area_selection_, sel);
    }  else if (self->iren_->GetShiftKey()) {
      SubtractSelection(self->area_selection_, sel);
    } else {
      if (sel->GetNumberOfNodes() > 0)
        self->area_selection_ = sel;
    }
    
    vtkIdTypeArray* cellids = IdArray(self->area_selection_);
    std::cout << "cellids = " << cellids << std::endl;
    for (int i = 0; i < cellids->GetNumberOfTuples(); ++i) {
      std::cout << " " << cellids->GetTuple(i)[0];
    }
    std::cout << std::endl;
    
    vtkNew<vtkExtractSelectedPolyDataIds> extr;
    extr->SetInputData(0, self->poly_data_);
    extr->SetInputData(1, self->area_selection_);
    self->mapper_sel_->SetInputConnection(extr->GetOutputPort());
    extr->Update();
  }
  
 protected:
  vtkSmartPointer<vtkRenderWindowInteractor> iren_;
  vtkSmartPointer<InteractorStyleRBP> iren_style_rb_pick_;
  vtkSmartPointer<vtkRenderedAreaPicker> area_picker_;
  vtkSmartPointer<vtkExtractSelectedFrustum> frustum_extractor_;
  vtkSmartPointer<vtkCallbackCommand> end_pick_cbc_;
  vtkSmartPointer<vtkRenderWindow> rwin_;
  vtkSmartPointer<vtkRenderer> renderer_;
  vtkSmartPointer<vtkActor> actor_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_;
  vtkSmartPointer<vtkPolyData> poly_data_;
  vtkSmartPointer<vtkActor> actor_sel_;
  vtkSmartPointer<vtkPolyDataMapper> mapper_sel_;
  vtkSmartPointer<vtkSelection> area_selection_;
  vtkNew<vtkNamedColors> colors_;
};

inline vtkIdTypeArray *IdArray(vtkSelection *sel) {
  return vtkIdTypeArray::SafeDownCast(
      sel->GetNode(0)->GetSelectionData()->GetArray("SelectedIds"));
}

inline void AddSelection(vtkSelection *total, vtkSelection *cur) {
  if (cur->GetNumberOfNodes() == 0 || IdArray(cur) == nullptr) { return; }
  vtkNew<vtkIdTypeArray> new_ids;
  new_ids->SetName("SelectedIds");
  AddSelection(IdArray(total), IdArray(cur), new_ids);
  total->GetNode(0)->GetSelectionData()->AddArray(new_ids);
}

inline void SubtractSelection(vtkSelection *total, vtkSelection *cur) {
  if (cur->GetNumberOfNodes() == 0 || IdArray(cur) == nullptr) {
    return;
  }
  SubtractSelection(IdArray(total), IdArray(cur));
}

inline void AddSelection(vtkIdTypeArray *old, vtkIdTypeArray *cur,
                         vtkIdTypeArray *out) {
  vtkIdType N1 = old->GetNumberOfTuples();
  vtkIdType N2 = cur->GetNumberOfTuples();
  out->SetNumberOfComponents(1);
  out->SetNumberOfTuples(N1 + N2);
  const vtkIdType *buf1 = old->GetPointer(0);
  const vtkIdType *buf2 = cur->GetPointer(0);
  vtkIdType *buf_out = out->GetPointer(0);
  vtkIdType Num = 0;
  vtkIdType i1 = 0, i2 = 0;
  while (i1 < N1 || i2 < N2) {
    if (i1 == N1) {
      buf_out[Num++] = buf2[i2++];
    } else if (i2 == N2) {
      buf_out[Num++] = buf1[i1++];
    } else if (buf1[i1] == buf2[i2]) {
      buf_out[Num++] = buf1[i1];
      ++i1;
      ++i2;
    } else if (buf1[i1] < buf2[i2]) {
      buf_out[Num++] = buf1[i1++];
    } else {
      buf_out[Num++] = buf2[i2++];
    }
  }
  out->Resize(Num);
}

inline void SubtractSelection(vtkIdTypeArray *total, vtkIdTypeArray *cur) {
  vtkIdType N1 = total->GetNumberOfTuples();
  vtkIdType N2 = cur->GetNumberOfTuples();
  vtkIdType *buf1 = total->GetPointer(0);
  const vtkIdType *buf2 = cur->GetPointer(0);
  vtkIdType Num = 0;
  vtkIdType i2 = 0;
  for (vtkIdType i1 = 0; i1 < N1; ++i1) {
    if (i2 >= N2 || buf1[i1] < buf2[i2]) {
      buf1[Num++] = buf1[i1];
    }
    while(i2 < N2 && buf2[i2] <= buf1[i1]) { ++i2; }
  }
  total->Resize(Num);
}
