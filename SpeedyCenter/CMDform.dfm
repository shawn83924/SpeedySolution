object TelnetForm: TTelnetForm
  Left = 576
  Top = 263
  BorderStyle = bsToolWindow
  Caption = 'Speedy CMD'
  ClientHeight = 605
  ClientWidth = 862
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -13
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnHide = FormHide
  OnShow = FormShow
  PixelsPerInch = 120
  TextHeight = 16
  object StdoutListBox: TListBox
    Left = 0
    Top = 0
    Width = 862
    Height = 576
    TabStop = False
    Style = lbVirtual
    Align = alClient
    BorderStyle = bsNone
    Color = clBlack
    Font.Charset = CHINESEBIG5_CHARSET
    Font.Color = clYellow
    Font.Height = -12
    Font.Name = 'Terminal'
    Font.Style = []
    ImeName = #20013#25991' ('#32321#39636') - '#26032#27880#38899
    IntegralHeight = True
    ItemHeight = 16
    ParentFont = False
    TabOrder = 0
    OnData = StdoutListBoxData
  end
  object Panel1: TPanel
    Left = 0
    Top = 576
    Width = 862
    Height = 29
    Align = alBottom
    Caption = 'Panel1'
    TabOrder = 1
    object Label1: TLabel
      Left = 8
      Top = 7
      Width = 62
      Height = 16
      Caption = 'Command'
    end
    object CMDEdit: TEdit
      Left = 77
      Top = 1
      Width = 785
      Height = 28
      Color = clBlack
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clYellow
      Font.Height = -17
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ImeName = #20013#25991' ('#32321#39636') - '#26032#27880#38899
      ParentFont = False
      TabOrder = 0
      OnKeyDown = CMDEditKeyDown
    end
  end
  object CMDPublisher: TPublisher
    MessageObject = SimTFXForm.MessageObject
    Subject = 'all'
    Key = 'all'
    Left = 432
    Top = 128
  end
  object STDIOSubscriber: TSubscriber
    MessageObject = SimTFXForm.MessageObject
    Subject = 'all'
    Key = 'all'
    OnMessage = STDIOSubscriberMessage
    Left = 432
    Top = 160
  end
end
