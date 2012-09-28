VERSION 5.00
Begin VB.Form frmED2Mode 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Mode"
   ClientHeight    =   2655
   ClientLeft      =   7065
   ClientTop       =   4485
   ClientWidth     =   2025
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   PaletteMode     =   1  'UseZOrder
   ScaleHeight     =   2655
   ScaleWidth      =   2025
   ShowInTaskbar   =   0   'False
   Visible         =   0   'False
   Begin VB.Timer tmrMain 
      Interval        =   100
      Left            =   0
      Top             =   0
   End
   Begin VB.CommandButton cmdModeCancel 
      Caption         =   "&Cancel"
      Height          =   375
      Left            =   1140
      TabIndex        =   6
      Top             =   2100
      Width           =   795
   End
   Begin VB.CommandButton cmdModeOK 
      Caption         =   "&OK"
      Default         =   -1  'True
      Height          =   375
      Left            =   60
      TabIndex        =   5
      Top             =   2100
      Width           =   915
   End
   Begin VB.CheckBox chkMode 
      Caption         =   "Size"
      Height          =   195
      Index           =   3
      Left            =   540
      TabIndex        =   3
      Top             =   600
      Width           =   1095
   End
   Begin VB.CheckBox chkMode 
      Caption         =   "Multi-Turn"
      Height          =   195
      Index           =   2
      Left            =   540
      TabIndex        =   2
      Top             =   300
      Width           =   1215
   End
   Begin VB.Frame fraMode 
      Caption         =   "Mode"
      Height          =   1935
      Left            =   60
      TabIndex        =   0
      Top             =   0
      Width           =   1875
      Begin VB.CheckBox chkMode 
         Caption         =   "unused"
         Height          =   195
         Index           =   4
         Left            =   720
         TabIndex        =   8
         Top             =   1440
         Visible         =   0   'False
         Width           =   1335
      End
      Begin VB.CheckBox chkMode 
         Caption         =   "unused"
         Height          =   195
         Index           =   1
         Left            =   720
         TabIndex        =   7
         Top             =   1200
         Visible         =   0   'False
         Width           =   1095
      End
      Begin VB.CheckBox chkMode 
         Caption         =   "unused"
         Height          =   195
         Index           =   5
         Left            =   720
         TabIndex        =   4
         Top             =   1680
         Visible         =   0   'False
         Width           =   1215
      End
      Begin VB.CheckBox chkMode 
         Caption         =   "unused"
         Height          =   195
         Index           =   0
         Left            =   720
         TabIndex        =   1
         Top             =   960
         Visible         =   0   'False
         Width           =   1215
      End
   End
End
Attribute VB_Name = "frmED2Mode"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub chkMode_Click(Index As Integer)
  
  FinalMode = chkMode(0) * 1 + chkMode(1) * 2 + chkMode(2) * 4 + chkMode(3) * 8
  FinalMode = FinalMode + chkMode(4) * 16 + chkMode(5) * 64
  
End Sub


Private Sub Form_Load()
    Dim TempVal As Integer

    Caption = ModeCaption
    TempVal = InitMode
    If TempVal >= 64 Then
        TempVal = TempVal - 64
        chkMode(5) = 1
    Else
        chkMode(5) = 0
    End If
 
    If TempVal >= 16 Then
        TempVal = TempVal - 16
        chkMode(4) = 1
    Else
        chkMode(4) = 0
    End If
    
    If TempVal >= 8 Then
        TempVal = TempVal - 8
        chkMode(3) = 1
    Else
        chkMode(3) = 0
    End If
 
    If TempVal >= 4 Then
        TempVal = TempVal - 4
        chkMode(2) = 1
    Else
        chkMode(2) = 0
    End If
    
    If TempVal >= 2 Then
        TempVal = TempVal - 2
        chkMode(1) = 1
    Else
        chkMode(1) = 0
    End If
 
    If TempVal >= 1 Then
        TempVal = TempVal - 1
        chkMode(0) = 1
    Else
        chkMode(0) = 0
    End If
  
End Sub



Private Sub cmdModeCancel_Click()
       FinalMode = InitMode
       GoodMode = False
       frmModeExists = False
       Unload Me
End Sub

Private Sub cmdModeOK_Click()
       
       FinalMode = chkMode(0) * 1 + chkMode(1) * 2 + chkMode(2) * 4 + chkMode(3) * 8
       FinalMode = FinalMode + chkMode(4) * 16 + chkMode(5) * 64
       GoodMode = True
       frmModeExists = False
       Unload Me
       
End Sub


Private Sub tmrMain_Timer()
Me.ZOrder 0
End Sub


