object VersionForm: TVersionForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = #36664#20837#29256#26412
  ClientHeight = 107
  ClientWidth = 176
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  Scaled = False
  PixelsPerInch = 96
  TextHeight = 19
  object Label1: TLabel
    Left = 9
    Top = 9
    Width = 40
    Height = 19
    Caption = 'Major'
  end
  object Label2: TLabel
    Left = 64
    Top = 9
    Width = 40
    Height = 19
    Caption = 'Minor'
  end
  object Label3: TLabel
    Left = 121
    Top = 9
    Width = 35
    Height = 19
    Caption = 'Build'
  end
  object MajorSpin: TCSpinEdit
    Left = 9
    Top = 34
    Width = 48
    Height = 29
    MaxValue = 99
    MinValue = 1
    TabOrder = 0
    Value = 1
  end
  object MinorSpin: TCSpinEdit
    Left = 64
    Top = 34
    Width = 48
    Height = 29
    MaxValue = 99
    TabOrder = 1
  end
  object BuildSpin: TCSpinEdit
    Left = 121
    Top = 34
    Width = 48
    Height = 29
    MaxValue = 99
    TabOrder = 2
  end
  object BitBtn1: TBitBtn
    Left = 9
    Top = 69
    Width = 77
    Height = 30
    Caption = #30906#23450
    Default = True
    DoubleBuffered = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
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
    ParentFont = False
    TabOrder = 3
  end
  object BitBtn2: TBitBtn
    Left = 90
    Top = 69
    Width = 77
    Height = 30
    Caption = #21462#28040
    DoubleBuffered = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = []
    Kind = bkCancel
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 4
  end
end
