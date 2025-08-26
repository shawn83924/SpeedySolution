object SessionForm: TSessionForm
  Left = 315
  Top = 192
  BorderStyle = bsDialog
  Caption = #26032#22686#36899#32218
  ClientHeight = 536
  ClientWidth = 436
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -17
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  PixelsPerInch = 120
  TextHeight = 21
  object Label13: TLabel
    Left = 9
    Top = 84
    Width = 157
    Height = 21
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'FIX'#29256#26412
  end
  object Label14: TLabel
    Left = 9
    Top = 146
    Width = 157
    Height = 21
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'SenderCompID'
  end
  object Label15: TLabel
    Left = 9
    Top = 178
    Width = 157
    Height = 21
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'TargetCompID'
  end
  object Label1: TLabel
    Left = 9
    Top = 115
    Width = 157
    Height = 21
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'FIX SPEC Directory'
  end
  object StartLabel: TLabel
    Left = 9
    Top = 209
    Width = 157
    Height = 21
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Start Time (UTC)'
  end
  object Label2: TLabel
    Left = 9
    Top = 242
    Width = 157
    Height = 21
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'End Time (UTC)'
  end
  object Bevel1: TBevel
    Left = 8
    Top = 490
    Width = 419
    Height = 12
    Shape = bsTopLine
  end
  object AppVLabel: TLabel
    Left = 270
    Top = 84
    Width = 36
    Height = 21
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'App'
  end
  object PageControl: TPageControl
    Left = 7
    Top = 270
    Width = 417
    Height = 165
    ActivePage = AcceptSheet
    TabOrder = 5
    TabStop = False
    object InitiatorSheet: TTabSheet
      Caption = 'Initiator'
      ImageIndex = 1
      TabVisible = False
      object Label4: TLabel
        Left = 14
        Top = 5
        Width = 136
        Height = 21
        Caption = 'Heartbeat Interval'
      end
      object Label5: TLabel
        Left = 260
        Top = 68
        Width = 30
        Height = 21
        Caption = 'Port'
      end
      object Label6: TLabel
        Left = 14
        Top = 68
        Width = 43
        Height = 21
        Caption = 'Host1'
      end
      object Label7: TLabel
        Left = 260
        Top = 98
        Width = 30
        Height = 21
        Caption = 'Port'
      end
      object Label9: TLabel
        Left = 14
        Top = 98
        Width = 43
        Height = 21
        Caption = 'Host2'
      end
      object Label10: TLabel
        Left = 260
        Top = 128
        Width = 30
        Height = 21
        Caption = 'Port'
      end
      object Label11: TLabel
        Left = 14
        Top = 128
        Width = 43
        Height = 21
        Caption = 'Host3'
      end
      object Label3: TLabel
        Left = 14
        Top = 34
        Width = 140
        Height = 21
        Caption = 'Reconnect Interval'
      end
      object Label16: TLabel
        Left = 234
        Top = 3
        Width = 25
        Height = 21
        Caption = 'sec'
      end
      object Label17: TLabel
        Left = 234
        Top = 34
        Width = 25
        Height = 21
        Caption = 'sec'
      end
      object HeartbeatEdit: TEdit
        Left = 187
        Top = 2
        Width = 41
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 3
        TabOrder = 0
        Text = '30'
      end
      object PortEdit: TEdit
        Left = 305
        Top = 64
        Width = 50
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 5
        TabOrder = 2
      end
      object HostEdit: TEdit
        Left = 73
        Top = 64
        Width = 170
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 128
        TabOrder = 1
      end
      object Port1Edit: TEdit
        Left = 305
        Top = 94
        Width = 50
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 5
        TabOrder = 4
      end
      object Host1Edit: TEdit
        Left = 73
        Top = 94
        Width = 170
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 128
        TabOrder = 3
      end
      object Port2Edit: TEdit
        Left = 305
        Top = 124
        Width = 50
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 5
        TabOrder = 6
      end
      object Host2Edit: TEdit
        Left = 73
        Top = 124
        Width = 170
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 128
        TabOrder = 5
      end
      object ReconnectEdit: TEdit
        Left = 187
        Top = 29
        Width = 41
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 3
        TabOrder = 7
        Text = '30'
      end
    end
    object AcceptSheet: TTabSheet
      Caption = 'Acceptor'
      TabVisible = False
      object Label12: TLabel
        Left = 58
        Top = 40
        Width = 101
        Height = 21
        Caption = 'Acceptor Port'
      end
      object Label8: TLabel
        Left = 31
        Top = 73
        Width = 339
        Height = 32
        AutoSize = False
        Caption = #27880#24847':'#25152#26377#30340'Accptor'#37117'Listen'#21516#19968#20491'Port.'
        Font.Charset = DEFAULT_CHARSET
        Font.Color = clRed
        Font.Height = -18
        Font.Name = 'Tahoma'
        Font.Style = []
        ParentFont = False
        WordWrap = True
      end
      object APortEdit: TEdit
        Left = 187
        Top = 37
        Width = 83
        Height = 29
        ImeMode = imSAlpha
        MaxLength = 5
        TabOrder = 0
      end
      object ResetOnLogonCheckBox: TCheckBox
        Left = 31
        Top = 6
        Width = 144
        Height = 26
        Caption = 'Reset on logon'
        TabOrder = 1
      end
    end
  end
  object OKBtn: TBitBtn
    Left = 112
    Top = 498
    Width = 97
    Height = 32
    Caption = #30906#23450
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
    TabOrder = 6
    OnClick = OKBtnClick
  end
  object CancelBtn: TBitBtn
    Left = 215
    Top = 498
    Width = 96
    Height = 32
    Caption = #21462#28040
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 7
  end
  object TypeRadioGroup: TRadioGroup
    Left = 8
    Top = 3
    Width = 417
    Height = 76
    Caption = ' '#36899#32218#22411#24907' '
    ItemIndex = 0
    Items.Strings = (
      'Initiator (Buy side)'
      'Acceptor (Sell side)')
    TabOrder = 0
    OnClick = TypeRadioGroupClick
  end
  object FIXVerComboBox: TComboBox
    Left = 174
    Top = 81
    Width = 91
    Height = 29
    Style = csDropDownList
    ItemIndex = 4
    TabOrder = 1
    Text = 'FIX.4.4'
    OnChange = FIXVerComboBoxChange
    Items.Strings = (
      'FIX.4.0'
      'FIX.4.1'
      'FIX.4.2'
      'FIX.4.3'
      'FIX.4.4'
      'FIXT.1.1')
  end
  object TargetIDEdit: TEdit
    Left = 174
    Top = 175
    Width = 160
    Height = 29
    ImeMode = imSAlpha
    MaxLength = 20
    TabOrder = 4
  end
  object SenderIDEdit: TEdit
    Left = 174
    Top = 143
    Width = 160
    Height = 29
    ImeMode = imSAlpha
    MaxLength = 20
    TabOrder = 3
  end
  object FIXDictComboBox: TComboBox
    Left = 174
    Top = 112
    Width = 246
    Height = 29
    Style = csDropDownList
    ImeMode = imClose
    TabOrder = 2
  end
  object StartHourSpinEdit: TCSpinEdit
    Left = 174
    Top = 206
    Width = 41
    Height = 32
    MaxValue = 23
    TabOrder = 8
  end
  object StartMinutesSpinEdit: TCSpinEdit
    Left = 216
    Top = 206
    Width = 41
    Height = 32
    MaxValue = 59
    TabOrder = 9
  end
  object StartSecSpinEdit: TCSpinEdit
    Left = 258
    Top = 206
    Width = 41
    Height = 32
    MaxValue = 59
    TabOrder = 10
  end
  object EndHourSpinEdit: TCSpinEdit
    Left = 174
    Top = 239
    Width = 41
    Height = 32
    MaxValue = 23
    TabOrder = 11
  end
  object EndMinutesSpinEdit: TCSpinEdit
    Left = 216
    Top = 239
    Width = 41
    Height = 32
    MaxValue = 59
    TabOrder = 12
  end
  object EndSecSpinEdit: TCSpinEdit
    Left = 258
    Top = 239
    Width = 41
    Height = 32
    MaxValue = 59
    TabOrder = 13
  end
  object PersistCheckBox: TCheckBox
    Left = 123
    Top = 441
    Width = 175
    Height = 17
    Caption = 'Persist Messages '
    TabOrder = 14
  end
  object VerifySeqCheckBox: TCheckBox
    Left = 123
    Top = 466
    Width = 254
    Height = 17
    Caption = 'Verify Message Seq Number'
    TabOrder = 15
  end
  object AppVComboBox: TComboBox
    Left = 312
    Top = 81
    Width = 108
    Height = 29
    Style = csDropDownList
    ItemIndex = 7
    TabOrder = 16
    Text = 'FIX.5.0SP2'
    OnChange = AppVComboBoxChange
    Items.Strings = (
      'FIX.4.0'
      'FIX.4.1'
      'FIX.4.2'
      'FIX.4.3'
      'FIX.4.4'
      'FIX.5.0'
      'FIX.5.0SP1'
      'FIX.5.0SP2')
  end
  object StartDayComboBox: TComboBox
    Left = 305
    Top = 208
    Width = 108
    Height = 29
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 17
    Text = 'everyday'
    OnChange = StartDayComboBoxChange
    Items.Strings = (
      'everyday'
      'sunday'
      'monday'
      'tuesday'
      'wednesday'
      'thursday'
      'friday'
      'saturday')
  end
  object EndDayComboBox: TComboBox
    Left = 305
    Top = 241
    Width = 108
    Height = 29
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 18
    Text = 'everyday'
    OnChange = EndDayComboBoxChange
    Items.Strings = (
      'everyday'
      'sunday'
      'monday'
      'tuesday'
      'wednesday'
      'thursday'
      'friday'
      'saturday')
  end
end
