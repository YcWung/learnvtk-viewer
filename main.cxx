/*=========================================================================

  Program:   Visualization Toolkit
  Module:    Cone6.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
//
// This example introduces 3D widgets. 3D widgets take advantage of the
// event/observer design pattern introduced previously. They typically
// have a particular representation in the scene which can be interactively
// selected and manipulated using the mouse and keyboard. As the widgets
// are manipulated, they in turn invoke events such as StartInteractionEvent,
// InteractionEvent, and EndInteractionEvent which can be used to manipulate
// the scene that the widget is embedded in. 3D widgets work in the context
// of the event loop which was set up in the previous example.
//
// Note: there are more 3D widget examples in VTK/Examples/GUI/.
//

// First include the required header files for the VTK classes we are using.
#include "vtkActor.h"
#include "vtkBoxWidget.h"
#include "vtkCamera.h"
#include "vtkCommand.h"
#include "vtkConeSource.h"
#include "vtkInteractorStyleTrackballCamera.h"
#include "vtkPolyDataMapper.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"
#include "vtkTransform.h"
#include "viewer.h"

//
// Similar to Cone2.cxx, we define a callback for interaction.
//
class vtkMyCallback : public vtkCommand
{
public:
  static vtkMyCallback* New() { return new vtkMyCallback; }
  void Execute(vtkObject* caller, unsigned long, void*) override
  {
    vtkTransform* t = vtkTransform::New();
    vtkBoxWidget* widget = reinterpret_cast<vtkBoxWidget*>(caller);
    widget->GetTransform(t);
    //widget->GetProp3D()->SetUserTransform(t);
    widget->GetProp3D()->GetUserTransform()->DeepCopy(t);
    t->Delete();
  }
};

int main()
{
  //
  // Next we create an instance of vtkConeSource and set some of its
  // properties. The instance of vtkConeSource "cone" is part of a
  // visualization pipeline (it is a source process object); it produces data
  // (output type is vtkPolyData) which other filters may process.
  //
  vtkConeSource* cone = vtkConeSource::New();
  cone->SetHeight(3.0);
  cone->SetRadius(1.0);
  cone->SetResolution(10);

  //
  // In this example we terminate the pipeline with a mapper process object.
  // (Intermediate filters such as vtkShrinkPolyData could be inserted in
  // between the source and the mapper.)  We create an instance of
  // vtkPolyDataMapper to map the polygonal data into graphics primitives. We
  // connect the output of the cone source to the input of this mapper.
  //
  PointCloudViewer viewer;
  //viewer.mapper()->AddInputConnection(cone->GetOutputPort());
  //vtkPolyDataMapper* coneMapper = viewer.mapper();
  cone->Update();
  viewer.SetData(cone->GetOutput());
  

  vtkBoxWidget* boxWidget = vtkBoxWidget::New();
  boxWidget->SetInteractor(viewer.interactor());
  boxWidget->SetPlaceFactor(1.25);
  boxWidget->SetProp3D(viewer.actor());
  boxWidget->PlaceWidget();
  vtkMyCallback* callback = vtkMyCallback::New();
  boxWidget->AddObserver(vtkCommand::InteractionEvent, callback);

  //
  // Normally the user presses the "i" key to bring a 3D widget to life. Here
  // we will manually enable it so it appears with the cone.
  //
  // boxWidget->On();

  //
  // Start the event loop.
  //
  viewer.Start();

  return 0;
}
