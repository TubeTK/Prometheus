/*=========================================================================

Library:   VesselView

Copyright 2010 Kitware Inc. 28 Corporate Drive,
Clifton Park, NY, 12065, USA.

All rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

=========================================================================*/

// Qt includes
#include <QDebug>

// SlicerQt includes
#include "qSlicerApplication.h"
#include "qSlicerInteractiveTubesToTreeModuleWidget.h"
#include "vtkSlicerInteractiveTubesToTreeLogic.h"
#include "ui_qSlicerInteractiveTubesToTreeModuleWidget.h"

// MRML includes
#include "vtkMRMLSpatialObjectsNode.h"
#include "vtkMRMLScene.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerInteractiveTubesToTreeModuleWidgetPrivate: public Ui_qSlicerInteractiveTubesToTreeModuleWidget
{

  Q_DECLARE_PUBLIC(qSlicerInteractiveTubesToTreeModuleWidget);

protected:
  qSlicerInteractiveTubesToTreeModuleWidget* const q_ptr;
public:
  qSlicerInteractiveTubesToTreeModuleWidgetPrivate(qSlicerInteractiveTubesToTreeModuleWidget& object);
  ~qSlicerInteractiveTubesToTreeModuleWidgetPrivate();
  vtkSlicerInteractiveTubesToTreeLogic* logic() const;

  void init();
  vtkMRMLSpatialObjectsNode* inputSpatialObject;
  vtkMRMLSpatialObjectsNode* outputSpatialObject;
};

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidgetPrivate::qSlicerInteractiveTubesToTreeModuleWidgetPrivate(
	qSlicerInteractiveTubesToTreeModuleWidget& object) : q_ptr(&object)
{
  this->inputSpatialObject = 0;
  this->outputSpatialObject = 0;
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidgetPrivate::~qSlicerInteractiveTubesToTreeModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
vtkSlicerInteractiveTubesToTreeLogic* qSlicerInteractiveTubesToTreeModuleWidgetPrivate::logic() const
{
	Q_Q(const qSlicerInteractiveTubesToTreeModuleWidget);
	return vtkSlicerInteractiveTubesToTreeLogic::SafeDownCast(q->logic());
}

//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidget::qSlicerInteractiveTubesToTreeModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerInteractiveTubesToTreeModuleWidgetPrivate(*this) )
{
}

//-----------------------------------------------------------------------------
// qSlicerInteractiveTubesToTreeModuleWidget methods
//-----------------------------------------------------------------------------
qSlicerInteractiveTubesToTreeModuleWidget::~qSlicerInteractiveTubesToTreeModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::setup()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);
  d->init();

  this->Superclass::setup();
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidgetPrivate::init()
{
  Q_Q(qSlicerInteractiveTubesToTreeModuleWidget);

  this->setupUi(q);

  QRegExp rx("[0-9]+([0-9]*[ ]*,[ ]*)*");
  QValidator *validator = new QRegExpValidator(rx);
  this->RootTubeIDListLineEdit->setValidator(validator);

  QObject::connect(
    this->InputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    q, SLOT(setInputSpatialObjectsNode(vtkMRMLNode*)));

  QObject::connect(
    this->OutputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    q, SLOT(setOutputSpatialObjectsNode(vtkMRMLNode*)));

  QObject::connect(
    this->RestoreDefaultsPushButton, SIGNAL(clicked()),
    q, SLOT(restoreDefaults()));

  QObject::connect(
    this->ApplyPushButton, SIGNAL(clicked()),
    q, SLOT(runConversion()));

  QObject::connect(
    this->InputSpacialObjectsNodeComboBox, SIGNAL(currentNodeChanged(vtkMRMLNode*)),
    this->Table, SLOT(setSpatialObjectsNode(vtkMRMLNode*)));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::enter()
{
  this->onEnter();
  this->Superclass::enter();
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::onEnter()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);
  if (this->mrmlScene() == 0)
  {
    return;
  }
  this->qvtkConnect(this->mrmlScene(), vtkMRMLScene::NodeAddedEvent,
   d->Table, SLOT(onNodeAddedEvent(vtkObject*, vtkObject*)));
  vtkMRMLNode* vtkMRMLMarkupFiducialNode = this->mrmlScene()->GetNthNodeByClass(0,"vtkMRMLMarkupsFiducialNode");
  if(vtkMRMLMarkupFiducialNode)
  {
    d->Table->onNodeAddedEvent(NULL, vtkMRMLMarkupFiducialNode);
  }
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget
::setInputSpatialObjectsNode(vtkMRMLNode* node)
{
  this->setInputSpatialObjectsNode(
    vtkMRMLSpatialObjectsNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget
::setInputSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node)
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  if (d->inputSpatialObject == node)
  {
    return;
  }
  d->inputSpatialObject = node;
  d->ApplyPushButton->setEnabled(d->inputSpatialObject != 0 && d->outputSpatialObject != 0);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::setOutputSpatialObjectsNode(vtkMRMLNode* node)
{
  this->setOutputSpatialObjectsNode(
    vtkMRMLSpatialObjectsNode::SafeDownCast(node));
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::setOutputSpatialObjectsNode(vtkMRMLSpatialObjectsNode* node)
{
	Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  if (d->outputSpatialObject == node)
  {
    return;
  }
  d->outputSpatialObject = node;
  d->ApplyPushButton->setEnabled(d->inputSpatialObject != 0 && d->outputSpatialObject != 0);
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::restoreDefaults()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  d->MaxContinuityAngleErrorSliderWidget->setValue(180);
  d->MaxTubeDistanceToRadiusSliderWidget->setValue(2);
  d->RemoveOrphanTubesCheckBox->setChecked(false);
  d->RootTubeIDListLineEdit->setText("");
  d->ApplyPushButton->setEnabled(d->inputSpatialObject != 0 && d->outputSpatialObject != 0);
  d->Table->restoreDefaults();
}

//------------------------------------------------------------------------------
void qSlicerInteractiveTubesToTreeModuleWidget::runConversion()
{
  Q_D(qSlicerInteractiveTubesToTreeModuleWidget);

  double maxTubeDistanceToRadiusRatio = d->MaxTubeDistanceToRadiusSliderWidget->value();
  double maxContinuityAngleError = d->MaxContinuityAngleErrorSliderWidget->value();
  bool removeOrphanTubes = d->RemoveOrphanTubesCheckBox->isChecked();
  const std::set<int> selectedTubeIds = d->inputSpatialObject->GetSelectedTubeIds();

  //getting root ids from the text box
  std::string rootTubeIdList = d->RootTubeIDListLineEdit->text().toStdString();
  if(rootTubeIdList.compare("") == 0 && selectedTubeIds.size() == 0)
  {
    return;
  }
  //getting root ids from the table
  std::set<int> copySelectedTubeIds = selectedTubeIds;
  std::string selectedRootIds = "";
  for (std::set<int>::iterator it=copySelectedTubeIds.begin(); it!=copySelectedTubeIds.end(); ++it)
  {
    selectedRootIds = std::to_string(*it) + " ," + selectedRootIds;
  }
  rootTubeIdList = rootTubeIdList + " ," + selectedRootIds;

  d->ApplyPushButton->setEnabled(false);
  d->inputSpatialObject->ClearSelectedTubes();

  if (!d->logic()->Apply(d->inputSpatialObject, d->outputSpatialObject, maxTubeDistanceToRadiusRatio,
    maxContinuityAngleError, removeOrphanTubes, rootTubeIdList))
  {
    qCritical("Error while running conversion !");
  }
  d->Table->buildTubeDisplayTable();
  for (std::set<int>::iterator it=copySelectedTubeIds.begin(); it!=copySelectedTubeIds.end(); ++it)
  {
    d->Table->SelectTube(*it);
  }
  d->ApplyPushButton->setChecked(false);
  d->ApplyPushButton->setEnabled(true);
}

// --------------------------------------------------------------------------
vtkMRMLSpatialObjectsNode* qSlicerInteractiveTubesToTreeModuleWidget::mrmlSpatialObjectNode()const
{
  Q_D(const qSlicerInteractiveTubesToTreeModuleWidget);
  return d->inputSpatialObject;
}