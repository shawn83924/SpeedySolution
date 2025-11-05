object PVCSettingForm: TPVCSettingForm
  Left = 114
  Top = 140
  BorderStyle = bsDialog
  Caption = 'New Session'
  ClientHeight = 341
  ClientWidth = 352
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 18
  object BrokerLabel: TLabel
    Left = 8
    Top = 40
    Width = 115
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Broker ID'
  end
  object CMorIPLabel: TLabel
    Left = 8
    Top = 180
    Width = 115
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Clear Member'
  end
  object PortLabel: TLabel
    Left = 13
    Top = 123
    Width = 110
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Port'
  end
  object PVCLabel: TLabel
    Left = 13
    Top = 98
    Width = 110
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'PVC'
  end
  object APCodeLabel: TLabel
    Left = 13
    Top = 68
    Width = 110
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = #26989#21209#21029
  end
  object PasswordLabel: TLabel
    Left = 8
    Top = 152
    Width = 115
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = #32218#36335#23494#30908
  end
  object MarketLabel: TLabel
    Left = 13
    Top = 12
    Width = 110
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = #31995#32113#21029
  end
  object Bevel: TBevel
    Left = 13
    Top = 236
    Width = 318
    Height = 11
    Shape = bsTopLine
  end
  object LocalPortURLLabel: TLabel
    Left = 8
    Top = 208
    Width = 115
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Local Port'
  end
  object PVCIDLabel: TLabel
    Left = 81
    Top = 98
    Width = 42
    Height = 18
    Alignment = taRightJustify
    Caption = 'PVCID'
  end
  object ProtocolLabel: TLabel
    Left = 206
    Top = 245
    Width = 57
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = #21332#23450
  end
  object GroupLabel: TLabel
    Left = 199
    Top = 125
    Width = 39
    Height = 18
    Caption = 'Group'
  end
  object TWSETPLabel: TLabel
    Left = 199
    Top = 273
    Width = 64
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = #27969#37327#21934#20301
  end
  object APCodeComboBox: TComboBox
    Left = 136
    Top = 65
    Width = 185
    Height = 26
    Style = csDropDownList
    TabOrder = 2
    OnChange = APCodeComboBoxChange
    Items.Strings = (
      '1:File Transfer'
      '2:File Transfer'
      '3:Fill Execution'
      '4:Order'
      '8:CM Confirm'
      '9:CM Filled'
      '10:Market Data')
  end
  object OKBitBtn: TBitBtn
    Left = 80
    Top = 301
    Width = 97
    Height = 32
    Caption = 'OK'
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
    TabOrder = 9
    OnClick = OKBitBtnClick
  end
  object CancelBitBtn: TBitBtn
    Left = 184
    Top = 301
    Width = 97
    Height = 32
    Caption = 'Cancel'
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 10
  end
  object LinkEdit: TEdit
    Left = 136
    Top = 121
    Width = 57
    Height = 26
    ImeMode = imSAlpha
    MaxLength = 3
    TabOrder = 4
  end
  object PVCEdit: TEdit
    Left = 136
    Top = 93
    Width = 57
    Height = 26
    ImeMode = imSAlpha
    MaxLength = 3
    TabOrder = 3
  end
  object SysComboBox: TComboBox
    Left = 136
    Top = 9
    Width = 97
    Height = 26
    Style = csDropDownList
    TabOrder = 0
    OnChange = SysComboBoxChange
  end
  object LinePWDEdit: TEdit
    Left = 136
    Top = 149
    Width = 65
    Height = 26
    ImeMode = imSAlpha
    MaxLength = 4
    TabOrder = 5
  end
  object CMEdit: TEdit
    Left = 136
    Top = 177
    Width = 65
    Height = 26
    ImeMode = imSAlpha
    MaxLength = 4
    TabOrder = 6
  end
  object FCMEdit: TEdit
    Left = 136
    Top = 37
    Width = 97
    Height = 26
    ImeMode = imSAlpha
    MaxLength = 7
    TabOrder = 1
  end
  object LocalPortURLEdit: TEdit
    Left = 136
    Top = 205
    Width = 65
    Height = 26
    ImeMode = imSAlpha
    MaxLength = 5
    TabOrder = 7
  end
  object OffHourCheckBox: TCheckBox
    Left = 77
    Top = 273
    Width = 100
    Height = 17
    Caption = #19979#21320#30436
    TabOrder = 8
  end
  object PVCIDEdit: TEdit
    Left = 136
    Top = 93
    Width = 39
    Height = 26
    ImeMode = imSAlpha
    MaxLength = 2
    TabOrder = 11
  end
  object EnabledCheckBox: TCheckBox
    Left = 77
    Top = 246
    Width = 76
    Height = 17
    Caption = #21855#29992
    TabOrder = 12
  end
  object ProtocolComboBox: TComboBox
    Left = 266
    Top = 242
    Width = 67
    Height = 26
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 13
    Text = 'TMP'
    Items.Strings = (
      'TMP'
      'FIX')
  end
  object GroupEdit: TEdit
    Left = 244
    Top = 120
    Width = 39
    Height = 26
    ImeMode = imSAlpha
    MaxLength = 3
    TabOrder = 14
  end
  object TWSETPComboBox: TComboBox
    Left = 266
    Top = 269
    Width = 67
    Height = 26
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 15
    Text = '1'
    Items.Strings = (
      '1'
      '2'
      '3'
      '4'
      '5'
      '6'
      '7'
      '8'
      '9'
      '10')
  end
end
