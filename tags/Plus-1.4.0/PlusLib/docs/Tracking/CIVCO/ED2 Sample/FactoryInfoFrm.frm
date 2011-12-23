VERSION 5.00
Begin VB.Form frmFactoryInfo 
   BorderStyle     =   3  'Fixed Dialog
   Caption         =   "Factory Information"
   ClientHeight    =   3540
   ClientLeft      =   6300
   ClientTop       =   3915
   ClientWidth     =   3090
   ControlBox      =   0   'False
   LinkTopic       =   "Form1"
   MaxButton       =   0   'False
   MinButton       =   0   'False
   ScaleHeight     =   3540
   ScaleWidth      =   3090
   ShowInTaskbar   =   0   'False
   Begin VB.Timer tmrMain 
      Interval        =   100
      Left            =   2280
      Top             =   3060
   End
   Begin VB.CommandButton cmdOK 
      Caption         =   "&OK"
      Height          =   315
      Left            =   1020
      TabIndex        =   0
      Top             =   3180
      Width           =   1035
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   9
      Left            =   0
      TabIndex        =   10
      Top             =   2700
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   8
      Left            =   0
      TabIndex        =   9
      Top             =   2400
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   7
      Left            =   0
      TabIndex        =   8
      Top             =   2100
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   6
      Left            =   0
      TabIndex        =   7
      Top             =   1800
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   5
      Left            =   0
      TabIndex        =   6
      Top             =   1500
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   4
      Left            =   0
      TabIndex        =   5
      Top             =   1200
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   3
      Left            =   0
      TabIndex        =   4
      Top             =   900
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   2
      Left            =   0
      TabIndex        =   3
      Top             =   600
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   1
      Left            =   0
      TabIndex        =   2
      Top             =   300
      Width           =   3075
   End
   Begin VB.Label Label 
      Alignment       =   2  'Center
      Caption         =   "xxx"
      BeginProperty Font 
         Name            =   "Arial"
         Size            =   12
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   -1  'True
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Index           =   0
      Left            =   0
      TabIndex        =   1
      Top             =   0
      Width           =   3075
   End
End
Attribute VB_Name = "frmFactoryInfo"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Public MyAddress As Long
Public MyMode As Long
Public MyResolution As Long
Public MySN As Long
Public MyIndex As Long  ' The array number of this form

Public Sub Update()
  Dim MajVer, MinVer As Integer
  Dim ManDate As Date
  Dim MyIndex, MyTestModel, MyTestSN, MyTestVer, MyTestaddr, MyMMonth, MyMDay, MyMyear, MyMConf
  
  Call GetAllDeviceInfo(MyIndex, MyTestModel, MyTestSN, MyTestVer, MyTestaddr, MyMMonth, MyMDay, MyMyear, MyMConf)
  Label(0) = ProductType(Model(MyIndex))
  Label(1) = ""
  Label(2) = "SN: " & Val(MyTestSN)
  Label(3) = "Address: " & Val(MyTestaddr)
  Label(4) = ""
  ManDate = DateSerial(MyMyear, MyMMonth, MyMDay)    ' Return a date.
  Label(5) = "Manufactured on"
  Label(6) = Format(ManDate, "dddd, mmm d yyyy")
  Label(7) = ""
  Label(8) = "Version: " & MyTestVer \ 256 & "." & Format(Val(MyTestVer) - ((Val(MyTestVer) \ 256) * 256), "00")
  Label(9) = "Configuration: " & MyMConf \ 256 & "." & Format(Val(MyMConf) - ((Val(MyMConf) \ 256) * 256), "00")
  
 End Sub



Private Sub cmdOK_Click()
frmFactoryInfoExists = False
Unload Me
End Sub

Private Sub tmrMain_Timer()
Me.ZOrder 0
End Sub

