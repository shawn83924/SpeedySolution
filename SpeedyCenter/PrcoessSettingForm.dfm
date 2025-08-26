object ProcessForm: TProcessForm
  Left = 446
  Top = 283
  BorderStyle = bsDialog
  Caption = 'Start/Stop Time Setting'
  ClientHeight = 315
  ClientWidth = 414
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -18
  Font.Name = 'Arial'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  Scaled = False
  DesignSize = (
    414
    315)
  PixelsPerInch = 120
  TextHeight = 21
  object Label3: TLabel
    Left = 12
    Top = 9
    Width = 70
    Height = 21
    Caption = 'Process:'
  end
  object ProcessLabel: TLabel
    Left = 86
    Top = 9
    Width = 315
    Height = 21
    AutoSize = False
    Caption = 'ProcessLabel'
    Font.Charset = ANSI_CHARSET
    Font.Color = clNavy
    Font.Height = -18
    Font.Name = 'Arial'
    Font.Style = []
    ParentFont = False
  end
  object WatchDogGroupBox: TGroupBox
    Left = 11
    Top = 148
    Width = 390
    Height = 117
    Caption = '      Enable Watch Dog'
    TabOrder = 3
    object CheckLabel: TLabel
      Left = 13
      Top = 39
      Width = 178
      Height = 21
      Caption = 'Check process every  '
    end
    object RestartLabel: TLabel
      Left = 11
      Top = 77
      Width = 171
      Height = 21
      Caption = 'Restart process up to'
    end
    object MinutesLabel: TLabel
      Left = 255
      Top = 39
      Width = 67
      Height = 21
      Caption = 'seconds'
    end
    object TimesLabel: TLabel
      Left = 255
      Top = 77
      Width = 42
      Height = 21
      Caption = 'times'
    end
    object WatchSecondsSpinEdit: TCSpinEdit
      Left = 189
      Top = 36
      Width = 56
      Height = 32
      Increment = 5
      MaxValue = 600
      MinValue = 5
      TabOrder = 0
      Value = 20
      OnChange = CStartHourSpinEditChange
      OnKeyPress = CStartHourSpinEditKeyPress
    end
    object RestartTimesSpinEdit: TCSpinEdit
      Left = 189
      Top = 74
      Width = 56
      Height = 32
      MaxValue = 30
      MinValue = 1
      TabOrder = 1
      Value = 3
      OnChange = CStartHourSpinEditChange
      OnKeyPress = CStartHourSpinEditKeyPress
    end
    object WatchDogCheckBox: TCheckBox
      Left = 13
      Top = 3
      Width = 18
      Height = 15
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
  end
  object AutoStartGroupBox: TGroupBox
    Left = 11
    Top = 37
    Width = 390
    Height = 101
    Caption = '     Auto Start/Stop process '
    TabOrder = 2
    object StartLabel: TLabel
      Left = 13
      Top = 44
      Width = 82
      Height = 21
      Caption = 'Start Time'
    end
    object StopLabel: TLabel
      Left = 192
      Top = 42
      Width = 81
      Height = 21
      Caption = 'Stop Time'
    end
    object NextDayLabel: TLabel
      Left = 192
      Top = 73
      Width = 158
      Height = 21
      AutoSize = False
      Caption = '(Stop at next day)'
      Font.Charset = ANSI_CHARSET
      Font.Color = clRed
      Font.Height = -18
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
      Transparent = True
    end
    object CStartHourSpinEdit: TCSpinEdit
      Left = 98
      Top = 39
      Width = 41
      Height = 32
      MaxValue = 23
      TabOrder = 0
      OnChange = CStartHourSpinEditChange
      OnKeyPress = CStartHourSpinEditKeyPress
    end
    object CStartMinutesSpinEdit: TCSpinEdit
      Left = 141
      Top = 39
      Width = 41
      Height = 32
      MaxValue = 59
      TabOrder = 1
      OnChange = CStartHourSpinEditChange
      OnKeyPress = CStartHourSpinEditKeyPress
    end
    object CStopHourSpinEdit: TCSpinEdit
      Left = 278
      Top = 38
      Width = 41
      Height = 32
      MaxValue = 23
      TabOrder = 2
      OnChange = CStartHourSpinEditChange
      OnKeyPress = CStartHourSpinEditKeyPress
    end
    object CStopMinutesSpinEdit: TCSpinEdit
      Left = 321
      Top = 38
      Width = 41
      Height = 32
      MaxValue = 59
      TabOrder = 3
      OnChange = CStartHourSpinEditChange
      OnKeyPress = CStartHourSpinEditKeyPress
    end
    object AutoCheckBox: TCheckBox
      Left = 13
      Top = 3
      Width = 18
      Height = 15
      Checked = True
      State = cbChecked
      TabOrder = 4
      OnClick = AutoCheckBoxClick
    end
  end
  object BitBtn1: TBitBtn
    Left = 96
    Top = 272
    Width = 97
    Height = 32
    Anchors = [akLeft, akBottom]
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
    ModalResult = 1
    NumGlyphs = 2
    TabOrder = 0
  end
  object BitBtn2: TBitBtn
    Left = 200
    Top = 272
    Width = 97
    Height = 32
    Anchors = [akLeft, akBottom]
    Caption = #21462#28040
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 1
  end
end
