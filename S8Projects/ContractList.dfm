object ContractListForm: TContractListForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu]
  BorderStyle = bsSizeToolWin
  Caption = #21830#21697#34920
  ClientHeight = 353
  ClientWidth = 943
  Color = clBtnFace
  Constraints.MinHeight = 400
  Constraints.MinWidth = 600
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = #24494#36575#27491#40657#39636
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnCloseQuery = FormCloseQuery
  OnCreate = FormCreate
  PixelsPerInch = 120
  TextHeight = 19
  object MarketDataList: TMarketDataList
    Left = 170
    Top = 29
    Width = 773
    Height = 324
    BevelEdges = []
    BevelInner = bvNone
    BevelOuter = bvNone
    BorderStyle = bsNone
    Align = alClient
    OnMouseDown = MarketDataListMouseDown
    ViewStyle = vsReport
    Exchange = 'TAIFEX'
    TextColor = clWhite
    OddRowColor = clSkyBlue
    EvenRowColor = 7748106
    OnItemPosChanged = MarketDataListItemPosChanged
    ExplicitLeft = 200
    ExplicitWidth = 743
    ExplicitHeight = 434
  end
  object GraphPanel: TGraphPanel
    Left = 0
    Top = 0
    Width = 943
    Height = 29
    Align = alTop
    BevelOuter = bvNone
    Color = 14515260
    UseDockManager = True
    TabOrder = 1
    StretchGlyph = True
    object ExchangeComboBox: TComboBox
      Left = 30
      Top = 1
      Width = 249
      Height = 27
      BevelInner = bvNone
      BevelOuter = bvNone
      Style = csDropDownList
      DropDownCount = 10
      PopupMenu = CustomPopupMenu
      TabOrder = 0
      OnChange = ExchangeComboBoxChange
      Items.Strings = (
        #33258#36984#21830#21697'-1'
        #33258#36984#21830#21697'-2'
        #33258#36984#21830#21697'-3'
        #33258#36984#21830#21697'-4'
        #33258#36984#21830#21697'-5')
    end
    object SerialComboBox: TComboBox
      Left = 285
      Top = 1
      Width = 181
      Height = 27
      BevelInner = bvNone
      BevelOuter = bvNone
      Style = csDropDownList
      TabOrder = 1
      OnChange = SerialComboBoxChange
      Items.Strings = (
        '')
    end
    object SortButton: TGraphButton
      Tag = 10
      Left = 2
      Top = 0
      Width = 0
      Height = 0
      Hint = #33258#35330#25490#24207
      TabOrder = 2
      ShowHint = True
      OnClick = SortButtonClick
      Transparent = True
      GraphCount = 5
      ButtonID = 2
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = DEFAULT_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 6
    end
    object SmallButton: TGraphButton
      Left = 500
      Top = 0
      Width = 0
      Height = 0
      Hint = #23383#22411#32302#23567
      TabOrder = 3
      ShowHint = True
      OnClick = SmallButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = DEFAULT_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Tahoma'
      FontAttrib.CaptionFont.Style = []
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 9
    end
    object LargeButton: TGraphButton
      Left = 470
      Top = 0
      Width = 0
      Height = 0
      Hint = #23383#22411#21152#22823
      TabOrder = 4
      ShowHint = True
      OnClick = LargeButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = DEFAULT_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Tahoma'
      FontAttrib.CaptionFont.Style = []
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 8
    end
  end
  object ToolSV: TSplitView
    Left = 0
    Top = 29
    Width = 170
    Height = 324
    AnimationDelay = 6
    AnimationStep = 30
    OpenedWidth = 170
    Placement = svpLeft
    TabOrder = 2
    object Label2: TLabel
      Left = 10
      Top = 6
      Width = 60
      Height = 19
      Caption = #25991#23383#38991#33394
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -15
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object Label1: TLabel
      Left = 10
      Top = 54
      Width = 105
      Height = 19
      Caption = #22855#25976#34892#32972#26223#38991#33394
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -15
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object Label3: TLabel
      Left = 10
      Top = 102
      Width = 105
      Height = 19
      Caption = #20598#25976#34892#32972#26223#38991#33394
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -15
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object Label4: TLabel
      Left = 10
      Top = 151
      Width = 60
      Height = 19
      Caption = #19978#28466#38991#33394
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -15
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object Label5: TLabel
      Left = 10
      Top = 199
      Width = 60
      Height = 19
      Caption = #19979#36300#38991#33394
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -15
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object DefColorButton: TSpeedButton
      Left = 28
      Top = 296
      Width = 106
      Height = 27
      Caption = #38928#35373#38991#33394
      Glyph.Data = {
        36050000424D3605000000000000360400002800000010000000100000000100
        0800000000000001000000000000000000000001000000010000000000000101
        0100020202000303030004040400050505000606060007070700080808000909
        09000A0A0A000B0B0B000C0C0C000D120D000F180F00111E1000122412001429
        1300152E140016331500173C160018451700194F18001A5718001B5C19001B62
        19001B6619001B6919001B6C19001B6E19001B7119001A7419001A7619001A78
        19001A7A19001A7D1900197F1900198019001980190019811900198019001980
        19001E801B0025801D002C802000338122003B82260044842A0057873200708C
        3B00869144009B944C00B2995600C19D5D00CCA06200D1A26300D5A46600D5A3
        6400D6A26200D69E5E00D69B5A00D5975500D3914E00D0894400D1843E00D181
        3800D3803300D47F2F00D27E3100CF7C3200CD7A3300C9763300C7743200C270
        3200BE6D3400BA6A3200B5653100B1612E00AF5E2D00AC5C2C00A95A2C00A658
        2C00A2552C009E522C009B4F2B00974C2A00954A290093492800934927009248
        2700904826008F4725008E4723008D4722008B4622008A452200884422008643
        22008542220085422300864224008743250088442B008846310087483B00854B
        4600834E54007F5161007B59740076608600726797006C6CA5006770AF006373
        B8005F75C0005977C9005577D1005377D5005277D800527CDC005280DE005283
        DF005182DD004F80DB004C7BD8004B7BD800497AD7004779D6004172D2003B6C
        CF003665CB003160C8002D5BC4002956C1002552BD00224FB800204FB5001F50
        B3001D4EB0001A4AAE001D50AB002156A6002556A4002957A3002E58A200335B
        A0003D5F9D0047649A0050689700586C9400627191006B768E00747A8B007C7E
        8800828186008483850085848500868584008785840087858400878684008886
        84008886840089878500898785008A8886008B8987008C8B89008F8D8B00918F
        8E009392900096949300989796009B9A99009E9D9C00A2A1A000A4A4A300A7A7
        A600AAAAA900ADADAD00B1B1B100B5B5B500B6B6B600B7B7B700B8B8B800B9B9
        B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
        C100C2C2C200C3C3C300C4C4C400C5C5C500CCCBCA00D3D1CF00D9D6D300E2C1
        DB00EBA1E300F185EA00F56DEE00F85AF200FA49F500FC37F700FD27FA00FD1C
        FB00FE13FC00FE0CFD00FE06FE00FE03FE00FE01FE00FE00FE00FE00FE00FE00
        FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
        FE00FE00FE00FE00FE00FE00FE00FE01FE00FE07FE00FE16FD00FE2AFC00FE51
        F900FE84F400FEA6F100FECAEF00FEE1ED00FEEBED00FEF1ED00FEF4EE00FEF6
        EE00FEF5EC00FEF5EA00FDF3E700FDF1E400FDF0E100FDEFDE00FDECD900FEE9
        D300FEE7CE00FEE5CA00FDE4C800FDE3C600FDE2C400FDE2C400DDDDDDDDDDDD
        DDDDDDDDDDDDDDDDDDDD5356585858585858585858585858555362F1F4A1F8F9
        A1FBFBA1FEFEA1FEFE5A62F5A1A1A1A1A1A1A1A1A1A1A1A1A15C62A1A1F3768B
        F7271AF93569FC798D5C62F1A1F1768BF7271AF93569FC798D5C62A1A1F1768B
        F7271AF9FDFDFC798D5C62F1A1F1768BF72A1AF9FDFDFC798D5C62A1A1F1768B
        F7271AF9FAFBFCFEFE5C62F1A1F1F1F3F72A1AF8F9FAFCFEFE5C63A1A1F1F1F3
        F7F7F7F7F8F9FBFDFE564B3A3939393939393939383A3838364A4F4343434343
        4343434338433834754EDD5050505050505050504E504D5253DDDDDDDDDDDDDD
        DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD}
      OnClick = DefColorButtonClick
    end
    object Label6: TLabel
      Left = 10
      Top = 248
      Width = 90
      Height = 19
      Caption = #21830#21697#25991#23383#38991#33394
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clBlack
      Font.Height = -15
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentColor = False
      ParentFont = False
      Transparent = True
    end
    object FallBKColorBox: TColorBox
      Left = 10
      Top = 217
      Width = 136
      Height = 22
      Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
      DropDownCount = 12
      ItemHeight = 22
      TabOrder = 0
      OnChange = ColorBoxChange
    end
    object RiseBKColorBox: TColorBox
      Left = 10
      Top = 169
      Width = 136
      Height = 22
      Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
      DropDownCount = 12
      ItemHeight = 22
      TabOrder = 1
      OnChange = ColorBoxChange
    end
    object EvenBKColorBox: TColorBox
      Left = 10
      Top = 121
      Width = 136
      Height = 22
      Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
      DropDownCount = 12
      ItemHeight = 22
      TabOrder = 2
      OnChange = ColorBoxChange
    end
    object OddBKColorBox: TColorBox
      Left = 10
      Top = 73
      Width = 136
      Height = 22
      Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
      DropDownCount = 12
      ItemHeight = 22
      TabOrder = 3
      OnChange = ColorBoxChange
    end
    object TextColorBox: TColorBox
      Left = 10
      Top = 25
      Width = 136
      Height = 22
      Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
      DropDownCount = 12
      ItemHeight = 22
      TabOrder = 4
      OnChange = ColorBoxChange
    end
    object SymbolColorBox: TColorBox
      Left = 10
      Top = 266
      Width = 136
      Height = 22
      Style = [cbStandardColors, cbExtendedColors, cbSystemColors, cbIncludeNone, cbCustomColor, cbPrettyNames, cbCustomColors]
      DropDownCount = 12
      ItemHeight = 22
      TabOrder = 5
      OnChange = ColorBoxChange
    end
  end
  object PopupMenu: TPopupMenu
    Left = 264
    Top = 88
    object DelSymbolMenuItem: TMenuItem
      Caption = #21034#38500#36984#21462#21830#21697
      OnClick = DelSymbolMenuItemClick
    end
    object OpenDepthMenuItem: TMenuItem
      Caption = #38283#21855#20116#27284
      OnClick = OpenDepthMenuItemClick
    end
    object OpenKMenuItem: TMenuItem
      Caption = #38283#21855'K'#32218
      OnClick = OpenKMenuItemClick
    end
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
    Left = 424
    Top = 304
  end
  object FitTimer: TTimer
    Enabled = False
    Interval = 200
    OnTimer = FitTimerTimer
    Left = 416
    Top = 176
  end
  object CustomPopupMenu: TPopupMenu
    OnPopup = CustomPopupMenuPopup
    Left = 280
    Top = 232
    object CustomNameMenuItem: TMenuItem
      Caption = #20462#25913#33258#36984#32676#32068#21517#31281
      OnClick = CustomNameMenuItemClick
    end
  end
end
