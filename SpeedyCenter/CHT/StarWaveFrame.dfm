object SWFrame: TSWFrame
  Left = 0
  Top = 0
  Width = 580
  Height = 209
  TabOrder = 0
  object ValueListEditor: TValueListEditor
    Left = 199
    Top = 36
    Width = 373
    Height = 163
    Margins.Left = 1
    Margins.Top = 1
    Margins.Right = 1
    Margins.Bottom = 1
    BorderStyle = bsNone
    Ctl3D = False
    DefaultColWidth = 135
    DefaultDrawing = False
    DefaultRowHeight = 22
    DisplayOptions = [doAutoColResize, doKeyColFixed]
    DoubleBuffered = True
    Enabled = False
    FixedColor = clSilver
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -16
    Font.Name = 'Arial'
    Font.Style = []
    Options = [goFixedHorzLine, goHorzLine, goRowSelect, goThumbTracking]
    ParentColor = True
    ParentCtl3D = False
    ParentDoubleBuffered = False
    ParentFont = False
    ScrollBars = ssNone
    Strings.Strings = (
      'Bandwidth='
      'UDP Count='
      'UDP Recv='
      'TCP Output='
      'MarketData Queue='
      'Command Queue='
      'Connections=')
    TabOrder = 0
    OnDrawCell = ValueListEditorDrawCell
    ColWidths = (
      135
      236)
  end
  object PerformanceGauge: TPerformanceGauge
    Left = 5
    Top = 36
    Width = 188
    Height = 161
    DrawFill = True
    MaxValue = 1000
    GridColor = clGreen
    LineColor = clYellow
  end
  object TitleStaticText: TStaticText
    Left = 5
    Top = 4
    Width = 209
    Height = 26
    Caption = 'StarWave@MDUbuntu'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 2
  end
end
