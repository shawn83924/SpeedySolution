object BrowserForm: TBrowserForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  Caption = 'BrowserForm'
  ClientHeight = 813
  ClientWidth = 1071
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  PixelsPerInch = 120
  TextHeight = 16
  object WebBrowser: TWebBrowser
    Left = 0
    Top = 0
    Width = 1071
    Height = 813
    Align = alClient
    TabOrder = 0
    OnBeforeNavigate2 = WebBrowserBeforeNavigate2
    OnNavigateComplete2 = WebBrowserNavigateComplete2
    ExplicitLeft = 280
    ExplicitTop = 144
    ExplicitWidth = 300
    ExplicitHeight = 150
    ControlData = {
      4C0000008E580000394300000000000000000000000000000000000000000000
      000000004C000000000000000000000001000000E0D057007335CF11AE690800
      2B2E126208000000000000004C0000000114020000000000C000000000000046
      8000000000000000000000000000000000000000000000000000000000000000
      00000000000000000100000000000000000000000000000000000000}
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
    Left = 760
    Top = 88
  end
end
