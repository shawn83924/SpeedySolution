object TradingForm: TTradingForm
  Left = 0
  Top = 0
  Caption = 'TradingForm'
  ClientHeight = 638
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
    638)
  PixelsPerInch = 110
  TextHeight = 14
  object Label1: TLabel
    Left = 8
    Top = 48
    Width = 54
    Height = 14
    Caption = 'BrokerID'
    Color = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold, fsUnderline]
    ParentColor = False
    ParentFont = False
  end
  object Label2: TLabel
    Left = 144
    Top = 48
    Width = 40
    Height = 14
    Caption = 'UserID'
    Color = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold, fsUnderline]
    ParentColor = False
    ParentFont = False
  end
  object Label3: TLabel
    Left = 280
    Top = 48
    Width = 59
    Height = 14
    Caption = 'Password'
    Color = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold, fsUnderline]
    ParentColor = False
    ParentFont = False
  end
  object Label4: TLabel
    Left = 416
    Top = 48
    Width = 88
    Height = 14
    Caption = 'ServerAddress'
    Color = clBlack
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -12
    Font.Name = 'Tahoma'
    Font.Style = [fsBold, fsUnderline]
    ParentColor = False
    ParentFont = False
  end
  object Button1: TButton
    Left = 8
    Top = 584
    Width = 120
    Height = 43
    Caption = 'New Order'
    TabOrder = 0
    OnClick = Button1Click
  end
  object Memo1: TMemo
    Left = 134
    Top = 104
    Width = 561
    Height = 526
    Anchors = [akLeft, akTop, akRight, akBottom]
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object Button2: TButton
    Left = 8
    Top = 389
    Width = 120
    Height = 43
    Caption = 'Settlement Info'
    TabOrder = 2
    OnClick = Button2Click
  end
  object Button3: TButton
    Left = 8
    Top = 439
    Width = 120
    Height = 43
    Caption = 'QrySettlementInfo'
    TabOrder = 3
    OnClick = Button3Click
  end
  object Button4: TButton
    Left = 8
    Top = 488
    Width = 120
    Height = 43
    Caption = 'QryTradingAccount'
    TabOrder = 4
    OnClick = Button4Click
  end
  object Button5: TButton
    Left = 8
    Top = 535
    Width = 120
    Height = 43
    Caption = 'Chang Password'
    TabOrder = 5
    OnClick = Button5Click
  end
  object BtnConn_Login: TButton
    Left = 8
    Top = 168
    Width = 120
    Height = 58
    Caption = 'Connect + Login'
    TabOrder = 6
    OnClick = BtnConn_LoginClick
  end
  object BtnDisconnect: TButton
    Left = 8
    Top = 232
    Width = 120
    Height = 58
    Caption = 'Disconnect'
    TabOrder = 7
    OnClick = BtnDisconnectClick
  end
  object TxtBrokerID: TEdit
    Left = 8
    Top = 68
    Width = 120
    Height = 22
    TabOrder = 8
  end
  object TxtUserID: TEdit
    Left = 144
    Top = 68
    Width = 120
    Height = 22
    TabOrder = 9
  end
  object TxtPassword: TEdit
    Left = 280
    Top = 68
    Width = 120
    Height = 22
    TabOrder = 10
  end
  object TxtServerAddress: TEdit
    Left = 416
    Top = 68
    Width = 225
    Height = 22
    TabOrder = 11
  end
  object RadioCTP: TRadioButton
    Left = 8
    Top = 14
    Width = 169
    Height = 17
    Caption = #19978#26399#21407#29983'CTP'
    Checked = True
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 12
    TabStop = True
  end
  object RadioHSctp: TRadioButton
    Left = 188
    Top = 14
    Width = 170
    Height = 17
    Caption = #24658#29983#20223'CTP (HSctp)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 13
  end
  object RadioKNctp: TRadioButton
    Left = 364
    Top = 14
    Width = 169
    Height = 17
    Caption = #37329#29275#20223'CTP (KNctp)'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -15
    Font.Name = 'Tahoma'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 14
  end
  object BtnLoadDll: TButton
    Left = 8
    Top = 104
    Width = 120
    Height = 58
    Caption = 'Load CTPLike dll'
    TabOrder = 15
    OnClick = BtnLoadDllClick
  end
end
