object ContractListColorSettingForm: TContractListColorSettingForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsDialog
  Caption = #21830#21697#34920#38991#33394#35373#23450
  ClientHeight = 218
  ClientWidth = 297
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poScreenCenter
  Scaled = False
  OnShow = FormShow
  DesignSize = (
    297
    218)
  PixelsPerInch = 96
  TextHeight = 18
  object GraphPanel: TGraphPanel
    Left = 0
    Top = 0
    Width = 297
    Height = 218
    Align = alClient
    BevelOuter = bvNone
    UseDockManager = True
    TabOrder = 7
    StretchGlyph = True
    object Label1: TLabel
      Left = 25
      Top = 43
      Width = 105
      Height = 18
      Caption = #22855#25976#34892#32972#26223#38991#33394
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clCream
      Font.Height = -15
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object Label2: TLabel
      Left = 25
      Top = 11
      Width = 60
      Height = 18
      Caption = #25991#23383#38991#33394
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clCream
      Font.Height = -15
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object Label3: TLabel
      Left = 25
      Top = 75
      Width = 105
      Height = 18
      Caption = #20598#25976#34892#32972#26223#38991#33394
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clCream
      Font.Height = -15
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object Label4: TLabel
      Left = 25
      Top = 107
      Width = 60
      Height = 18
      Caption = #19978#28466#38991#33394
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clCream
      Font.Height = -15
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object Label5: TLabel
      Left = 25
      Top = 139
      Width = 60
      Height = 18
      Caption = #19979#36300#38991#33394
      Color = clBtnFace
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clCream
      Font.Height = -15
      Font.Name = 'Tahoma'
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
  end
  object TextColorBox: TColorBox
    Left = 139
    Top = 8
    Width = 136
    Height = 22
    Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
    DropDownCount = 12
    ItemHeight = 22
    TabOrder = 0
  end
  object OddBKColorBox: TColorBox
    Left = 139
    Top = 40
    Width = 136
    Height = 22
    Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
    DropDownCount = 12
    ItemHeight = 22
    TabOrder = 1
  end
  object EvenBKColorBox: TColorBox
    Left = 139
    Top = 72
    Width = 136
    Height = 22
    Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
    DropDownCount = 12
    ItemHeight = 22
    TabOrder = 2
  end
  object RiseBKColorBox: TColorBox
    Left = 139
    Top = 104
    Width = 136
    Height = 22
    Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
    DropDownCount = 12
    ItemHeight = 22
    TabOrder = 3
  end
  object FallBKColorBox: TColorBox
    Left = 139
    Top = 136
    Width = 136
    Height = 22
    Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
    DropDownCount = 12
    ItemHeight = 22
    TabOrder = 4
  end
  object OKButton: TGraphButton
    Left = 32
    Top = 179
    Width = 0
    Height = 0
    TabOrder = 5
    OnClick = OKButtonClick
    Anchors = []
    GraphCount = 3
    FontAttrib.FontColor = clWhite
    FontAttrib.HighlightFontColor = clWhite
    FontAttrib.HighlightShadowColor = clSilver
    FontAttrib.CaptionFont.Charset = DEFAULT_CHARSET
    FontAttrib.CaptionFont.Color = clWhite
    FontAttrib.CaptionFont.Height = -17
    FontAttrib.CaptionFont.Name = 'Tahoma'
    FontAttrib.CaptionFont.Style = []
    SizeFitBmp = True
    ButtonText = #30906#23450
    ImageList = MainToolForm.BtnImageList
    ImageIndex = 0
  end
  object CancelButton: TGraphButton
    Left = 163
    Top = 179
    Width = 0
    Height = 0
    TabOrder = 6
    OnClick = CancelButtonClick
    Anchors = []
    GraphCount = 3
    FontAttrib.FontColor = clWhite
    FontAttrib.HighlightFontColor = clWhite
    FontAttrib.HighlightShadowColor = clSilver
    FontAttrib.CaptionFont.Charset = DEFAULT_CHARSET
    FontAttrib.CaptionFont.Color = clWhite
    FontAttrib.CaptionFont.Height = -17
    FontAttrib.CaptionFont.Name = 'Tahoma'
    FontAttrib.CaptionFont.Style = []
    SizeFitBmp = True
    ButtonText = #21462#28040
    ImageList = MainToolForm.BtnImageList
    ImageIndex = 0
  end
  object RoundFormEx: TRoundFormEx
    Resource = MainToolForm.SmallRoundFormExRes
    Radius = 5
    InactiveFontColor = 19
    Lockbox = True
    CaptionFont.Charset = DEFAULT_CHARSET
    CaptionFont.Color = clCream
    CaptionFont.Height = -17
    CaptionFont.Name = 'Tahoma'
    CaptionFont.Style = []
    Left = 7
    Top = 168
  end
end
