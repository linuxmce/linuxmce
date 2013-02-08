Option Strict Off
Option Explicit On
Friend Class frmOutLookUpdate
	Inherits System.Windows.Forms.Form
#Region "Windows Form Designer generated code "
	Public Sub New()
		MyBase.New()
		If m_vb6FormDefInstance Is Nothing Then
			If m_InitializingDefInstance Then
				m_vb6FormDefInstance = Me
			Else
				Try 
					'For the start-up form, the first instance created is the default instance.
					If System.Reflection.Assembly.GetExecutingAssembly.EntryPoint.DeclaringType Is Me.GetType Then
						m_vb6FormDefInstance = Me
					End If
				Catch
				End Try
			End If
		End If
		'This call is required by the Windows Form Designer.
		InitializeComponent()
	End Sub
	'Form overrides dispose to clean up the component list.
	Protected Overloads Overrides Sub Dispose(ByVal Disposing As Boolean)
		If Disposing Then
			If Not components Is Nothing Then
				components.Dispose()
			End If
		End If
		MyBase.Dispose(Disposing)
	End Sub
	'Required by the Windows Form Designer
	Private components As System.ComponentModel.IContainer
	Public ToolTip1 As System.Windows.Forms.ToolTip
	Public WithEvents txtURL As System.Windows.Forms.TextBox
	Public WithEvents txt As System.Windows.Forms.TextBox
	Public WithEvents Winsock1 As AxMSWinsockLib.AxWinsock
	Public WithEvents Command1 As System.Windows.Forms.Button
	Public WithEvents Label4 As System.Windows.Forms.Label
	Public WithEvents Label2 As System.Windows.Forms.Label
	Public WithEvents Label1 As System.Windows.Forms.Label
	'NOTE: The following procedure is required by the Windows Form Designer
	'It can be modified using the Windows Form Designer.
	'Do not modify it using the code editor.
	<System.Diagnostics.DebuggerStepThrough()> Private Sub InitializeComponent()
        Me.components = New System.ComponentModel.Container
        Dim resources As System.Resources.ResourceManager = New System.Resources.ResourceManager(GetType(frmOutLookUpdate))
        Me.ToolTip1 = New System.Windows.Forms.ToolTip(Me.components)
        Me.txtURL = New System.Windows.Forms.TextBox
        Me.txt = New System.Windows.Forms.TextBox
        Me.Winsock1 = New AxMSWinsockLib.AxWinsock
        Me.Command1 = New System.Windows.Forms.Button
        Me.Label4 = New System.Windows.Forms.Label
        Me.Label2 = New System.Windows.Forms.Label
        Me.Label1 = New System.Windows.Forms.Label
        CType(Me.Winsock1, System.ComponentModel.ISupportInitialize).BeginInit()
        Me.SuspendLayout()
        '
        'txtURL
        '
        Me.txtURL.AcceptsReturn = True
        Me.txtURL.AutoSize = False
        Me.txtURL.BackColor = System.Drawing.SystemColors.Window
        Me.txtURL.Cursor = System.Windows.Forms.Cursors.IBeam
        Me.txtURL.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txtURL.ForeColor = System.Drawing.SystemColors.WindowText
        Me.txtURL.Location = New System.Drawing.Point(20, 45)
        Me.txtURL.MaxLength = 0
        Me.txtURL.Name = "txtURL"
        Me.txtURL.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.txtURL.Size = New System.Drawing.Size(297, 29)
        Me.txtURL.TabIndex = 0
        Me.txtURL.Text = "http://dcerouter"
        '
        'txt
        '
        Me.txt.AcceptsReturn = True
        Me.txt.AutoSize = False
        Me.txt.BackColor = System.Drawing.SystemColors.Window
        Me.txt.Cursor = System.Windows.Forms.Cursors.IBeam
        Me.txt.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.txt.ForeColor = System.Drawing.SystemColors.WindowText
        Me.txt.Location = New System.Drawing.Point(15, 203)
        Me.txt.MaxLength = 0
        Me.txt.Multiline = True
        Me.txt.Name = "txt"
        Me.txt.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.txt.ScrollBars = System.Windows.Forms.ScrollBars.Vertical
        Me.txt.Size = New System.Drawing.Size(662, 239)
        Me.txt.TabIndex = 4
        Me.txt.Text = ""
        '
        'Winsock1
        '
        Me.Winsock1.Enabled = True
        Me.Winsock1.Location = New System.Drawing.Point(25, 223)
        Me.Winsock1.Name = "Winsock1"
        Me.Winsock1.OcxState = CType(resources.GetObject("Winsock1.OcxState"), System.Windows.Forms.AxHost.State)
        Me.Winsock1.Size = New System.Drawing.Size(28, 28)
        Me.Winsock1.TabIndex = 5
        '
        'Command1
        '
        Me.Command1.BackColor = System.Drawing.SystemColors.Control
        Me.Command1.Cursor = System.Windows.Forms.Cursors.Default
        Me.Command1.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Command1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Command1.Location = New System.Drawing.Point(210, 134)
        Me.Command1.Name = "Command1"
        Me.Command1.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Command1.Size = New System.Drawing.Size(104, 32)
        Me.Command1.TabIndex = 1
        Me.Command1.Text = "&Update"
        '
        'Label4
        '
        Me.Label4.AutoSize = True
        Me.Label4.BackColor = System.Drawing.SystemColors.Control
        Me.Label4.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label4.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label4.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label4.Location = New System.Drawing.Point(20, 20)
        Me.Label4.Name = "Label4"
        Me.Label4.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label4.Size = New System.Drawing.Size(45, 19)
        Me.Label4.TabIndex = 5
        Me.Label4.Text = "Server"
        '
        'Label2
        '
        Me.Label2.BackColor = System.Drawing.SystemColors.ActiveCaptionText
        Me.Label2.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D
        Me.Label2.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label2.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label2.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label2.Location = New System.Drawing.Point(20, 99)
        Me.Label2.Name = "Label2"
        Me.Label2.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label2.Size = New System.Drawing.Size(294, 27)
        Me.Label2.TabIndex = 3
        '
        'Label1
        '
        Me.Label1.AutoSize = True
        Me.Label1.BackColor = System.Drawing.SystemColors.Control
        Me.Label1.Cursor = System.Windows.Forms.Cursors.Default
        Me.Label1.Font = New System.Drawing.Font("Arial", 8.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.Label1.ForeColor = System.Drawing.SystemColors.ControlText
        Me.Label1.Location = New System.Drawing.Point(20, 78)
        Me.Label1.Name = "Label1"
        Me.Label1.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.Label1.Size = New System.Drawing.Size(48, 19)
        Me.Label1.TabIndex = 2
        Me.Label1.Text = "Status:"
        '
        'frmOutLookUpdate
        '
        Me.AutoScaleBaseSize = New System.Drawing.Size(7, 18)
        Me.BackColor = System.Drawing.SystemColors.Control
        Me.ClientSize = New System.Drawing.Size(682, 455)
        Me.Controls.Add(Me.txtURL)
        Me.Controls.Add(Me.txt)
        Me.Controls.Add(Me.Winsock1)
        Me.Controls.Add(Me.Command1)
        Me.Controls.Add(Me.Label4)
        Me.Controls.Add(Me.Label2)
        Me.Controls.Add(Me.Label1)
        Me.Cursor = System.Windows.Forms.Cursors.Default
        Me.Font = New System.Drawing.Font("Arial", 9.0!, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, CType(0, Byte))
        Me.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog
        Me.Location = New System.Drawing.Point(4, 28)
        Me.MaximizeBox = False
        Me.MinimizeBox = False
        Me.Name = "frmOutLookUpdate"
        Me.RightToLeft = System.Windows.Forms.RightToLeft.No
        Me.ShowInTaskbar = False
        Me.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen
        Me.Text = "Update OutLook"
        CType(Me.Winsock1, System.ComponentModel.ISupportInitialize).EndInit()
        Me.ResumeLayout(False)

    End Sub
#End Region 
#Region "Upgrade Support "
	Private Shared m_vb6FormDefInstance As frmOutLookUpdate
	Private Shared m_InitializingDefInstance As Boolean
	Public Shared Property DefInstance() As frmOutLookUpdate
		Get
			If m_vb6FormDefInstance Is Nothing OrElse m_vb6FormDefInstance.IsDisposed Then
				m_InitializingDefInstance = True
				m_vb6FormDefInstance = New frmOutLookUpdate()
				m_InitializingDefInstance = False
			End If
			DefInstance = m_vb6FormDefInstance
		End Get
		Set
			m_vb6FormDefInstance = Value
		End Set
	End Property
#End Region 
	Dim fs As New Scripting.FileSystemObject
	Dim Website As String
	Dim Port As String
	Dim blnConnected As Boolean
	Dim StrFilename As String
	Dim Resp, Reqs As String
	
	Dim mx As New MSXML2.XMLHTTP
	Dim ID, myNamespace, myFolder, myOlApp As Object
	Dim mvar As String
    Dim mArr(300, 15) As String
	Dim IdArr() As Object
	Private Function ReadIni(ByRef operationType As String) As Object
		If operationType = "I" Then
			'UPGRADE_WARNING: Couldn't resolve default property of object ReadIni. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
            ReadIni = Trim(txtURL.Text) & "/lmce-admin/outlook_sync/insert.php"
		Else
			'UPGRADE_WARNING: Couldn't resolve default property of object ReadIni. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
            ReadIni = Trim(txtURL.Text) & "/lmce-admin/outlook_sync/extract.php"
		End If
	End Function
	
	Private Sub Command1_Click(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles Command1.Click
		Dim J As Object
		Dim I As Object
		Dim arrrows As Object
		Dim mUrl As Object
        Dim Arr As Object
 		Dim mstring, mRecord As String
		Dim mCArr As Short
		Label2.Text = " Synchronizing with MySql table..."
		'UPGRADE_WARNING: Couldn't resolve default property of object ReadIni(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		'UPGRADE_WARNING: Couldn't resolve default property of object mUrl. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		mUrl = ReadIni("E")
		'UPGRADE_WARNING: Couldn't resolve default property of object mUrl. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
        mx.open("GET", mUrl, "False")
		mx.send()
		
		Label2.Text = " Retrieving Data from MySql..."
		mvar = mx.responseText
		Label2.Text = " Synchronizing.."
		mstring = mvar
		If Trim(mstring) <> "" Then
			Label2.Text = " Updating Contacts in MS OutLook..."
			Label2.Refresh()
			'UPGRADE_WARNING: Couldn't resolve default property of object arrrows. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
			arrrows = Split(mstring, "|")
			mCArr = UBound(arrrows)
			ReDim mArr(mCArr, 15)
			For I = 0 To mCArr - 1
				'UPGRADE_WARNING: Couldn't resolve default property of object arrrows(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				'UPGRADE_WARNING: Couldn't resolve default property of object Arr. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				Arr = Split(arrrows(I), "~",  , CompareMethod.Text)
				For J = 0 To UBound(Arr)
					'UPGRADE_WARNING: Couldn't resolve default property of object J. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
					'UPGRADE_WARNING: Couldn't resolve default property of object I. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
					'UPGRADE_WARNING: Couldn't resolve default property of object Arr(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
					mArr(I, J) = Arr(J)
				Next 
			Next 
			' update outlook data from mysql tables
			UpdateOutLook()
			' prepare outlook data to be exported to mysql. Taking only those contacts which were not updated from mysql
			Label2.Text = " Preparing Contacts in MS OutLook..."
			Label2.Refresh()
			
			PutData2MySql()
			' actually updating the web data
			Label2.Text = "Uploading Contacts..."
			Label2.Refresh()
			Callweb()
			MsgBox(" Synchronization Complete...", MsgBoxStyle.Information, "Complete")
			FileOpen(1, VB6.GetPath & "\Setup.ini", OpenMode.Output)
			PrintLine(1, "Website=" & Trim(txtURL.Text))
			FileClose(1)
			'''''''End
		Else
			MsgBox("Some problem at the WEB end. Please check the php files or the URL")
		End If
		
	End Sub
	
	Private Sub frmOutLookUpdate_Load(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles MyBase.Load
		Dim St As Object
		Dim olFolderContacts As Object
		Dim mUnForm As Boolean
        Dim mArr(1, 15)
		myOlApp = CreateObject("Outlook.Application")
		'UPGRADE_WARNING: Couldn't resolve default property of object myOlApp.GetNamespace. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		myNamespace = myOlApp.GetNamespace("MAPI")
		'UPGRADE_WARNING: Couldn't resolve default property of object olFolderContacts. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		olFolderContacts = 10
		'UPGRADE_WARNING: Couldn't resolve default property of object myNamespace.GetDefaultFolder. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		myFolder = myNamespace.GetDefaultFolder(olFolderContacts)
		Label2.Text = " Click on update to Start Synchronizing."
		Dim Arr As Object
		'UPGRADE_WARNING: Dir has a new behavior. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1041"'
		If Dir(VB6.GetPath & "\setup.ini") <> "" Then
			FileOpen(1, VB6.GetPath & "\Setup.ini", OpenMode.Input)
			Do While Not EOF(1)
				St = LineInput(1)
				'UPGRADE_WARNING: Couldn't resolve default property of object St. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				'UPGRADE_WARNING: Couldn't resolve default property of object Arr. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				Arr = Split(St, "=")
				If (UBound(Arr)) = 1 Then
					'UPGRADE_WARNING: Couldn't resolve default property of object Arr(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
					If UCase(Trim(Arr(0))) = "WEBSITE" Then
						'UPGRADE_WARNING: Couldn't resolve default property of object Arr(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
						txtURL.Text = Trim(Arr(1))
					End If
				End If
			Loop 
			FileClose(1)
		End If
		
	End Sub
	Private Function UpdateOutLook() As Object
		Dim olContactItem As Object
        'On Error GoTo errh
		Dim I As Short
		Dim Phone As String
		'UPGRADE_WARNING: Couldn't resolve default property of object olContactItem. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		olContactItem = 2
        Dim IdArr(UBound(mArr))
		
		'UPGRADE_WARNING: Couldn't resolve default property of object myFolder.Items. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		For	Each ID In myFolder.Items
			'UPGRADE_WARNING: TypeName has a new behavior. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1041"'
			'UPGRADE_WARNING: Couldn't resolve default property of object ID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
            If UCase(TypeName(ID)) = "CONTACTITEM" Then
                For I = 0 To UBound(mArr) - 1
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.CustomerID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    If Trim(mArr(I, 0)) = ID.CustomerID Then
                        mArr(I, 15) = "Y"
                        IdArr(I) = ID
                        '                ID.body = ""
                    End If
                Next
            End If
        Next ID
		I = 0
		Do While I < UBound(mArr)
			Phone = ""
			If Trim(mArr(I, 6)) <> "" Then
				Phone = Phone & "+" & Trim(mArr(I, 6))
			End If
			If Trim(mArr(I, 7)) <> "" Then
				Phone = Phone & IIf(Phone = "", "", " ") & "(" & Trim(mArr(I, 7)) & ")"
			End If
			If Trim(mArr(I, 8)) <> "" Then
				Phone = Phone & IIf(Phone = "", "", " ") & Trim(mArr(I, 8))
			End If
			If Trim(mArr(I, 9)) <> "" Then
				Phone = Phone & IIf(Phone = "", "", " x ") & Trim(mArr(I, 9))
			End If
			If mArr(I, 15) = "Y" Then
				ID = IdArr(I)
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.CustomerID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.CustomerID = Trim(mArr(I, 0))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.FirstName. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.FirstName = mArr(I, 1)
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.CompanyName. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.CompanyName = Trim(mArr(I, 2))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.Department. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.Department = Trim(mArr(I, 3))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.JobTitle. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.JobTitle = Trim(mArr(I, 4))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.Email1Address. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.Email1Address = Trim(mArr(I, 5))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.CustomerID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				Do While mArr(I, 0) = ID.CustomerID
					Select Case UCase(Trim(mArr(I, 10)))
						Case Is = "HOME"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.HomeTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.HomeTelephoneNumber = Phone
						Case Is = "WORK"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.BusinessTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.BusinessTelephoneNumber = Phone
						Case Is = "MOBILE"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.MobileTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.MobileTelephoneNumber = Phone
						Case Is = "PAGER"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.PagerNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.PagerNumber = Phone
						Case Is = "FAX"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.BusinessFaxNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.BusinessFaxNumber = Phone
						Case Else
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.body. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.body = ID.body & IIf(ID.body <> "", vbCrLf, "") & mArr(I, 10) & " : " & Phone
					End Select
					I = I + 1
				Loop 
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.Save. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.Save()
			Else
				'UPGRADE_WARNING: Couldn't resolve default property of object myOlApp.CreateItem. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID = myOlApp.CreateItem(olContactItem)
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.CustomerID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.CustomerID = Trim(mArr(I, 0))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.FirstName. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.FirstName = mArr(I, 1)
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.CompanyName. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.CompanyName = Trim(mArr(I, 2))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.Department. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.Department = Trim(mArr(I, 3))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.JobTitle. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.JobTitle = Trim(mArr(I, 4))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.Email1Address. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.Email1Address = Trim(mArr(I, 5))
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.CustomerID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				Do While mArr(I, 0) = ID.CustomerID
					Select Case UCase(Trim(mArr(I, 10)))
						Case Is = "HOME"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.HomeTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.HomeTelephoneNumber = Phone
						Case Is = "WORK"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.BusinessTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.BusinessTelephoneNumber = Phone
						Case Is = "MOBILE"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.MobileTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.MobileTelephoneNumber = Phone
						Case Is = "PAGER"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.PagerNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.PagerNumber = Phone
						Case Is = "FAX"
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.BusinessFaxNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.BusinessFaxNumber = Phone
						Case Else
							'UPGRADE_WARNING: Couldn't resolve default property of object ID.body. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
							ID.body = ID.body & IIf(ID.body <> "", vbCrLf, "") & mArr(I, 10) & " : " & Phone
					End Select
					I = I + 1
				Loop 
				'UPGRADE_WARNING: Couldn't resolve default property of object ID.Save. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				ID.Save()
			End If
		Loop 
		Exit Function
errh: 
		MsgBox(Err.Description & " Missing standard field ")
		Err.Clear()
		Resume Next
	End Function
	Private Function PutData2MySql() As Object
		Dim olContactItem As Object
        'On Error GoTo errh
		Dim mFoundEntry As Boolean
		Dim I As Short
		Dim blnIdFound As Boolean
		Dim blnSameCustomer As Boolean
		Dim Phone As String
        Dim Arr1() As Object
		Dim Arr2() As Object
		Dim St As String
		'UPGRADE_WARNING: Couldn't resolve default property of object olContactItem. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		olContactItem = 2
		ReDim IdArr(1)
		
		FileClose()
		FileOpen(1, VB6.GetPath & "\Insert.txt", OpenMode.Output)
		PrintLine(1, "xcevw12e9f5kj") 'Security Code To be checked again in server
		'UPGRADE_WARNING: Couldn't resolve default property of object myFolder.Items. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		For	Each ID In myFolder.Items
			'UPGRADE_WARNING: TypeName has a new behavior. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1041"'
			'UPGRADE_WARNING: Couldn't resolve default property of object ID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
            If UCase(TypeName(ID)) = "CONTACTITEM" Then
                blnIdFound = False
                For I = 0 To UBound(mArr) - 1
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.CustomerID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    If Trim(mArr(I, 0)) = ID.CustomerID Then
                        blnIdFound = True
                        Exit For
                    End If
                Next I
                If Not blnIdFound Then
                    'We need To Insert
                    'Insert into person(CustomerID,Name,Company,JobDescription,Title,Email)
                    IdArr(UBound(IdArr)) = ID
                    ReDim Preserve IdArr(UBound(IdArr) + 1)
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.Email1Address. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.JobTitle. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.Department. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.CompanyName. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.FileAs. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.EntryID. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    PrintLine(1, ID.EntryID & "~" & ID.FileAs & "~" & ID.CompanyName & "~" & ID.Department & "~" & ID.JobTitle & "~" & ID.Email1Address & "")
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.HomeTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    If ID.HomeTelephoneNumber <> "" Then
                        'UPGRADE_WARNING: Couldn't resolve default property of object ID.HomeTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                        Call PrintPhone(1, "Home", ID.HomeTelephoneNumber)
                    End If

                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.BusinessTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    If ID.BusinessTelephoneNumber <> "" Then
                        'UPGRADE_WARNING: Couldn't resolve default property of object ID.BusinessTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                        Call PrintPhone(1, "Work", ID.BusinessTelephoneNumber)
                    End If
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.MobileTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    If ID.MobileTelephoneNumber <> "" Then
                        'UPGRADE_WARNING: Couldn't resolve default property of object ID.MobileTelephoneNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                        Call PrintPhone(1, "Mobile", ID.MobileTelephoneNumber)
                    End If
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.PagerNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    If ID.PagerNumber <> "" Then
                        'UPGRADE_WARNING: Couldn't resolve default property of object ID.PagerNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                        Call PrintPhone(1, "Pager", ID.PagerNumber)
                    End If
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.BusinessFaxNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    If ID.BusinessFaxNumber <> "" Then
                        'UPGRADE_WARNING: Couldn't resolve default property of object ID.BusinessFaxNumber. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                        Call PrintPhone(1, "Fax", ID.BusinessFaxNumber)
                    End If
                    'UPGRADE_WARNING: Couldn't resolve default property of object ID.body. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                    If ID.body <> "" Then
                        'UPGRADE_WARNING: Couldn't resolve default property of object ID.body. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                        St = ID.body
                        Arr1 = Split(St, vbLf, , CompareMethod.Text)
                        For I = 0 To UBound(Arr1) - 1
                            'UPGRADE_WARNING: Couldn't resolve default property of object Arr1(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                            Arr2 = Split(Arr1(I), ":")
                            'UPGRADE_WARNING: Couldn't resolve default property of object Arr2(1). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                            'UPGRADE_WARNING: Couldn't resolve default property of object Arr2(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
                            Call PrintPhone(1, Arr2(0) & "", IIf(UBound(Arr2) > 0, Arr2(1) & "", ""))
                        Next
                    End If
                    PrintLine(1, "")
                End If

            End If

        Next ID
		FileClose(1)
		
		
		
		Exit Function
errh: 
		' MsgBox Err.Description & " Missing standard field "
		Err.Clear()
		Resume Next
	End Function
	Private Function PrintPhone(ByRef Buff As Short, ByRef PhoneType As String, ByRef Phone As String) As Object
		Dim St As String
		Dim Arr As Object
		Dim I As Short
		Dim Phoneno, Country, Area, Extn As String
		I = 1
		If Mid(Phone, I, 1) = "+" Then 'Country Code Follwed
			I = I + 1
			Do While Mid(Phone, I, 1) <> " "
				Country = Country & Mid(Phone, I, 1)
				I = I + 1
			Loop 
		End If
		'UPGRADE_WARNING: Couldn't resolve default property of object Arr. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		Arr = Split(Trim(Mid(Phone, I)), " ")
		For I = 0 To UBound(Arr)
			If I = 0 Then
				'UPGRADE_WARNING: Couldn't resolve default property of object Arr(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				If InStr(1, Trim(Arr(I)), ")", CompareMethod.Text) > 0 Then
					'UPGRADE_WARNING: Couldn't resolve default property of object Arr(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
					Area = Replace(Replace(Trim(Arr(I)), "(", "",  ,  , CompareMethod.Text), ")", "",  ,  , CompareMethod.Text)
				Else
					'UPGRADE_WARNING: Couldn't resolve default property of object Arr(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
					Phoneno = Arr(I)
				End If
			ElseIf I = 1 Then 
				'UPGRADE_WARNING: Couldn't resolve default property of object Arr(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				Phoneno = Phoneno & " " & Arr(I)
			ElseIf I = 3 Then 
				'UPGRADE_WARNING: Couldn't resolve default property of object Arr(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
				Extn = Arr(I)
			End If
		Next 
		'PhoneNumber(FK_Contact,FK_PhoneType,Countrycode,AreaCode,PhoneNumber,Extension)
		PrintLine(Buff, PhoneType)
		PrintLine(Buff, "'" & Country & "','" & Area & "','" & Phoneno & "','" & Extn & "'")
	End Function
	
	' this function builds a http request baes on the following parameters:
	'
	' data = the data from the file to be uploaded
	' DestUrl = a URL to containing information on where to send the data
	' UploadName = the field upload name usually pass by <input type="file" name="uploadname"
	' Filename = the name of the file
	' The MIME type of the file
	Private Function BuildFileUploadRequest(ByRef strData As String, ByRef DestUrl As URL, ByVal UploadName As String, ByVal FileName As String, ByVal MimeType As String) As String
		
		Dim strHttp As String ' holds the entire HTTP request
		Dim strBoundary As String 'the boundary between each entity
		Dim strBody As String ' holds the body of the HTTP request
		Dim lngLength As Integer ' the length of the HTTP request
		
		' create a boundary consisting of a random string
		'UPGRADE_WARNING: Couldn't resolve default property of object RandomAlphaNumString(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		strBoundary = RandomAlphaNumString(32)
		
		' create the body of the http request in the form
		'
		' --boundary
		' Content-Disposition: form-data; name="UploadName"; filename="FileName"
		' Content-Type: MimeType
		'
		' file data here
		'--boundary--
		strBody = "--" & strBoundary & vbCrLf
		strBody = strBody & "Content-Disposition: form-data; name=""" & "fileinsert" & """; filename=""" & VB6.GetPath & "\insert.txt" & """" & vbCrLf
		strBody = strBody & "Content-Type: " & MimeType & vbCrLf
		strBody = strBody & vbCrLf & strData
		strBody = strBody & vbCrLf & "--" & strBoundary & "--"
		
		' find the length of the request body - this is required for the
		' Content-Length header
		lngLength = Len(strBody)
		
		' construct the HTTP request in the form:
		'
		' POST /path/to/reosurce HTTP/1.0
		' Host: host
		' Content-Type: multipart-form-data, boundary=boundary
		' Content-Length: len(strbody)
		'
		' HTTP request body
		strHttp = "POST " & DestUrl.URI & "?" & DestUrl.Query & " HTTP/1.0" & vbCrLf
		strHttp = strHttp & "Host: " & DestUrl.Host & vbCrLf
		strHttp = strHttp & "Content-Type: multipart/form-data, boundary=" & strBoundary & vbCrLf
		strHttp = strHttp & "Content-Length: " & lngLength & vbCrLf & vbCrLf
		strHttp = strHttp & strBody
		
		BuildFileUploadRequest = strHttp
	End Function
	' this is executed when winsock sneds us data
	' in our case it will be the HTTP response
	Private Sub Winsock1_DataArrival(ByVal eventSender As System.Object, ByVal eventArgs As AxMSWinsockLib.DMSWinsockControlEvents_DataArrivalEvent) Handles Winsock1.DataArrival
		Dim strData As String
		Winsock1.GetData(strData, VariantType.String, eventArgs.bytesTotal)
		Resp = Resp & strData
		txt.Text = Resp
		txt.Refresh()
	End Sub
	
	' this is executed when winsock generates an error
	Private Sub Winsock1_Error(ByVal eventSender As System.Object, ByVal eventArgs As AxMSWinsockLib.DMSWinsockControlEvents_ErrorEvent) Handles Winsock1.Error
		MsgBox(eventArgs.Description, MsgBoxStyle.Exclamation, "ERROR")
		Winsock1.Close()
	End Sub
	
	' this is executed when we make a connection to winsock and it has been accepted
	Private Sub Winsock1_ConnectEvent(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles Winsock1.ConnectEvent
		blnConnected = True
	End Sub
	
	' this is executed when the connection to winsock is closed
	Private Sub Winsock1_CloseEvent(ByVal eventSender As System.Object, ByVal eventArgs As System.EventArgs) Handles Winsock1.CloseEvent
		Winsock1.Close()
		blnConnected = False
		
	End Sub
	
	' this function retireves the contents of a file and returns it as a string
	' this is also ture for binary files
	Private Function GetFileContents(ByVal strPath As String) As String
		Dim StrReturn As String
		Dim lngLength As Integer
		
		lngLength = FileLen(strPath)
		StrReturn = New String(Chr(0), lngLength)
		
		On Error GoTo ERR_HANDLER
		
		FileOpen(1, strPath, OpenMode.Binary)
		
		'UPGRADE_WARNING: Get was upgraded to FileGet and has a new behavior. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1041"'
		FileGet(1, StrReturn)
		
		GetFileContents = StrReturn
		
		FileClose(1)
		
		Exit Function
		
ERR_HANDLER: 
		MsgBox(Err.Description, MsgBoxStyle.Critical, "ERROR")
		
		Err.Clear()
	End Function
	
	' generates a random alphanumeirc string of a given length
	Private Function RandomAlphaNumString(ByVal intLen As Short) As Object
		Dim StrReturn As String
		
		Dim X As Short
		Dim c As Byte
		
		Randomize()
		
		For X = 1 To intLen
			c = Int(Rnd() * 127)
			
			If (c >= Asc("0") And c <= Asc("9")) Or (c >= Asc("A") And c <= Asc("Z")) Or (c >= Asc("a") And c <= Asc("z")) Then
				
				StrReturn = StrReturn & Chr(c)
			Else
				X = X - 1
			End If
		Next X
		
		'UPGRADE_WARNING: Couldn't resolve default property of object RandomAlphaNumString. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		RandomAlphaNumString = StrReturn
	End Function
	
	
	' this routine does all the work - it gathers the info required for the HTTP request
	' and sends it via the winsock control
	Private Sub Callweb()
		Dim strFile As String
		Dim strHttp As String
		Dim DestUrl As URL
		'UPGRADE_WARNING: Couldn't resolve default property of object ReadIni(). Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		Website = ReadIni("I")
		
		StrFilename = VB6.GetPath & "\insert.txt"
		
		' if a request is allredy being sent
		' exit
		If blnConnected Then Exit Sub
		
		' check that a file was selected
		If StrFilename = vbNullString Then
			MsgBox("No File Chosen", MsgBoxStyle.Critical, "ERROR")
			
			Exit Sub
		End If
		
		' extract the URL using a helper function
		'UPGRADE_WARNING: Couldn't resolve default property of object DestUrl. Click for more: 'ms-help://MS.VSCC.2003/commoner/redir/redirect.htm?keyword="vbup1037"'
		DestUrl = ExtractUrl(Website)
		
		If DestUrl.Host = vbNullString Then
			MsgBox("Invalid Host", MsgBoxStyle.Critical, "ERROR")
			Exit Sub
		End If
		
		' clear the old response
		Resp = ""
		
		' read the file contents as a string
		' N.B: in HTTP everything is a string, even binary files
		strFile = GetFileContents(StrFilename)
		
		' build the HTTP request
		strHttp = BuildFileUploadRequest(strFile, DestUrl, "fileinsert", "insert.txt", "text/plain")
		
		' assign the protocol host and port
		Winsock1.Protocol = MSWinsockLib.ProtocolConstants.sckTCPProtocol
		Winsock1.RemoteHost = DestUrl.Host
		
		If DestUrl.Port <> 0 Then
			Winsock1.RemotePort = DestUrl.Port
		Else
			Winsock1.RemotePort = 80
		End If
		
		' make the connection and send the HTTP request
		Winsock1.Connect()
		
		While Not blnConnected
			System.Windows.Forms.Application.DoEvents()
		End While
		
		Reqs = strHttp
		Winsock1.SendData(strHttp)
	End Sub
End Class
