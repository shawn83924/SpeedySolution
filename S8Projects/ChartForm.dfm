object LineChartForm: TLineChartForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'ChartForm'
  ClientHeight = 1059
  ClientWidth = 1514
  Color = clBtnFace
  Constraints.MinHeight = 750
  Constraints.MinWidth = 1000
  Font.Charset = ANSI_CHARSET
  Font.Color = clWindowText
  Font.Height = -19
  Font.Name = #24494#36575#27491#40657#39636
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  Touch.InteractiveGestures = [igPressAndTap]
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 24
  object SettingPanel: TGraphPanel
    Left = 0
    Top = 0
    Width = 1514
    Height = 36
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    Align = alTop
    BevelOuter = bvNone
    Color = 14515260
    UseDockManager = True
    TabOrder = 0
    StretchGlyph = True
    object AtAOELabel: TLabel
      Left = 859
      Top = 9
      Width = 85
      Height = 22
      Caption = #19979#21934#21069#30906#35469
      Font.Charset = ANSI_CHARSET
      Font.Color = clWhite
      Font.Height = -17
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentFont = False
      StyleElements = []
      OnClick = AtAOELabelClick
    end
    object AutoZoomButton: TGraphButton
      Tag = 1
      Left = 410
      Top = 5
      Width = 27
      Height = 27
      Hint = #33258#21205#32302#25918
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 0
      ShowHint = True
      OnClick = ZoomXYButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 5
    end
    object ShowQtyButton: TGraphButton
      Tag = 10
      Left = 240
      Top = 5
      Width = 27
      Height = 27
      Hint = #39023#31034'/'#24433#34255#32218#22294#32302#22294
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 1
      ShowHint = True
      OnClick = ShowQtyButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -19
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold, fsItalic]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 1
    end
    object KBar1Button: TGraphButton
      Tag = 1
      Left = 40
      Top = 5
      Width = 27
      Height = 27
      Hint = #19968#20998#37912
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 2
      ShowHint = True
      OnClick = KBar1ButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clCream
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.SelectedFontColor = 14515260
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clCream
      FontAttrib.CaptionFont.Height = -15
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = []
      SizeFitBmp = True
      ButtonText = '1'
      ImageList = MainForm.ToolImageList
      ImageIndex = 0
    end
    object KBar10Button: TGraphButton
      Tag = 5
      Left = 100
      Top = 5
      Width = 27
      Height = 27
      Hint = #20116#20998#37912
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 3
      ShowHint = True
      OnClick = KBar1ButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clCream
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.SelectedFontColor = 14515260
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clCream
      FontAttrib.CaptionFont.Height = -15
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = []
      SizeFitBmp = True
      ButtonText = '5'
      ImageList = MainForm.ToolImageList
      ImageIndex = 0
    end
    object KBar15Button: TGraphButton
      Tag = 10
      Left = 130
      Top = 5
      Width = 27
      Height = 27
      Hint = #21313#20998#37912
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 4
      ShowHint = True
      OnClick = KBar1ButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clCream
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.SelectedFontColor = 14515260
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clCream
      FontAttrib.CaptionFont.Height = -15
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = []
      SizeFitBmp = True
      ButtonText = '10'
      ImageList = MainForm.ToolImageList
      ImageIndex = 0
    end
    object KBar5Button: TGraphButton
      Tag = 3
      Left = 70
      Top = 5
      Width = 27
      Height = 27
      Hint = #19977#20998#37912
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 5
      ShowHint = True
      OnClick = KBar1ButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clCream
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.SelectedFontColor = 14515260
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clCream
      FontAttrib.CaptionFont.Height = -15
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = []
      SizeFitBmp = True
      ButtonText = '3'
      ImageList = MainForm.ToolImageList
      ImageIndex = 0
    end
    object KBar20Button: TGraphButton
      Tag = 20
      Left = 160
      Top = 5
      Width = 27
      Height = 27
      Hint = #20108#21313#20998#37912
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 6
      ShowHint = True
      OnClick = KBar1ButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clCream
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.SelectedFontColor = 14515260
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clCream
      FontAttrib.CaptionFont.Height = -15
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = []
      SizeFitBmp = True
      ButtonText = '20'
      ImageList = MainForm.ToolImageList
      ImageIndex = 0
    end
    object MA1Button: TGraphButton
      Tag = 1
      Left = 270
      Top = 5
      Width = 27
      Height = 27
      Hint = #22343#32218'1'
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 7
      ShowHint = True
      OnClick = MA1ButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clCream
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clCream
      FontAttrib.CaptionFont.Height = -13
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 11
    end
    object MA2Button: TGraphButton
      Tag = 1
      Left = 300
      Top = 5
      Width = 27
      Height = 27
      Hint = #22343#32218'2'
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 8
      ShowHint = True
      OnClick = MA2ButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clCream
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clCream
      FontAttrib.CaptionFont.Height = -13
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 12
    end
    object MA3Button: TGraphButton
      Tag = 1
      Left = 330
      Top = 5
      Width = 27
      Height = 27
      Hint = #22343#32218'3'
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 9
      ShowHint = True
      OnClick = MA3ButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clCream
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clCream
      FontAttrib.CaptionFont.Height = -13
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 13
    end
    object ZoomXButton: TGraphButton
      Tag = 1
      Left = 440
      Top = 5
      Width = 27
      Height = 27
      Hint = #27700#24179#32302#25918
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 10
      ShowHint = True
      OnClick = ZoomXYButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 2
    end
    object ZoomXYButton: TGraphButton
      Tag = 1
      Left = 500
      Top = 5
      Width = 27
      Height = 27
      Hint = #32302#25918
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 11
      ShowHint = True
      OnClick = ZoomXYButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 4
    end
    object ZoomYButton: TGraphButton
      Tag = 1
      Left = 470
      Top = 5
      Width = 27
      Height = 27
      Hint = #22402#30452#32302#25918
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 12
      ShowHint = True
      OnClick = ZoomXYButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 3
    end
    object SettingButton: TGraphButton
      Left = 3
      Top = 5
      Width = 27
      Height = 27
      Hint = #35774#23450
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 13
      ShowHint = True
      OnClick = imgMenuClick
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
      ImageIndex = 6
    end
    object TradePointsButton: TGraphButton
      Tag = 1
      Left = 240
      Top = 5
      Width = 27
      Height = 27
      Hint = #39023#31034#36914#20986#22580#40670
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 14
      ShowHint = True
      OnClick = TradePointsButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 15
    end
    object CrossGraphButton: TGraphButton
      Tag = 1
      Left = 380
      Top = 5
      Width = 27
      Height = 27
      Hint = #21462#20729#27169#24335
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 15
      ShowHint = True
      OnClick = CrossGraphButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      Selected = True
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 23
    end
    object HLineButton: TGraphButton
      Tag = 1
      Left = 560
      Top = 5
      Width = 27
      Height = 27
      Hint = #27700#24179#32218
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 16
      ShowHint = True
      OnClick = EditButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 19
    end
    object VLineButton: TGraphButton
      Tag = 1
      Left = 590
      Top = 5
      Width = 27
      Height = 27
      Hint = #22402#30452#32218
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 17
      ShowHint = True
      OnClick = EditButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 20
    end
    object LineButton: TGraphButton
      Tag = 1
      Left = 620
      Top = 5
      Width = 27
      Height = 27
      Hint = #32218#27573
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 18
      ShowHint = True
      OnClick = EditButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 21
    end
    object DelButton: TGraphButton
      Tag = 1
      Left = 710
      Top = 5
      Width = 27
      Height = 27
      Hint = #21034#38500
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 19
      ShowHint = True
      OnClick = EditButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 22
    end
    object LineColorBox: TColorBox
      Left = 745
      Top = 3
      Width = 52
      Height = 31
      Hint = #32218#26781#38991#33394
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      DefaultColorColor = clWhite
      Selected = clCream
      Style = [cbStandardColors, cbExtendedColors, cbCustomColor]
      BevelEdges = [beBottom]
      BevelInner = bvNone
      BevelOuter = bvNone
      DropDownCount = 12
      Font.Charset = ANSI_CHARSET
      Font.Color = clWhite
      Font.Height = -15
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ItemHeight = 25
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 20
      StyleElements = []
      OnChange = LineColorBoxChange
    end
    object CopyDepthButton: TGraphButton
      Tag = 10
      Left = 805
      Top = 5
      Width = 27
      Height = 27
      Hint = #38283#21855#38275#38651
      Margins.Left = 5
      Margins.Top = 5
      Margins.Right = 5
      Margins.Bottom = 5
      TabOrder = 21
      ShowHint = True
      OnClick = CopyDepthButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -19
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold, fsItalic]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 14
    end
    object OrdConfirmCheckBox: TCheckBox
      Left = 836
      Top = 12
      Width = 19
      Height = 17
      TabStop = False
      Checked = True
      Color = clBtnFace
      Ctl3D = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWhite
      Font.Height = -17
      Font.Name = #24494#36575#27491#40657#39636
      Font.Style = []
      ParentColor = False
      ParentCtl3D = False
      ParentFont = False
      State = cbChecked
      TabOrder = 22
      StyleElements = []
      OnClick = OrdConfirmCheckBoxClick
    end
    object TextButton: TGraphButton
      Tag = 1
      Left = 680
      Top = 5
      Width = 27
      Height = 27
      Hint = #25991#23383
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 23
      ShowHint = True
      OnClick = EditButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 26
    end
    object SelectButton: TGraphButton
      Tag = 1
      Left = 530
      Top = 5
      Width = 27
      Height = 27
      Hint = #36984#21462
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 24
      ShowHint = True
      OnClick = EditButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 18
    end
    object RectButton: TGraphButton
      Tag = 1
      Left = 650
      Top = 5
      Width = 27
      Height = 27
      Hint = #26694#32218
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      TabOrder = 25
      ShowHint = True
      OnClick = EditButtonClick
      Transparent = True
      GraphCount = 5
      FontAttrib.FontColor = clWhite
      FontAttrib.HighlightFontColor = clWhite
      FontAttrib.HighlightShadowColor = clSilver
      FontAttrib.CaptionFont.Charset = ANSI_CHARSET
      FontAttrib.CaptionFont.Color = clWhite
      FontAttrib.CaptionFont.Height = -17
      FontAttrib.CaptionFont.Name = 'Arial'
      FontAttrib.CaptionFont.Style = [fsBold]
      SizeFitBmp = True
      ImageList = MainForm.ToolImageList
      ImageIndex = 21
    end
  end
  object CandleStickChart: TCandleStickChart
    Left = 310
    Top = 36
    Width = 1204
    Height = 1023
    Cursor = 6
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    OnDelete = CandleStickChartDelete
    OnDeleteStopOrder = CandleStickChartDeleteStopOrder
    OnReplacePx = CandleStickChartReplacePx
    OnStopOrderReplacePx = CandleStickChartStopOrderReplacePx
    OnNewOrder = CandleStickChartNewOrder
    OnNewStopOrder = CandleStickChartNewStopOrder
    OnClosePosition = CandleStickChartClosePosition
    OnNewText = CandleStickChartNewText
    Align = alClient
    Color = 3
    Store = MainForm.CMarketDataStore
    MinutesPerStick = 1
    PixelsPerTick = 10
    KBarDays = 7
    ShowQtyBar = 30
    TicksPerGrid = 10
    PxGrid = pgtAuto
    TimeGrid = tgtAuto
    ShowBox = True
    MaxFPS = 15
    BullColor = clPurple
    BearColor = clYellow
    CandleLineColor = clNone
    GridColor = clGray
    TextColor = clSilver
    MAColor1 = clYellow
    MAColor2 = clYellow
    MAColor3 = clYellow
    LineColor = clWhite
    ShowMA1 = True
    ShowMA2 = True
    ShowMA3 = True
    MACount1 = 10
    MACount2 = 30
    MACount3 = 60
    Edit = etHLine
    GradientCandle = True
    GDIPlus = True
    EmojiIcons = EmojiImageList
    XScale = 0.072361111111111110
    YScale = 1.000000000000000000
    SectionLineColor = clOlive
    RightSpace = 5
    ShowAvgPx = True
    PivotDeviation = 0.002000000000000000
    BuyStopOrderType = otLimit
    SellStopOrderType = otLimit
  end
  object ToolSV: TSplitView
    Left = 0
    Top = 36
    Width = 310
    Height = 1023
    Margins.Left = 4
    Margins.Top = 4
    Margins.Right = 4
    Margins.Bottom = 4
    AnimationDelay = 6
    AnimationStep = 30
    BorderStyle = bsSingle
    FullRepaint = False
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -17
    Font.Name = #24494#36575#27491#40657#39636
    Font.Style = []
    OpenedWidth = 310
    ParentFont = False
    Placement = svpLeft
    TabOrder = 2
    object Label3: TLabel
      Left = 9
      Top = 1031
      Width = 277
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = #36984#25799'['#27700#24179#32302#25918']'#26178',5'#40670#30340'K'#26834','#22266#23450#39640#24230
      Visible = False
    end
    object PixelsLabel: TLabel
      Left = 90
      Top = 1069
      Width = 34
      Height = 22
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Caption = #20687#32032
      Visible = False
    end
    object KBarPaintBox: TPaintBox
      Left = 135
      Top = 1063
      Width = 13
      Height = 177
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Visible = False
      OnPaint = KBarPaintBoxPaint
    end
    object PixelsComboBox: TComboBox
      Left = 23
      Top = 1069
      Width = 60
      Height = 30
      Margins.Left = 4
      Margins.Top = 4
      Margins.Right = 4
      Margins.Bottom = 4
      Style = csDropDownList
      TabOrder = 0
      Visible = False
      OnChange = PixelsComboBoxChange
      Items.Strings = (
        '20'
        '30'
        '40'
        '50'
        '60'
        '70'
        '80'
        '90'
        '100')
    end
    object PageControl: TPageControl
      Left = 0
      Top = 0
      Width = 306
      Height = 1019
      ActivePage = AOETabSheet
      Align = alClient
      TabOrder = 1
      object SettingTabSheet: TTabSheet
        Caption = #35373#23450
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object DefColorButton: TSpeedButton
          Left = 156
          Top = 440
          Width = 133
          Height = 34
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
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
        object MA3ColorLabel: TLabel
          Left = 10
          Top = 419
          Width = 44
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #22343#32218'3'
        end
        object MA2ColorLabel: TLabel
          Left = 156
          Top = 363
          Width = 44
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #22343#32218'2'
        end
        object MA1ColorLabel: TLabel
          Left = 10
          Top = 363
          Width = 44
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #22343#32218'1'
        end
        object BearColorLabel: TLabel
          Left = 158
          Top = 306
          Width = 17
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #36300
        end
        object BullColorLabel: TLabel
          Left = 10
          Top = 306
          Width = 17
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #28466
        end
        object KLineColorLabel: TLabel
          Left = 158
          Top = 250
          Width = 62
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'K'#32218#24341#32218
        end
        object GridColodLabel: TLabel
          Left = 10
          Top = 250
          Width = 68
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #26684#32218#38991#33394
        end
        object TextColorLabel: TLabel
          Left = 156
          Top = 194
          Width = 68
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #25991#23383#38991#33394
        end
        object BKColorLabel: TLabel
          Left = 10
          Top = 194
          Width = 68
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #32972#26223#38991#33394
        end
        object MA3Label: TLabel
          Left = 11
          Top = 154
          Width = 279
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #22343#32218'3               K'#26834','#20197'                       '#35336#31639
        end
        object MA2Label: TLabel
          Left = 11
          Top = 116
          Width = 279
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #22343#32218'2               K'#26834','#20197'                       '#35336#31639
        end
        object MA1Label: TLabel
          Left = 11
          Top = 80
          Width = 279
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #22343#32218'1               K'#26834','#20197'                       '#35336#31639
        end
        object Label6: TLabel
          Left = 16
          Top = 4
          Width = 107
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = 'K'#32218#20998#37912#35373#23450
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object MA3ColorBox: TColorBox
          Left = 10
          Top = 440
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 0
          OnChange = MA3ColorBoxChange
        end
        object MA2ColorBox: TColorBox
          Left = 155
          Top = 384
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 1
          OnChange = MA2ColorBoxChange
        end
        object MA1ColorBox: TColorBox
          Left = 10
          Top = 384
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 2
          OnChange = MA1ColorBoxChange
        end
        object BearColorBox: TColorBox
          Left = 156
          Top = 328
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 3
          OnChange = BearColorBoxChange
        end
        object BullColorBox: TColorBox
          Left = 10
          Top = 328
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 4
          OnChange = BullColorBoxChange
        end
        object CandleLineColorBox: TColorBox
          Left = 156
          Top = 271
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 5
          OnChange = CandleLineColorBoxChange
        end
        object GridColorBox: TColorBox
          Left = 10
          Top = 271
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 6
          OnChange = GridColorBoxChange
        end
        object ChartFGColorBox: TColorBox
          Left = 155
          Top = 215
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 7
          OnChange = ChartFGColorBoxChange
        end
        object ChartBKColorBox: TColorBox
          Left = 10
          Top = 215
          Width = 133
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 8
          OnChange = ChartBKColorBoxChange
        end
        object Px3ComboBox: TComboBox
          Left = 167
          Top = 149
          Width = 86
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          AutoComplete = False
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 9
          Text = #25910#30436#20729
          OnChange = Px3ComboBoxChange
          Items.Strings = (
            #25910#30436#20729
            #38283#30436#20729
            #26368#39640#20729
            #26368#20302#20729
            #39640#20302#22343#20729)
        end
        object MA3KUpDown: TUpDown
          Left = 90
          Top = 150
          Width = 20
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Associate = MA3KEdit
          Min = 5
          Max = 200
          Position = 5
          TabOrder = 10
          OnChangingEx = MA3KUpDownChangingEx
        end
        object MA3KEdit: TEdit
          Left = 57
          Top = 150
          Width = 33
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          ImeMode = imDisable
          MaxLength = 3
          TabOrder = 11
          Text = '5'
          OnChange = MA3KEditChange
          OnMouseDown = MA1KEditMouseDown
        end
        object Px2ComboBox: TComboBox
          Left = 167
          Top = 112
          Width = 86
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          AutoComplete = False
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 12
          Text = #25910#30436#20729
          OnChange = Px2ComboBoxChange
          Items.Strings = (
            #25910#30436#20729
            #38283#30436#20729
            #26368#39640#20729
            #26368#20302#20729
            #39640#20302#22343#20729)
        end
        object MA2KUpDown: TUpDown
          Left = 90
          Top = 113
          Width = 20
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Associate = MA2KEdit
          Min = 5
          Max = 200
          Position = 5
          TabOrder = 13
          OnChangingEx = MA2KUpDownChangingEx
        end
        object MA2KEdit: TEdit
          Left = 57
          Top = 113
          Width = 33
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          ImeMode = imDisable
          MaxLength = 3
          TabOrder = 14
          Text = '5'
          OnChange = MA2KEditChange
          OnMouseDown = MA1KEditMouseDown
        end
        object MA1KUpDown: TUpDown
          Left = 90
          Top = 75
          Width = 20
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Associate = MA1KEdit
          Min = 5
          Max = 200
          Position = 5
          TabOrder = 15
          OnChangingEx = MA1KUpDownChangingEx
        end
        object MA1KEdit: TEdit
          Left = 57
          Top = 75
          Width = 33
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          ImeMode = imDisable
          MaxLength = 3
          TabOrder = 16
          Text = '5'
          OnChange = MA1KEditChange
          OnMouseDown = MA1KEditMouseDown
        end
        object Px1ComboBox: TComboBox
          Left = 167
          Top = 75
          Width = 86
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          AutoComplete = False
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 17
          Text = #25910#30436#20729
          OnChange = Px1ComboBoxChange
          Items.Strings = (
            #25910#30436#20729
            #38283#30436#20729
            #26368#39640#20729
            #26368#20302#20729
            #39640#20302#22343#20729)
        end
        object K5ComboBox: TComboBox
          Left = 242
          Top = 33
          Width = 50
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          DropDownCount = 9
          ItemIndex = 6
          TabOrder = 18
          Text = '20'
          OnChange = K1ComboBoxChange
          Items.Strings = (
            '1'
            '2'
            '3'
            '5'
            '10'
            '15'
            '20'
            '25'
            '30')
        end
        object K4ComboBox: TComboBox
          Left = 184
          Top = 33
          Width = 50
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          DropDownCount = 9
          ItemIndex = 4
          TabOrder = 19
          Text = '10'
          OnChange = K1ComboBoxChange
          Items.Strings = (
            '1'
            '2'
            '3'
            '5'
            '10'
            '15'
            '20'
            '25'
            '30')
        end
        object K3ComboBox: TComboBox
          Left = 126
          Top = 33
          Width = 50
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          DropDownCount = 9
          ItemIndex = 3
          TabOrder = 20
          Text = '5'
          OnChange = K1ComboBoxChange
          Items.Strings = (
            '1'
            '2'
            '3'
            '5'
            '10'
            '15'
            '20'
            '25'
            '30')
        end
        object K2ComboBox: TComboBox
          Left = 68
          Top = 33
          Width = 50
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          DropDownCount = 9
          ItemIndex = 2
          TabOrder = 21
          Text = '3'
          OnChange = K1ComboBoxChange
          Items.Strings = (
            '1'
            '2'
            '3'
            '5'
            '10'
            '15'
            '20'
            '25'
            '30')
        end
        object K1ComboBox: TComboBox
          Left = 10
          Top = 33
          Width = 50
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          DropDownCount = 9
          ItemIndex = 0
          TabOrder = 22
          Text = '1'
          OnChange = K1ComboBoxChange
          Items.Strings = (
            '1'
            '2'
            '3'
            '5'
            '10'
            '15'
            '20'
            '25'
            '30')
        end
      end
      object AOETabSheet: TTabSheet
        Caption = #36914#38542
        ImageIndex = 1
        ExplicitLeft = 0
        ExplicitTop = 0
        ExplicitWidth = 0
        ExplicitHeight = 0
        object BOXLabel: TLabel
          Left = 10
          Top = 547
          Width = 57
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #36264#21218#31665
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object SkyLandLabel: TLabel
          Left = 129
          Top = 547
          Width = 76
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #22825#32645#22320#32178
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object PivotLabel: TLabel
          Left = 9
          Top = 268
          Width = 57
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #27166#32016#32218
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object PivotDevLabel: TLabel
          Left = 122
          Top = 270
          Width = 113
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #36681#25240#25935#24863#24230' 0.2'
        end
        object Label4: TLabel
          Left = 9
          Top = 194
          Width = 57
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #20998#21106#32218
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label2: TLabel
          Left = 9
          Top = 73
          Width = 114
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #26085#20839#39640#20302#20729#32218
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label5: TLabel
          Left = 129
          Top = 38
          Width = 156
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #21491#37002#38291#36317'               K'#26834
        end
        object Label1: TLabel
          Left = 9
          Top = 10
          Width = 152
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #25104#20132#20729#26684#33258#21205#32622#20013
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label7: TLabel
          Left = 144
          Top = 73
          Width = 95
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #25104#20132#22343#20729#32218
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object Label8: TLabel
          Left = 144
          Top = 134
          Width = 108
          Height = 23
          Margins.Left = 5
          Margins.Top = 5
          Margins.Right = 5
          Margins.Bottom = 5
          Caption = #35320#20729#26781#20214#35373#23450
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -18
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object StopSettingBtn: TSpeedButton
          Left = 144
          Top = 158
          Width = 63
          Height = 28
          Hint = #35320#20729#35373#23450
          Margins.Left = 5
          Margins.Top = 5
          Margins.Right = 5
          Margins.Bottom = 5
          Glyph.Data = {
            36050000424D3605000000000000360400002800000010000000100000000100
            0800000000000001000000000000000000000001000000010000000000000101
            0100020202000303030004040400050505000606060007070700080808000909
            09000A0A0A000B0B0B000C0C0C000D0D0D000E0E0E000F0F0F00101010001111
            1100121212001313130014141400151515001616160017171700181818001919
            19001A1A1A001B1B1B001C1C1C001D1D1D001E1E1E001F1F1F00202020002121
            2100222222002323230024242400252525002626260027272700282828002929
            29002A2A2A002B2B2B002C2C2C002D2D2D002E2E2E002F2F2F00303030003131
            3100323232003333330034343400353535003636360037373700383838003939
            39003A3A3A003B3B3B003C3C3C003D3D3D003E3E3E003F3F3F00404040004141
            4100424242004343430044444400454545004646460047474700484848004949
            49004A4A4A004B4B4B004C4C4C004D4D4D004E4E4E004F4F4F00505050005151
            5100525252005353530054545400555555005656560057575700585858005959
            59005A5A5A005B5B5B005F5F5F0064636300686868006C6B6B00727171007777
            77007C7B7B007F7F7F008382820086858500898888008E8D8D00919090009392
            9200959494009695950096959500969595009695950097969600979696009796
            96009796960098969600999797009A9898009C9999009E9B9B00A19D9D00A39E
            9E00A59F9F00A6A0A000A8A0A000AAA1A100ACA3A200ADA3A300AEA3A300AFA4
            A400B0A5A400B1A6A600B2A8A800B4ABAB00BAAFAE00BEB2B200B9B3B400B0B4
            B700A8B5BB009DB7C00093BAC60089BECD0081BFD10074C1D6006CC1D90064C1
            DA005EC1DC0057C1DD004FC2E1004AC5E50045C7E90041C9EB003ACBEF0033CD
            F2002DCFF40027D0F70020D0F8001DD1FA0022D1F9002DD3F9003CD5F8004AD7
            F70056D9F7005DD9F60063DAF60067DAF6006BDBF60070DCF50076DEF5007FDF
            F5008BE2F50091E3F50096E4F5009DE3F200A3E3F100AEE2ED00B9E1EA00C4E1
            E700CFE1E500D4E1E500D7E1E500DCE1E400E0E0E300E2DFE300E3DBE400E4D4
            E500E7C4E700ECAAEA00F090ED00F378F000F55BF300F935F800FC15FB00FE09
            FD00FE02FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
            FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
            FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
            FE00F703FC00E50EF500C91DEC00A034DF007F47D4006954CD00575EC6004768
            C1003970BC002D78B800237EB4001A82B2001585B0001486B0001587B1001488
            B2001488B3001589B400168AB400178AB500178CB600188DB8001A8FBA001B91
            BC001B94BF001E98C2001D9CC7001D9EC9001D9FCA001DA0CC001EA1CD0021A3
            CE0023A4CE0027A5CF0029A7D1002AA9D2002CAAD3002CABD300CECECECECECE
            CEF8FEE6CECECECECECECECECEFBFAE6F3FFFFEDE8FFFCCECECECECECEFBFFFF
            F79CA1FB9291FBCECECECEEEF394A994989DA0A38FAFABEDCECECE92FF94ACAA
            A39B9DA2A8A998FFFBCECEF1FA94A9ABA792949A9D9D9D9CFFCECEF0FF96A68D
            6F6F6F6F949D9D9DF5CECEF1FFFFA36FB7897E876FA2ADAFF3CECEEEF8FF966F
            B7897E876FA9AF8DFBCECECEE6F9986FB7897E876FA591E6CECECECECEEBF06F
            B7897E876FF3F1CECECECECECECECE6FB7867D876FCECECECECECECECECECE6F
            897B737C6FCECECECECECECECECECE6FB6877B826FCECECECECECECECECECE6F
            B7B788796FCECECECECECECECECECECE6F6F6F6FCECECECECECE}
          ParentShowHint = False
          ShowHint = True
          OnClick = StopSettingBtnClick
        end
        object Label9: TLabel
          Left = 11
          Top = 133
          Width = 76
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #32218#22294#20132#26131
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -19
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentFont = False
        end
        object PxGridRadioGroup: TRadioGroup
          Left = 2
          Top = 335
          Width = 280
          Height = 131
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #20729#37666#26684#36317#35373#23450
          Items.Strings = (
            #22266#23450#26684#36317
            #33258#21205#26684#36317
            'AOE'#33258#21205#26684#36317)
          TabOrder = 4
          OnClick = PxGridRadioGroupClick
        end
        object ShowBoxSwitch: TToggleSwitch
          Left = 10
          Top = 579
          Width = 78
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          StateCaptions.CaptionOn = #38283#21855
          StateCaptions.CaptionOff = #38364#38281
          SwitchHeight = 18
          SwitchWidth = 38
          TabOrder = 0
          ThumbWidth = 20
          OnClick = ShowBoxSwitchClick
        end
        object SkyLandToggleSwitch: TToggleSwitch
          Left = 129
          Top = 579
          Width = 78
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          StateCaptions.CaptionOn = #38283#21855
          StateCaptions.CaptionOff = #38364#38281
          SwitchHeight = 18
          SwitchWidth = 38
          TabOrder = 1
          ThumbWidth = 20
          OnClick = SkyLandToggleSwitchClick
        end
        object PxGridUpDown: TUpDown
          Left = 152
          Top = 361
          Width = 20
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Associate = PxGridEdit
          Min = 5
          Max = 200
          Position = 10
          TabOrder = 2
          OnChangingEx = PxGridUpDownChangingEx
        end
        object PivotSwitch: TToggleSwitch
          Left = 9
          Top = 295
          Width = 78
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          StateCaptions.CaptionOn = #38283#21855
          StateCaptions.CaptionOff = #38364#38281
          SwitchHeight = 18
          SwitchWidth = 38
          TabOrder = 5
          ThumbWidth = 20
          OnClick = PivotSwitchClick
        end
        object DeviationTrackBar: TTrackBar
          Left = 116
          Top = 295
          Width = 150
          Height = 49
          Position = 5
          TabOrder = 6
          OnChange = DeviationTrackBarChange
        end
        object SectionTypeComboBox: TComboBox
          Left = 10
          Top = 225
          Width = 119
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          AutoComplete = False
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 7
          Text = #19981#39023#31034#11
          OnChange = SectionTypeComboBoxChange
          Items.Strings = (
            #19981#39023#31034#11
            #40643#37329#20998#21106#11
            #33258#28982#20998#21106)
        end
        object DayHLSwitch: TToggleSwitch
          Left = 9
          Top = 101
          Width = 78
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          StateCaptions.CaptionOn = #38283#21855
          StateCaptions.CaptionOff = #38364#38281
          SwitchHeight = 18
          SwitchWidth = 38
          TabOrder = 8
          ThumbWidth = 20
          OnClick = DayHLSwitchClick
        end
        object SpaceComboBox: TComboBox
          Left = 201
          Top = 35
          Width = 53
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = csDropDownList
          DropDownCount = 7
          ItemIndex = 1
          TabOrder = 9
          Text = '5'
          OnChange = SpaceComboBoxChange
          Items.Strings = (
            '0'
            '5'
            '10'
            '15'
            '30'
            '45'
            '60')
        end
        object AutoCenterSwitch: TToggleSwitch
          Left = 9
          Top = 38
          Width = 78
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          StateCaptions.CaptionOn = #38283#21855
          StateCaptions.CaptionOff = #38364#38281
          SwitchHeight = 18
          SwitchWidth = 38
          TabOrder = 10
          ThumbWidth = 20
        end
        object SectionLineColorBox: TColorBox
          Left = 148
          Top = 225
          Width = 132
          Height = 22
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Style = [cbStandardColors, cbExtendedColors, cbCustomColor, cbPrettyNames]
          DropDownCount = 12
          ItemHeight = 25
          TabOrder = 11
          OnChange = SectionLineColorBoxChange
        end
        object PxGridEdit: TEdit
          Left = 114
          Top = 361
          Width = 38
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          ImeMode = imDisable
          MaxLength = 3
          TabOrder = 3
          Text = '10'
          OnChange = PxGridEditChange
          OnMouseDown = MA1KEditMouseDown
        end
        object RadioGroup1: TRadioGroup
          Left = 2
          Top = 472
          Width = 280
          Height = 72
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          Caption = #26178#38291#27161#31034#38291#36317
          TabOrder = 13
          OnClick = PxGridRadioGroupClick
        end
        object TimeGridComboBox: TComboBox
          Left = 11
          Top = 500
          Width = 194
          Height = 30
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          AutoComplete = False
          Style = csDropDownList
          ItemIndex = 3
          TabOrder = 12
          Text = #33258#21205#38291#36317
          OnChange = TimeGridComboBoxChange
          Items.Strings = (
            #27599'10'#20998#37912#27161#31034#26178#38291#11
            #27599'15'#20998#37912#27161#31034#26178#38291#11
            #27599'30'#20998#37912#27161#31034#26178#38291#11
            #33258#21205#38291#36317)
        end
        object AvgPxSwitch: TToggleSwitch
          Left = 144
          Top = 101
          Width = 78
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          StateCaptions.CaptionOn = #38283#21855
          StateCaptions.CaptionOff = #38364#38281
          SwitchHeight = 18
          SwitchWidth = 38
          TabOrder = 14
          ThumbWidth = 20
          OnClick = AvgPxSwitchClick
        end
        object TradingSwitch: TToggleSwitch
          Left = 11
          Top = 161
          Width = 78
          Height = 24
          Margins.Left = 4
          Margins.Top = 4
          Margins.Right = 4
          Margins.Bottom = 4
          StateCaptions.CaptionOn = #38283#21855
          StateCaptions.CaptionOff = #38364#38281
          SwitchHeight = 18
          SwitchWidth = 38
          TabOrder = 15
          ThumbWidth = 20
          OnClick = TradingSwitchClick
        end
        object HLSectionBox: TCheckBox
          Left = 72
          Top = 199
          Width = 222
          Height = 17
          TabStop = False
          Caption = #20197#26085#20839#39640#20302#20729#20998#21106
          Checked = True
          Color = clBtnFace
          Ctl3D = False
          Font.Charset = ANSI_CHARSET
          Font.Color = clWhite
          Font.Height = -18
          Font.Name = #24494#36575#27491#40657#39636
          Font.Style = [fsBold]
          ParentColor = False
          ParentCtl3D = False
          ParentFont = False
          State = cbChecked
          TabOrder = 16
          StyleElements = []
          OnClick = HLSectionBoxClick
        end
      end
    end
  end
  object RoundFormEx: TRoundFormEx
    Resource = MainForm.RoundFormExRes
    InactiveFontColor = 19
    Lockbox = True
    OnIconClick = RoundFormExIconClick
    OnLockIconClick = RoundFormExLockIconClick
    CaptionFont.Charset = ANSI_CHARSET
    CaptionFont.Color = clWhite
    CaptionFont.Height = -17
    CaptionFont.Name = #24494#36575#27491#40657#39636
    CaptionFont.Style = []
    Left = 368
    Top = 80
  end
  object RepaintTimer: TTimer
    Enabled = False
    Interval = 500
    OnTimer = RepaintTimerTimer
    Left = 368
    Top = 146
  end
  object EmojiImageList: TImageList
    ColorDepth = cd32Bit
    Height = 32
    Width = 32
    Left = 368
    Top = 208
    Bitmap = {
      494C010104000800580220002000FFFFFFFF2110FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000800000004000000001002000000000000080
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000000304230B3F54931780
      ADD31FA3E2F122B0F8FC22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B0F8FC1FA3
      E2F11780ADD30B3F549300030423000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000106082E0C445B991881
      B1D51FA5E4F222B0F8FC22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B0F8FC1FA5
      E4F21881B1D50C445B990106082E000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000051D27641B95CCE522B2FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B2FEFF1B95CCE5051D276400000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000106082E0C445B991881
      B1D51FA5E4F222B0F8FC22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B0F8FC1FA5
      E4F21881B1D50C445B990106082E000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000202072E16175399292C
      9FD53337CCF2343ADBFC3239DFFF3239DFFF3239DFFF3239DFFF343ADBFC3337
      CCF2292C9FD5161753990202072E000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000082D3C7C1D9CD7EB22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF1D9CD7EB082D3C7C00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000001070A331C96CEE622B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF1C96CEE601070A330000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000082D3C7C1D9CD7EB22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF1D9CD7EB082D3C7C00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000F0F367C3035C1EB3239DFFF3239DFFF3239
      DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239
      DFFF3239DFFF3239DFFF3239DFFF3035C1EB0F0F367C00000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000210154B1EA2E1F022B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF1EA2E1F00210154B0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000727347422B1FCFE22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FCFE0727
      3474000000000000000000000000000000000000000000000000000000000000
      0000000000000210154B1EA2E1F022B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF1EA2E1F00210154B0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000505144B3236C9F03239DFFF3239DFFF3239DFFF3239DFFF3239
      DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239
      DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3236C9F00505144B0000
      0000000000000000000000000000000000000000000000000000000000000000
      00000C42599722B2FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B2FEFF0C42
      5997000000000000000000000000000000000000000000000000000000000930
      418122B2FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B4FEFF22BBFEFF22C2
      FEFF22C6FEFF22C9FEFF22CCFEFF22CEFEFF22CEFEFF22CEFEFF22CDFEFF22CA
      FEFF22C6FEFF22C0FEFF22B8FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B2
      FEFF093041810000000000000000000000000000000000000000000000000000
      00000C42599722B2FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1
      FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B1FEFF22B2FEFF0C42
      5997000000000000000000000000000000000000000000000000000000000000
      000015175197333AE0FF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239
      DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239
      DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF3239DFFF333AE0FF1517
      5197000000000000000000000000000000000000000000000000000000000E4E
      69A422B2FEFF22B2FEFF22B2FEFF22B2FEFF22B3FEFF22BAFEFF22C1FEFF22C6
      FEFF22CBFEFF22CEFEFF22D0FEFF22D2FEFF22D2FEFF22D2FEFF22D1FEFF22CF
      FEFF22CBFEFF22C5FEFF22BEFEFF22B5FEFF22B2FEFF22B2FEFF22B2FEFF22B2
      FEFF0E4E69A400000000000000000000000000000000000000000311174D22B4
      FEFF22B3FEFF22B3FEFF22B5FEFF22C0FEFF22CCFEFF22D2FEFF22D2FEFF22D2
      FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2
      FEFF22D2FEFF22D2FEFF22D2FEFF22CFFEFF22C3FEFF22B5FEFF22B3FEFF22B3
      FEFF22B4FEFF0311174D00000000000000000000000000000000000000000E4E
      69A422B2FEFF22B2FEFF22B2FEFF22B2FEFF22B3FEFF22BAFEFF22C1FEFF22C6
      FEFF22CBFEFF22CEFEFF22D0FEFF22D2FEFF22D2FEFF22D2FEFF22D1FEFF22CF
      FEFF22CBFEFF22C5FEFF22BEFEFF22B5FEFF22B2FEFF22B2FEFF22B2FEFF22B2
      FEFF0E4E69A40000000000000000000000000000000000000000000000001A1B
      60A43339DFFF3339DFFF3339DFFF3339DFFF3339DFFF3A3FE4FF4043E9FF4548
      EDFF494BF0FF4C4DF2FF4E4FF4FF5050F5FF5050F5FF5050F5FF4F4FF4FF4D4E
      F3FF494BF0FF4447ECFF3D41E7FF353BE1FF3339DFFF3339DFFF3339DFFF3339
      DFFF1A1B60A400000000000000000000000000000000000000000729367622B3
      FEFF22B3FEFF22B3FEFF22B9FEFF22C6FEFF22D0FEFF22D2FEFF22D2FEFF22CF
      FCFF1F9BD5FF1D6BB1FF1B4A98FF1B3D8EFF1B3D8EFF1B4B98FF1D6CB2FF1F9E
      D7FF22CFFCFF22D2FEFF22D2FEFF22D1FEFF22C8FEFF22B9FEFF22B3FEFF22B3
      FEFF22B3FEFF072936760000000000000000000000000000000620ABEAF522B4
      FEFF22B4FEFF22C1FEFF22D0FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2
      FEFF22D2FEFF25D3FEFF2AD6FEFF2CD7FEFF2CD7FEFF29D6FEFF25D3FEFF22D2
      FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D0FEFF22BEFEFF22B4
      FEFF22B4FEFF20ABEAF5000000060000000000000000000000000729367622B3
      FEFF22B3FEFF22B3FEFF22B9FEFF22C6FEFF22D0FEFF22D2FEFF22D2FEFF22D2
      FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2
      FEFF22D2FEFF22D2FEFF22D2FEFF22D1FEFF22C8FEFF22B9FEFF22B3FEFF22B3
      FEFF22B3FEFF07293676000000000000000000000000000000000D0E3176343A
      E0FF343AE0FF343AE0FF393EE4FF4547EDFF4E4FF4FF5050F5FF5050F5FF5050
      F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050
      F5FF5050F5FF5050F5FF5050F5FF4F50F5FF4749EEFF393EE4FF343AE0FF343A
      E0FF343AE0FF0D0E31760000000000000000000000000002021B22B4F8FC22B5
      FEFF22B7FEFF22C7FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF1F9CD5FF1B42
      92FF23369BFF323DBFFF3B42D6FF3F45E1FF3F44E0FF3B42D5FF313DBEFF2235
      9AFF1B4494FF1F9FD8FF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22C5FEFF22B5
      FEFF22B5FEFF22B4F8FC0002021B00000000000000000D4B64A022B6FEFF22B7
      FEFF22C9FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF2AD6
      FEFF39DBFCFF3ABFE6FF35A7D5FF329ECFFF329ECFFF35A8D6FF3ABFE6FF3ADA
      FBFF2BD7FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22C4
      FEFF22B6FEFF22B6FEFF0D4B64A000000000000000000002021B22B4F8FC22B5
      FEFF22B7FEFF22C7FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2
      FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2
      FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22C5FEFF22B5
      FEFF22B5FEFF22B4F8FC0002021B00000000000000000000021B373DDEFC363C
      E2FF373DE3FF4648EEFF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050
      F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050
      F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF4447ECFF373C
      E2FF363CE2FF373DDEFC0000021B0000000000000000136D90C022B7FEFF22BA
      FEFF22CEFEFF22D2FEFF22D2FEFF22D2FEFF22D0FDFF1D72B5FF1A3184FF1B32
      86FF4A4CE8FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF484B
      E4FF1A3185FF1A3184FF1E7ABCFF22D1FEFF22D2FEFF22D2FEFF22D2FEFF22CA
      FEFF22B8FEFF22B7FEFF136D90C0000000000000000021B3F2F922B9FEFF23C9
      FEFF23D2FEFF23D2FEFF23D2FEFF23D2FEFF23D2FEFF23D2FEFF2AD5FEFF2894
      CBFF1C3D8DFF1A3083FF1A3083FF1A3083FF1A3083FF1A3083FF1A3083FF1C3D
      8CFF278AC4FF2BD3FCFF23D2FEFF23D2FEFF23D2FEFF23D2FEFF23D2FEFF23D2
      FEFF22C3FEFF22B9FEFF21B3F2F90000000000000000136D90C022B7FEFF22BA
      FEFF22CEFEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2
      FEFF31D6FCFF37D3F7FF37D0F5FF37D1F5FF36D3F8FF35D8FCFF32DAFEFF2FD8
      FEFF2CD7FEFF23D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22D2FEFF22CA
      FEFF22B8FEFF22B7FEFF136D90C00000000000000000242684C0373DE3FF3A3F
      E5FF4C4DF2FF5050F5FF293AA4FF3440BAFF5050F5FF5050F5FF5050F5FF5050
      F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050F5FF5050
      F5FF5050F5FF5050F5FF5050F5FF5050F5FF3440BBFF2839A2FF5050F5FF494A
      F0FF373DE3FF373DE3FF242684C0000000000000000E22B9FCFE22BAFEFF23CF
      FEFF23D3FEFF23D3FEFF23D3FEFF23D3FEFF1D6CAFFF192E7DFF192E7DFF192E
      7DFF1C3084FF2E3BABFF353FBAFF27379DFF28379FFF343FB8FF2D3AA8FF1B2F
      83FF192E7DFF192E7DFF192E7DFF1E7AB9FF23D3FEFF23D3FEFF23D3FEFF23D3
      FEFF23C9FEFF22B9FEFF22B9FCFE0000000E051D256222BBFEFF23C4FEFF24D3
      FEFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D1FCFF1D67A7FF182B
      75FF182B75FF182B75FF182B75FF182B75FF182B75FF182B75FF182B75FF182B
      75FF182B75FF1C5E9FFF24CFFBFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D3
      FEFF24D2FEFF22BFFEFF22BBFEFF051D25620000000E22B9FCFE22BAFEFF23CF
      FEFF23D3FEFF23D3FEFF23D3FEFF23D3FEFF23D3FEFF23D3FEFF23D3FEFF23D3
      FEFF1A4B92FF182D79FF182D79FF182D79FF182D79FF182D79FF193984FF1A4F
      95FF1C68AAFF22C6F4FF23D3FEFF23D3FEFF23D3FEFF23D3FEFF23D3FEFF23D3
      FEFF23C9FEFF22B9FEFF22B9FCFE0000000E0000000E3B40E4FE3B40E6FF4D4E
      F2FF5151F5FF5151F5FF3C45C9FF1A3084FF3641BBFF5252F5FF5151F5FF5151
      F5FF5151F5FF5151F5FF5151F5FF5151F5FF5151F5FF5151F5FF5151F5FF5151
      F5FF5151F5FF5151F5FF5252F5FF3642BBFF1A3084FF3B44C6FF5151F5FF5151
      F5FF494AEFFF3A3FE5FF3B40E4FE0000000E0936468622BCFEFF23C9FEFF24D3
      FEFF24D3FEFF24D3FEFF24D3FEFF1F92C9FF182C76FF182C76FF182C76FF182C
      76FF182C76FF182C76FF182C76FF182C76FF182C76FF182C76FF182C76FF182C
      76FF182C76FF182C76FF182C76FF182C76FF209ED3FF24D3FEFF24D3FEFF24D3
      FEFF24D3FEFF23C3FEFF22BCFEFF09364686146F90C022BFFEFF26D2FEFF26D4
      FEFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF1C7EB4FF152463FF1525
      64FF1A5690FF1F8DBFFF22A9D9FF23B3E1FF22ADDCFF21A0D0FF1E82B6FF1951
      8CFF152463FF152463FF1B76ADFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF26D4
      FEFF26D4FEFF25CEFEFF22BEFEFF146F90C00936468622BCFEFF23C9FEFF24D3
      FEFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D3
      FEFF18568BFF131F56FF131F56FF131F56FF131F56FF131F56FF131F56FF131F
      56FF131F56FF1FA0CEFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D3FEFF24D3
      FEFF24D3FEFF23C3FEFF22BCFEFF09364686131340863C41E6FF484BEEFF5252
      F5FF5252F5FF5252F5FF5051F2FF233490FF182D7AFF313FA5FF5759EFFF5656
      F6FF5252F5FF5252F5FF5252F5FF5252F5FF5252F5FF5252F5FF5252F5FF5252
      F5FF5656F6FF5759EFFF313FA5FF182D7AFF21338CFF5051F1FF5252F5FF5252
      F5FF5252F5FF4347EBFF3C41E6FF131340861885ACD222C1FEFF26D4FEFF26D4
      FEFF26D4FEFF26D4FEFF25C9F5FF183276FF17286EFF17286EFF17286EFF1728
      6EFF17286EFF17286EFF17286EFF17286EFF17286EFF17286EFF17286EFF1728
      6EFF17286EFF17286EFF17286EFF17286EFF193A7DFF25CDF8FF26D4FEFF26D4
      FEFF26D4FEFF25D1FEFF22BFFEFF1885ACD21EA8D9EC24C8FEFF27D5FEFF27D5
      FEFF27D5FEFF27D5FEFF27D5FEFF27D5FEFF27D3FCFF132456FF153B6CFF23B0
      DBFF27D5FEFF27D5FEFF27D5FEFF27D5FEFF27D5FEFF27D5FEFF27D5FEFF27D5
      FEFF23B0DBFF153969FF132355FF27D3FCFF27D5FEFF27D5FEFF27D5FEFF27D5
      FEFF27D5FEFF27D5FEFF23C4FEFF1EA8D9EC1885ACD222C1FEFF26D4FEFF26D4
      FEFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF26D4
      FEFF26D4FEFF26D1FBFF25CAF4FF24C2ECFF23B9E3FF22B1DBFF21A9D3FF20A5
      CFFF21A6D0FF25D0FAFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF26D4FEFF26D4
      FEFF26D4FEFF25D1FEFF22BFFEFF1885ACD22E309FD24144EAFF5354F6FF5454
      F6FF5454F6FF5454F6FF5454F6FF494CDDFF1B2B75FF17286EFF1D2D78FF444D
      B6FF6365E7FF6A6AFAFF6868FAFF6767F9FF6767F9FF6969FAFF6B6BF9FF6164
      E3FF444DB6FF1E2E79FF17286EFF1A2A73FF464AD8FF5454F6FF5454F6FF5454
      F6FF5454F6FF5152F4FF3E42E8FF2E309FD220B3E6F325CBFEFF28D5FEFF28D5
      FEFF28D5FEFF28D5FEFF1F84B7FF152564FF152564FF1B2A67FF29376FFF3542
      76FF39467AFF3C497DFF414E82FF424F82FF424F83FF424F82FF3D4A7EFF3A48
      7CFF354278FF2A3870FF1A2A67FF152564FF152564FF208CBEFF28D5FEFF28D5
      FEFF28D5FEFF28D5FEFF24C8FEFF20B3E6F322BFF6FB27D0FEFF29D5FEFF29D5
      FEFF29D5FEFF29D5FEFF29D5FEFF29D5FEFF29D4FDFF219AC3FF28CFF8FF29D5
      FEFF29D5FEFF29D5FEFF29D5FEFF29D5FEFF29D5FEFF29D5FEFF29D5FEFF29D5
      FEFF29D5FEFF28CFF8FF219AC3FF29D4FDFF29D5FEFF29D5FEFF29D5FEFF29D5
      FEFF29D5FEFF29D5FEFF26CDFEFF22BFF6FB20B3E6F325CBFEFF28D5FEFF28D5
      FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5
      FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5
      FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5FEFF28D5
      FEFF28D5FEFF28D5FEFF24C8FEFF20B3E6F33F41D6F34B4DF0FF5656F6FF5656
      F6FF5656F6FF5656F6FF5656F6FF5656F6FF4649D1FF1A276BFF152360FF1523
      60FF152360FF1E2B6EFF2C3783FF323C8BFF313B89FF27327AFF182665FF1523
      60FF152360FF152360FF182667FF4347CBFF5656F6FF5656F6FF5656F6FF5656
      F6FF5656F6FF5656F6FF484AEEFF3F41D6F323C1FAFD28D3FEFF2AD6FEFF2AD6
      FEFF2AD6FEFF2AD6FEFF184479FF17235BFF938E8FFFD0C8BAFFDFDAD0FFEAE7
      E1FFF3F1EDFFF9F8F6FFFDFCFCFFFFFFFEFFFFFFFFFFFFFEFEFFFDFCFCFFF9F8
      F6FFF2F0EDFFE9E6E0FFDCD7CEFF98969DFF15225AFF184479FF2AD6FEFF2AD6
      FEFF2AD6FEFF2AD6FEFF28D1FEFF23C1FAFD22C7FEFF2BD5FEFF2BD6FEFF2BD6
      FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6
      FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6
      FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6FEFF2BD6
      FEFF2BD6FEFF2BD6FEFF2AD4FEFF22C6FEFF23C1FAFD28D3FEFF2AD6FEFF2AD6
      FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6
      FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6
      FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6FEFF2AD6
      FEFF2AD6FEFF2AD6FEFF28D1FEFF23C1FAFD4446E9FD5455F4FF5858F6FF5858
      F6FF5858F6FF5858F6FF5858F6FF5858F6FF5858F6FF5051E3FF283086FF121D
      51FF121D51FF121D51FF121D51FF121D51FF121D51FF121D51FF121D51FF121D
      51FF121D51FF222B78FF4C4EDBFF5858F6FF5858F6FF5858F6FF5858F6FF5858
      F6FF5858F6FF5858F6FF5253F3FF4446E9FD23C8FEFF2CD7FEFF2CD7FEFF2CD7
      FEFF2CD7FEFF2CD7FEFF1C6392FF131E50FF484C68FF7E7E8DFFAEADB1FFDCDA
      D7FFF3F1EDFFF9F8F6FFFDFCFCFFFFFFFEFFFFFFFFFFFFFEFEFFFDFCFCFFF9F8
      F6FFDFDEDFFFB4B5BCFF848697FF4C5170FF131E4FFF1D6998FF2CD7FEFF2CD7
      FEFF2CD7FEFF2CD7FEFF2CD7FEFF22C8FEFF24CCFEFF2DD7FEFF2DD7FEFF2DD7
      FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7
      FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7
      FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7FEFF2DD7
      FEFF2DD7FEFF2DD7FEFF2DD7FEFF24CBFEFF23C8FEFF2CD7FEFF2CD7FEFF2CD7
      FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7FEFF2BD2FEFF28C8FEFF2ACF
      FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7
      FEFF28C8FEFF27C3FEFF2AD0FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7FEFF2CD7
      FEFF2CD7FEFF2CD7FEFF2CD7FEFF22C8FEFF474AEFFF5A5AF7FF5A5AF7FF5A5A
      F7FF5A5AF7FF5A5AF7FF5A5AF7FF5A5AF7FF5A5AF7FF5A5AF7FF5A5AF7FF4E4F
      D9FF35389CFF242972FF191F57FF141A4AFF131A49FF1A205BFF252A76FF3538
      9CFF4B4DD4FF5A5AF7FF5A5AF7FF5A5AF7FF5A5AF7FF5A5AF7FF5A5AF7FF5A5A
      F7FF5A5AF7FF5A5AF7FF595AF7FF474AEEFF26CEFEFF2ED8FEFF2ED8FEFF2ED8
      FEFF2ED8FEFF2ED8FEFF2ED8FEFF2BC2E9FF228AB3FF1B5C87FF153964FF101B
      47FF121A45FF171F4AFF1B234DFF1C244DFF1C244DFF1B234DFF171F4AFF121A
      45FF111C48FF153A65FF1B5D88FF228BB3FF2BC3EAFF2ED8FEFF2ED8FEFF2ED8
      FEFF2ED8FEFF2ED8FEFF2ED8FEFF25CEFEFF27D1FEFF2FD9FEFF2FD9FEFF2FD9
      FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9
      FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9
      FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9FEFF2FD9
      FEFF2FD9FEFF2FD9FEFF2FD9FEFF27D1FEFF26CEFEFF2ED8FEFF2ED8FEFF2ED8
      FEFF2ED8FEFF2ED8FEFF2ED8FEFF2ED8FEFF2DD3FEFF21A6F3FF1F88D7FF219D
      EBFF2ACBFEFF2ED8FEFF2ED8FEFF2ED8FEFF2ED8FEFF2ED8FEFF2ED8FEFF28C5
      FEFF1F88D7FF1E73C4FF219EEBFF2DD4FEFF2ED8FEFF2ED8FEFF2ED8FEFF2ED8
      FEFF2ED8FEFF2ED8FEFF2ED8FEFF25CEFEFF4F50F2FF5C5CF7FF5C5CF7FF5C5C
      F7FF5C5CF7FF5C5CF7FF5C5CF7FF5C5CF7FF5C5CF7FF4F51F0FF3F43E6FF474B
      EBFF5C5CF7FF5C5CF7FF5C5CF7FF5C5CF7FF5C5CF7FF5C5CF7FF5C5CF7FF5B5B
      F6FF4448E9FF3D42E5FF4F51F0FF5C5CF7FF5C5CF7FF5C5CF7FF5C5CF7FF5C5C
      F7FF5C5CF7FF5C5CF7FF5C5CF7FF4E50F2FF29D3FEFF30D9FEFF30D9FEFF30D9
      FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9
      FEFF2FD4FAFF2FD1F6FF2ECEF3FF2ECDF2FF2ECDF2FF2ECEF3FF2FD1F6FF2FD4
      FAFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9
      FEFF30D9FEFF30D9FEFF30D9FEFF28D3FEFF2ACEF6FB32DAFEFF32DAFEFF32DA
      FEFF32DAFEFF32DAFEFF32DAFEFF32DAFEFF32DAFEFF43E1FEFF4ADAF5FF47E2
      FDFF34DBFEFF32DAFEFF32DAFEFF32DAFEFF32DAFEFF32DAFEFF32DAFEFF35DB
      FEFF48E1FBFF4ADAF5FF42E1FEFF32DAFEFF32DAFEFF32DAFEFF32DAFEFF32DA
      FEFF32DAFEFF32DAFEFF32DAFEFF2ACFF8FC29D3FEFF30D9FEFF30D9FEFF30D9
      FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF2AC4F9FF1A398BFF1A3184FF1A31
      84FF24A1E4FF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF30D9FEFF2186
      CFFF1A3185FF1A3185FF1A398CFF2AC4F9FF30D9FEFF30D9FEFF30D9FEFF30D9
      FEFF30D9FEFF30D9FEFF30D9FEFF28D3FEFF5555F4FF5E5EF7FF5E5EF7FF5E5E
      F7FF5E5EF7FF5E5EF7FF5E5EF7FF5E5EF7FF5657F3FF2B37C3FF2133A0FF2635
      B3FF4B4EEDFF5E5EF7FF5E5EF7FF5E5EF7FF5E5EF7FF5E5EF7FF5E5EF7FF464A
      E8FF2635B0FF2133A1FF2C37C7FF5859F4FF5E5EF7FF5E5EF7FF5E5EF7FF5E5E
      F7FF5E5EF7FF5E5EF7FF5E5EF7FF5455F4FF29C0E2F133DAFEFF33DAFEFF33DA
      FEFF33DAFEFF31D6FEFF31D6FEFF33DAFEFF33DAFEFF33DAFEFF33DAFEFF33DA
      FEFF31D5FEFF32D7FEFF33DAFEFF33DAFEFF33DAFEFF33DAFEFF32D7FEFF31D6
      FEFF33DAFEFF33DAFEFF33DAFEFF33DAFEFF33DAFEFF31D5FEFF32D7FEFF33DA
      FEFF33DAFEFF33DAFEFF33DAFEFF29C0E2F128AECBE434DBFEFF34DBFEFF34DB
      FEFF34DBFEFF34DBFEFF34DBFEFF34DBFEFF35D8FBFF235AA1FF1A3185FF1E42
      90FF36C8EEFF34DBFEFF34DBFEFF34DBFEFF34DBFEFF34DBFEFF34DBFEFF36BF
      E8FF1D3E8EFF1A3185FF2562A7FF35DBFDFF34DBFEFF34DBFEFF34DBFEFF34DB
      FEFF34DBFEFF34DBFEFF34DBFEFF28AECBE429C0E2F133DAFEFF33DAFEFF33DA
      FEFF33DAFEFF33DAFEFF33DAFEFF33DAFEFF2AB8EBFF182B74FF182B74FF182B
      74FF2184C2FF33DAFEFF33DAFEFF33DAFEFF33DAFEFF33DAFEFF33DAFEFF1D63
      ACFF192F80FF192F80FF192F80FF28B2EBFF33DAFEFF33DAFEFF33DAFEFF33DA
      FEFF33DAFEFF33DAFEFF33DAFEFF29C0E2F15151DCF16060F8FF6060F8FF6060
      F8FF6060F8FF6060F8FF6060F8FF6060F8FF464BE1FF1A3186FF1A3185FF1A31
      85FF333EC5FF6060F8FF6060F8FF6060F8FF6060F8FF6060F8FF6060F8FF2D3B
      BBFF1A3185FF1A3185FF1B3187FF4A4EE8FF6060F8FF6060F8FF6060F8FF6060
      F8FF6060F8FF6060F8FF6060F8FF5151DCF12390A5CE35DCFEFF35DCFEFF35DC
      FEFF35DCFEFF28B5EFFF21A6F1FF24B5FEFF27BBFEFF28BEFEFF27BCFEFF24B5
      FEFF21A5EFFF22ACF4FF35DCFEFF35DCFEFF35DCFEFF34DAFEFF22A9F3FF21A6
      F0FF24B6FEFF27BCFEFF28BEFEFF26BBFEFF23B4FEFF21A5F0FF2ABCF3FF35DC
      FEFF35DCFEFF35DCFEFF35DCFEFF2390A5CE1E7B8EBF36DCFEFF36DCFEFF36DC
      FEFF36DCFEFF36DCFEFF36DCFEFF36DCFEFF31BCE8FF1A3084FF1A3084FF1A30
      84FF2887C3FF36DCFEFF36DCFEFF36DCFEFF36DCFEFF36DCFEFF36DCFEFF2676
      B7FF1A3084FF1A3084FF1A3084FF33C9F0FF36DCFEFF36DCFEFF36DCFEFF36DC
      FEFF36DCFEFF36DCFEFF36DCFEFF1E7B8EBF2390A5CE35DCFEFF35DCFEFF35DC
      FEFF35DCFEFF35DCFEFF35DCFEFF35DCFEFF30C6ECFF152360FF152360FF1523
      60FF2792C1FF35DCFEFF35DCFEFF35DCFEFF35DCFEFF35DCFEFF35DCFEFF1C54
      99FF182B75FF182B75FF182B75FF28AFE4FF35DCFEFF35DCFEFF35DCFEFF35DC
      FEFF35DCFEFF35DCFEFF35DCFEFF2390A5CE4141A1CE6363F8FF6363F8FF6363
      F8FF6363F8FF6363F8FF6363F8FF6363F8FF3F46D0FF192E7DFF192E7DFF192E
      7DFF2C39ABFF6363F8FF6363F8FF6363F8FF6363F8FF6363F8FF6363F8FF2736
      A0FF192E7DFF192E7DFF192E7DFF454BDBFF6363F8FF6363F8FF6363F8FF6363
      F8FF6363F8FF6363F8FF6363F8FF4141A1CE165A67A338DDFEFF38DDFEFF38DD
      FEFF38DDFEFF226A9AFF14215AFF16356FFF184C8AFF195898FF184D8CFF1636
      72FF14215AFF162E67FF38DDFEFF38DDFEFF38DDFEFF37DAFBFF152861FF1421
      5AFF163974FF184F8DFF195898FF184A88FF16336DFF14215AFF267CAAFF38DD
      FEFF38DDFEFF38DDFEFF38DDFEFF165A67A311424C8C39DDFEFF39DDFEFF39DD
      FEFF39DDFEFF39DDFEFF39DDFEFF39DDFEFF2BAADCFF192D7AFF192D7AFF192D
      7AFF216FB0FF39DDFEFF39DDFEFF39DDFEFF39DDFEFF39DDFEFF39DDFEFF1F5F
      A3FF192D7AFF192D7AFF192D7AFF30BBE7FF39DDFEFF39DDFEFF39DDFEFF39DD
      FEFF39DDFEFF39DDFEFF39DDFEFF11424C8C165A67A338DDFEFF38DDFEFF38DD
      FEFF38DDFEFF38DDFEFF38DDFEFF38DDFEFF38DBFBFF193762FF111A48FF1321
      4EFF37C5E5FF38DDFEFF38DDFEFF38DDFEFF38DDFEFF38DDFEFF38DDFEFF1D55
      92FF162668FF162668FF162668FF2DB4E3FF38DDFEFF38DDFEFF38DDFEFF38DD
      FEFF38DDFEFF38DDFEFF38DDFEFF165A67A3292965A36565F9FF6565F9FF6565
      F9FF6565F9FF6565F9FF6565F9FF6565F9FF4147CDFF172A72FF172A72FF172A
      72FF2D39A3FF6565F9FF6565F9FF6565F9FF6565F9FF6565F9FF6565F9FF2634
      93FF172A72FF172A72FF172A72FF474DD8FF6565F9FF6565F9FF6565F9FF6565
      F9FF6565F9FF6565F9FF6565F9FF292965A309242A683BDEFEFF3BDEFEFF3BDE
      FEFF3BDEFEFF3BDDFDFF2C97C5FF1B4181FF16276BFF16276BFF16276BFF1627
      6BFF16276BFF1E4F8DFF3BDEFEFF3BDEFEFF3BDEFEFF3BDDFDFF1B4484FF1627
      6BFF16276BFF16276BFF16276BFF16276BFF1C4685FF2E9DCAFF3BDEFEFF3BDE
      FEFF3BDEFEFF3BDEFEFF3BDEFEFF09242A68051519503CDFFEFF3CDFFEFF3CDF
      FEFF3CDFFEFF3CDFFEFF3CDFFEFF3CDFFEFF2AA3D5FF17296FFF17296FFF1729
      6FFF1C64A4FF3CDFFEFF3CDFFEFF3CDFFEFF3CDFFEFF3CDFFEFF3CDFFEFF1B52
      94FF17296FFF17296FFF17296FFF2FB6E3FF3CDFFEFF3CDFFEFF3CDFFEFF3CDF
      FEFF3CDFFEFF3CDFFEFF3CDFFEFF0515195009242A683BDEFEFF3BDEFEFF3BDE
      FEFF3BDEFEFF3BDEFEFF3BDEFEFF3BDEFEFF3BDEFEFF51E6FBFF55DCEEFF54E5
      F7FF3DDFFEFF3BDEFEFF3BDEFEFF3BDEFEFF3BDEFEFF3BDEFEFF3BDEFEFF236A
      9CFF14215AFF14215AFF14215AFF36C6EAFF3BDEFEFF3BDEFEFF3BDEFEFF3BDE
      FEFF3BDEFEFF3BDEFEFF3BDEFEFF09242A68111129686868F9FF6868F9FF6868
      F9FF6868F9FF6868F9FF6868F9FF6868F9FF494ECEFF152564FF152564FF1525
      64FF313B9EFF6868F9FF6868F9FF6868F9FF6868F9FF6868F9FF6868F9FF2A35
      8FFF152564FF152564FF152564FF5256DCFF6868F9FF6868F9FF6868F9FF6868
      F9FF6868F9FF6868F9FF6868F9FF111129680003031E3EE0FEFF3EE0FEFF3EE0
      FEFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF37C3E9FF2494CFFF193683FF182D
      7AFF1B3B86FF39C9EDFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF38C2E8FF1A36
      82FF182D7AFF193B87FF2597D2FF39C7ECFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0
      FEFF3EE0FEFF3EE0FEFF3EE0FEFF0003031E0000000B3AD2EEF73EE0FEFF3EE0
      FEFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF2CAAD8FF152462FF152462FF1524
      62FF1C6AA5FF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF1A58
      94FF152462FF152462FF152462FF32BBE5FF3EE0FEFF3EE0FEFF3EE0FEFF3EE0
      FEFF3EE0FEFF3EE0FEFF3AD2EEF70000000B0003031E3EE0FEFF3EE0FEFF3EE0
      FEFF3EE0FEFF3EE0FEFF3EE0FEFF38D8FEFF2FCDFEFF2BC8FEFF2BC7FEFF2ECC
      FEFF38D9FEFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0FEFF34A1
      C3FF111A4AFF111A4AFF152856FF3EDCFAFF3EE0FEFF3EE0FEFF3EE0FEFF3EE0
      FEFF3EE0FEFF3EE0FEFF3EE0FEFF0003031E0101031E6B6BFAFF6B6BFAFF6B6B
      FAFF6B6BFAFF6B6BFAFF6B6BFAFF6B6BFAFF5C5EDEFF131F55FF131F55FF131F
      55FF4248AEFF6B6BFAFF6B6BFAFF6B6BFAFF6B6BFAFF6B6BFAFF6B6BFAFF3A40
      9EFF131F55FF131F55FF131F55FF6264EAFF6B6BFAFF6B6BFAFF6B6BFAFF6B6B
      FAFF6B6BFAFF6B6BFAFF6B6BFAFF0101031E00000000268394C340E1FEFF40E1
      FEFF40E1FEFF40E1FEFF40E1FEFF40E1FEFF2ACFFBFF1C60AAFF1A3185FF1D41
      90FF39C2E9FF40E1FEFF40E1FEFF40E1FEFF40E1FEFF40E1FEFF40E1FEFF38BB
      E4FF1C3D8DFF1A3185FF1D68B0FF2CD2FDFF40E1FEFF40E1FEFF40E1FEFF40E1
      FEFF40E1FEFF40E1FEFF268394C300000000000000001A5C67A341E1FEFF41E1
      FEFF41E1FEFF41E1FEFF41E1FEFF41E1FEFF34BFE6FF131E53FF131E53FF131E
      53FF2083B7FF41E1FEFF41E1FEFF41E1FEFF41E1FEFF41E1FEFF41E1FEFF1D72
      A7FF131E53FF131E53FF131E53FF39CDF1FF41E1FEFF41E1FEFF41E1FEFF41E1
      FEFF41E1FEFF41E1FEFF1A5C67A30000000000000000268394C340E1FEFF40E1
      FEFF40E1FEFF40E1FEFF2DCCFEFF20B3F8FF1E9EECFF1C92E6FF1B90E5FF1C9B
      EBFF20B3F8FF38D8FEFF40E1FEFF40E1FEFF40E1FEFF40E1FEFF40E1FEFF45E4
      FEFF4AC2D8FF43AFC7FF4ED8EDFF40E1FEFF40E1FEFF40E1FEFF40E1FEFF40E1
      FEFF40E1FEFF40E1FEFF268394C30000000000000000414193C46D6DFAFF6D6D
      FAFF6D6DFAFF6D6DFAFF6D6DFAFF6D6DFAFF6E6EF9FF292E6EFF111845FF1A20
      54FF6869E7FF6869F8FF6D6DFAFF6D6DFAFF6D6DFAFF6C6CFAFF6969F8FF6566
      DFFF171E50FF111845FF2F3478FF6E6EFAFF6D6DFAFF6D6DFAFF6D6DFAFF6D6D
      FAFF6D6DFAFF6D6DFAFF414193C4000000000000000007191C5643E2FEFF43E2
      FEFF43E2FEFF43E2FEFF43E2FEFF43E2FEFF2569ACFF1A3185FF2665A9FF3FD1
      F3FF43E2FEFF43E2FEFF43E2FEFF43E2FEFF43E2FEFF43E2FEFF43E2FEFF43E2
      FEFF3ECBEEFF245DA3FF1A3185FF2874B4FF43E2FEFF43E2FEFF43E2FEFF43E2
      FEFF43E2FEFF43E2FEFF07191C560000000000000000030A0B3644E3FEFF44E3
      FEFF44E3FEFF44E3FEFF44E3FEFF44E3FEFF3FDEFDFF154A78FF101843FF122D
      59FF2FC4EDFF44E3FEFF44E3FEFF44E3FEFF44E3FEFF44E3FEFF44E3FEFF2BBA
      E5FF122854FF101843FF165383FF41E1FEFF44E3FEFF44E3FEFF44E3FEFF44E3
      FEFF44E3FEFF44E3FEFF030A0B36000000000000000007191C5643E2FEFF43E2
      FEFF43E2FEFF43E2FEFF269EE7FF1777D9FF1879DAFF1A7DDBFF1878D9FF1777
      D9FF1778D9FF37C6F5FF43E2FEFF43E2FEFF43E2FEFF43E2FEFF43E2FEFF43E2
      FEFF44E3FEFF45E3FEFF43E2FEFF43E2FEFF43E2FEFF43E2FEFF43E2FEFF43E2
      FEFF43E2FEFF43E2FEFF07191C5600000000000000000C0C1C567070FBFF7070
      FBFF7070FBFF7070FBFF7070FBFF7070FBFF7070FBFF7E7EFCFF8484F5FF8181
      FBFF4F52EFFF4145EAFF5C5DF4FF7070FBFF6F6FFBFF4A4DEDFF4145EAFF5F60
      F5FF8383FAFF8484F5FF7D7DFCFF7070FBFF7070FBFF7070FBFF7070FBFF7070
      FBFF7070FBFF7070FBFF0C0C1C560000000000000000000000022E96A7CF46E4
      FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF3BB6DFFF40CDEEFF46E4FEFF46E4
      FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4
      FEFF46E4FEFF46E4FEFF3FC9ECFF3BB9E1FF46E4FEFF46E4FEFF46E4FEFF46E4
      FEFF46E4FEFF2E96A7CF00000002000000000000000000000000216A76AE47E4
      FEFF47E4FEFF47E4FEFF47E4FEFF47E4FEFF47E4FEFF2AD4FEFF22CDFBFF24D0
      FDFF41E1FEFF47E4FEFF47E4FEFF47E4FEFF47E4FEFF47E4FEFF47E4FEFF40E0
      FEFF24D0FDFF22CEFCFF2CD5FEFF47E4FEFF47E4FEFF47E4FEFF47E4FEFF47E4
      FEFF47E4FEFF216A76AE000000000000000000000000000000022E96A7CF46E4
      FEFF46E4FEFF46E4FEFF3CCDF6FF40D6F9FF46E4FEFF46E4FEFF46E4FEFF41D8
      FAFF3AC9F5FF43DEFCFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4
      FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4FEFF46E4
      FEFF46E4FEFF2E96A7CF000000020000000000000000000000024C4CA5CF7373
      FBFF7373FBFF7373FBFF7373FBFF7373FBFF7373FBFF7373FBFF7171FAFF4F50
      EEFF3238CAFF2B33BDFF494CE3FF7373FBFF6F70FAFF353BCEFF2B33BDFF393E
      D8FF6161F5FF7373FBFF7373FBFF7373FBFF7373FBFF7373FBFF7373FBFF7373
      FBFF7373FBFF4C4CA5CF00000002000000000000000000000000030A0B3647DF
      F8FC49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5
      FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5
      FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5
      FEFF47DFF8FC030A0B36000000000000000000000000000000000103031E43CF
      E4F24AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF48E5FEFF49E6
      FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6
      FEFF49E6FEFF48E5FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6FEFF4AE6
      FEFF43CFE4F20103031E00000000000000000000000000000000030A0B3647DF
      F8FC49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5
      FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF49E5FEFF42E1FEFF36D9FEFF3CDD
      FEFF40E0FEFF42E1FEFF41E0FEFF3ADCFEFF3BDCFEFF49E5FEFF49E5FEFF49E5
      FEFF47DFF8FC030A0B360000000000000000000000000000000005050B367474
      F6FC7676FCFF7676FCFF7676FCFF7676FCFF7676FCFF6F70FAFF4E4FECFF2E36
      C3FF2932BAFF2932BAFF444AD2FF7676FCFF7474FAFF2932BAFF2932BAFF2932
      BAFF373DD2FF5E5EF5FF7575FCFF7676FCFF7676FCFF7676FCFF7676FCFF7676
      FCFF7474F6FC05050B3600000000000000000000000000000000000000001032
      37774CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7
      FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7
      FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7
      FEFF103237770000000000000000000000000000000000000000000000000818
      1A534DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7
      FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7
      FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7FEFF4DE7
      FEFF08181A530000000000000000000000000000000000000000000000001032
      37774CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7
      FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF4CE7FEFF3DD3F9FF1FACEEFF1FB7
      F4FF20C2F8FF21C5F9FF20C1F8FF21B5F2FF31BBF1FF4CE7FEFF4CE7FEFF4CE7
      FEFF103237770000000000000000000000000000000000000000000000001A1A
      36777979FCFF6667F3FF6767F8FF6767F8FF585AF2FF3D42D9FF2B34BDFF2932
      BAFF2932BAFF383FC6FF6F70F4FF7979FCFF7979FCFF5C5FE4FF2C35BDFF2932
      BAFF2932BAFF3038C6FF474AE4FF6263F7FF6869F9FF6364F7FF7071F6FF7979
      FCFF1A1A36770000000000000000000000000000000000000000000000000000
      00001C50579650E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8
      FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8
      FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF1C50
      5796000000000000000000000000000000000000000000000000000000000000
      00001131367650E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9
      FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9
      FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF50E9FEFF1131
      3676000000000000000000000000000000000000000000000000000000000000
      00001C50579650E8FEFF50E8FEFF50E8FEFF50E8FEFF50E8FEFF53E9FEFF56EA
      FEFF56EAFEFF54E9FEFF51E8FEFF50E8FEFF50E8FEFF42CDF5FF1A7EDBFF1777
      D9FF1777D9FF1777D9FF1777D9FF1B80DCFF39BBEFFF50E8FEFF50E8FEFF1C50
      5796000000000000000000000000000000000000000000000000000000000000
      00002B2B57964B50D6FF3139C2FF323AC3FF2A33BBFF2932BAFF2932BAFF3A41
      C8FF5D60E4FF7B7BFCFF7C7CFDFF7C7CFDFF7C7CFDFF7C7CFDFF7475F6FF4E53
      D8FF3038BFFF2932BAFF2932BAFF2C35BEFF333BC4FF333BC3FF6668EBFF2B2B
      5796000000000000000000000000000000000000000000000000000000000000
      000000000001153A3F8053EAFEFF53EAFEFF53EAFEFF53EAFEFF53EAFEFF53EA
      FEFF59EBFEFF6BEEFEFF73F0FFFF77F0FFFF77F0FFFF73F0FFFF6BEEFEFF59EB
      FEFF53EAFEFF53EAFEFF53EAFEFF53EAFEFF53EAFEFF53EAFEFF153A3F800000
      0001000000000000000000000000000000000000000000000000000000000000
      0000000000000C23266350E0F4FA53EAFEFF53EAFEFF53EAFEFF53EAFEFF53EA
      FEFF5FECFEFF76F0FFFF81F2FFFF85F2FFFF85F2FFFF81F2FFFF76F0FFFF5FEC
      FEFF53EAFEFF53EAFEFF53EAFEFF53EAFEFF53EAFEFF50E0F4FA0C2326630000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000001153A3F8053EAFEFF53EAFEFF53EAFEFF5AEBFEFFA3F6FFFFA4F6
      FFFFA4F6FFFFA4F6FFFFA2F6FFFF8EF3FFFF63ECFEFF53EAFEFF4FE3FCFF3EC2
      F1FF36B3ECFF38B6EDFF42CAF4FF51E6FDFF53EAFEFF53EAFEFF153A3F800000
      0001000000000000000000000000000000000000000000000000000000000000
      00000000000120203F807879F8FF6467E8FF5F62E4FF6467E8FF7677F6FF8080
      FDFF8585FDFF9292FEFF9696FEFF9797FEFF9797FEFF9696FEFF9292FEFF8585
      FDFF7E7EFCFF6E70F0FF6164E6FF6063E5FF6A6CEDFF7E7EFCFF20203F800000
      0001000000000000000000000000000000000000000000000000000000000000
      000000000000000000000611134641B2C0DE56ECFEFF56ECFEFF56ECFEFF56EC
      FEFF9AF5FFFFB5F8FFFFB5F8FFFFB5F8FFFFB5F8FFFFB5F8FFFFB5F8FFFF9AF5
      FFFF56ECFEFF56ECFEFF56ECFEFF56ECFEFF41B2C0DE06111346000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000003090A34399EAAD156ECFEFF56ECFEFF56ECFEFF56EC
      FEFF97F4FFFFB5F8FFFFB5F8FFFFB5F8FFFFB5F8FFFFB5F8FFFFB5F8FFFF97F4
      FFFF56ECFEFF56ECFEFF56ECFEFF56ECFEFF399EAAD103090A34000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000611134641B2C0DE56ECFEFF56ECFEFF66EEFEFF8FF3
      FFFFAAF7FFFFB5F8FFFFB5F8FFFFB5F8FFFF9DF5FFFF56ECFEFF56ECFEFF56EC
      FEFF56ECFEFF56ECFEFF56ECFEFF56ECFEFF41B2C0DE06111346000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000090913466262C0DE8383FEFF8383FEFF8383FEFF8383
      FEFFA6A6FFFFBDBDFFFFBDBDFFFFBDBDFFFFBDBDFFFFBDBDFFFFBDBDFFFFA5A5
      FFFF8383FEFF8383FEFF8383FEFF8383FEFF6262C0DE09091346000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000009102A2D6C43B0BDDC5AEDFEFF5AED
      FEFF5AEDFEFF64EEFEFF6CEFFEFF70F0FEFF70F0FEFF6CEFFEFF63EEFEFF5AED
      FEFF5AEDFEFF5AEDFEFF43B0BDDC102A2D6C0000000900000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000040C2124603EA3AFD45AEDFEFF5AED
      FEFF5AEDFEFF60EEFEFF67EFFEFF6BEFFEFF6BEFFEFF67EFFEFF60EEFEFF5AED
      FEFF5AEDFEFF5AEDFEFF3EA3AFD40C2124600000000400000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000009102A2D6C43B0BDDC5AEDFEFF5AED
      FEFF5AEDFEFF5BEDFEFF5DEDFEFF5DEDFEFF5AEDFEFF5AEDFEFF5AEDFEFF5AED
      FEFF5AEDFEFF5AEDFEFF43B0BDDC102A2D6C0000000900000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000918182D6C6363BDDC8787FEFF8787
      FEFF8888FEFF9090FEFF9494FEFF9696FEFF9696FEFF9595FEFF9090FEFF8888
      FEFF8787FEFF8787FEFF6363BDDC18182D6C0000000900000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000001040B0C381639
      3D7D2F7A81B645B3BEDD57E0EEF75DEFFEFF5DEFFEFF57E0EEF745B3BEDD2F7A
      81B616393D7D040B0C3800000001000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000030809301434
      38782E767DB345B0BBDB57DEECF65DEFFEFF5DEFFEFF57DEECF645B0BBDB2E76
      7DB3143438780308093000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000001040B0C381639
      3D7D2F7A81B645B3BEDD57E0EEF75DEFFEFF5DEFFEFF57E0EEF745B3BEDD2F7A
      81B616393D7D040B0C3800000001000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000000000000106060C382121
      3D7D454582B66767BFDD8282EFF78A8AFFFF8A8AFFFF8282EFF76767BFDD4545
      82B621213D7D06060C3800000001000000000000000000000000000000000000
      000000000000000000000000000000000000424D3E000000000000003E000000
      2800000080000000400000000100010000000000000400000000000000000000
      000000000000000000000000FFFFFF0000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000}
  end
  object LightImageList: TImageList
    ColorDepth = cd32Bit
    Height = 25
    Width = 25
    Left = 368
    Top = 280
    Bitmap = {
      494C010103004C01740219001900FFFFFFFF2110FFFFFFFFFFFFFFFF424D3600
      0000000000003600000028000000640000001900000001002000000000001027
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000404040455181818B6292929E9333333FE333333FF333333FE292929E91818
      18B6040404550000000400000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000404040455181818B6292929E9333333FE333333FF333333FE2929
      29E9181818B60404045500000004000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000404040455181818B6292929E9333333FE333333FF3333
      33FE292929E9181818B604040455000000040000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000060606632D2D2DF03D3D3DFF3E3E3EFF3D3D3DFF3636
      36FF323232FF363636FF3D3D3DFF3E3E3EFF3D3D3DFF2D2D2DF0060606630000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000060606632D2D2DF03D3D3DFF3E3E3EFF3D3D
      3DFF363636FF323232FF363636FF3D3D3DFF3E3E3EFF3D3D3DFF2D2D2DF00606
      0663000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000060606632D2D2DF03D3D3DFF3E3E
      3EFF3D3D3DFF363636FF323232FF363636FF3D3D3DFF3E3E3EFF3D3D3DFF2D2D
      2DF0060606630000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000A1A1A1AC0393939FF3B3B
      3BFF272626FF141414FF111111FF151414FF161515FF151414FF111111FF1414
      14FF272626FF3B3B3BFF393939FF1A1A1AC00000000A00000000000000000000
      000000000000000000000000000000000000000000000000000A1A1A1AC03939
      39FF3B3B3BFF272626FF141414FF111111FF151414FF161515FF151414FF1111
      11FF141414FF272626FF3B3B3BFF393939FF1A1A1AC00000000A000000000000
      00000000000000000000000000000000000000000000000000000000000A1A1A
      1AC0393939FF3B3B3BFF272626FF141414FF111111FF151414FF161515FF1514
      14FF111111FF141414FF272626FF3B3B3BFF393939FF1A1A1AC00000000A0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000A202020D7393939FF303030FF141313FF1D1C1CFF1F1E1FFF2E2E30FF3F3F
      40FF474749FF3E3E3FFF262628FF1F1E1FFF1D1C1CFF141313FF303030FF3939
      39FF202020D70000000A00000000000000000000000000000000000000000000
      00000000000A202020D7393939FF303030FF141313FF1D1C1CFF1F1E1FFF1F1F
      34FF1D1D49FF1D1D55FF1E1E48FF1F1F2AFF1F1E1FFF1D1C1CFF141313FF3030
      30FF393939FF202020D70000000A000000000000000000000000000000000000
      0000000000000000000A202020D7393939FF303030FF141313FF1D1C1CFF1F1E
      1FFF1D2D27FF1A3D2EFF184533FF1A3C2EFF1E2624FF1F1E1FFF1D1C1CFF1413
      13FF303030FF393939FF202020D70000000A0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000181818C0363636FF282828FF181717FF2120
      21FF494A4CFF838383FF939393FF919191FF909090FF919191FF939393FF7171
      72FF36373AFF212021FF181717FF282828FF363636FF181818C0000000000000
      000000000000000000000000000000000000181818C0363636FF282828FF1817
      17FF212021FF1F2057FF1616A0FF0F0FB8FF0C0CB6FF0B0BB6FF0D0DB7FF1010
      B7FF191A8BFF20213FFF212021FF181717FF282828FF363636FF181818C00000
      00000000000000000000000000000000000000000000181818C0363636FF2828
      28FF181717FF212021FF1A4936FF0D7E4AFF078D4EFF058A4CFF05894BFF068A
      4DFF078D4FFF116E44FF1D362EFF212021FF181717FF282828FF363636FF1818
      18C0000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000060606633232
      32FF2B2B2BFF191919FF29282AFF78787AFF9A9A9AFF939393FF8F8F8FFF8D8D
      8DFF8C8C8CFF8D8D8DFF8F8F8FFF949494FF9B9B9BFF58595BFF232224FF1919
      19FF2B2B2BFF323232FF06060663000000000000000000000000000000000606
      0663323232FF2B2B2BFF191919FF23222BFF1D1E91FF1111BEFF0A0AB9FF0606
      B6FF0404B5FF0404B4FF0404B5FF0606B7FF0B0BBAFF1414BEFF202168FF2322
      24FF191919FF2B2B2BFF323232FF060606630000000000000000000000000000
      000006060663323232FF2B2B2BFF191919FF222827FF13784BFF089755FF048E
      4EFF02894BFF018749FF018649FF02874AFF038A4BFF059050FF099856FF1959
      3EFF232224FF191919FF2B2B2BFF323232FF0606066300000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000004262626F0303030FF151515FF272728FF858687FFA0A0
      A0FF979797FF929292FF909090FF8F8F8FFF8E8E8EFF8F8F8FFF909090FF9393
      93FF9A9A9AFFA4A4A4FF5C5D5FFF252526FF151515FF303030FF262626F00000
      0004000000000000000000000004262626F0303030FF151515FF252528FF2121
      9EFF1313C4FF0909BDFF0303B9FF0101B7FF0000B7FF0000B6FF0000B7FF0101
      B7FF0404BAFF0B0BBFFF1717C6FF23246CFF252526FF151515FF303030FF2626
      26F000000004000000000000000000000004262626F0303030FF151515FF2527
      27FF148754FF09A15BFF049653FF01904EFF008D4CFF008C4CFF008B4BFF008C
      4CFF008D4DFF02914FFF059955FF0AA55EFF1B5F43FF252526FF151515FF3030
      30FF262626F00000000400000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000040404552D2D2DFF1E1E
      1EFF252525FF6C6D6FFFABABABFFA0A0A0FF999999FF959595FF949494FF9494
      94FF949494FF949494FF949494FF969696FF9B9B9BFFA3A3A3FFADADADFF3F40
      43FF252525FF1E1E1EFF2D2D2DFF040404550000000000000000040404552D2D
      2DFF1E1E1EFF252525FF27287DFF1919CCFF0D0DC4FF0404BEFF0101BCFF0000
      BBFF0000BBFF0000BBFF0000BBFF0000BBFF0101BCFF0707C0FF1010C6FF1E1E
      CCFF272848FF252525FF1E1E1EFF2D2D2DFF0404045500000000000000000404
      04552D2D2DFF1E1E1EFF252525FF1D704CFF0CAF64FF06A35AFF029A54FF0097
      51FF009550FF009550FF009550FF009550FF009550FF009752FF039D56FF07A6
      5DFF0EB267FF234137FF252525FF1E1E1EFF2D2D2DFF04040455000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000131313B62B2B2BFF161515FF313031FFB0B0B0FFABABABFFA2A2
      A2FF9C9C9CFF999999FF999999FF999999FF999999FF999999FF999999FF9A9A
      9AFF9D9D9DFFA5A5A5FFAFAFAFFF8A8A8BFF2A292AFF161616FF2B2B2BFF1313
      13B60000000000000000131313B62B2B2BFF161515FF2B2A33FF2525CDFF1515
      CDFF0A0AC6FF0303C2FF0000C0FF0000C0FF0000C0FF0000C0FF0000C0FF0000
      C0FF0000C0FF0404C3FF0D0DC8FF1A1AD0FF29299EFF2A292AFF161616FF2B2B
      2BFF131313B60000000000000000131313B62B2B2BFF161515FF29312EFF12B9
      6CFF0AB365FF04A95DFF01A258FF009F55FF009F55FF009F55FF009F55FF009F
      55FF009F55FF00A056FF02A459FF06AC5FFF0CB768FF19915AFF2A292AFF1616
      16FF2B2B2BFF131313B600000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000202020E9272727FF1A19
      19FF605F60FFBBBBBBFFAFAFAFFFA6A6A6FFA1A1A1FF9F9F9FFF9F9F9FFF9F9F
      9FFF9F9F9FFF9F9F9FFF9F9F9FFF9F9F9FFFA2A2A2FFA9A9A9FFB2B2B2FFBABA
      BAFF353435FF1A1919FF272727FF202020E90000000000000000202020E92727
      27FF1A1919FF303069FF2323D9FF1414D1FF0909CBFF0202C8FF0000C6FF0000
      C6FF0000C6FF0000C6FF0000C6FF0000C6FF0000C6FF0404C8FF0C0CCDFF1818
      D4FF2929D7FF2E2D37FF1A1919FF272727FF202020E900000000000000002020
      20E9272727FF1A1919FF276349FF10C773FF09BB69FF04B261FF00AC5DFF00AA
      5BFF00AA5BFF00AA5BFF00AA5BFF00AA5BFF00AA5BFF00AA5BFF01AE5EFF05B4
      64FF0BBE6CFF13C774FF2C3432FF1A1919FF272727FF202020E9000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000252525FE202020FF212020FF858586FFBFBFBFFFB4B4B4FFACAC
      ACFFA7A7A7FFA5A5A5FFA5A5A5FFA5A5A5FFA5A5A5FFA5A5A5FFA5A5A5FFA6A6
      A6FFA9A9A9FFAEAEAEFFB8B8B8FFC3C3C3FF545355FF212020FF202020FF2525
      25FE0000000000000000252525FE202020FF212020FF343394FF2424DDFF1515
      D6FF0909D0FF0202CDFF0000CBFF0000CBFF0000CBFF0000CBFF0000CBFF0000
      CBFF0000CBFF0404CDFF0C0CD2FF1919D8FF2A2AE0FF32325BFF212020FF2020
      20FF252525FE0000000000000000252525FE202020FF212020FF248D5DFF11CE
      77FF09C36DFF04BB66FF01B662FF00B460FF00B460FF00B460FF00B460FF00B4
      60FF00B460FF00B561FF01B863FF05BD68FF0CC770FF14D27AFF2B5644FF2120
      20FF202020FF252525FE00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000000000000232323FF1B1B1BFF2525
      25FF979798FFC6C6C6FFBBBBBBFFB3B3B3FFAEAEAEFFACACACFFABABABFFABAB
      ABFFABABABFFABABABFFABABABFFACACACFFAFAFAFFFB6B6B6FFBEBEBEFFC9C9
      C9FF646566FF252525FF1B1B1BFF232323FF0000000000000000232323FF1B1B
      1BFF252525FF3737A8FF2828E2FF1818DBFF0B0BD6FF0404D2FF0000D0FF0000
      D0FF0000D0FF0000D0FF0000D0FF0000D0FF0101D1FF0606D3FF0F0FD7FF1D1D
      DDFF2E2EE5FF35366EFF252525FF1B1B1BFF232323FF00000000000000002323
      23FF1B1B1BFF252525FF23A268FF13D77DFF0BCD74FF05C66DFF01C169FF00BF
      66FF00BE66FF00BE66FF00BE66FF00BE66FF00BE66FF00BF67FF02C26AFF07C8
      6FFF0DD076FF15DB81FF2B6A4FFF252525FF1B1B1BFF232323FF000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000212121FE1B1B1BFF242424FF969697FFCCCCCCFFC3C3C3FFBBBB
      BBFFB5B5B5FFB2B2B2FFB1B1B1FFB0B0B0FFB0B0B0FFB0B0B0FFB1B1B1FFB3B3
      B3FFB7B7B7FFBDBDBDFFC6C6C6FFD0D0D0FF5F6060FF242424FF1B1B1BFF2121
      21FE0000000000000000212121FE1B1B1BFF242424FF3B3BA5FF2E2EE8FF1E1E
      E1FF1010DCFF0707D8FF0202D6FF0000D5FF0000D5FF0000D5FF0000D5FF0101
      D6FF0303D7FF0A0AD9FF1414DDFF2323E3FF3434EAFF383966FF242424FF1B1B
      1BFF212121FE0000000000000000212121FE1B1B1BFF242424FF26A169FF16DF
      84FF0ED77BFF07CF74FF03CA6FFF00C86CFF00C76CFF00C66BFF00C66BFF00C6
      6BFF00C76CFF01C86DFF04CC71FF09D176FF10D97DFF18E287FF2E644DFF2424
      24FF1B1B1BFF212121FE00000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000000000000000000001B1B1BE91C1C1CFF1E1E
      1EFF7B7B7BFFD4D4D4FFCBCBCBFFC3C3C3FFBDBDBDFFBABABAFFB8B8B8FFB7B7
      B7FFB7B7B7FFB7B7B7FFB8B8B8FFBBBBBBFFBFBFBFFFC6C6C6FFCECECEFFD6D6
      D6FF474747FF1E1E1EFF1C1C1CFF1B1B1BE900000000000000001B1B1BE91C1C
      1CFF1E1E1EFF3D3D85FF3737EDFF2626E7FF1919E2FF0E0EDEFF0707DCFF0404
      DBFF0202DAFF0202DAFF0303DAFF0404DBFF0909DDFF1111E0FF1D1DE4FF2C2C
      E9FF3D3DEEFF383849FF1E1E1EFF1C1C1CFF1B1B1BE900000000000000001B1B
      1BE91C1C1CFF1E1E1EFF2E845CFF1AE88AFF12E183FF0BDB7BFF06D676FF03D3
      73FF01D171FF01D171FF00D070FF01D171FF02D172FF04D474FF08D778FF0DDD
      7DFF15E385FF1DEA8DFF344940FF1E1E1EFF1C1C1CFF1B1B1BE9000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000111111B61A1A1AFF171717FF4A4A4AFFDBDBDBFFD4D4D4FFCDCD
      CDFFC7C7C7FFC3C3C3FFC1C1C1FFBFBFBFFFBFBFBFFFC0C0C0FFC1C1C1FFC4C4
      C4FFC9C9C9FFCFCFCFFFD7D7D7FFBABABAFF363637FF171717FF1A1A1AFF1111
      11B60000000000000000111111B61A1A1AFF171717FF39394DFF4141F0FF3232
      EDFF2424E9FF1919E5FF1010E3FF0C0CE1FF0909E0FF0909E0FF0A0AE0FF0D0D
      E2FF1313E3FF1C1CE6FF2929EAFF3737EFFF4141CBFF363637FF171717FF1A1A
      1AFF111111B60000000000000000111111B61A1A1AFF171717FF344C42FF1FEE
      90FF18EA8AFF11E484FF0BE07EFF07DD7BFF05DB79FF04DA78FF04DA77FF04DA
      78FF06DB79FF09DE7CFF0DE181FF13E686FF1AEC8CFF24C97DFF363637FF1717
      17FF1A1A1AFF111111B600000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000003030355181818FF1111
      11FF363535FFA8A8A9FFDCDCDCFFD6D6D6FFD0D0D0FFCCCCCCFFC9C9C9FFC7C7
      C7FFC7C7C7FFC8C8C8FFCACACAFFCDCDCDFFD2D2D2FFD8D8D8FFDEDEDEFF6E6E
      70FF363535FF111111FF181818FF030303550000000000000000030303551818
      18FF111111FF363535FF4545B7FF3F3FF3FF3232EFFF2727ECFF1E1EEAFF1818
      E8FF1515E7FF1414E7FF1515E7FF1A1AE9FF2020EAFF2A2AEDFF3636F1FF4343
      F4FF404077FF363535FF111111FF181818FF0303035500000000000000000303
      0355181818FF111111FF363535FF2BB576FF1EF191FF18ED8CFF12E987FF0EE6
      83FF0BE481FF09E380FF09E380FF0AE380FF0CE582FF0FE784FF14EA89FF1AEE
      8DFF20F393FF327458FF363535FF111111FF181818FF03030355000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000004181818F0141414FF1C1C1CFF4D4D4DFFD3D3D4FFDEDE
      DEFFD9D9D9FFD5D5D5FFD3D3D3FFD1D1D1FFD1D1D1FFD1D1D1FFD3D3D3FFD7D7
      D7FFDBDBDBFFE0E0E0FFADAEAEFF3C3C3CFF1C1C1CFF141414FF181818F00000
      0004000000000000000000000004181818F0141414FF1C1C1CFF3F3F4FFF4A4A
      E6FF4242F5FF3737F2FF2E2EF0FF2828EEFF2424EDFF2424EDFF2525EDFF2A2A
      EEFF3131F0FF3A3AF3FF4545F5FF4748BBFF3C3C3CFF1C1C1CFF141414FF1818
      18F000000004000000000000000000000004181818F0141414FF1C1C1CFF3A4F
      45FF26E68EFF1FF493FF1AF18FFF16EE8CFF13ED8AFF11EC88FF11EB88FF11EC
      88FF14ED8AFF17EF8DFF1BF290FF21F594FF2BBB79FF3C3C3CFF1C1C1CFF1414
      14FF181818F00000000400000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000050505631313
      13FF0E0E0EFF2B2B2BFF5E5E5FFFD5D5D6FFE1E1E1FFDEDEDEFFDBDBDBFFDADA
      DAFFD9D9D9FFDADADAFFDCDCDCFFDFDFDFFFE2E2E2FFB8B8B9FF454546FF2B2B
      2BFF0E0E0EFF131313FF05050563000000000000000000000000000000000505
      0563131313FF0E0E0EFF2B2B2BFF434362FF4E4EE7FF4848F6FF4141F4FF3B3B
      F3FF3838F2FF3737F2FF3838F2FF3D3DF3FF4343F5FF4A4AF7FF4B4BC7FF3F3F
      46FF2B2B2BFF0E0E0EFF131313FF050505630000000000000000000000000000
      000005050563131313FF0E0E0EFF2B2B2BFF3A6150FF28E790FF22F695FF1FF4
      93FF1CF291FF1AF18FFF1AF18FFF1BF190FF1DF291FF20F494FF23F696FF2BC6
      80FF3D4543FF2B2B2BFF0E0E0EFF131313FF0505056300000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000101010C00E0E0EFF0C0C0CFF2C2C2CFF5050
      50FFA9A9AAFFE4E4E4FFE3E3E3FFE2E2E2FFE2E2E2FFE2E2E2FFE4E4E4FFDBDB
      DBFF8C8C8EFF434343FF2C2C2CFF0C0C0CFF0E0E0EFF101010C0000000000000
      000000000000000000000000000000000000101010C00E0E0EFF0C0C0CFF2C2C
      2CFF434352FF4D4DB6FF5050F7FF4C4CF7FF4A4AF7FF4949F7FF4B4BF7FF4E4E
      F8FF5050EDFF494A96FF404043FF2C2C2CFF0C0C0CFF0E0E0EFF101010C00000
      00000000000000000000000000000000000000000000101010C00E0E0EFF0C0C
      0CFF2C2C2CFF3E5249FF31B579FF26F698FF24F797FF23F696FF23F696FF23F6
      97FF25F798FF28ED93FF349569FF404341FF2C2C2CFF0C0C0CFF0E0E0EFF1010
      10C0000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000A121212D70C0C0CFF0A0A0AFF1E1E1EFF3D3D3DFF555555FF898989FFA1A1
      A2FFABABACFF9C9D9DFF787979FF494949FF3D3D3DFF1E1E1EFF0A0A0AFF0C0C
      0CFF121212D70000000A00000000000000000000000000000000000000000000
      00000000000A121212D70C0C0CFF0A0A0AFF1E1E1EFF3D3D3DFF454557FF4C4D
      91FF4F4FACFF5050B7FF4E4EA7FF4A4A80FF43434AFF3D3D3DFF1E1E1EFF0A0A
      0AFF0C0C0CFF121212D70000000A000000000000000000000000000000000000
      0000000000000000000A121212D70C0C0CFF0A0A0AFF1E1E1EFF3D3D3DFF3F57
      4CFF389168FF34AB75FF33B77AFF35A673FF3B8060FF414A46FF3D3D3DFF1E1E
      1EFF0A0A0AFF0C0C0CFF121212D70000000A0000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000A0F0F0FC00B0B0BFF0909
      09FF090909FF181818FF272727FF303030FF343434FF303030FF272727FF1818
      18FF090909FF090909FF0B0B0BFF0F0F0FC00000000A00000000000000000000
      000000000000000000000000000000000000000000000000000A0F0F0FC00B0B
      0BFF090909FF090909FF181818FF272727FF303030FF343434FF303030FF2727
      27FF181818FF090909FF090909FF0B0B0BFF0F0F0FC00000000A000000000000
      00000000000000000000000000000000000000000000000000000000000A0F0F
      0FC00B0B0BFF090909FF090909FF181818FF272727FF303030FF343434FF3030
      30FF272727FF181818FF090909FF090909FF0B0B0BFF0F0F0FC00000000A0000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000004040463101010F0080808FF060606FF060606FF0505
      05FF050505FF050505FF060606FF060606FF080808FF101010F0040404630000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000000000000000000004040463101010F0080808FF060606FF0606
      06FF050505FF050505FF050505FF060606FF060606FF080808FF101010F00404
      0463000000000000000000000000000000000000000000000000000000000000
      00000000000000000000000000000000000004040463101010F0080808FF0606
      06FF060606FF050505FF050505FF050505FF060606FF060606FF080808FF1010
      10F0040404630000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0004030303550D0D0DB6131313E9141414FE131313FF141414FE131313E90D0D
      0DB6030303550000000400000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000004030303550D0D0DB6131313E9141414FE131313FF141414FE1313
      13E90D0D0DB60303035500000004000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      00000000000000000004030303550D0D0DB6131313E9141414FE131313FF1414
      14FE131313E90D0D0DB603030355000000040000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000424D3E000000000000003E00000028000000640000001900000001000100
      00000000900100000000000000000000000000000000000000000000FFFFFF00
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      0000000000000000000000000000000000000000000000000000000000000000
      000000000000}
  end
end
