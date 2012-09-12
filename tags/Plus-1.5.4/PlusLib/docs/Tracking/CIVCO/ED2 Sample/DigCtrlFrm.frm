VERSION 5.00
Begin VB.Form frmDigital 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Output Control"
   ClientHeight    =   3195
   ClientLeft      =   6675
   ClientTop       =   3735
   ClientWidth     =   2730
   Icon            =   "DigCtrlFrm.frx":0000
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3195
   ScaleWidth      =   2730
   ShowInTaskbar   =   0   'False
   Begin VB.Timer tmrToggle 
      Interval        =   300
      Left            =   1140
      Top             =   600
   End
   Begin VB.CheckBox chkToggle 
      Caption         =   "Toggle"
      Height          =   195
      Left            =   60
      TabIndex        =   5
      Top             =   2880
      Width           =   1035
   End
   Begin VB.Timer tmrMain 
      Interval        =   100
      Left            =   1140
      Top             =   60
   End
   Begin VB.CommandButton cmdOff 
      Caption         =   "OFF"
      Height          =   555
      Left            =   1560
      TabIndex        =   4
      Top             =   1860
      Width           =   915
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "&OK"
      Default         =   -1  'True
      Height          =   435
      Left            =   1440
      TabIndex        =   3
      Top             =   2760
      Width           =   1275
   End
   Begin VB.CommandButton cmdOn 
      Caption         =   "ON"
      Height          =   555
      Left            =   240
      TabIndex        =   2
      Top             =   1860
      Width           =   915
   End
   Begin VB.PictureBox picOff 
      Height          =   1395
      Left            =   1560
      ScaleHeight     =   1335
      ScaleWidth      =   855
      TabIndex        =   1
      Top             =   360
      Width           =   915
   End
   Begin VB.PictureBox picOn 
      Height          =   1395
      Left            =   240
      ScaleHeight     =   1335
      ScaleWidth      =   855
      TabIndex        =   0
      Top             =   360
      Width           =   915
   End
End
Attribute VB_Name = "frmDigital"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Public TriStateOutput As Integer  ' 0=> no change, 1=> ON, 2=> OFF
Public StateSelected As Boolean ' If user chooses a state, I want to know about it

Dim TogglingOn As Boolean  ' Set to true if in middle of a toggle On operation
Dim TogglingOff As Boolean ' Set to true if in middle of a toggle Off operation

Private Sub chkToggle_Click()
    If chkToggle.Value = 1 Then
        cmdOn.Caption = "Toggle On"
        cmdOff.Caption = "Toggle Off"
    Else
        cmdOn.Caption = "On"
        cmdOff.Caption = "Off"
    End If

End Sub

Private Sub cmdOff_Click()
  If Not tmrToggle.Enabled Then
    If chkToggle Then
      TogglingOff = True
      TogglingOn = False
      tmrToggle.Enabled = True
    End If
  End If
  TriStateOutput = 2
  StateSelected = True ' Tell the world that the user picked a state
  Call Update
End Sub


Private Sub cmdOK_Click()
  frmDigitalExists = False  ' Global variable to tell program that form is dead
  Unload Me

End Sub



Private Sub cmdOn_Click()
  If Not tmrToggle.Enabled Then
    If chkToggle Then
      TogglingOn = True
      TogglingOff = False
      tmrToggle.Enabled = True
    End If
  End If
  TriStateOutput = 1
  StateSelected = True ' Tell the world that the user picked a state
  Call Update
End Sub

Private Sub Form_Load()

    StateSelected = False ' Initialize to False, so I know if a state was picked
    TogglingOn = False ' Initialize to False
    TogglingOff = False ' Initialize to False

    If chkToggle Then
        cmdOn.Caption = "Toggle On"
        cmdOff.Caption = "Toggle Off"
    Else
        cmdOn.Caption = "On"
        cmdOff.Caption = "Off"
    End If
    
End Sub

Private Sub tmrMain_Timer()
  Me.ZOrder 0
  
End Sub
Public Sub Update()
'  After the form loads, another form can set the TriStateOutput property
'  and then call this routine.  This routine will set the colors of the
'  annunciators, telling the user what the current state of the output is.

  picOn.BackColor = &H8000000F  ' turn ON  annunciator Grey
  picOff.BackColor = &H8000000F ' turn OFF annunciator Grey
  
  If TriStateOutput = 1 Then
    picOn.BackColor = vbGreen
  End If
  
  If TriStateOutput = 2 Then
    picOff.BackColor = vbRed
  End If
End Sub

Private Sub tmrToggle_Timer()
  If Not StateSelected Then
    If TogglingOn Then
      Call cmdOff_Click
    End If
    If TogglingOff Then
      Call cmdOn_Click
    End If
    tmrToggle.Enabled = False
    TogglingOff = False
    TogglingOn = False
  End If
  DoEvents
End Sub
