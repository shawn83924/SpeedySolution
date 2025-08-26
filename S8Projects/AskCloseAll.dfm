object AskCloseAllForm: TAskCloseAllForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #20840#37096#37096#20301#24179#20489
  ClientHeight = 203
  ClientWidth = 333
  Color = clBtnFace
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -18
  Font.Name = #24494#36575#27491#40657#39636
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 23
  object Label1: TLabel
    Left = 24
    Top = 112
    Width = 126
    Height = 23
    Caption = #24179#20489#21934#27599#31558#24373#25976
  end
  object Label2: TLabel
    Left = 236
    Top = 57
    Width = 36
    Height = 23
    Caption = #24179#20489
  end
  object RadioButton1: TRadioButton
    Left = 17
    Top = 61
    Width = 112
    Height = 17
    Caption = #20197#25104#20132#20729
    Checked = True
    TabOrder = 2
    TabStop = True
  end
  object CloseAllStepComboBox: TComboBox
    Left = 119
    Top = 54
    Width = 105
    Height = 31
    Margins.Left = 8
    Margins.Top = 8
    Margins.Right = 8
    Margins.Bottom = 8
    AutoComplete = False
    Style = csDropDownList
    ItemIndex = 0
    TabOrder = 0
    Text = #21152#19968#27284
    Items.Strings = (
      #21152#19968#27284
      #21152#20108#27284
      #21152#19977#27284
      #21152#22235#27284
      #21152#20116#27284
      #21152#20845#27284
      #21152#19971#27284
      #21152#20843#27284
      #21152#20061#27284
      #21152#21313#27284
      #21152'11'#27284
      #21152'12'#27284
      #21152'13'#27284
      #21152'14'#27284
      #21152'15'#27284
      #21152'16'#27284
      #21152'17'#27284
      #21152'18'#27284
      #21152'19'#27284
      #21152'20'#27284
      #21152'21'#27284
      #21152'22'#27284
      #21152'23'#27284
      #21152'24'#27284
      #21152'25'#27284
      #21152'26'#27284
      #21152'27'#27284
      #21152'28'#27284
      #21152'29'#27284
      #21152'30'#27284)
  end
  object MarketRadioButton: TRadioButton
    Left = 16
    Top = 24
    Width = 113
    Height = 17
    Caption = #24066#20729#21934#24179#20489
    TabOrder = 1
  end
  object OKButton: TButton
    Left = 70
    Top = 153
    Width = 107
    Height = 31
    Caption = #30906#23450#22519#34892
    ModalResult = 1
    TabOrder = 3
  end
  object CancelButton: TButton
    Left = 180
    Top = 153
    Width = 78
    Height = 31
    Caption = #21462#28040
    ModalResult = 2
    TabOrder = 4
  end
  object LotsPerOrderEdit: TEdit
    Left = 158
    Top = 108
    Width = 53
    Height = 29
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    DoubleBuffered = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -18
    Font.Name = 'Arial'
    Font.Style = []
    ImeMode = imDisable
    MaxLength = 3
    ParentDoubleBuffered = False
    ParentFont = False
    TabOrder = 5
    Text = '10'
  end
  object LotsUpDown: TUpDown
    Left = 211
    Top = 108
    Width = 27
    Height = 29
    Margins.Left = 5
    Margins.Top = 5
    Margins.Right = 5
    Margins.Bottom = 5
    Associate = LotsPerOrderEdit
    DoubleBuffered = True
    Max = 499
    ParentDoubleBuffered = False
    Position = 10
    TabOrder = 6
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
    Left = 272
    Top = 8
  end
end
