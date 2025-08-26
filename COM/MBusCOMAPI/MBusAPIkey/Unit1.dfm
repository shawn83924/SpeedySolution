object Form1: TForm1
  Left = 0
  Top = 0
  BorderStyle = bsToolWindow
  Caption = 'MBus API keygen'
  ClientHeight = 268
  ClientWidth = 391
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 19
  object Label3: TLabel
    Left = 8
    Top = 8
    Width = 51
    Height = 19
    Caption = 'Vendor'
  end
  object Label1: TLabel
    Left = 8
    Top = 43
    Width = 59
    Height = 19
    Caption = 'Subjects'
  end
  object FCreateNewKeyButton: TButton
    Left = 226
    Top = 5
    Width = 157
    Height = 52
    Caption = 'Generate Reg'
    TabOrder = 1
    OnClick = FCreateNewKeyButtonClick
  end
  object FVendorEdit: TEdit
    Left = 65
    Top = 5
    Width = 145
    Height = 27
    TabOrder = 0
  end
  object FMemo: TMemo
    Left = 8
    Top = 66
    Width = 374
    Height = 193
    TabOrder = 2
  end
  object XPManifest1: TXPManifest
    Left = 344
    Top = 8
  end
  object SaveDialog: TSaveDialog
    Filter = 'Registry file|*.reg'
    Title = 'Save Registry file'
    Left = 184
    Top = 32
  end
end
