object PublisherForm: TPublisherForm
  Left = 538
  Top = 160
  BorderStyle = bsDialog
  Caption = 'Publisher'
  ClientHeight = 408
  ClientWidth = 226
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object Label1: TLabel
    Left = 16
    Top = 369
    Width = 193
    Height = 25
    AutoSize = False
    Caption = '0 msg/sec'
  end
  object RadioGroup1: TRadioGroup
    Left = 15
    Top = 11
    Width = 194
    Height = 315
    Caption = 'Message Out Rate'
    ItemIndex = 8
    Items.Strings = (
      'sleep 1ms'
      'sleep 2 ms'
      'sleep 5 ms'
      'sleep 10 ms'
      'sleep 20 ms'
      'sleep 50 ms'
      'sleep 100 ms'
      'sleep 200 ms'
      'sleep 500 ms'
      'sleep 1000 ms'
      'don'#39't sleep'
      'stop')
    TabOrder = 0
    OnClick = RadioGroup1Click
  end
  object BitBtn1: TBitBtn
    Left = 16
    Top = 336
    Width = 75
    Height = 25
    Caption = 'Sync IO'
    TabOrder = 1
    OnClick = BitBtn1Click
  end
  object MessageObject1: TMessageObject
    AppName = 'WinPublisher'
    Host = '127.0.0.1'
    Port = 12345
    OnConnected = MessageObject1Connected
    OnError = MessageObject1Error
    OnDisconnected = MessageObject1Disconnected
    Left = 176
    Top = 96
  end
  object Publisher1: TPublisher
    MessageObject = MessageObject1
    Subject = 'TFX.UNI.PROD.SCH'
    Key = 'OPT'
    Left = 176
    Top = 64
  end
  object BenchmarkTimer: TTimer
    OnTimer = BenchmarkTimerTimer
    Left = 168
    Top = 288
  end
  object Timer: TThreadedTimer
    Enabled = False
    Interval = 20
    OnTimer = TimerTimer
    Left = 176
    Top = 128
  end
end
