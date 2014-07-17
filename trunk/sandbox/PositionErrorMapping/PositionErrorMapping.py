import os, math
from __main__ import vtk, qt, ctk, slicer

#
# PositionErrorMapping
#

class PositionErrorMapping:
  def __init__(self, parent):
    parent.title = "Position tracking error mapping"
    parent.categories = ["IGT"]
    parent.dependencies = []
    parent.contributors = ["Andras Lasso (PerkLab, Queen's)"]
    parent.helpText = "This is a simple example of using two trackers for mapping of the position tracking error. One of the trackers is used as ground truth and the other one is compared to that."
    parent.acknowledgementText = ""
    self.parent = parent

#
# qPositionErrorMappingWidget
#

class PositionErrorMappingWidget:
  def __init__(self, parent = None):
    if not parent:
      self.parent = slicer.qMRMLWidget()
      self.parent.setLayout(qt.QVBoxLayout())
      self.parent.setMRMLScene(slicer.mrmlScene)
    else:
      self.parent = parent
    self.layout = self.parent.layout()
    if not parent:
      self.setup()
      self.parent.show()

    self.logic = PositionErrorMappingLogic()
      
  def setup(self):
    # Instantiate and connect widgets ...

    #
    # Reload and Test area
    #
    reloadCollapsibleButton = ctk.ctkCollapsibleButton()
    reloadCollapsibleButton.collapsed = True
    reloadCollapsibleButton.text = "Reload && Test"
    self.layout.addWidget(reloadCollapsibleButton)
    reloadFormLayout = qt.QFormLayout(reloadCollapsibleButton)

    # reload button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadButton = qt.QPushButton("Reload")
    self.reloadButton.toolTip = "Reload this module."
    self.reloadButton.name = "PositionErrorMapping Reload"
    reloadFormLayout.addWidget(self.reloadButton)
    self.reloadButton.connect('clicked()', self.onReload)

    # reload and test button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadAndTestButton = qt.QPushButton("Reload and Test")
    self.reloadAndTestButton.toolTip = "Reload this module and then run the self tests."
    reloadFormLayout.addWidget(self.reloadAndTestButton)
    self.reloadAndTestButton.connect('clicked()', self.onReloadAndTest)

    #
    # Parameters Area
    #
    parametersCollapsibleButton = ctk.ctkCollapsibleButton()
    parametersCollapsibleButton.text = "Parameters"
    self.layout.addWidget(parametersCollapsibleButton)
    parametersFormLayout = qt.QFormLayout(parametersCollapsibleButton)

    # ground truth transform selector
    self.groundTruthTransformSelectorLabel = qt.QLabel()
    self.groundTruthTransformSelectorLabel.setText( "Ground truth transform: " )
    self.groundTruthTransformSelector = slicer.qMRMLNodeComboBox()
    self.groundTruthTransformSelector.nodeTypes = ( "vtkMRMLLinearTransformNode", "" )
    self.groundTruthTransformSelector.noneEnabled = False
    self.groundTruthTransformSelector.addEnabled = True
    self.groundTruthTransformSelector.removeEnabled = True
    self.groundTruthTransformSelector.setMRMLScene( slicer.mrmlScene )
    self.groundTruthTransformSelector.setToolTip( "Pick the input ground truth transform (e.g., optical tracker)" )
    parametersFormLayout.addRow(self.groundTruthTransformSelectorLabel, self.groundTruthTransformSelector)     
    
    # mapped transform selector
    self.mappedTransformSelectorLabel = qt.QLabel()
    self.mappedTransformSelectorLabel.setText( "Mapped transform: " )
    self.mappedTransformSelector = slicer.qMRMLNodeComboBox()
    self.mappedTransformSelector.nodeTypes = ( "vtkMRMLLinearTransformNode", "" )
    self.mappedTransformSelector.noneEnabled = False
    self.mappedTransformSelector.addEnabled = True
    self.mappedTransformSelector.removeEnabled = True
    self.mappedTransformSelector.setMRMLScene( slicer.mrmlScene )
    self.mappedTransformSelector.setToolTip( "Pick the input transform to be mapped compared to the ground truth (e.g., electromagnetic tracker)" )
    parametersFormLayout.addRow(self.mappedTransformSelectorLabel, self.mappedTransformSelector)     
    
    # output volume selector
    self.outputVisitedPointsModelSelectorLabel = qt.QLabel()
    self.outputVisitedPointsModelSelectorLabel.setText( "Output visited points model: " )
    self.outputVisitedPointsModelSelector = slicer.qMRMLNodeComboBox()
    self.outputVisitedPointsModelSelector.nodeTypes = ( "vtkMRMLModelNode", "" )
    self.outputVisitedPointsModelSelector.noneEnabled = True
    self.outputVisitedPointsModelSelector.addEnabled = True
    self.outputVisitedPointsModelSelector.removeEnabled = True
    self.outputVisitedPointsModelSelector.setMRMLScene( slicer.mrmlScene )
    self.outputVisitedPointsModelSelector.setToolTip( "A glyph is added to the model at each measurement point. Optional." )
    parametersFormLayout.addRow(self.outputVisitedPointsModelSelectorLabel, self.outputVisitedPointsModelSelector)

    # output grid transform selector
    self.outputTransformSelectorLabel = qt.QLabel()
    self.outputTransformSelectorLabel.setText( "Output transform: " )
    self.outputTransformSelector = slicer.qMRMLNodeComboBox()
    self.outputTransformSelector.nodeTypes = ( "vtkMRMLTransformNode", "" )
    self.outputTransformSelector.noneEnabled = False
    self.outputTransformSelector.addEnabled = True
    self.outputTransformSelector.removeEnabled = True
    self.outputTransformSelector.setMRMLScene( slicer.mrmlScene )
    self.outputTransformSelector.setToolTip( "The transform node will store and interpolate the measurement points to generate a vector field of the position error of the mapped transform compared to the ground truth transform. Optional." )
    parametersFormLayout.addRow(self.outputTransformSelectorLabel, self.outputTransformSelector)

    #
    # Check box to enable creating output transforms automatically.
    # The function is useful for testing and initial creation of the transforms but not recommended when the
    # transforms are already in the scene.
    #
    self.enableTransformMappingCheckBox = qt.QCheckBox()
    self.enableTransformMappingCheckBox.checked = 0
    self.enableTransformMappingCheckBox.setToolTip("If checked, then the mapped transform difference compared to the ground truth is written into the volume.")
    parametersFormLayout.addRow("Enable mapping", self.enableTransformMappingCheckBox)
    self.enableTransformMappingCheckBox.connect('stateChanged(int)', self.setEnableTransformMapping)

    #
    # Export Area
    #
    exportCollapsibleButton = ctk.ctkCollapsibleButton()
    exportCollapsibleButton.text = "Export"
    self.layout.addWidget(exportCollapsibleButton)
    exportFormLayout = qt.QFormLayout(exportCollapsibleButton)

    # ROI selector
    self.exportRoiSelectorLabel = qt.QLabel()
    self.exportRoiSelectorLabel.setText( "Region of interest: " )
    self.exportRoiSelector = slicer.qMRMLNodeComboBox()
    self.exportRoiSelector.nodeTypes = ( "vtkMRMLAnnotationROINode", "" )
    self.exportRoiSelector.noneEnabled = False
    self.exportRoiSelector.addEnabled = False
    self.exportRoiSelector.removeEnabled = True
    self.exportRoiSelector.setMRMLScene( slicer.mrmlScene )
    self.exportRoiSelector.setToolTip( "Pick the input region of interest for export" )
    exportFormLayout.addRow(self.exportRoiSelectorLabel, self.exportRoiSelector)     

    # Export button
    self.exportButton = qt.QPushButton("Export")
    self.exportButton.toolTip = "Export the transform in the selected region of interest to a vector volume"
    self.exportButton.enabled = True
    exportFormLayout.addRow(self.exportButton)
    self.exportButton.connect('clicked(bool)', self.onExport)

    # Add vertical spacer
    self.layout.addStretch(1)
    
  def cleanup(self):
    pass

  def setEnableTransformMapping(self, enable):
    if enable:
      self.logic.startTransformMapping(self.groundTruthTransformSelector.currentNode(), self.mappedTransformSelector.currentNode(), self.outputVisitedPointsModelSelector.currentNode(), self.outputTransformSelector.currentNode())
    else:
      self.logic.stopTransformMapping()

  def onExport(self, clicked):
    self.logic.exportTransformToVectorVolume(self.outputTransformSelector.currentNode(), self.exportRoiSelector.currentNode())    
  
  def onReload(self,moduleName="PositionErrorMapping"):
    """Generic reload method for any scripted module.
    ModuleWizard will subsitute correct default moduleName.
    """
    import imp, sys, os, slicer

    widgetName = moduleName + "Widget"

    # reload the source code
    # - set source file path
    # - load the module to the global space
    filePath = eval('slicer.modules.%s.path' % moduleName.lower())
    p = os.path.dirname(filePath)
    if not sys.path.__contains__(p):
      sys.path.insert(0,p)
    fp = open(filePath, "r")
    globals()[moduleName] = imp.load_module(
        moduleName, fp, filePath, ('.py', 'r', imp.PY_SOURCE))
    fp.close()

    # rebuild the widget
    # - find and hide the existing widget
    # - create a new widget in the existing parent
    parent = slicer.util.findChildren(name='%s Reload' % moduleName)[0].parent().parent()
    for child in parent.children():
      try:
        child.hide()
      except AttributeError:
        pass
    # Remove spacer items
    item = parent.layout().itemAt(0)
    while item:
      parent.layout().removeItem(item)
      item = parent.layout().itemAt(0)

    # delete the old widget instance
    if hasattr(globals()['slicer'].modules, widgetName):
      getattr(globals()['slicer'].modules, widgetName).cleanup()

    # create new widget inside existing parent
    globals()[widgetName.lower()] = eval(
        'globals()["%s"].%s(parent)' % (moduleName, widgetName))
    globals()[widgetName.lower()].setup()
    setattr(globals()['slicer'].modules, widgetName, globals()[widgetName.lower()])

  def onReloadAndTest(self,moduleName="PositionErrorMapping"):
    try:
      self.onReload()
      evalString = 'globals()["%s"].%sTest()' % (moduleName, moduleName)
      tester = eval(evalString)
      tester.runTest()
    except Exception, e:
      import traceback
      traceback.print_exc()
      qt.QMessageBox.warning(slicer.util.mainWindow(), 
          "Reload and Test", 'Exception!\n\n' + str(e) + "\n\nSee Python Console for Stack Trace")


#
# PositionErrorMappingLogic
#

class PositionErrorMappingLogic:
  """This class implements all the actual computation in the module.
  """
  
  def __init__(self):  
    self.groundTruthTransformNode = None
    self.transformNodeObserverTags = []
    self.mappedTransformNode = None
    self.outputVisitedPointsModelNode = None
    self.previousMappedPosition = [0,0,0]
    
    # no sample is collected if the pointer moves less than this distance
    # from the previous sample
    self.minimumSamplingDistance = 10
    
    # spacing of the exported volume
    self.exportVolumeSpacingMm = 3.0

  def addObservers(self):
    transformModifiedEvent = 15000
    transformNode = self.groundTruthTransformNode
    while transformNode:
      print "Add observer to {0}".format(transformNode.GetName())
      self.transformNodeObserverTags.append([transformNode, transformNode.AddObserver(transformModifiedEvent, self.onGroundTruthTransformNodeModified)])
      transformNode = transformNode.GetParentTransformNode()

  def removeObservers(self):
    print "Remove observers"
    for nodeTagPair in self.transformNodeObserverTags:
      nodeTagPair[0].RemoveObserver(nodeTagPair[1])
      
  def startTransformMapping(self, groundTruthTransformNode, mappedTransformNode, outputVisitedPointsModelNode, outputTransformNode):
    self.removeObservers()
    self.groundTruthTransformNode=groundTruthTransformNode
    self.mappedTransformNode=mappedTransformNode    
    self.outputVisitedPointsModelNode=outputVisitedPointsModelNode
    self.outputTransformNode=outputTransformNode
    
    if self.outputVisitedPointsModelNode:
      # Create model display node if does not exist yet
      if not self.outputVisitedPointsModelNode.GetDisplayNode():
        modelDisplay = slicer.vtkMRMLModelDisplayNode()
        modelDisplay.SetEdgeVisibility(True)
        slicer.mrmlScene.AddNode(modelDisplay)
        self.outputVisitedPointsModelNode.SetAndObserveDisplayNodeID(modelDisplay.GetID())
      # Set up glyph filter
      self.visitedPoints = vtk.vtkPoints()
      self.visitedPointsPolydata = vtk.vtkPolyData()
      self.visitedPointsPolydata.SetPoints(self.visitedPoints)      
      glyph = vtk.vtkPolyData()
      cubeSource = vtk.vtkCubeSource()
      cubeSource.SetXLength(self.minimumSamplingDistance)
      cubeSource.SetYLength(self.minimumSamplingDistance)
      cubeSource.SetZLength(self.minimumSamplingDistance)
      self.visitedPointsGlyph3d = vtk.vtkGlyph3D()
      self.visitedPointsGlyph3d.SetSourceConnection(cubeSource.GetOutputPort())
      self.visitedPointsGlyph3d.SetInputData(self.visitedPointsPolydata)
      self.visitedPointsGlyph3d.Update()
      self.outputVisitedPointsModelNode.SetPolyDataConnection(self.visitedPointsGlyph3d.GetOutputPort())

    # Initialize the output transform
    if self.outputTransformNode:
      alwaysClearOutputTransformOnStart = True
      outputTransform=self.outputTransformNode.GetTransformToParentAs('vtkThinPlateSplineTransform', False)
      if alwaysClearOutputTransformOnStart or not outputTransform:
        outputTransform=vtk.vtkThinPlateSplineTransform()
        outputTransform.SetBasisToR()
        groundTruthPoints=vtk.vtkPoints()
        mappedPoints=vtk.vtkPoints()
        outputTransform.SetSourceLandmarks(groundTruthPoints)
        outputTransform.SetTargetLandmarks(mappedPoints)
        self.outputTransformNode.SetAndObserveTransformToParent(outputTransform)      
        
    # Start the updates
    self.addObservers()
    self.onGroundTruthTransformNodeModified(0,0)
    
  def stopTransformMapping(self):
    self.removeObservers()
    
  def onGroundTruthTransformNodeModified(self, observer, eventid):

    mappedTransformMatrix = vtk.vtkMatrix4x4()
    self.mappedTransformNode.GetMatrixTransformToWorld(mappedTransformMatrix)    
    mappedPos = [mappedTransformMatrix.GetElement(0,3), mappedTransformMatrix.GetElement(1,3), mappedTransformMatrix.GetElement(2,3)]
      
    # return if did not move enough compared to the previous sampling position    
    if vtk.vtkMath.Distance2BetweenPoints(self.previousMappedPosition,mappedPos) < self.minimumSamplingDistance*self.minimumSamplingDistance:
      return
    
    self.previousMappedPosition = mappedPos

    # Compute distortion vector   
    groundTruthTransformMatrix = vtk.vtkMatrix4x4()
    self.groundTruthTransformNode.GetMatrixTransformToWorld(groundTruthTransformMatrix)
    gtPos = [groundTruthTransformMatrix.GetElement(0,3), groundTruthTransformMatrix.GetElement(1,3), groundTruthTransformMatrix.GetElement(2,3)]
    distortionVectorR = mappedPos[0]-gtPos[0]
    distortionVectorA = mappedPos[1]-gtPos[1]
    distortionVectorS = mappedPos[2]-gtPos[2]

    # Compute voxel position
    distortionVectorPosition_Ras = [gtPos[0], gtPos[1], gtPos[2], 1]

    # Paint voxel value   
    if self.outputVisitedPointsModelNode:
      self.visitedPoints.InsertNextPoint(distortionVectorPosition_Ras[0:3])
      self.visitedPoints.Modified()
    
    # Update transform
    if self.outputTransformNode:
      outputTransform=self.outputTransformNode.GetTransformToParent()
      outputTransform.GetSourceLandmarks().InsertNextPoint(gtPos[0], gtPos[1], gtPos[2])
      outputTransform.GetTargetLandmarks().InsertNextPoint(mappedPos[0],mappedPos[1],mappedPos[2])
      self.outputTransformNode.GetTransformToParent().Modified()

  def exportTransformToVectorVolume(self, outputTransform, exportRoi):
    roiBounds_Ras = [0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
    exportRoi.GetRASBounds(roiBounds_Ras)
    exportVolumeSize = [(roiBounds_Ras[1]-roiBounds_Ras[0]+1)/self.exportVolumeSpacingMm, (roiBounds_Ras[3]-roiBounds_Ras[2]+1)/self.exportVolumeSpacingMm, (roiBounds_Ras[5]-roiBounds_Ras[4]+1)/self.exportVolumeSpacingMm]
    exportVolumeSize = [int(math.ceil(x)) for x in exportVolumeSize]

    exportImageData = vtk.vtkImageData()
    exportImageData.SetExtent(0, exportVolumeSize[0]-1, 0, exportVolumeSize[1]-1, 0, exportVolumeSize[2]-1)
    if vtk.VTK_MAJOR_VERSION <= 5:
      exportImageData.SetScalarType(vtk.VTK_DOUBLE)
      exportImageData.SetNumberOfScalarComponents(3)
      exportImageData.AllocateScalars()
    else:
      exportImageData.AllocateScalars(vtk.VTK_DOUBLE, 3)

    exportVolume = slicer.vtkMRMLVectorVolumeNode()
    exportVolume.SetAndObserveImageData(exportImageData)
    exportVolume.SetSpacing(self.exportVolumeSpacingMm, self.exportVolumeSpacingMm, self.exportVolumeSpacingMm)
    exportVolume.SetOrigin(roiBounds_Ras[0], roiBounds_Ras[2], roiBounds_Ras[4])

    slicer.modules.transforms.logic().CreateDisplacementVolumeFromTransform(outputTransform, exportVolume, False)
