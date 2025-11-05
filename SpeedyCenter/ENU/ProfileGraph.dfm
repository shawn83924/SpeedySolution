object ProfileFrame: TProfileFrame
  Left = 0
  Top = 0
  Width = 464
  Height = 206
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -16
  Font.Name = 'Tahoma'
  Font.Style = []
  ParentFont = False
  TabOrder = 0
  object PerformanceGauge: TPerformanceGauge
    Left = 5
    Top = 36
    Width = 188
    Height = 161
    MaxValue = 1000
    GridColor = clGreen
    LineColor = clYellow
  end
  object TitleStaticText: TStaticText
    Left = 5
    Top = 4
    Width = 157
    Height = 26
    Caption = 'Futures MBus in'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -19
    Font.Name = 'Arial'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 1
  end
  object ValueListEditor: TValueListEditor
    Left = 199
    Top = 36
    Width = 259
    Height = 163
    BorderStyle = bsNone
    Ctl3D = False
    DefaultColWidth = 135
    DefaultDrawing = False
    DefaultRowHeight = 22
    DisplayOptions = [doAutoColResize, doKeyColFixed]
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
    ParentFont = False
    ScrollBars = ssNone
    Strings.Strings = (
      'Max='
      'Min='
      'Avg='
      'Key of Max value='
      'Throughput='
      'Throughput Max='
      'Count=')
    TabOrder = 2
    OnDrawCell = ValueListEditorDrawCell
    ColWidths = (
      135
      122)
  end
end
