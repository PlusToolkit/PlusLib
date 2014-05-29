import os
from __main__ import vtk, qt, ctk, slicer

#
# FieldDistortionMapping
#

class FieldDistortionMapping:
  def __init__(self, parent):
    parent.title = "Field distortion mapping"
    parent.categories = ["IGT"]
    parent.dependencies = []
    parent.contributors = ["Andras Lasso (PerkLab, Queen's)"]
    parent.helpText = "This is a simple example of using two trackers for field distortion mapping"
    parent.acknowledgementText = ""
    self.parent = parent
    
    # Create the logic to start processing Leap messages on Slicer startup
    logic = FieldDistortionMappingLogic()

#
# qFieldDistortionMappingWidget
#

class FieldDistortionMappingWidget:
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

    self.logic = FieldDistortionMappingLogic()
      
  def setup(self):
    # Instantiate and connect widgets ...

    #
    # Reload and Test area
    #
    reloadCollapsibleButton = ctk.ctkCollapsibleButton()
    reloadCollapsibleButton.text = "Reload && Test"
    self.layout.addWidget(reloadCollapsibleButton)
    reloadFormLayout = qt.QFormLayout(reloadCollapsibleButton)

    # reload button
    # (use this during development, but remove it when delivering
    #  your module to users)
    self.reloadButton = qt.QPushButton("Reload")
    self.reloadButton.toolTip = "Reload this module."
    self.reloadButton.name = "FieldDistortionMapping Reload"
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
    self.outputVolumeSelectorLabel = qt.QLabel()
    self.outputVolumeSelectorLabel.setText( "Output vector volume: " )
    self.outputVolumeSelector = slicer.qMRMLNodeComboBox()
    #self.outputVolumeSelector.nodeTypes = ( "vtkMRMLVectorVolumeNode", "" )
    self.outputVolumeSelector.nodeTypes = ( "vtkMRMLScalarVolumeNode", "" )
    self.outputVolumeSelector.noneEnabled = False
    self.outputVolumeSelector.addEnabled = True
    self.outputVolumeSelector.removeEnabled = True
    self.outputVolumeSelector.setMRMLScene( slicer.mrmlScene )
    self.outputVolumeSelector.setToolTip( "Pick the vector volume to fill" )
    parametersFormLayout.addRow(self.outputVolumeSelectorLabel, self.outputVolumeSelector)

    # output grid transform selector
    self.outputTransformSelectorLabel = qt.QLabel()
    self.outputTransformSelectorLabel.setText( "Output grid transform: " )
    self.outputTransformSelector = slicer.qMRMLNodeComboBox()
    self.outputTransformSelector.nodeTypes = ( "vtkMRMLTransformNode", "" )
    self.outputTransformSelector.noneEnabled = False
    self.outputTransformSelector.addEnabled = True
    self.outputTransformSelector.removeEnabled = True
    self.outputTransformSelector.setMRMLScene( slicer.mrmlScene )
    self.outputTransformSelector.setToolTip( "Pick the grid transform node to fill. The extents, origin, and spacing will be copied from the output volume." )
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

    # Add vertical spacer
    self.layout.addStretch(1)
    
  def cleanup(self):
    pass

  def setEnableTransformMapping(self, enable):
    if enable:
      self.logic.startTransformMapping(self.groundTruthTransformSelector.currentNode(), self.mappedTransformSelector.currentNode(), self.outputVolumeSelector.currentNode(), self.outputTransformSelector.currentNode())
    else:
      self.logic.stopTransformMapping()
  
  def onReload(self,moduleName="FieldDistortionMapping"):
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

  def onReloadAndTest(self,moduleName="FieldDistortionMapping"):
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
# FieldDistortionMappingLogic
#

class FieldDistortionMappingLogic:
  """This class implements all the actual computation in the module.
  """
  
  def __init__(self):  
    self.groundTruthTransformNode = None
    self.groundTruthTransformNodeObserverTag = None
    self.mappedTransformNode = None
    self.mappedTransformNodeObserverTag = None    
    self.outputVolumeNode = None
    self.previousMappedPosition = [0,0,0]

  def addObservers(self):
    transformModifiedEvent = 15000
    self.groundTruthTransformNodeObserverTag = self.groundTruthTransformNode.AddObserver(transformModifiedEvent, self.onGroundTruthTransformNodeModified)


  def removeObservers(self):
    if self.groundTruthTransformNode and self.groundTruthTransformNodeObserverTag:
      self.groundTruthTransformNode.RemoveObserver(self.groundTruthTransformNodeObserverTag)
    if self.mappedTransformNode and self.mappedTransformNodeObserverTag:
      self.mappedTransformNode.RemoveObserver(self.mappedTransformNodeObserverTag)
      
  def startTransformMapping(self, groundTruthTransformNode, mappedTransformNode, outputVolumeNode, outputTransformNode):
    self.removeObservers()
    self.groundTruthTransformNode=groundTruthTransformNode
    self.mappedTransformNode=mappedTransformNode
    self.outputVolumeNode=outputVolumeNode
    self.outputTransformNode=outputTransformNode

    # Get the output volume's RAS to IJK matrix
    ijkToRas = vtk.vtkMatrix4x4()
    outputVolumeNode.GetIJKToRASMatrix(ijkToRas)
    transformNode = self.outputVolumeNode.GetParentTransformNode()
    if transformNode:
      if transformNode.IsTransformToWorldLinear():
        rasToRAS = vtk.vtkMatrix4x4()
        transformNode.GetMatrixTransformToWorld(rasToRAS)
        rasToRAS.Multiply4x4(rasToRAS, ijkToRas, ijkToRas)
      else:
        print ("Cannot handle non-linear transforms - skipping")
    self.rasToIjk = vtk.vtkMatrix4x4()
    vtk.vtkMatrix4x4.Invert(ijkToRas, self.rasToIjk)

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
    self.mappedTransformNode.GetMatrixTransformToParent(mappedTransformMatrix)    
    mappedPos = [mappedTransformMatrix.GetElement(0,3), mappedTransformMatrix.GetElement(1,3), mappedTransformMatrix.GetElement(2,3)]
      
    # return if did not move enough compared to the previous sampling position
    minimumSamplingDistance = 15
    if vtk.vtkMath.Distance2BetweenPoints(self.previousMappedPosition,mappedPos) < minimumSamplingDistance*minimumSamplingDistance:
      return
    
    self.previousMappedPosition = mappedPos

    # Compute distortion vector   
    groundTruthTransformMatrix = vtk.vtkMatrix4x4()
    self.groundTruthTransformNode.GetMatrixTransformToParent(groundTruthTransformMatrix)
    gtPos = [groundTruthTransformMatrix.GetElement(0,3), groundTruthTransformMatrix.GetElement(1,3), groundTruthTransformMatrix.GetElement(2,3)]
    distortionVectorR = mappedPos[0]-gtPos[0]
    distortionVectorA = mappedPos[1]-gtPos[1]
    distortionVectorS = mappedPos[2]-gtPos[2]

    # Compute voxel position
    distortionVectorPosition_Ras = [gtPos[0], gtPos[1], gtPos[2], 1]
    distortionVectorPosition_Ijk=self.rasToIjk.MultiplyPoint(distortionVectorPosition_Ras)
    #distortionVectorPosition_Ijk = [0,1,2,3]

    # Paint voxel value
    outputVolumeImageData=self.outputVolumeNode.GetImageData()
    fillValue=1000
    outputVolumeImageData.SetScalarComponentFromFloat(distortionVectorPosition_Ijk[0], distortionVectorPosition_Ijk[1], distortionVectorPosition_Ijk[2], 0, fillValue)
    outputVolumeImageData.Modified()
    
    # Update transform
    outputTransform=self.outputTransformNode.GetTransformToParent()
    outputTransform.GetSourceLandmarks().InsertNextPoint(gtPos[0], gtPos[1], gtPos[2])
    outputTransform.GetTargetLandmarks().InsertNextPoint(mappedPos[0],mappedPos[1],mappedPos[2])
    self.outputTransformNode.GetTransformToParent().Modified()


