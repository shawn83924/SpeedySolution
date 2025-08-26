object TradingForm: TTradingForm
  Left = 0
  Top = 0
  Caption = 'TradingForm'
  ClientHeight = 557
  ClientWidth = 702
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  DesignSize = (
    702
    557)
  PixelsPerInch = 110
  TextHeight = 14
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 64
    Height = 17
    Caption = 'BrokerID'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold, fsUnderline]
    ParentFont = False
    Transparent = False
  end
  object Label2: TLabel
    Left = 144
    Top = 8
    Width = 48
    Height = 17
    Caption = 'UserID'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold, fsUnderline]
    ParentFont = False
    Transparent = False
  end
  object Label3: TLabel
    Left = 280
    Top = 8
    Width = 64
    Height = 17
    Caption = 'Password'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold, fsUnderline]
    ParentFont = False
    Transparent = False
  end
  object Label4: TLabel
    Left = 416
    Top = 8
    Width = 104
    Height = 17
    Caption = 'ServerAddress'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold, fsUnderline]
    ParentFont = False
    Transparent = False
  end
  object Memo1: TMemo
    Left = 134
    Top = 56
    Width = 561
    Height = 493
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
    ScrollBars = ssVertical
    TabOrder = 0
  end
  object Button2: TButton
    Left = 8
    Top = 303
    Width = 120
    Height = 43
    Caption = 'Settlement Info'
    TabOrder = 1
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 8
    Top = 351
    Width = 120
    Height = 43
    Caption = 'Investor Position'
    TabOrder = 2
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 8
    Top = 400
    Width = 120
    Height = 43
    Caption = 'QryTradingAccount'
    TabOrder = 3
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 8
    Top = 447
    Width = 120
    Height = 43
    Caption = 'Chang Password'
    TabOrder = 4
    OnClick = Button5Click
  end
  object Button1: TButton
    Left = 8
    Top = 496
    Width = 120
    Height = 43
    Caption = 'New Order'
    TabOrder = 5
    OnClick = Button1Click
  end
  object BtnConn_Login: TButton
    Left = 8
    Top = 64
    Width = 120
    Height = 58
    Caption = 'Connect + Login'
    TabOrder = 6
    OnClick = BtnConn_LoginClick
  end
  object BtnDisconnect: TButton
    Left = 8
    Top = 127
    Width = 120
    Height = 58
    Caption = 'Disconnect'
    TabOrder = 7
    OnClick = BtnDisconnectClick
  end
  object TxtBrokerID: TEdit
    Left = 8
    Top = 28
    Width = 120
    Height = 25
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 8
  end
  object TxtUserID: TEdit
    Left = 144
    Top = 28
    Width = 120
    Height = 25
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 9
  end
  object TxtPassword: TEdit
    Left = 280
    Top = 28
    Width = 120
    Height = 25
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 10
  end
  object TxtServerAddress: TEdit
    Left = 416
    Top = 28
    Width = 225
    Height = 25
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -14
    Font.Name = 'Consolas'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 11
  end
end
