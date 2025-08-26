object UnifyDlgs: TUnifyDlgs
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #37325#26032#21629#21517
  ClientHeight = 92
  ClientWidth = 316
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
  DesignSize = (
    316
    92)
  PixelsPerInch = 120
  TextHeight = 22
  object MsgLabel: TLabel
    Left = 6
    Top = 9
    Width = 303
    Height = 42
    Alignment = taCenter
    Anchors = [akLeft, akTop, akRight, akBottom]
    AutoSize = False
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = #24494#36575#27491#40657#39636
    Font.Style = []
    ParentFont = False
  end
  object NameEdit: TEdit
    Left = 26
    Top = 18
    Width = 263
    Height = 30
    TabStop = False
    TabOrder = 2
    Text = 'NameEdit'
    OnKeyPress = NameEditKeyPress
  end
  object OKButton: TButton
    Left = 74
    Top = 56
    Width = 75
    Height = 27
    Anchors = [akLeft, akBottom]
    Caption = #30906#35469
    ModalResult = 1
    TabOrder = 0
  end
  object CancelButton: TButton
    Left = 154
    Top = 56
    Width = 75
    Height = 27
    Anchors = [akLeft, akBottom]
    Caption = #21462#28040
    ModalResult = 2
    TabOrder = 1
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
    Left = 256
    Top = 48
  end
end
