object LoginSettingForm: TLoginSettingForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #30331#20837#35373#23450
  ClientHeight = 255
  ClientWidth = 397
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWhite
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnShow = FormShow
  DesignSize = (
    397
    255)
  PixelsPerInch = 96
  TextHeight = 13
  object ValueListEditor: TValueListEditor
    Left = 0
    Top = 0
    Width = 397
    Height = 209
    Align = alTop
    DefaultRowHeight = 24
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = #24494#36575#27491#40657#39636
    Font.Style = [fsBold]
    ParentFont = False
    Strings.Strings = (
      'IP='
      'Port='
      'FutBrokerID='
      'StockBrokerID='
      'ClearMemberID='
      'StockAccount='
      'FutAccount='
      'TryVersion='
      'ProxyLogon=')
    TabOrder = 0
    TitleCaptions.Strings = (
      #21517#31281
      #20540)
    ColWidths = (
      123
      251)
  end
  object OKButton: TButton
    Left = 82
    Top = 220
    Width = 75
    Height = 27
    Anchors = [akLeft, akBottom]
    Caption = #30906#35469
    ModalResult = 1
    TabOrder = 1
    OnClick = OKButtonClick
  end
  object CancelButton: TButton
    Left = 226
    Top = 220
    Width = 75
    Height = 27
    Anchors = [akLeft, akBottom]
    Caption = #21462#28040
    ModalResult = 2
    TabOrder = 2
  end
  object RoundFormEx: TRoundFormEx
    Resource = MainForm.RoundFormExRes
    InactiveFontColor = clWhite
    Lockbox = True
    CaptionFont.Charset = ANSI_CHARSET
    CaptionFont.Color = clWhite
    CaptionFont.Height = -17
    CaptionFont.Name = #24494#36575#27491#40657#39636
    CaptionFont.Style = []
    Left = 360
    Top = 216
  end
end
