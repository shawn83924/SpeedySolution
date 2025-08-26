object LineFrame: TLineFrame
  Left = 0
  Top = 0
  Width = 919
  Height = 36
  TabOrder = 0
  object Bevel1: TBevel
    Left = 0
    Top = 0
    Width = 56
    Height = 36
    Align = alLeft
  end
  object BrokerIDText: TLabel
    Left = 56
    Top = 0
    Width = 74
    Height = 36
    Align = alLeft
    Alignment = taCenter
    AutoSize = False
    Caption = #26399#36008#21830#20195#34399
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object PasswordText: TLabel
    Left = 130
    Top = 0
    Width = 74
    Height = 36
    Align = alLeft
    Alignment = taCenter
    AutoSize = False
    Caption = '****'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object PVCNoText: TLabel
    Left = 300
    Top = 0
    Width = 75
    Height = 36
    Align = alLeft
    AutoSize = False
    Caption = 'PVCNoText'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGreen
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object ApplicationText: TLabel
    Left = 375
    Top = 0
    Width = 150
    Height = 36
    Align = alLeft
    AutoSize = False
    Caption = 'ApplicationText'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clMaroon
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object MessageText: TLabel
    Left = 525
    Top = 0
    Width = 394
    Height = 36
    Align = alClient
    Caption = 'MessageText'
  end
  object StatusText: TLabel
    Left = 204
    Top = 0
    Width = 96
    Height = 36
    Align = alLeft
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clGreen
    Font.Height = -13
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
  end
  object ResetButton: TSpeedButton
    Left = 4
    Top = 3
    Width = 48
    Height = 28
    Glyph.Data = {
      76010000424D7601000000000000760000002800000020000000100000000100
      04000000000000010000120B0000120B00001000000000000000000000000000
      800000800000008080008000000080008000808000007F7F7F00BFBFBF000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333000003
      333333333F777773FF333333008888800333333377333F3773F3333077870787
      7033333733337F33373F3308888707888803337F33337F33337F330777880887
      7703337F33337FF3337F3308888000888803337F333777F3337F330777700077
      7703337F33377733337FB3088888888888033373FFFFFFFFFF733B3000000000
      0033333777777777773333BBBB3333080333333333F3337F7F33BBBB707BB308
      03333333373F337F7F3333BB08033308033333337F7F337F7F333B3B08033308
      033333337F73FF737F33B33B778000877333333373F777337333333B30888880
      33333333373FFFF73333333B3300000333333333337777733333}
    NumGlyphs = 2
    OnClick = ResetButtonClick
  end
  object Timer: TTimer
    Enabled = False
    OnTimer = TimerTimer
    Left = 520
  end
end
