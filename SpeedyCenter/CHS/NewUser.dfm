object NewUserForm: TNewUserForm
  Left = 484
  Top = 186
  BorderStyle = bsDialog
  Caption = #26032#22686#20351#29992#32773
  ClientHeight = 231
  ClientWidth = 329
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -17
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 21
  object IDPanel: TPanel
    Left = 0
    Top = 0
    Width = 329
    Height = 41
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    object Label1: TLabel
      Left = 16
      Top = 16
      Width = 85
      Height = 21
      Alignment = taRightJustify
      AutoSize = False
      Caption = #36134#21495
    end
    object IDEdit: TEdit
      Left = 112
      Top = 12
      Width = 168
      Height = 29
      ImeMode = imSAlpha
      TabOrder = 0
    end
  end
  object PwdPanel: TPanel
    Left = 0
    Top = 41
    Width = 329
    Height = 68
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object Label2: TLabel
      Left = 16
      Top = 9
      Width = 85
      Height = 21
      Alignment = taRightJustify
      AutoSize = False
      Caption = #23494#30721
    end
    object Label3: TLabel
      Left = 16
      Top = 41
      Width = 83
      Height = 21
      Alignment = taRightJustify
      AutoSize = False
      Caption = #30830#35748#23494#30721
    end
    object PwdConfirmEdit: TMaskEdit
      Left = 112
      Top = 38
      Width = 169
      Height = 29
      ImeMode = imSAlpha
      PasswordChar = '*'
      TabOrder = 1
      Text = ''
    end
    object PwdEdit: TMaskEdit
      Left = 112
      Top = 5
      Width = 169
      Height = 29
      ImeMode = imSAlpha
      PasswordChar = '*'
      TabOrder = 0
      Text = ''
    end
  end
  object AttrPanel: TPanel
    Left = 0
    Top = 109
    Width = 329
    Height = 74
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 2
    object Label4: TLabel
      Left = 16
      Top = 6
      Width = 85
      Height = 21
      Alignment = taRightJustify
      AutoSize = False
      Caption = #26435#38480
    end
    object Label5: TLabel
      Left = 16
      Top = 40
      Width = 85
      Height = 21
      Alignment = taRightJustify
      AutoSize = False
      Caption = #29366#24577
    end
    object StateComboBox: TComboBox
      Left = 112
      Top = 37
      Width = 169
      Height = 29
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 1
      Text = #27491#24120
      Items.Strings = (
        #27491#24120
        #38145#20303
        #23494#30721#36807#26399)
    end
    object GroupComboBox: TComboBox
      Left = 112
      Top = 3
      Width = 169
      Height = 29
      Style = csDropDownList
      TabOrder = 0
    end
  end
  object BtnPanel: TPanel
    Left = 0
    Top = 183
    Width = 329
    Height = 54
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 3
    object OKBtn: TBitBtn
      Left = 63
      Top = 6
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
      NumGlyphs = 2
      TabOrder = 0
      OnClick = OKBtnClick
    end
    object CancelBtn: TBitBtn
      Left = 167
      Top = 6
      Width = 97
      Height = 32
      Caption = #21462#28040
      Kind = bkCancel
      NumGlyphs = 2
      TabOrder = 1
    end
  end
end
