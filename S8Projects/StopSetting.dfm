object StopSettingForm: TStopSettingForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #35320#20729#21934#26781#20214#35373#23450
  ClientHeight = 155
  ClientWidth = 362
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = #24494#36575#27491#40657#39636
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 19
  object BuyRadioGroup: TRadioGroup
    Left = 8
    Top = 8
    Width = 169
    Height = 97
    Caption = #22996#35351#20729#26684#36984#25799'-'#36023#36914
    Items.Strings = (
      #24066#20729'IOC'#21934
      #25104#20132#20729'               '#27284)
    TabOrder = 0
  end
  object SellRadioGroup: TRadioGroup
    Left = 183
    Top = 8
    Width = 169
    Height = 97
    Caption = #22996#35351#20729#26684#36984#25799'-'#36067#20986
    Items.Strings = (
      #24066#20729'IOC'#21934
      #25104#20132#20729'               '#27284)
    TabOrder = 1
  end
  object BuyStopTickComboBox: TComboBox
    Left = 82
    Top = 68
    Width = 59
    Height = 27
    Style = csDropDownList
    DropDownCount = 12
    ItemIndex = 50
    TabOrder = 2
    Text = '0'
    Items.Strings = (
      '+50'
      '+49'
      '+48'
      '+47'
      '+46'
      '+45'
      '+44'
      '+43'
      '+42'
      '+41'
      '+40'
      '+39'
      '+38'
      '+37'
      '+36'
      '+35'
      '+34'
      '+33'
      '+32'
      '+31'
      '+30'
      '+29'
      '+28'
      '+27'
      '+26'
      '+25'
      '+24'
      '+23'
      '+22'
      '+21'
      '+20'
      '+19'
      '+18'
      '+17'
      '+16'
      '+15'
      '+14'
      '+13'
      '+12'
      '+11'
      '+10'
      '+9'
      '+8'
      '+7'
      '+6'
      '+5'
      '+4'
      '+3'
      '+2'
      '+1'
      '0'
      '-1'
      '-2'
      '-3'
      '-4'
      '-5'
      '-6'
      '-7'
      '-8'
      '-9'
      '-10'
      '-11'
      '-12'
      '-13'
      '-14'
      '-15'
      '-16'
      '-17'
      '-18'
      '-19'
      '-20'
      '-21'
      '-22'
      '-23'
      '-24'
      '-25'
      '-26'
      '-27'
      '-28'
      '-29'
      '-30'
      '-31'
      '-32'
      '-33'
      '-34'
      '-35'
      '-36'
      '-37'
      '-38'
      '-39'
      '-40'
      '-41'
      '-42'
      '-43'
      '-44'
      '-45'
      '-46'
      '-47'
      '-48'
      '-49'
      '-50')
  end
  object SellStopTickComboBox: TComboBox
    Left = 257
    Top = 68
    Width = 59
    Height = 27
    Style = csDropDownList
    DropDownCount = 12
    ItemIndex = 50
    TabOrder = 3
    Text = '0'
    Items.Strings = (
      '+50'
      '+49'
      '+48'
      '+47'
      '+46'
      '+45'
      '+44'
      '+43'
      '+42'
      '+41'
      '+40'
      '+39'
      '+38'
      '+37'
      '+36'
      '+35'
      '+34'
      '+33'
      '+32'
      '+31'
      '+30'
      '+29'
      '+28'
      '+27'
      '+26'
      '+25'
      '+24'
      '+23'
      '+22'
      '+21'
      '+20'
      '+19'
      '+18'
      '+17'
      '+16'
      '+15'
      '+14'
      '+13'
      '+12'
      '+11'
      '+10'
      '+9'
      '+8'
      '+7'
      '+6'
      '+5'
      '+4'
      '+3'
      '+2'
      '+1'
      '0'
      '-1'
      '-2'
      '-3'
      '-4'
      '-5'
      '-6'
      '-7'
      '-8'
      '-9'
      '-10'
      '-11'
      '-12'
      '-13'
      '-14'
      '-15'
      '-16'
      '-17'
      '-18'
      '-19'
      '-20'
      '-21'
      '-22'
      '-23'
      '-24'
      '-25'
      '-26'
      '-27'
      '-28'
      '-29'
      '-30'
      '-31'
      '-32'
      '-33'
      '-34'
      '-35'
      '-36'
      '-37'
      '-38'
      '-39'
      '-40'
      '-41'
      '-42'
      '-43'
      '-44'
      '-45'
      '-46'
      '-47'
      '-48'
      '-49'
      '-50')
  end
  object OKButton: TButton
    Left = 100
    Top = 111
    Width = 78
    Height = 31
    Caption = #30906#23450
    TabOrder = 4
    OnClick = OKButtonClick
  end
  object CancelButton: TButton
    Left = 181
    Top = 111
    Width = 78
    Height = 31
    Caption = #21462#28040
    TabOrder = 5
    OnClick = CancelButtonClick
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
    Left = 56
    Top = 112
  end
end
