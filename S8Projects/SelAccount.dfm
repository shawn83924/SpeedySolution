object DefAccForm: TDefAccForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #38928#35373#24115#34399
  ClientHeight = 208
  ClientWidth = 259
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -17
  Font.Name = #24494#36575#27491#40657#39636
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 22
  object Label1: TLabel
    Left = 24
    Top = 85
    Width = 102
    Height = 22
    Caption = #26399#27402#38928#35373#24115#34399
  end
  object Label2: TLabel
    Left = 24
    Top = 8
    Width = 102
    Height = 22
    Caption = #35657#21048#38928#35373#24115#34399
  end
  object CancelButton: TButton
    Left = 135
    Top = 169
    Width = 75
    Height = 25
    Caption = #21462#28040
    ModalResult = 2
    TabOrder = 0
  end
  object OKButton: TButton
    Left = 51
    Top = 169
    Width = 75
    Height = 25
    Caption = #30906#23450
    ModalResult = 1
    TabOrder = 1
  end
  object FUTComboBox: TComboBox
    Left = 24
    Top = 113
    Width = 209
    Height = 30
    Style = csDropDownList
    TabOrder = 2
  end
  object TSEComboBox: TComboBox
    Left = 24
    Top = 36
    Width = 209
    Height = 30
    Style = csDropDownList
    TabOrder = 3
  end
  object RoundFormEx: TRoundFormEx
    Resource = MainForm.RoundFormExRes
    InactiveFontColor = 19
    Lockbox = True
    CaptionFont.Charset = ANSI_CHARSET
    CaptionFont.Color = clWhite
    CaptionFont.Height = -17
    CaptionFont.Name = #24494#36575#27491#40657#39636
    CaptionFont.Style = []
    Left = 8
    Top = 48
  end
end
