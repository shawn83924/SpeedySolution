object AddValueForm: TAddValueForm
  Left = 0
  Top = 0
  BorderStyle = bsDialog
  Caption = 'Add Name-Value'
  ClientHeight = 133
  ClientWidth = 256
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poOwnerFormCenter
  Scaled = False
  PixelsPerInch = 120
  TextHeight = 16
  object Label1: TLabel
    Left = 15
    Top = 11
    Width = 42
    Height = 16
    Caption = 'Section'
  end
  object Label2: TLabel
    Left = 15
    Top = 40
    Width = 33
    Height = 16
    Caption = 'Name'
  end
  object Label3: TLabel
    Left = 15
    Top = 70
    Width = 32
    Height = 16
    Caption = 'Value'
  end
  object OkBtn: TBitBtn
    Left = 43
    Top = 101
    Width = 85
    Height = 26
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
    TabOrder = 0
    OnClick = OkBtnClick
  end
  object CancelBtn: TBitBtn
    Left = 136
    Top = 101
    Width = 84
    Height = 26
    Caption = #21462#28040
    Kind = bkCancel
    NumGlyphs = 2
    TabOrder = 1
  end
  object SectionComboBox: TComboBox
    Left = 65
    Top = 8
    Width = 145
    Height = 24
    Style = csDropDownList
    TabOrder = 2
  end
  object NameEdit: TEdit
    Left = 65
    Top = 37
    Width = 121
    Height = 24
    TabOrder = 3
  end
  object ValueEdit: TEdit
    Left = 65
    Top = 67
    Width = 121
    Height = 24
    TabOrder = 4
  end
end
