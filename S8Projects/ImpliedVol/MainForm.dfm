object Form1: TForm1
  Left = 0
  Top = 0
  Caption = 'Form1'
  ClientHeight = 178
  ClientWidth = 376
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 120
  TextHeight = 16
  object Button1: TButton
    Left = 40
    Top = 32
    Width = 89
    Height = 25
    Caption = 'Normal ND'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 40
    Top = 63
    Width = 89
    Height = 25
    Caption = 'Optimize ND'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 40
    Top = 94
    Width = 89
    Height = 25
    Caption = 'LUT ND'
    TabOrder = 2
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 152
    Top = 32
    Width = 89
    Height = 25
    Caption = 'Implied Vol'
    TabOrder = 3
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 152
    Top = 63
    Width = 89
    Height = 25
    Caption = 'Implied Vol'
    TabOrder = 4
    OnClick = Button5Click
  end
end
