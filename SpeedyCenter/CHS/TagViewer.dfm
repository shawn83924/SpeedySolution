object TagForm: TTagForm
  Left = 416
  Top = 272
  BorderIcons = [biSystemMenu]
  Caption = 'Tag Inspector'
  ClientHeight = 316
  ClientWidth = 736
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -17
  Font.Name = 'Tahoma'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poDesktopCenter
  Scaled = False
  PixelsPerInch = 120
  TextHeight = 21
  object TagListView: TListView
    Left = 0
    Top = 33
    Width = 736
    Height = 283
    Align = alClient
    Color = clNavy
    Columns = <
      item
        Caption = 'Tag'
        Width = 60
      end
      item
        Caption = 'Field Name'
        Width = 200
      end
      item
        Caption = 'Value'
        Width = 200
      end
      item
        Caption = 'Value Description'
        Width = 250
      end>
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWhite
    Font.Height = -18
    Font.Name = 'Arial'
    Font.Style = []
    GridLines = True
    StyleElements = []
    ReadOnly = True
    RowSelect = True
    ParentFont = False
    TabOrder = 0
    ViewStyle = vsReport
    OnColumnClick = TagListViewColumnClick
    OnCompare = TagListViewCompare
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 736
    Height = 33
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object MsgTypeLabel: TLabel
      Left = 13
      Top = 3
      Width = 668
      Height = 28
      AutoSize = False
      Font.Charset = DEFAULT_CHARSET
      Font.Color = clWindowText
      Font.Height = -20
      Font.Name = 'Arial'
      Font.Style = [fsBold]
      ParentFont = False
    end
  end
end
