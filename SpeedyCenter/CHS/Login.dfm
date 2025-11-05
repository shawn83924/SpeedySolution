object LoginForm: TLoginForm
  Left = 730
  Top = 311
  BorderStyle = bsDialog
  Caption = #30331#20837
  ClientHeight = 138
  ClientWidth = 339
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -18
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 22
  object UserNameLabel: TLabel
    Left = 32
    Top = 19
    Width = 90
    Height = 22
    Caption = #20351#29992#32773#21517#31216
  end
  object PasswordLabel: TLabel
    Left = 32
    Top = 59
    Width = 36
    Height = 22
    Caption = #23494#30721
  end
  object IDEdit: TEdit
    Left = 136
    Top = 16
    Width = 169
    Height = 30
    ImeMode = imSAlpha
    TabOrder = 0
    Text = 'Simon'
  end
  object PwdEdit: TMaskEdit
    Left = 136
    Top = 56
    Width = 169
    Height = 30
    ImeMode = imClose
    PasswordChar = '*'
    TabOrder = 1
    Text = ''
  end
  object OkBtn: TBitBtn
    Left = 71
    Top = 96
    Width = 97
    Height = 32
    Caption = #30830#23450
    Default = True
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    ModalResult = 1
    NumGlyphs = 2
    TabOrder = 2
    OnClick = OkBtnClick
  end
  object CancelBtn: TBitBtn
    Left = 175
    Top = 96
    Width = 97
    Height = 32
    Caption = #21462#28040
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 3
  end
  object AutoLogonTimer: TTimer
    Enabled = False
    Interval = 300
    OnTimer = AutoLogonTimerTimer
    Left = 64
    Top = 56
  end
end
