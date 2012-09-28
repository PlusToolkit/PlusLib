Attribute VB_Name = "Declarations"
Option Explicit
' The following functions are available to the user for use in controlling and
' interrogating US Digital SEI devices.

' What happens is a server program, SEISrv32.exe is used to communicate with
' the SEI bus.  This server also communicates to SEIDrv32.dll functions.
' The purpose of all this is that rather then having only a dll, and allowing
' only one application to poll the SEI bus, this arrangement allows multiple
' applications to poll the SEI device, through the SEI Server program.
' Most of the time, you will only have one application polling the SEI bus,
' but the capability for simultaneous multiple apps exists.

'---------------------------------- General Functions ------------------------------------
Public Declare Function ShowServer Lib "SEIDrv32.DLL" (ByVal show As Long) As Long
Public Declare Function GetDLLVersionText Lib "SEIDrv32.DLL" (ByVal msg As String) As Long
Public Declare Function GetLastErrorText Lib "SEIDrv32.DLL" (ByVal msg As String) As Long

'---------------------------------- SEI Functions ------------------------------------
Public Declare Function InitializeSEI Lib "SEIDrv32.DLL" (ByVal Commport As Long, ByVal Mode As Long) As Long
'Used to initialize the SEI system.  Commport= the com port number of the SEI bus.
'The comport can be 0, in which case, it will poll all com ports.
'The mode is defined in the constants section below.

Public Declare Function IsInitialized Lib "SEIDrv32.DLL" () As Long
Public Declare Function GetCommPort Lib "SEIDrv32.DLL" () As Long
Public Declare Function IsAnalog Lib "SEIDrv32.DLL" () As Long
Public Declare Function GetNumberOfDevices Lib "SEIDrv32.DLL" () As Long
Public Declare Function GetDeviceInfo Lib "SEIDrv32.DLL" (ByVal DevNum As Long, Model As Long, SN As Long, Ver As Long, addr As Long) As Long
Public Declare Function GetAllDeviceInfo Lib "SEIDrv32.DLL" (ByVal DevNum As Long, Model As Long, SN As Long, Ver As Long, addr As Long, MMonth As Long, MDay As Long, MYear As Long, MConf As Long) As Long
Public Declare Function SetDeviceAddress Lib "SEIDrv32.DLL" (ByVal SerialNum As Long, ByVal Address As Long) As Long
Public Declare Function SetBaudRate Lib "SEIDrv32.DLL" (ByVal Baudrate As Long) As Long
Public Declare Function ResetSEI Lib "SEIDrv32.DLL" () As Long
Public Declare Function GetBusyStatus Lib "SEIDrv32.DLL" () As Long
Public Declare Function CloseSEI Lib "SEIDrv32.DLL" () As Long

'----------------------------------- ED2 Functions ------------------------------------
Public Declare Function ED2SetOrigin Lib "SEIDrv32.DLL" (ByVal Address As Long) As Long
Public Declare Function ED2GetMode Lib "SEIDrv32.DLL" (ByVal Address As Long, Mode As Long) As Long
Public Declare Function ED2SetMode Lib "SEIDrv32.DLL" (ByVal Address As Long, ByVal Mode As Long) As Long
Public Declare Function ED2GetResolution Lib "SEIDrv32.DLL" (ByVal Address As Long, Res As Long) As Long
Public Declare Function ED2SetResolution Lib "SEIDrv32.DLL" (ByVal Address As Long, ByVal Res As Long) As Long
Public Declare Function ED2GetPosition1 Lib "SEIDrv32.DLL" (ByVal Address As Long, Pos As Single) As Long
Public Declare Function ED2GetPosition2 Lib "SEIDrv32.DLL" (ByVal Address As Long, Pos As Single) As Long
Public Declare Function ED2SetPosition1 Lib "SEIDrv32.DLL" (ByVal Address As Long, ByVal Pos As Single) As Long
Public Declare Function ED2SetPosition2 Lib "SEIDrv32.DLL" (ByVal Address As Long, ByVal Pos As Single) As Long
Public Declare Function ED2GetInput Lib "SEIDrv32.DLL" (ByVal Address As Long, ED2input As Long) As Long
Public Declare Function ED2SetOutput Lib "SEIDrv32.DLL" (ByVal Address As Long, ByVal ED2Output As Long) As Long

#If Win32 Then
    Declare Function SetWindowPos Lib "user32.dll" _
    (ByVal hWnd As Long, ByVal hWndInsertAfter As Long, _
    ByVal X As Long, ByVal Y As Long, ByVal cx As Long, _
    ByVal cy As Long, ByVal wFlags As Long) As Long
#Else 'Win16
     Declare Sub SetWindowPos Lib "User.dll" (ByVal hWnd As Integer, _
     ByVal hWndInsertAfter As Integer, ByVal X As Integer, _
     ByVal Y As Integer, ByVal cx As Integer, ByVal cy As Integer, _
     ByVal wFlags As Integer)
#End If
    
'------------ Constant Variables ---------
Public Const AUTOASSIGN As Long = &H1       ' autoassign addresses if they conflict
Public Const SKIPDEVICESEARCH As Long = &H2 ' don't check if devices exist on chosen comm port
Public Const SKIPDEVICEINFO As Long = &H4   ' don't retrieve devices info
Public Const NORESET As Long = &H8          ' don't generate reset to SEI devices
Public Const NOCHECKSUM As Long = &H10      ' don't check checksum of commands
Public Const REINITIALIZE As Long = &H20    ' reinitialize if already initialized

'------------ SEI Variables --------------
Global Baudrate As Long         ' Baudrate of SEI bus.
Global Devices As Long          ' Number of devices.
Global SerialNum(15) As Long    ' Serial number of devices.
Global Address(15) As Long      ' Address of devices.
Global Model(15) As Long        ' Model of devices.
Global Positions(15) As Long    ' Number of Positions reported back by that device
Global Product(15) As String    ' SEI product name.
Global Ver As Long

'------------- ED2 Variables --------------
Global ED2Res(15) As Long        ' ED2 resolution.
Global ED2Mode(15) As Long       ' ED2 mode.
Global ED2Sleeping As Long       ' ED2 Sleep mode.
Global ED2Output(15) As Long     ' ED2 outputs.

'--------------- Variables ---------------
Global Radius As Long           ' Radius of the circle.
Global ErrMsg As String * 512   ' Error information message.

Global frmFactoryInfoExists As Boolean  ' So user can't open more than one form

Global InitMode As Byte         ' Value to pass into Mode Form
Global FinalMode As Byte        ' Value Mode Form passes back
Global GoodMode As Boolean      ' User hit OK, so use FinalMode
Global frmModeExists As Boolean ' Make program wait while user picks a mode
Global frmDigitalExists As Boolean ' Make program wait while user changes dig output
Global ModeCaption As String * 30 ' Header of Mode Form

Global DeviceLastError As Long
Public Function CheckIfError(ByVal Errcode As Long) As Boolean
    
    If Errcode <> 0 Then
        Call GetLastErrorText(ErrMsg)
        MsgBox ErrMsg, vbExclamation, "Error Code " & Errcode
        CheckIfError = True
    Else
        CheckIfError = False
    End If
    
End Function


Public Function ProductType(ProductModel As Long) As String
' This function maps US Digital Product Model Numbers into US Digital Product Names
' It will be modified as new products are added
Select Case ProductModel
        Case 1
            ProductType = "M1"
        Case 2
            ProductType = "A2"
        Case 3
            ProductType = "ED2"
        Case 4
            ProductType = "A3"
        Case 5
            ProductType = "AD5"
        Case Else
            ProductType = "Undefined"
        End Select
End Function

Public Function BBClr(ByVal bitnum As Byte, ByVal bytevalue As Byte) As Byte
' This function clears bit number bitnum in bytevalue and returns it
  
  BBClr = bytevalue And Not (2 ^ bitnum)
    
End Function

Public Function BBSet(ByVal bitnum As Byte, ByVal bytevalue As Byte) As Byte
' This function sets bit number bitnum in bytevalue and returns it
  
  BBSet = bytevalue Or 2 ^ bitnum
  
End Function
Public Function BBTst(ByVal bitnum As Byte, ByVal bytevalue As Byte) As Boolean
' This function tests bit number bitnum in bytevalue and returns True if it's 1 and False if it's 0
  
  If bytevalue = BBClr(bitnum, bytevalue) Then BBTst = False
  If bytevalue = BBSet(bitnum, bytevalue) Then BBTst = True
  
End Function

Function SafeVal(ByRef InputString As Variant, ByRef OutputNum As Variant) As Variant
' Tom Carlson, 4/9/97
' This routine is used to convert a string to a number
' I use it instead of Val, because I can make this do all my
' error checking, and so I won't forget to do it, or do it incorrectly somehow
' It returns the string which was passed to it, or if it fails, it returns an
' empty string
' use it by placing the following style line in the change procedure of
' a text box, in this example, called MTxt.  MTxt = SafeVal(MTxt,MONTH)

  Dim Result As Boolean
  Result = False

  If VarType(OutputNum) = vbByte Then
    If Val(InputString) <= 255 And Val(InputString) >= 0 Then
      OutputNum = Val(InputString)
      Result = True
    End If
  End If

  If VarType(OutputNum) = vbInteger Then
    If Val(InputString) <= 32767 And Val(InputString) >= -32768 Then
      OutputNum = Val(InputString)
      Result = True
    End If
  End If

  If VarType(OutputNum) = vbLong Then
    If Val(InputString) <= 2147483647 And Val(InputString) >= -2147483648# Then
      OutputNum = Val(InputString)
      Result = True
    End If
  End If
    
  If VarType(OutputNum) = vbSingle Then
    If Val(InputString) <= 3.402823E+38 And Val(InputString) >= -3.402823E+38 Then
      OutputNum = Val(InputString)
      Result = True
    End If
  End If
  
  If Not Result Then
    SafeVal = ""
  Else
    SafeVal = InputString
  End If
  
  
End Function

Public Sub Trace(Optional msg As String = "", Optional moduleAddr As Long = -1)
    On Error GoTo errHandler
    Dim itm     As ListItem
    Dim vData   As Variant
    Dim I       As Integer
    
    Static sLastMsg As String
    Static lLastModAddr As Long
    
    If msg = "" Then
        GetLastErrorText ErrMsg
        msg = ErrMsg
        If InStr(msg, Chr(0)) = 1 Then msg = Mid(msg, 2)
        If InStr(msg, Chr(0)) > 1 Then msg = Mid(msg, 1, InStr(msg, Chr(0)) - 1)
    End If
    
    If msg = "Running" Then
        frmED2Main.StatusBar1.Panels(1).Text = msg
        Exit Sub
    End If

    ' If this is a repeat message, then increment the message counter and exit the sub.
    If sLastMsg = msg And lLastModAddr = moduleAddr Then
        frmActivityLog.lvLog.ListItems.Item(frmActivityLog.lvLog.ListItems.Count).SubItems(3) = frmActivityLog.lvLog.ListItems.Item(frmActivityLog.lvLog.ListItems.Count).SubItems(3) + 1
        Exit Sub
    End If
    
    sLastMsg = msg
    lLastModAddr = moduleAddr
    vData = Split(msg, vbLf)
    
    For I = 0 To UBound(vData)
        If Trim(vData(I)) <> "" Then
            Set itm = frmActivityLog.lvLog.ListItems.Add(, , vData(I))
            frmED2Main.StatusBar1.Panels(1).Text = vData(I)
            itm.Selected = True
            itm.SubItems(1) = IIf(moduleAddr < 0, "", moduleAddr)
            itm.SubItems(2) = Now
            itm.SubItems(3) = 1
            itm.EnsureVisible
        End If
    Next I
    
    While frmActivityLog.lvLog.ListItems.Count > 2000
        frmActivityLog.lvLog.ListItems.Remove 1
    Wend
    Exit Sub
errHandler:
    Err.Clear
End Sub

