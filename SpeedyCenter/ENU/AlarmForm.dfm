object AlarmSettingForm: TAlarmSettingForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'No trade alarm setting'
  ClientHeight = 198
  ClientWidth = 373
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  Scaled = False
  PixelsPerInch = 120
  TextHeight = 18
  object StartLabel: TLabel
    Left = 8
    Top = 43
    Width = 130
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Begin Time'
  end
  object StopLabel: TLabel
    Left = 8
    Top = 77
    Width = 130
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'End Time'
  end
  object Label3: TLabel
    Left = 8
    Top = 11
    Width = 130
    Height = 18
    Alignment = taRightJustify
    AutoSize = False
    Caption = 'Market'
  end
  object Label1: TLabel
    Left = 8
    Top = 120
    Width = 350
    Height = 21
    AutoSize = False
    Caption = 'No trades within           seconds will alert'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -17
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object Bevel1: TBevel
    Left = 8
    Top = 108
    Width = 343
    Height = 16
    Shape = bsTopLine
  end
  object CStartHourSpinEdit: TCSpinEdit
    Left = 156
    Top = 40
    Width = 40
    Height = 28
    MaxValue = 23
    TabOrder = 0
  end
  object CStartMinutesSpinEdit: TCSpinEdit
    Left = 199
    Top = 40
    Width = 40
    Height = 28
    MaxValue = 59
    TabOrder = 1
  end
  object CStopHourSpinEdit: TCSpinEdit
    Left = 156
    Top = 74
    Width = 40
    Height = 28
    MaxValue = 23
    TabOrder = 2
  end
  object CStopMinutesSpinEdit: TCSpinEdit
    Left = 199
    Top = 74
    Width = 40
    Height = 28
    MaxValue = 59
    TabOrder = 3
  end
  object BitBtn1: TBitBtn
    Left = 78
    Top = 156
    Width = 96
    Height = 32
    Caption = 'OK'
    Default = True
    DoubleBuffered = True
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
    ParentDoubleBuffered = False
    TabOrder = 4
    OnClick = BitBtn1Click
  end
  object BitBtn2: TBitBtn
    Left = 181
    Top = 156
    Width = 96
    Height = 32
    Caption = 'Cancel'
    DoubleBuffered = True
    Kind = bkCancel
    ParentDoubleBuffered = False
    TabOrder = 5
  end
  object MarketComboBox: TComboBox
    Left = 156
    Top = 8
    Width = 149
    Height = 26
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 6
    Text = 'TAIFEX Options'
    Items.Strings = (
      'TAIFEX Options'
      'TAIFEX Futures'
      'TSE'
      'OTC')
  end
  object CStartSecSpinEdit: TCSpinEdit
    Left = 244
    Top = 40
    Width = 40
    Height = 28
    MaxValue = 59
    TabOrder = 7
  end
  object CStopSecSpinEdit: TCSpinEdit
    Left = 244
    Top = 74
    Width = 40
    Height = 28
    MaxValue = 59
    TabOrder = 8
  end
  object TimerComboBox: TComboBox
    Left = 152
    Top = 120
    Width = 49
    Height = 26
    Style = csDropDownList
    ItemIndex = 5
    TabOrder = 9
    Text = '30'
    Items.Strings = (
      '5'
      '10'
      '15'
      '20'
      '25'
      '30'
      '35'
      '40'
      '45'
      '50'
      '55'
      '60'
      '70'
      '80'
      '90'
      '100'
      '110'
      '120'
      '180'
      '240'
      '300'
      '360'
      '420'
      '480'
      '540'
      '600')
  end
end
