object StopOrderSettingForm: TStopOrderSettingForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  Caption = #35320#20729#21934#26781#20214#35373#23450
  ClientHeight = 144
  ClientWidth = 363
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  PixelsPerInch = 120
  TextHeight = 18
  object BuyRadioGroup: TRadioGroup
    Left = 8
    Top = 8
    Width = 169
    Height = 97
    Caption = #22996#35351#20729#26684#36984#25799'-'#36023#36914
    Items.Strings = (
      #24066#20729'IOC'#21934
      #25104#20132#20729'             '#27284)
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
      #25104#20132#20729'             '#27284)
    TabOrder = 1
  end
  object ComboBox2: TComboBox
    Left = 82
    Top = 72
    Width = 55
    Height = 26
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
  object ComboBox4: TComboBox
    Left = 258
    Top = 72
    Width = 55
    Height = 26
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
  object Button1: TButton
    Left = 185
    Top = 112
    Width = 78
    Height = 31
    Caption = #30906#23450
    TabOrder = 4
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 266
    Top = 112
    Width = 78
    Height = 31
    Caption = #21462#28040
    TabOrder = 5
    OnClick = Button2Click
  end
end
