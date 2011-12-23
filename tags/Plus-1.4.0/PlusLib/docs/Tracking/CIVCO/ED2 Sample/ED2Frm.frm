VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmED2Main 
   Caption         =   "ED2 Sample"
   ClientHeight    =   5130
   ClientLeft      =   4995
   ClientTop       =   2700
   ClientWidth     =   5670
   FillColor       =   &H00E0E0E0&
   ForeColor       =   &H00E0E0E0&
   Icon            =   "ED2Frm.frx":0000
   LinkTopic       =   "Form1"
   NegotiateMenus  =   0   'False
   ScaleHeight     =   5130
   ScaleWidth      =   5670
   Begin VB.Timer tmrMain 
      Enabled         =   0   'False
      Interval        =   100
      Left            =   3660
      Top             =   3660
   End
   Begin VB.PictureBox picPosition 
      Height          =   375
      Index           =   2
      Left            =   240
      ScaleHeight     =   315
      ScaleWidth      =   2535
      TabIndex        =   23
      Top             =   1620
      Width           =   2595
      Begin VB.Label lblPosition 
         Alignment       =   2  'Center
         Caption         =   "Position"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   210
         Index           =   2
         Left            =   870
         TabIndex        =   24
         Top             =   60
         Width           =   855
      End
   End
   Begin VB.PictureBox picPosition 
      Height          =   375
      Index           =   1
      Left            =   300
      ScaleHeight     =   315
      ScaleWidth      =   2535
      TabIndex        =   21
      Top             =   1140
      Width           =   2595
      Begin VB.Label lblPosition 
         Alignment       =   2  'Center
         Caption         =   "Position"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   210
         Index           =   1
         Left            =   840
         TabIndex        =   22
         Top             =   60
         Width           =   855
      End
   End
   Begin VB.PictureBox picDigital 
      Height          =   975
      Index           =   1
      Left            =   180
      ScaleHeight     =   915
      ScaleWidth      =   2655
      TabIndex        =   4
      Top             =   3180
      Width           =   2715
      Begin VB.PictureBox picOutputBit 
         Height          =   255
         Index           =   7
         Left            =   2220
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   20
         ToolTipText     =   "Bit 7, OR this value with Back Switch"
         Top             =   60
         Width           =   255
      End
      Begin VB.PictureBox picOutputBit 
         Height          =   255
         Index           =   6
         Left            =   1860
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   19
         ToolTipText     =   "Bit 6, OR this value with TTL Input #4"
         Top             =   60
         Width           =   255
      End
      Begin VB.PictureBox picOutputBit 
         Height          =   255
         Index           =   5
         Left            =   1500
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   18
         ToolTipText     =   "Bit 5, Control TTL Output #3"
         Top             =   60
         Width           =   255
      End
      Begin VB.PictureBox picOutputBit 
         Height          =   255
         Index           =   4
         Left            =   1200
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   17
         ToolTipText     =   "Bit 4, Control TTL Output #2"
         Top             =   60
         Width           =   255
      End
      Begin VB.PictureBox picOutputBit 
         Height          =   255
         Index           =   3
         Left            =   900
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   16
         ToolTipText     =   "Bit 3, Control TTL Output #1"
         Top             =   60
         Width           =   255
      End
      Begin VB.PictureBox picOutputBit 
         Height          =   255
         Index           =   2
         Left            =   600
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   15
         ToolTipText     =   "Bit 2, ""Push""  PB #3"
         Top             =   60
         Width           =   255
      End
      Begin VB.PictureBox picOutputBit 
         Height          =   255
         Index           =   1
         Left            =   300
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   14
         ToolTipText     =   "Bit 1, ""Push""  PB #2"
         Top             =   60
         Width           =   255
      End
      Begin VB.PictureBox picOutputBit 
         Height          =   255
         Index           =   0
         Left            =   0
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   13
         ToolTipText     =   "Bit 0, ""Push""  PB #1"
         Top             =   60
         Width           =   255
      End
   End
   Begin VB.PictureBox picDigital 
      Height          =   375
      Index           =   0
      Left            =   180
      ScaleHeight     =   315
      ScaleWidth      =   2535
      TabIndex        =   2
      Top             =   2640
      Width           =   2595
      Begin VB.PictureBox picInputBit 
         Height          =   255
         Index           =   7
         Left            =   2280
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   12
         ToolTipText     =   "Bit 7, Back Switch State"
         Top             =   0
         Width           =   255
      End
      Begin VB.PictureBox picInputBit 
         Height          =   255
         Index           =   6
         Left            =   1860
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   11
         ToolTipText     =   "Bit 6, TTL Input #4 State"
         Top             =   0
         Width           =   255
      End
      Begin VB.PictureBox picInputBit 
         Height          =   255
         Index           =   5
         Left            =   1560
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   10
         ToolTipText     =   "Bit 5, TTL Input #3 State"
         Top             =   0
         Width           =   255
      End
      Begin VB.PictureBox picInputBit 
         Height          =   255
         Index           =   4
         Left            =   1200
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   9
         ToolTipText     =   "Bit 4, TTL Input #2 State"
         Top             =   0
         Width           =   255
      End
      Begin VB.PictureBox picInputBit 
         Height          =   255
         Index           =   3
         Left            =   900
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   8
         ToolTipText     =   "Bit 3, TTL Input #1 State"
         Top             =   0
         Width           =   255
      End
      Begin VB.PictureBox picInputBit 
         Height          =   255
         Index           =   2
         Left            =   600
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   7
         ToolTipText     =   "Bit 2, Push Button #3 State"
         Top             =   0
         Width           =   255
      End
      Begin VB.PictureBox picInputBit 
         Height          =   255
         Index           =   1
         Left            =   360
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   6
         ToolTipText     =   "Bit 1, Push Button #2 State"
         Top             =   0
         Width           =   255
      End
      Begin VB.PictureBox picInputBit 
         Height          =   255
         Index           =   0
         Left            =   60
         ScaleHeight     =   195
         ScaleWidth      =   195
         TabIndex        =   5
         ToolTipText     =   "Bit 0, Push Button #1 State"
         Top             =   0
         Width           =   255
      End
   End
   Begin VB.PictureBox picResolution 
      Height          =   375
      Left            =   180
      ScaleHeight     =   315
      ScaleWidth      =   2535
      TabIndex        =   1
      Top             =   2100
      Width           =   2595
      Begin VB.Label lblResolution 
         Alignment       =   2  'Center
         Caption         =   "Resolution"
         BeginProperty Font 
            Name            =   "Arial"
            Size            =   8.25
            Charset         =   0
            Weight          =   400
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   210
         Left            =   915
         TabIndex        =   3
         Top             =   60
         Width           =   945
      End
   End
   Begin VB.PictureBox picCircle 
      BackColor       =   &H00C0C0C0&
      FillColor       =   &H0080C0FF&
      FillStyle       =   0  'Solid
      Height          =   1635
      Left            =   3120
      ScaleHeight     =   1575
      ScaleWidth      =   2115
      TabIndex        =   0
      Top             =   1440
      Width           =   2175
      Begin VB.Label lblErrors 
         Alignment       =   2  'Center
         Appearance      =   0  'Flat
         BackColor       =   &H000000FF&
         BorderStyle     =   1  'Fixed Single
         Caption         =   "View Errors"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H8000000E&
         Height          =   255
         Left            =   0
         TabIndex        =   26
         Top             =   0
         Visible         =   0   'False
         Width           =   1155
      End
      Begin VB.Shape shpCircle 
         BackColor       =   &H00808080&
         BorderColor     =   &H00E0E0E0&
         BorderWidth     =   2
         FillColor       =   &H00808080&
         Height          =   1035
         Left            =   900
         Shape           =   3  'Circle
         Top             =   180
         Width           =   1155
      End
      Begin VB.Shape shpCircleShadow 
         BackColor       =   &H00808080&
         BorderColor     =   &H00808080&
         BorderWidth     =   2
         FillColor       =   &H00808080&
         Height          =   1035
         Left            =   60
         Shape           =   3  'Circle
         Top             =   180
         Width           =   1155
      End
   End
   Begin MSComctlLib.StatusBar StatusBar1 
      Align           =   2  'Align Bottom
      Height          =   345
      Left            =   0
      TabIndex        =   25
      Top             =   4785
      Width           =   5670
      _ExtentX        =   10001
      _ExtentY        =   609
      _Version        =   393216
      BeginProperty Panels {8E3867A5-8586-11D1-B16A-00C0F0283628} 
         NumPanels       =   1
         BeginProperty Panel1 {8E3867AB-8586-11D1-B16A-00C0F0283628} 
            AutoSize        =   1
            Object.Width           =   9499
         EndProperty
      EndProperty
      BeginProperty Font {0BE35203-8F91-11CE-9DE3-00AA004BB851} 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
   End
   Begin VB.Line ReferenceLine 
      Visible         =   0   'False
      X1              =   15120
      X2              =   7020
      Y1              =   60
      Y2              =   60
   End
   Begin VB.Menu mnuFile 
      Caption         =   "File"
      Begin VB.Menu mnuFileExit 
         Caption         =   "E&xit"
      End
   End
   Begin VB.Menu mnuLocal 
      Caption         =   "&Commands"
      Begin VB.Menu mnuLocalED2SetMode 
         Caption         =   "Set Mode"
      End
      Begin VB.Menu mnuLocalED2SetOrigin 
         Caption         =   "Set Origin"
      End
      Begin VB.Menu mnuLocalED2SetPos1 
         Caption         =   "Set Absolute Position #1"
      End
      Begin VB.Menu mnuLocalED2SetPos2 
         Caption         =   "Set Absolute Position #2"
      End
      Begin VB.Menu mnuLocalED2SetRes 
         Caption         =   "Set Resolution"
      End
      Begin VB.Menu mnuLocalSpace 
         Caption         =   "-"
      End
      Begin VB.Menu mnuLocalSEIChangeAdd 
         Caption         =   "Change Address"
      End
      Begin VB.Menu mnuLocalSEISetBaudRate 
         Caption         =   "Set Baud Rate"
      End
      Begin VB.Menu mnuLocalSEIResetSEI 
         Caption         =   "Reset Devices"
      End
      Begin VB.Menu mnuLocalGetFactoryInfo 
         Caption         =   "Get Factory Info"
      End
      Begin VB.Menu mnuLocalSpace2 
         Caption         =   "-"
      End
      Begin VB.Menu mnuLocalGetLastError 
         Caption         =   "Get Last Error"
      End
      Begin VB.Menu mnuLocalRestartDevice 
         Caption         =   "Restart Device"
         Visible         =   0   'False
      End
   End
   Begin VB.Menu mnuCommunication 
      Caption         =   "Communication"
      Begin VB.Menu mnuCommunicaitonSetCOMPort 
         Caption         =   "Set COM Port..."
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "&Help"
      Begin VB.Menu mnuHelpAbout 
         Caption         =   "About"
      End
   End
End
Attribute VB_Name = "frmED2Main"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Const MyErrLimit = 5
Private MyErrCount As Integer
Private m_COMPort As Byte

Dim Radius As Integer
Dim CenterX As Integer
Dim CenterY As Integer
Dim MyAngle(1 To 2) As Double
Dim MyPosition(1 To 2) As Single
Dim iOldPosition(1 To 2) As Single  'Variable to check if position changed
Dim lOldLength(1 To 2) As Long  'Stores length of position label
Dim bDeviceFailed As Boolean    'Stores if this device failed which stops the polling
Dim Errcode As Variant
Dim iFontSize As Byte           'Temporary variable
Dim I As Integer
Dim Answer As String

Public MyAddress As Long
Public MyMode As Long
Public MyResolution As Long
Public MySN As Long

Public MyOutputs As Byte
Public MyInputs As Long
Public MyIndex As Long          'The array number of this form

Private Sub Form_Unload(Cancel As Integer)
    If frmModeExists Then Unload frmED2Mode
    If frmFactoryInfoExists Then Unload frmFactoryInfo
    If frmDigitalExists Then Unload frmDigital
    Call CloseSEI         ' Close SEI DLL.
    End                       ' Exit program.
End Sub

Private Sub lblPosition_Change(Index As Integer)
    'resize label when # of chars inside change
    iFontSize = lblPosition(Index).Width / ((Len(lblPosition(Index)) + 3) * 8.5)
    If lblPosition(Index).Height / 20 < iFontSize Then
        iFontSize = lblPosition(Index).Height / 20
    End If
    If iFontSize < 1 Then iFontSize = 1
    lblPosition(Index).FontSize = iFontSize
    lblPosition(Index).Top = picPosition(Index).ScaleHeight / 2 - 10 * lblPosition(Index).FontSize
End Sub

Private Sub lblPosition_Click(Index As Integer)

If Index = 1 Then
  Call mnuLocalED2SetPos1_Click
End If
If Index = 2 Then
  Call mnuLocalED2SetPos2_Click
End If


End Sub

Private Sub lblResolution_Click()
    Call mnuLocalED2SetRes_Click
End Sub

Private Sub mnuFileExit_Click()
    Unload Me
End Sub

Private Sub mnuLocalRestartDevice_Click()
    EnableDevice
End Sub

Private Sub picCircle_Click()
    Call mnuLocalGetFactoryInfo_Click
End Sub

Sub ResizeHoriz()
    Dim iEdgeOffSet As Integer   'spacing between objects
    
    If Me.ScaleWidth > 3000 Then
        iEdgeOffSet = 60
        picCircle.DrawWidth = 2 'width of line
    Else
        iEdgeOffSet = Me.ScaleWidth / 50
        picCircle.DrawWidth = 1 'width of line
    End If
    
    'Picture boxes align left
    picPosition(1).Left = iEdgeOffSet
    picPosition(2).Left = iEdgeOffSet
    picResolution.Left = iEdgeOffSet
    picDigital(0).Left = iEdgeOffSet
    picDigital(1).Left = iEdgeOffSet
    
    'Picture boxes heights
    If Me.ScaleHeight - iEdgeOffSet < 1 Then
        picCircle.Height = 1
    Else
        picCircle.Height = Me.ScaleHeight - iEdgeOffSet - StatusBar1.Height
    End If
    
    If (Me.ScaleHeight - 4 * iEdgeOffSet) / 4 < 1 Then
        picPosition(1).Height = 1
    Else
        picPosition(1).Height = (picCircle.Height - 4 * iEdgeOffSet) / 5
    End If
    picPosition(2).Height = picPosition(1).Height
    picResolution.Height = picPosition(1).Height
    picDigital(0).Height = picPosition(1).Height
    picDigital(1).Height = picPosition(1).Height
  
    'Picture boxes widths
    If (Me.ScaleWidth - 3 * iEdgeOffSet) / 2 < 0 Then
        picPosition(1).Width = 0
    ElseIf (Me.ScaleWidth - 3 * iEdgeOffSet) / 2 < 2500 Then
        picPosition(1).Width = (Me.ScaleWidth - 3 * iEdgeOffSet) / 2
    Else
        picPosition(1).Width = 2500
    End If
    
    picPosition(2).Width = picPosition(1).Width
    picResolution.Width = picPosition(1).Width
    picDigital(0).Width = picPosition(1).Width
    picDigital(1).Width = picPosition(1).Width
    
    If Me.ScaleWidth - picPosition(1).Width - 3 * iEdgeOffSet < 0 Then
        picCircle.Width = 0
    Else
        picCircle.Width = Me.ScaleWidth - picPosition(1).Width - 3 * iEdgeOffSet
    End If
    picCircle.Left = picPosition(1).Width + 2 * iEdgeOffSet
    
    'Setup labels inside boxes
    lblPosition(1).Left = 0
    lblPosition(1).Width = picPosition(1).ScaleWidth
    lblPosition(1).Height = picPosition(1).ScaleHeight
    lblPosition(2).Left = lblPosition(1).Left
    lblPosition(2).Width = lblPosition(1).Width
    lblPosition(2).Height = lblPosition(1).Height
    
    lblResolution.Left = 0
    lblResolution.Width = picResolution.ScaleWidth
    lblResolution.Height = picResolution.ScaleHeight
    
    'Picture boxes tops
    picPosition(1).Top = 0
    picPosition(2).Top = picPosition(1).Top + picPosition(1).Height + iEdgeOffSet
    picCircle.Top = 0
    picResolution.Top = picPosition(2).Top + picPosition(2).Height + iEdgeOffSet
    picDigital(0).Top = picResolution.Top + picResolution.Height + iEdgeOffSet
    picDigital(1).Top = picDigital(0).Top + picDigital(0).Height + iEdgeOffSet
    
    'resize label
    iFontSize = lblPosition(1).Width / ((Len(lblPosition(1)) + 3) * 8.5)
    If lblPosition(1).Height / 20 < iFontSize Then
        iFontSize = lblPosition(1).Height / 20
    End If
    If iFontSize < 1 Then iFontSize = 1
    lblPosition(1).FontSize = iFontSize
    lblPosition(1).Top = picPosition(1).ScaleHeight / 2 - 10 * lblPosition(1).FontSize
    
    'resize label
    iFontSize = lblPosition(2).Width / ((Len(lblPosition(2)) + 3) * 8.5)
    If lblPosition(2).Height / 20 < iFontSize Then
        iFontSize = lblPosition(2).Height / 20
    End If
    If iFontSize < 1 Then iFontSize = 1
    lblPosition(2).FontSize = iFontSize
    lblPosition(2).Top = picPosition(2).ScaleHeight / 2 - 10 * lblPosition(2).FontSize
    
    'resize label
    iFontSize = lblResolution.Width / ((Len(lblResolution) + 3) * 8.5)
    If lblResolution.Height / 20 < iFontSize Then
        iFontSize = lblResolution.Height / 20
    End If
    If iFontSize < 1 Then iFontSize = 1
    lblResolution.FontSize = iFontSize
    lblResolution.Top = picResolution.ScaleHeight / 2 - 10 * lblResolution.FontSize
      
    ' Draw the Circle Finally
    Radius = picCircle.ScaleHeight / 2.1
    If picCircle.ScaleWidth / 2.1 < Radius Then
        Radius = picCircle.ScaleWidth / 2.1
    End If
      
    CenterX = picCircle.ScaleWidth / 2
    CenterY = picCircle.ScaleHeight / 2
  
    shpCircle.Width = 2 * Radius
    shpCircle.Height = 2 * Radius
    shpCircleShadow.Width = 2 * Radius
    shpCircleShadow.Height = 2 * Radius
    
    shpCircle.Top = CenterY - Radius
    shpCircle.Left = CenterX - Radius
    shpCircleShadow.Top = CenterY + 25 - Radius
    shpCircleShadow.Left = CenterX + 25 - Radius
    
    For I = 0 To 7
        If (picDigital(0).ScaleWidth - 9 * iEdgeOffSet) / 8 < 0 Then
            picInputBit(I).Width = 0
        Else
            picInputBit(I).Width = (picDigital(0).ScaleWidth - 9 * iEdgeOffSet) / 8
        End If
        If (picDigital(0).ScaleHeight - 2 * iEdgeOffSet) * 0.75 < 0 Then
            picInputBit(I).Height = 0
        Else
            picInputBit(I).Height = (picDigital(0).ScaleHeight - 2 * iEdgeOffSet) * 0.75
        End If
        picInputBit(I).Left = I * picInputBit(I).Width + (I + 1) * iEdgeOffSet
        picInputBit(I).Top = (picDigital(0).ScaleHeight - picInputBit(I).Height) / 2
        
        picOutputBit(I).Width = picInputBit(I).Width
        picOutputBit(I).Height = picInputBit(I).Height
        picOutputBit(I).Left = picInputBit(I).Left
        picOutputBit(I).Top = picInputBit(I).Top
    Next I
End Sub

Sub ResizeVert()
    Dim iEdgeOffSet As Integer   'spacing between objects
    
    If Me.ScaleHeight > 3000 Then
        iEdgeOffSet = 60
        picCircle.DrawWidth = 2 'width of line
    Else
        iEdgeOffSet = Me.ScaleHeight / 50
        picCircle.DrawWidth = 1 'width of line
    End If
    
    'Picture boxes align left
    picCircle.Left = iEdgeOffSet
    picPosition(1).Left = iEdgeOffSet
    picPosition(2).Left = iEdgeOffSet
    picResolution.Left = iEdgeOffSet
    picDigital(0).Left = iEdgeOffSet
    picDigital(1).Left = iEdgeOffSet
  
    'Picture boxes widths
    If Me.ScaleWidth - 2 * iEdgeOffSet < 100 Then
        picPosition(1).Width = 100
    Else
        picPosition(1).Width = Me.ScaleWidth - 2 * iEdgeOffSet
    End If
    picPosition(2).Width = picPosition(1).Width
    picResolution.Width = picPosition(1).Width
    picDigital(0).Width = picPosition(1).Width
    picDigital(1).Width = picPosition(1).Width
    picCircle.Width = picPosition(1).Width
    
    'Picture boxes heights
    If (Me.ScaleHeight - 6 * iEdgeOffSet) / 8 < 0 Then
        picPosition(1).Height = 0
    ElseIf (Me.ScaleHeight - 6 * iEdgeOffSet) / 8 < 600 Then
        picPosition(1).Height = (Me.ScaleHeight - 5 * iEdgeOffSet) / 8
    Else
        picPosition(1).Height = 600
    End If
    picPosition(2).Height = picPosition(1).Height
    picResolution.Height = picPosition(1).Height
    picDigital(0).Height = picPosition(1).Height
    picDigital(1).Height = picPosition(1).Height
    
    If Me.ScaleHeight - 5 * picPosition(1).Height - 6 * iEdgeOffSet < 0 Then
        picCircle.Height = 0
    Else
        picCircle.Height = Me.ScaleHeight - 5 * picPosition(1).Height - 6 * iEdgeOffSet - StatusBar1.Height
    End If
    
    'Picture boxes tops
    picPosition(1).Top = 0
    picPosition(2).Top = picPosition(1).Top + picPosition(1).Height + iEdgeOffSet
    picResolution.Top = picPosition(2).Top + picPosition(2).Height + iEdgeOffSet
    picDigital(0).Top = picResolution.Top + picResolution.Height + iEdgeOffSet
    picDigital(1).Top = picDigital(0).Top + picDigital(0).Height + iEdgeOffSet
    picCircle.Top = picDigital(1).Top + picDigital(1).Height + iEdgeOffSet
    
    lblPosition(1).Left = 0
    lblPosition(2).Left = 0
    lblResolution.Left = 0
    
    'Setup labels inside boxes
    lblPosition(1).Width = picPosition(1).ScaleWidth
    lblPosition(2).Width = picPosition(2).ScaleWidth
    lblResolution.Width = picResolution.ScaleWidth
    lblPosition(1).Height = picPosition(1).ScaleHeight
    lblPosition(2).Height = picPosition(2).ScaleHeight
    lblResolution.Height = picResolution.ScaleHeight
  
    'resize label
    iFontSize = lblPosition(1).Width / ((Len(lblPosition(1)) + 3) * 8.5)
    If lblPosition(1).Height / 20 < iFontSize Then
        iFontSize = lblPosition(1).Height / 20
    End If
    If iFontSize < 1 Then iFontSize = 1
    lblPosition(1).FontSize = iFontSize
    lblPosition(1).Top = picPosition(1).ScaleHeight / 2 - 10 * lblPosition(1).FontSize
    
    'resize label
    iFontSize = lblPosition(2).Width / ((Len(lblPosition(2)) + 3) * 8.5)
    If lblPosition(2).Height / 20 < iFontSize Then
        iFontSize = lblPosition(2).Height / 20
    End If
    If iFontSize < 1 Then iFontSize = 1
    lblPosition(2).FontSize = iFontSize
    lblPosition(2).Top = picPosition(2).ScaleHeight / 2 - 10 * lblPosition(2).FontSize
    
    'resize label
    iFontSize = lblResolution.Width / ((Len(lblResolution) + 3) * 8.5)
    If lblResolution.Height / 20 < iFontSize Then
        iFontSize = lblResolution.Height / 20
    End If
    If iFontSize < 1 Then iFontSize = 1
    lblResolution.FontSize = iFontSize
    lblResolution.Top = picResolution.ScaleHeight / 2 - 10 * lblResolution.FontSize

    ' Draw the Circle Finally
    Radius = picCircle.ScaleHeight / 2.1
    If picCircle.ScaleWidth / 2.1 < Radius Then
        Radius = picCircle.ScaleWidth / 2.1
    End If
      
    CenterX = picCircle.ScaleWidth / 2
    CenterY = picCircle.ScaleHeight / 2
  
    shpCircle.Width = 2 * Radius
    shpCircle.Height = 2 * Radius
    shpCircleShadow.Width = 2 * Radius
    shpCircleShadow.Height = 2 * Radius
    
    shpCircle.Top = CenterY - Radius
    shpCircle.Left = CenterX - Radius
    shpCircleShadow.Top = CenterY + 25 - Radius
    shpCircleShadow.Left = CenterX + 25 - Radius
    
    For I = 0 To 7
        If (picDigital(0).ScaleWidth - 9 * iEdgeOffSet) / 8 < 0 Then
            picInputBit(I).Width = 0
        Else
            picInputBit(I).Width = (picDigital(0).ScaleWidth - 9 * iEdgeOffSet) / 8
        End If
        If (picDigital(0).ScaleHeight - 2 * iEdgeOffSet) * 0.9 < 0 Then
            picInputBit(I).Height = 0
        Else
            picInputBit(I).Height = (picDigital(0).ScaleHeight - 2 * iEdgeOffSet) * 0.9
        End If
        picInputBit(I).Left = I * picInputBit(I).Width + (I + 1) * iEdgeOffSet
        picInputBit(I).Top = (picDigital(0).ScaleHeight - picInputBit(I).Height) / 2
        
        picOutputBit(I).Width = picInputBit(I).Width
        picOutputBit(I).Height = picInputBit(I).Height
        picOutputBit(I).Left = picInputBit(I).Left
        picOutputBit(I).Top = picInputBit(I).Top
    Next I
End Sub

Public Sub InitForm()
    Dim DeviceFound As Boolean
    ' The following code shows how to initialize the SEI Server software, which
    ' communicates with US Digital SEI Devices
    Me.show
    Form_Resize
    DoEvents
    Me.MousePointer = vbHourglass
    
    ' SEI Initialization.
    ' Start the SEI Server Program, and look for devices on the SEI bus
    ' the zero means to look on all com ports, and the AUTOASSIGN means
    ' that if there are address conflicts on the SEI bus, the device
    ' addresses will automatically be reassigned so there are no conflicts
   
    Trace "Initializing Please Wait..."
    Errcode = InitializeSEI(m_COMPort, REINITIALIZE + AUTOASSIGN + NOCHECKSUM)
    If Errcode <> 0 Then
        Trace
        Me.MousePointer = vbDefault
        Exit Sub
    End If

    m_COMPort = GetCommPort()
    Trace "Initialized COM Port " & m_COMPort
    mnuCommunicaitonSetCOMPort.Caption = "Set COM Port... (" & m_COMPort & ")"

    ' Get device information.
   
    ' Ask the SEI Server how many devices it detected on the SEI bus
    Devices = GetNumberOfDevices()
    
    For I = 0 To Devices - 1
        ' For each device on the SEI bus, obtain the important information for it
        Errcode = GetDeviceInfo(I, Model(I), SerialNum(I), Ver, Address(I))
        If Errcode <> 0 Then
            Trace
            Me.MousePointer = vbDefault
            Exit Sub
        End If
        
        ' This is an ED2 Sample Code, so we're only interested in ED2's
        ' Furthermore, we only expect to find one ED2 on the bus in this example
        ' As with all US Digital SEI Products, there can actually be up to 14
        ' devices on the bus, use the SEI Explorer for more than one device.
        Product(I) = ProductType(Model(I)) ' Get Product Name for Product
        
        If Model(I) = 3 Then ' If ED2
            If DeviceFound = True Then
                MsgBox "More than one ED2 is on the SEI bus, use the SEI Explorer!", 16, "Error"
                End
            End If
            DeviceFound = True
            MyAddress = Address(I)
            MySN = SerialNum(I)
            MyIndex = I
        End If
    Next
    If DeviceFound = False Then
        MsgBox "The ED2 was not found on the SEI bus, exiting program.", 16, "Error"
        End
    End If

    Baudrate = 9600
    
    'Get mode of ED2
    Errcode = ED2GetMode(MyAddress, MyMode)
    If Errcode <> 0 Then
        DisableDevice
        Me.MousePointer = vbDefault
        Exit Sub
    End If
    
    'Get Resolution of ED2
    Errcode = ED2GetResolution(MyAddress, MyResolution)
    If Errcode <> 0 Then
        DisableDevice
        Me.MousePointer = vbDefault
        Exit Sub
    End If
    lblResolution = "Res.= " & MyResolution
    Me.MousePointer = vbDefault
    
    Caption = Me.Caption = App.FileDescription & " Version " & App.Major & "." & App.Minor & IIf(App.Revision > 0, "." & App.Revision & " ", " ") & Product(MyIndex) & " at Address " & MyAddress & " with Serial # " & MySN

    Trace "InitializeSEI Complete."
    
    ' At this point, if everything is healthy, the SEI Server is started
    ' You can see it as another program on your task bar
    ' This sample program knows the address of the ED2 on the bus, and its
    ' Serial Number.  It also has read the mode and resolution values which
    ' are in the ED2 device
    ' You are now ready to ask for positions, change mode, set positions, etc.
    
    ' Start polling device
    MyPosition(1) = -2147483647
    MyPosition(2) = -2147483647
    tmrMain.Enabled = True
    bDeviceFailed = False
End Sub

Private Sub Form_Load()
    Width = Screen.Width / 2
    Height = Screen.Height / 2
    Top = Screen.Height / 2 - Me.Height / 2
    Left = Screen.Width / 2 - Me.Width / 2
    
    lblPosition(1).ForeColor = vbBlue
    lblPosition(2).ForeColor = vbBlack
    picPosition(1).MouseIcon = LoadPicture("hand.ico")
    picPosition(2).MouseIcon = LoadPicture("hand.ico")
    picResolution.MouseIcon = LoadPicture("hand.ico")
    lblResolution.MouseIcon = LoadPicture("hand.ico")
    picCircle.MouseIcon = LoadPicture("hand.ico")
    picPosition(1).MousePointer = 99
    picPosition(2).MousePointer = 99
    picResolution.MousePointer = 99
    lblResolution.MousePointer = 99
    picCircle.MousePointer = 99
    For I = 0 To 7
        picOutputBit(I).MouseIcon = LoadPicture("hand.ico")
        picOutputBit(I).MousePointer = 99
    Next
    
    Me.Caption = App.FileDescription & " Version " & App.Major & "." & App.Minor & IIf(App.Revision > 0, "." & App.Revision, "")
    InitForm
End Sub

Private Sub Form_Resize()
    If Me.ScaleWidth < Me.ScaleHeight Then
        ResizeVert
    Else
        ResizeHoriz
    End If
End Sub

Public Sub UpdateDisplay()
    
    ' if device failed then don't update
    If bDeviceFailed Then Exit Sub
    
    ' Variables to compare if position changed
    iOldPosition(1) = MyPosition(1)
    iOldPosition(2) = MyPosition(2)
    
    ' Get position
    Errcode = ED2GetPosition1(MyAddress, MyPosition(1))
    If Errcode <> 0 Then ' Begin error processing
        MyErrCount = MyErrCount + 1
        Trace , MyAddress
        lblErrors.Visible = True
        If MyErrCount > MyErrLimit Then
            DisableDevice
            Exit Sub
        End If
        MyPosition(1) = 0
    Else
        Trace "Running"
    End If
    
    ' Get position
    Errcode = ED2GetPosition2(MyAddress, MyPosition(2))
    If Errcode <> 0 Then ' Begin error processing
        MyErrCount = MyErrCount + 1
        Trace , MyAddress
        lblErrors.Visible = True
        If MyErrCount > MyErrLimit Then
            DisableDevice
            Exit Sub
        End If
        MyPosition(2) = 0
    End If
    
    ' Get Input bits
    Errcode = ED2GetInput(MyAddress, MyInputs)
    If Errcode <> 0 Then ' Begin error processing
        MyErrCount = MyErrCount + 1
        Trace , MyAddress
        lblErrors.Visible = True
        If MyErrCount > MyErrLimit Then
            DisableDevice
            Exit Sub
        End If
        MyInputs = 0
    End If
    
    'Only need to do the following when the position changed
    If iOldPosition(1) <> MyPosition(1) Then
        'draw grey line over old one
        picCircle.Line (CenterX, CenterY)-Step((Radius - 75) * Cos(MyAngle(1)), (Radius - 75) * Sin(MyAngle(1))), RGB(192, 192, 192)
        MyAngle(1) = MyPosition(1) * 2 * 3.141593 / MyResolution - 0.5 * 3.141593
        'fill device label
        lblPosition(1) = MyPosition(1)
    End If
    'Only need to do the following when the position changed
    If iOldPosition(2) <> MyPosition(2) Then
        'draw grey line over old one
        picCircle.Line (CenterX, CenterY)-Step((Radius - 75) * Cos(MyAngle(2)), (Radius - 75) * Sin(MyAngle(2))), RGB(192, 192, 192)
        MyAngle(2) = MyPosition(2) * 2 * 3.141593 / MyResolution - 0.5 * 3.141593
        'fill device label
        lblPosition(2) = MyPosition(2)
    End If
    
    'draw blue lines
    picCircle.Line (CenterX, CenterY)-Step((Radius - 75) * Cos(MyAngle(1)), (Radius - 75) * Sin(MyAngle(1))), lblPosition(1).ForeColor
    picCircle.Line (CenterX, CenterY)-Step((Radius - 75) * Cos(MyAngle(2)), (Radius - 75) * Sin(MyAngle(2))), lblPosition(2).ForeColor

    If (MyInputs < 0) Or (MyInputs > 256) Then Exit Sub
    
    ' Update Input Annunciator Display
    For I = 0 To 7
        If BBTst(I, MyInputs) Then
            picInputBit(I).BackColor = vbGreen
        Else
            picInputBit(I).BackColor = vbRed
        End If
    Next I
    
    ' Update Output Annunciator Display
    For I = 0 To 7
        If Not BBTst(I, MyOutputs) Then
            picOutputBit(I).BackColor = vbGreen
        Else
            picOutputBit(I).BackColor = vbRed
        End If
    Next I
        
End Sub
Sub DisableDevice()
'This is used when the device returns an error to disable polling and labels etc.
    bDeviceFailed = True
    mnuLocalRestartDevice.Visible = True
    lblPosition(1) = "Failed"
    lblPosition(2) = "Failed"
    DeviceLastError = Errcode
    picPosition(1).Enabled = False
    picPosition(2).Enabled = False
    picResolution.Enabled = False
    picDigital(0).Enabled = False
    picDigital(1).Enabled = False
    picCircle.Enabled = False
    lblPosition(1).Enabled = False
    lblPosition(2).Enabled = False
    lblResolution.Enabled = False
    
    mnuLocalED2SetMode.Enabled = False
    mnuLocalED2SetOrigin.Enabled = False
    mnuLocalED2SetPos1.Enabled = False
    mnuLocalED2SetPos2.Enabled = False
    mnuLocalED2SetRes.Enabled = False
    
    mnuLocalGetFactoryInfo.Enabled = False
    mnuLocalSEIChangeAdd.Enabled = False
    mnuLocalSEIResetSEI.Enabled = False
    mnuLocalSEISetBaudRate.Enabled = False
    Form_Resize
End Sub

Sub EnableDevice()
'Use this sub to restart everything
    mnuLocalRestartDevice.Visible = False
    lblPosition(1) = "Failed"
    lblPosition(2) = "Failed"
    DeviceLastError = Errcode
    picPosition(1).Enabled = True
    picPosition(2).Enabled = True
    picResolution.Enabled = True
    picDigital(0).Enabled = True
    picDigital(1).Enabled = True
    picCircle.Enabled = True
    lblPosition(1).Enabled = True
    lblPosition(2).Enabled = True
    lblResolution.Enabled = True
    
    mnuLocalED2SetMode.Enabled = True
    mnuLocalED2SetOrigin.Enabled = True
    mnuLocalED2SetPos1.Enabled = True
    mnuLocalED2SetPos2.Enabled = True
    mnuLocalED2SetRes.Enabled = True
    
    mnuLocalGetFactoryInfo.Enabled = True
    mnuLocalSEIChangeAdd.Enabled = True
    mnuLocalSEIResetSEI.Enabled = True
    mnuLocalSEISetBaudRate.Enabled = True
    InitForm
End Sub




Private Sub mnuLocalED2SetMode_Click()
If frmDigitalExists Or frmDigitalExists Then Exit Sub
     ' Set ED2 mode.
     
       InitMode = MyMode
       frmModeExists = True
       GoodMode = False
       ModeCaption = Caption
       frmED2Mode.show
       While frmModeExists
         DoEvents
       Wend
       
       If GoodMode Then
         MyMode = FinalMode
         Errcode = ED2SetMode(MyAddress, MyMode)
        
         If Errcode <> 0 Then ' Begin if error
            DisableDevice
            Exit Sub
         End If               ' End if error
       End If
End Sub

Private Sub mnuLocalED2SetOrigin_Click()
   
  Errcode = ED2SetOrigin(MyAddress)
  If Errcode <> 0 Then ' Begin if error
    DisableDevice
    Exit Sub
  End If               ' End if error
   
End Sub
Private Sub mnuLocalED2SetPos1_Click()
    ' Set ED2 Position.
    Dim Position As Single
    Answer = InputBox$("Setting the position sets the given absolute position to the current position. " & Chr(10) & Chr(10) & "Please enter the absolute position that you want to set your ED2 to.", "Set Position", MyPosition(1))
    If Answer <> "" Then
        If Val(Answer) > MyResolution Then
            MsgBox "You entered an invalid position, it must be less than the resolution.", 48, "Error!"
        Else
            Call SafeVal(Answer, Position)
            Errcode = ED2SetPosition1(MyAddress, Position)
            If Errcode <> 0 Then ' Begin if error
                DisableDevice
                Exit Sub
            End If               ' End if error
        End If
    End If
End Sub
Private Sub mnuLocalED2SetPos2_Click()
    ' Set ED2 Position.
    Dim Position As Single
    Answer = InputBox$("Setting the position sets the given absolute position to the current position. " & Chr(10) & Chr(10) & "Please enter the absolute position that you want to set your ED2 to.", "Set Position", MyPosition(2))
    If Answer <> "" Then
        If Val(Answer) > MyResolution Then
            MsgBox "You entered an invalid position, it must be less than the resolution.", 48, "Error!"
        Else
            Call SafeVal(Answer, Position)
            Errcode = ED2SetPosition2(MyAddress, Position)
            If Errcode <> 0 Then ' Begin if error
                DisableDevice
                Exit Sub
            End If               ' End if error
        End If
    End If
End Sub
Private Sub mnuLocalED2SetRes_Click()
    ' Set ED2 Resolution.
    Dim Resolution As Long                          ' New ED2 resolution
   
    Answer = InputBox$("Please enter the resolution that you want to set your ED2 to.  Valid resolutions are from 2 to 65535.", "Set Resolution", MyResolution)
    If Answer <> "" Then
        Call SafeVal(Answer, Resolution)
        If Resolution < 2 Or Resolution > 65536 Then
            MsgBox "You entered an invalid resolution, valid resolutions are from 2 to 65535.", 48, "Error!"
        Else
            If Resolution = 65536 Then Resolution = 0
            Errcode = ED2SetResolution(MyAddress, Resolution)
            If Errcode <> 0 Then ' Begin if error
                DisableDevice
                Exit Sub
            End If               ' End if error
            Errcode = ED2GetResolution(MyAddress, Resolution)
            If Errcode <> 0 Then ' Begin if error
                DisableDevice
                Exit Sub
            End If               ' End if error
            If Resolution = 0 Then Resolution = 65536
            MyResolution = Resolution
            lblResolution = "Res.= " & MyResolution
        End If
    End If
End Sub
Private Sub mnuLocalGetFactoryInfo_Click()
If frmDigitalExists Or frmModeExists Then Exit Sub
  frmFactoryInfoExists = True
  frmFactoryInfo.show
  frmFactoryInfo.tmrMain.Enabled = True
  
  frmFactoryInfo.Top = Me.Top + (Me.ScaleHeight / 2 - frmFactoryInfo.ScaleHeight / 2)
  frmFactoryInfo.Left = Me.Left + (Me.ScaleWidth / 2 - frmFactoryInfo.ScaleWidth / 2)
    
  frmFactoryInfo.MyIndex = MyIndex
  
  Call frmFactoryInfo.Update
End Sub

Private Sub mnulocalGetLastError_Click()
    If bDeviceFailed Or Not DeviceLastError = 0 Then
        Call CheckIfError(DeviceLastError)
    Else
        Call MsgBox("No errors found", vbInformation, "Get Last Error")
    End If
End Sub

Private Sub mnuHelpAbout_Click()
    ' Display information about this program.
    Dim sMsg As String
    sMsg = App.FileDescription & vbCrLf & _
           "Version " & App.Major & "." & App.Minor & IIf(App.Revision > 0, "." & App.Revision, "") & vbCrLf & vbCrLf & _
           "US Digital" & vbCrLf & "1400 NE 136th Avenue" & vbCrLf & "Vancouver, Washington 98684" & vbCrLf & "USA" & vbCrLf & vbCrLf & _
           "usdigital.com" & vbCrLf & _
           "800.736.0194" & vbCrLf & "360.260.2468"
    MsgBox sMsg, vbInformation, "About " & App.FileDescription
End Sub

Public Sub SEIChangeAddress(NewAdd As Integer)
    ' Change the address of the SEI Device
    Errcode = SetDeviceAddress(MySN, NewAdd)
    If Errcode <> 0 Then ' Begin if error
        DisableDevice
        Exit Sub
    End If
    
    DeviceLastError = 0
    Address(MyIndex) = NewAdd
    MyAddress = NewAdd
    UpdateDisplay

End Sub

Public Sub mnuLocalSEIChangeAdd_Click()

    'Change Address of an SEI Device.
    Dim NewAddress As Integer                       ' New address.
    Dim Errors As Boolean
    Errors = False
    
    Answer = InputBox$("Please enter the new address you want for this device, serial number " & MySN & " with a current address of " & MyAddress & ".  Valid addresses are from 0 to 14.", "Set Address", "0")
    If Answer <> "" Then
        Call SafeVal(Answer, NewAddress)
        If NewAddress < 0 Or NewAddress > 14 Then
            MsgBox "You entered an invalid address!", 48, "Error!"
        Else
            For I = 0 To Devices - 1
                If Address(I) = NewAddress Then
                    MsgBox "This address is already in use by another device!", 48, "Error!"
                    Errors = True
                End If
            Next
            If Errors <> True Then
                Call SEIChangeAddress(NewAddress)
            End If
        End If
    End If
End Sub

Private Sub mnulocalSEIResetSEI_Click()
 ' Reset SEI Bus.
    Baudrate = 9600
    Errcode = ResetSEI()
    Call CheckIfError(Errcode)
End Sub

Private Sub mnuLocalSEISetBaudRate_Click()
 
    ' Set Baud Rate.
    Dim Baud As Long

    Answer = InputBox("Please enter the baud rate you want. " & Chr(10) & Chr(10) & "Your current baudrate is " & Baudrate & " baud. " & Chr(10) & Chr(10) & "Valid baud rates are: 9600, 19200, 38400, 57600, 115200.", "Set Baud Rate", Baudrate)
    If Answer = "" Then Exit Sub
    Call SafeVal(Answer, Baud)
    If Baud = 9600 Or Baud = 19200 Or Baud = 38400 Or Baud = 57600 Or Baud = 115200 Then
        Baudrate = Baud
        Errcode = SetBaudRate(Baudrate)
        If Errcode <> 0 Then ' Begin if error
            DisableDevice
            Exit Sub
        End If               ' End if error
    Else
        MsgBox "You entered an invalid baudrate!", 48, "Error!"
    End If
End Sub
Private Sub picOutputBit_Click(Index As Integer)
If frmFactoryInfoExists Or frmModeExists Then Exit Sub

    frmDigitalExists = True  ' Global variable which frm will set to false on exit
    Load frmDigital
    
    If BBTst(Index, MyOutputs) Then
        frmDigital.TriStateOutput = 2
    Else
        frmDigital.TriStateOutput = 1
    End If
    
    Call frmDigital.Update ' Set up the annunciator
    frmDigital.Caption = picOutputBit(Index).ToolTipText
    If Index < 3 Then frmDigital.chkToggle = 1 ' default to toggle mode
    frmDigital.show
  
    While frmDigitalExists
        If frmDigital.StateSelected Then  ' Begin State Selected
            If frmDigital.TriStateOutput = 1 Then  ' They turned it on
                MyOutputs = BBClr(Index, MyOutputs)
            End If
            If frmDigital.TriStateOutput = 2 Then  ' They turned it off
                MyOutputs = BBSet(Index, MyOutputs)
            End If
            Errcode = ED2SetOutput(MyAddress, MyOutputs)  ' Send command
            If Errcode <> 0 Then ' Begin if error
                DisableDevice
                Exit Sub
            End If               ' End if error
            frmDigital.StateSelected = False ' Reset, in case they want to do it again
        End If ' End State Selected
        DoEvents ' Let everybody else play a bit
        DoEvents
    Wend
End Sub

Private Sub lblResolution_Change()
    'resize label when # of chars inside change
    iFontSize = lblResolution.Width / ((Len(lblResolution) + 3) * 8.5)
    If lblResolution.Height / 20 < iFontSize Then
        iFontSize = lblResolution.Height / 20
    End If
    If iFontSize < 1 Then iFontSize = 1
    lblResolution.FontSize = iFontSize
    lblResolution.Top = picResolution.ScaleHeight / 2 - 10 * lblResolution.FontSize
End Sub

Private Sub picResolution_Click()
    Call mnuLocalED2SetRes_Click
End Sub

Private Sub tmrMain_Timer()
    UpdateDisplay
End Sub

Private Sub mnuCommunicaitonSetCOMPort_Click()
    Dim sData As String
    sData = InputBox("Enter the COM Port number (1-256) or * to scan across " & _
                     "all COM ports for the first device found.", "Select COM Port", 0)
    If sData = "" Then Exit Sub
    If sData = "*" Then
        m_COMPort = 0
    Else
        m_COMPort = Val(sData)
    End If
    mnuCommunicaitonSetCOMPort.Caption = "Set COM Port... (" & m_COMPort & ")"
    Trace "Change to COM Port " & m_COMPort
    InitForm
End Sub

Private Sub lblErrors_MouseMove(Button As Integer, Shift As Integer, X As Single, Y As Single)
    frmActivityLog.show
    If frmActivityLog.WindowState = vbMinimized Then frmActivityLog.WindowState = vbNormal
    MyErrCount = 0
    lblErrors.Visible = False
    Dim itemX As ListItem
    For Each itemX In frmActivityLog.lvLog.ListItems
        If itemX.SubItems(1) <> "" And Val(itemX.SubItems(1)) = MyAddress Then
            itemX.ForeColor = vbRed
            itemX.Selected = False
        Else
            itemX.ForeColor = vbBlack
            itemX.Selected = False
        End If
    Next itemX
End Sub



