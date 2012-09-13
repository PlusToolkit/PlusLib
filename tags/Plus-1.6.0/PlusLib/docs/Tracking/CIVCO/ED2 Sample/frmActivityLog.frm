VERSION 5.00
Object = "{F9043C88-F6F2-101A-A3C9-08002B2F49FB}#1.2#0"; "comdlg32.ocx"
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Begin VB.Form frmActivityLog 
   Caption         =   "Activity Log"
   ClientHeight    =   4380
   ClientLeft      =   165
   ClientTop       =   450
   ClientWidth     =   6225
   Icon            =   "frmActivityLog.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   4380
   ScaleWidth      =   6225
   Begin MSComDlg.CommonDialog CommonDialog1 
      Left            =   2880
      Top             =   1980
      _ExtentX        =   847
      _ExtentY        =   847
      _Version        =   393216
   End
   Begin MSComctlLib.ListView lvLog 
      Height          =   4335
      Left            =   30
      TabIndex        =   0
      Top             =   30
      Width           =   6195
      _ExtentX        =   10927
      _ExtentY        =   7646
      View            =   3
      MultiSelect     =   -1  'True
      LabelWrap       =   -1  'True
      HideSelection   =   -1  'True
      _Version        =   393217
      ForeColor       =   -2147483640
      BackColor       =   -2147483643
      BorderStyle     =   1
      Appearance      =   1
      NumItems        =   4
      BeginProperty ColumnHeader(1) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         Text            =   "Description"
         Object.Width           =   5741
      EndProperty
      BeginProperty ColumnHeader(2) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   1
         Text            =   "Module Address"
         Object.Width           =   2540
      EndProperty
      BeginProperty ColumnHeader(3) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   2
         Text            =   "TimeStamp"
         Object.Width           =   3387
      EndProperty
      BeginProperty ColumnHeader(4) {BDD1F052-858B-11D1-B16A-00C0F0283628} 
         SubItemIndex    =   3
         Text            =   "Repeat Count"
         Object.Width           =   2540
      EndProperty
   End
   Begin VB.Menu mnuFile 
      Caption         =   "&File"
      Begin VB.Menu mnuFileClear 
         Caption         =   "Clea&r All"
      End
      Begin VB.Menu mnuFileSaveAs 
         Caption         =   "Save &As..."
      End
      Begin VB.Menu mnuFileSep1 
         Caption         =   "-"
      End
      Begin VB.Menu mnuFileClose 
         Caption         =   "&Close"
      End
   End
End
Attribute VB_Name = "frmActivityLog"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit
Public Event LogCleared()

Private m_PreviouslySaveAsFileName As String

Private Sub Form_KeyPress(KeyAscii As Integer)
    If KeyAscii = vbKeyEscape Then Unload Me
End Sub

Private Sub Form_QueryUnload(Cancel As Integer, UnloadMode As Integer)
    If UnloadMode = 0 Then
        Hide
        Cancel = 1
    End If
End Sub

Private Sub Form_Resize()
    On Error Resume Next
    lvLog.Move 0, 0, ScaleWidth, ScaleHeight
End Sub

Private Sub lvLog_KeyPress(KeyAscii As Integer)
    If KeyAscii = vbKeyEscape Then Unload Me
End Sub

Private Sub mnuFileClear_Click()
    lvLog.ListItems.Clear
    RaiseEvent LogCleared
End Sub

Private Sub mnuFileSaveAs_Click()
    On Error GoTo errHandler
    Dim intFileNum As Integer
    Dim itemX As MSComctlLib.ListItem '  ComctlLib.ListItem
    Dim col As Integer
    Dim sData As String
    
    CommonDialog1.CancelError = True
    CommonDialog1.DefaultExt = "txt"
    CommonDialog1.InitDir = App.Path
    CommonDialog1.FileName = "SEILog.txt"
    CommonDialog1.ShowSave
    m_PreviouslySaveAsFileName = CommonDialog1.FileName
    If m_PreviouslySaveAsFileName = "" Then Exit Sub
    intFileNum = FreeFile
    
    ' Open the file for output
    Open m_PreviouslySaveAsFileName For Output As #intFileNum
    
    ' Loop through each item in the sample data listview...
    For Each itemX In lvLog.ListItems
        sData = itemX.Text
        For col = 1 To lvLog.ColumnHeaders.Count - 1
            sData = sData & "," & itemX.SubItems(col)
        Next col
        Print #intFileNum, sData
    Next itemX
    
    ' Close the file
    Close intFileNum
    
    Trace "Detail Data Saved to " & m_PreviouslySaveAsFileName
    
    Exit Sub
errHandler:
    Debug.Print "Error attempting to save detail data: " & Err.Description
    On Error Resume Next
    Err.Clear
    Close intFileNum
    Exit Sub
    Resume
End Sub

Public Sub ClearModuleErrors(ByVal lAddr As Long)
    Dim I As Integer
    ' Loop through each log entry in the listview...
    For I = lvLog.ListItems.Count To 1 Step -1
        If Val(lvLog.ListItems.Item(I).SubItems(1)) = lAddr Then lvLog.ListItems.Remove I
    Next I
End Sub
