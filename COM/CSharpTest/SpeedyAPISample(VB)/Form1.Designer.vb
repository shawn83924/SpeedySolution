<Global.Microsoft.VisualBasic.CompilerServices.DesignerGenerated()> _
Partial Class Form1
    Inherits System.Windows.Forms.Form

    'Form overrides dispose to clean up the component list.
    <System.Diagnostics.DebuggerNonUserCode()> _
    Protected Overrides Sub Dispose(ByVal disposing As Boolean)
        If disposing AndAlso components IsNot Nothing Then
            components.Dispose()
        End If
        MyBase.Dispose(disposing)
    End Sub

    'Required by the Windows Form Designer
    Private components As System.ComponentModel.IContainer

    'NOTE: The following procedure is required by the Windows Form Designer
    'It can be modified using the Windows Form Designer.  
    'Do not modify it using the code editor.
    <System.Diagnostics.DebuggerStepThrough()> _
    Private Sub InitializeComponent()
        Me.ConnectButton = New System.Windows.Forms.Button
        Me.ListBox = New System.Windows.Forms.ListBox
        Me.LogonButton = New System.Windows.Forms.Button
        Me.FutButton = New System.Windows.Forms.Button
        Me.OptButton = New System.Windows.Forms.Button
        Me.SuspendLayout()
        '
        'ConnectButton
        '
        Me.ConnectButton.Location = New System.Drawing.Point(558, 12)
        Me.ConnectButton.Name = "ConnectButton"
        Me.ConnectButton.Size = New System.Drawing.Size(90, 40)
        Me.ConnectButton.TabIndex = 0
        Me.ConnectButton.Text = "Connect"
        Me.ConnectButton.UseVisualStyleBackColor = True
        '
        'ListBox
        '
        Me.ListBox.FormattingEnabled = True
        Me.ListBox.ItemHeight = 15
        Me.ListBox.Location = New System.Drawing.Point(12, 12)
        Me.ListBox.Name = "ListBox"
        Me.ListBox.Size = New System.Drawing.Size(540, 334)
        Me.ListBox.TabIndex = 1
        '
        'LogonButton
        '
        Me.LogonButton.Location = New System.Drawing.Point(558, 58)
        Me.LogonButton.Name = "LogonButton"
        Me.LogonButton.Size = New System.Drawing.Size(90, 35)
        Me.LogonButton.TabIndex = 2
        Me.LogonButton.Text = "Logon"
        Me.LogonButton.UseVisualStyleBackColor = True
        '
        'FutButton
        '
        Me.FutButton.Location = New System.Drawing.Point(558, 270)
        Me.FutButton.Name = "FutButton"
        Me.FutButton.Size = New System.Drawing.Size(90, 35)
        Me.FutButton.TabIndex = 3
        Me.FutButton.Text = "期貨下單"
        Me.FutButton.UseVisualStyleBackColor = True
        '
        'OptButton
        '
        Me.OptButton.Location = New System.Drawing.Point(558, 311)
        Me.OptButton.Name = "OptButton"
        Me.OptButton.Size = New System.Drawing.Size(90, 35)
        Me.OptButton.TabIndex = 4
        Me.OptButton.Text = "選擇權下單"
        Me.OptButton.UseVisualStyleBackColor = True
        '
        'Form1
        '
        Me.AutoScaleDimensions = New System.Drawing.SizeF(8.0!, 15.0!)
        Me.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font
        Me.ClientSize = New System.Drawing.Size(660, 360)
        Me.Controls.Add(Me.OptButton)
        Me.Controls.Add(Me.FutButton)
        Me.Controls.Add(Me.LogonButton)
        Me.Controls.Add(Me.ListBox)
        Me.Controls.Add(Me.ConnectButton)
        Me.Name = "Form1"
        Me.Text = "Form1"
        Me.ResumeLayout(False)

    End Sub
    Friend WithEvents ConnectButton As System.Windows.Forms.Button
    Friend WithEvents ListBox As System.Windows.Forms.ListBox
    Friend WithEvents LogonButton As System.Windows.Forms.Button
    Friend WithEvents FutButton As System.Windows.Forms.Button
    Friend WithEvents OptButton As System.Windows.Forms.Button

End Class
