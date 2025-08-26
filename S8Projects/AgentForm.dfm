object AgentMain: TAgentMain
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = 'Speedy Unify Update'
  ClientHeight = 351
  ClientWidth = 463
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -19
  Font.Name = #24494#36575#27491#40657#39636
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnShow = FormShow
  DesignSize = (
    463
    351)
  PixelsPerInch = 120
  TextHeight = 24
  object OKButton: TButton
    Left = 164
    Top = 298
    Width = 107
    Height = 39
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akRight, akBottom]
    Caption = #30906#35469
    Enabled = False
    ModalResult = 1
    TabOrder = 0
    OnClick = OKButtonClick
    ExplicitLeft = 178
    ExplicitTop = 284
  end
  object Memo: TMemo
    Left = 10
    Top = 10
    Width = 444
    Height = 278
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Anchors = [akLeft, akTop, akRight, akBottom]
    HideSelection = False
    Lines.Strings = (
      '')
    ReadOnly = True
    TabOrder = 1
    ExplicitWidth = 455
    ExplicitHeight = 263
  end
  object CheckProcessTimer: TTimer
    Enabled = False
    Interval = 50
    OnTimer = CheckProcessTimerTimer
    Left = 100
    Top = 61
  end
  object UpdateTimer: TTimer
    Enabled = False
    Interval = 50
    OnTimer = UpdateTimerTimer
    Left = 244
    Top = 133
  end
end
