object Form1: TForm1
  Left = 542
  Top = 176
  Width = 777
  Height = 503
  Caption = 'WinSubscriber'
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
  object ListView: TListView
    Left = 0
    Top = 65
    Width = 769
    Height = 398
    Align = alClient
    Columns = <
      item
        Caption = 'Subject'
        Width = 250
      end
      item
        Caption = 'Key'
        Width = 150
      end
      item
        Caption = 'Count'
        Width = 80
      end>
    TabOrder = 0
    ViewStyle = vsReport
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 769
    Height = 65
    Align = alTop
    TabOrder = 1
    object Label2: TLabel
      Left = 178
      Top = 8
      Width = 23
      Height = 16
      Caption = 'Key'
    end
    object Label1: TLabel
      Left = 8
      Top = 8
      Width = 45
      Height = 16
      Caption = 'Subject'
    end
    object KeyEdit: TEdit
      Left = 176
      Top = 24
      Width = 121
      Height = 24
      TabOrder = 0
      Text = 'all'
    end
    object SubjectEdit: TEdit
      Left = 8
      Top = 24
      Width = 161
      Height = 24
      TabOrder = 1
      Text = 'all'
    end
    object SubscribeButton: TButton
      Left = 304
      Top = 24
      Width = 89
      Height = 25
      Caption = 'Subscribe'
      TabOrder = 2
      OnClick = SubscribeButtonClick
    end
  end
  object MessageObject: TMessageObject
    AppName = 'WinSubscriber'
    Host = '127.0.0.1'
    Port = 12345
    OnConnected = MessageObjectConnected
    OnError = MessageObjectError
    OnDisconnected = MessageObjectDisconnected
    Left = 64
    Top = 272
  end
end
