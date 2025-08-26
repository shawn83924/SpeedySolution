object TradingForm: TTradingForm
  Left = 0
  Top = 0
  Caption = 'TradingForm'
  ClientHeight = 862
  ClientWidth = 1214
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -10
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  OnCreate = FormCreate
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 12
  object Panel_Main: TPanel
    Left = 139
    Top = 49
    Width = 1075
    Height = 639
    Align = alClient
    BevelOuter = bvNone
    TabOrder = 0
    object Splitter1: TSplitter
      Left = 0
      Top = 350
      Width = 1075
      Height = 4
      Cursor = crVSplit
      Align = alTop
      Color = clBtnShadow
      ParentColor = False
      ExplicitTop = 297
      ExplicitWidth = 745
    end
    object Memo1: TMemo
      Left = 0
      Top = 0
      Width = 1075
      Height = 350
      Align = alTop
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold]
      ParentFont = False
      ScrollBars = ssVertical
      TabOrder = 0
    end
    object Panel_LockRtn: TPanel
      Left = 0
      Top = 354
      Width = 1075
      Height = 285
      Align = alClient
      BevelOuter = bvNone
      TabOrder = 1
      object Panel_LockRtn_Menu: TPanel
        Left = 0
        Top = 0
        Width = 270
        Height = 285
        Align = alLeft
        BevelOuter = bvNone
        TabOrder = 0
        object Panel_LockRtn_Grid_Tit: TPanel
          Left = 242
          Top = 0
          Width = 28
          Height = 285
          Align = alRight
          TabOrder = 0
          object Panel_LockRtn_StockQry_Tit: TPanel
            Left = 1
            Top = 1
            Width = 26
            Height = 144
            Align = alTop
            BevelKind = bkTile
            BevelOuter = bvNone
            Color = clWhite
            ParentBackground = False
            TabOrder = 0
            object Label_LockRtn_StockQry_Tit: TLabel
              Left = 0
              Top = 0
              Width = 22
              Height = 140
              Align = alClient
              Alignment = taCenter
              Color = 16512227
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -13
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = False
              Layout = tlCenter
              WordWrap = True
              ExplicitWidth = 4
              ExplicitHeight = 16
            end
          end
          object Panel_LockRtn_StockLock_Tit: TPanel
            Left = 1
            Top = 145
            Width = 26
            Height = 139
            Align = alClient
            BevelKind = bkTile
            BevelOuter = bvNone
            Color = clWhite
            ParentBackground = False
            TabOrder = 1
            object Label_LockRtn_StockLock_Tit: TLabel
              Left = 0
              Top = 0
              Width = 22
              Height = 135
              Align = alClient
              Alignment = taCenter
              Color = 14220026
              Font.Charset = DEFAULT_CHARSET
              Font.Color = clWindowText
              Font.Height = -13
              Font.Name = 'Tahoma'
              Font.Style = []
              ParentColor = False
              ParentFont = False
              Transparent = False
              Layout = tlCenter
              WordWrap = True
              ExplicitWidth = 4
              ExplicitHeight = 16
            end
          end
        end
        object Panel_LockRtn_Menu_Main: TPanel
          Left = 0
          Top = 0
          Width = 242
          Height = 285
          Align = alClient
          TabOrder = 1
          object Label_LockRtn_Menu: TLabel
            Left = 1
            Top = 1
            Width = 240
            Height = 29
            Align = alTop
            Alignment = taCenter
            AutoSize = False
            Caption = #20633#20812#35657#21048#26597#35426#65295#35657#21048#37782#23450'('#35299#37782')'
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = [fsBold]
            ParentFont = False
            Layout = tlCenter
          end
          object Label_StockID: TLabel
            Left = 6
            Top = 36
            Width = 80
            Height = 25
            Alignment = taRightJustify
            AutoSize = False
            Caption = #20195#30908
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            Layout = tlCenter
          end
          object Label_MaxLock: TLabel
            Left = 6
            Top = 81
            Width = 80
            Height = 25
            Alignment = taRightJustify
            AutoSize = False
            Caption = #26368#22823#21487#37782#23450
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            Layout = tlCenter
          end
          object Label_MaxUnLock: TLabel
            Left = 6
            Top = 111
            Width = 80
            Height = 25
            Alignment = taRightJustify
            AutoSize = False
            Caption = #26368#22823#21487#35299#37782
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            Layout = tlCenter
          end
          object Label_Qty: TLabel
            Left = 6
            Top = 192
            Width = 80
            Height = 25
            Alignment = taRightJustify
            AutoSize = False
            Caption = #25976#37327
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            Layout = tlCenter
          end
          object ComboBox_StockID: TComboBox
            Left = 92
            Top = 36
            Width = 142
            Height = 24
            Style = csDropDownList
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 0
          end
          object Edit_MaxLock: TEdit
            Left = 92
            Top = 82
            Width = 70
            Height = 24
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 1
          end
          object Edit_MaxUnLock: TEdit
            Left = 92
            Top = 111
            Width = 70
            Height = 24
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 2
          end
          object Edit_Qty: TEdit
            Left = 94
            Top = 192
            Width = 142
            Height = 24
            BiDiMode = bdLeftToRight
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentBiDiMode = False
            ParentFont = False
            TabOrder = 3
          end
          object BitBtn_MaxLock_Qry: TBitBtn
            Left = 164
            Top = 82
            Width = 70
            Height = 25
            Caption = #26597#35426
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 4
          end
          object BitBtn_MaxUnLock_Qry: TBitBtn
            Left = 164
            Top = 110
            Width = 70
            Height = 25
            Caption = #26597#35426
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            ParentFont = False
            TabOrder = 5
          end
          object BitBtn_Lock: TBitBtn
            Left = 94
            Top = 220
            Width = 70
            Height = 25
            Caption = #37782#23450
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            Glyph.Data = {
              E6040000424DE604000000000000360000002800000014000000140000000100
              180000000000B0040000130B0000130B00000000000000000000FF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
              00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A
              763A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A
              0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A76
              3A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0A763A0A
              763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A
              0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0A763A0A763A0A76
              3A0A763A0A763A0AFF00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0A
              FF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A
              0A763A0AFF00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF
              00FFFF00FFFF00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FF
              FF00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF00
              FFFF00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF
              00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FF
              FF00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF00FF763A
              0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF76
              3A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A
              763A0A763A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A
              0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A76
              3A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0A
              763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A
              0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF
              00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF
              00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
              00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A
              763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0AFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FF}
            ParentFont = False
            TabOrder = 6
          end
          object BitBtn_UnLock: TBitBtn
            Left = 166
            Top = 220
            Width = 70
            Height = 25
            Caption = #35299#37782
            Font.Charset = DEFAULT_CHARSET
            Font.Color = clWindowText
            Font.Height = -13
            Font.Name = 'Tahoma'
            Font.Style = []
            Glyph.Data = {
              E6040000424DE604000000000000360000002800000014000000140000000100
              180000000000B0040000130B0000130B00000000000000000000FF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
              00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A
              763A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A
              0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A76
              3A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0A763A0A
              763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A
              0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0A763A0A763A0A76
              3A0A763A0A763A0AFF00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0A
              FF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A
              0A763A0AFF00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF
              00FFFF00FFFF00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FF
              FF00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF00
              FFFF00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF
              00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FF
              FF00FF763A0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF00FF763A
              0A763A0A763A0A763A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF76
              3A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A
              763A0A763A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A
              0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A76
              3A0A763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0A
              763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A
              0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
              00FFFF00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF
              00FFFF00FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FF763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF
              00FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A
              763A0A763A0AFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FF763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0A763A0AFF
              00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FF
              FF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00FFFF00
              FFFF00FFFF00FFFF00FF}
            ParentFont = False
            TabOrder = 7
          end
        end
      end
      object Panel_LockRtn_Grid: TPanel
        Left = 270
        Top = 0
        Width = 805
        Height = 285
        Align = alClient
        BevelOuter = bvNone
        TabOrder = 1
        object StringGrid_StockQry: TStringGrid
          Left = 0
          Top = 0
          Width = 805
          Height = 143
          Align = alTop
          BevelInner = bvNone
          BevelOuter = bvNone
          Color = clWhite
          Ctl3D = False
          DefaultColWidth = 80
          DefaultRowHeight = 20
          DefaultDrawing = False
          FixedColor = clWhite
          FixedCols = 0
          RowCount = 10
          Options = [goFixedVertLine, goHorzLine]
          ParentCtl3D = False
          ScrollBars = ssVertical
          TabOrder = 0
          OnDrawCell = StringGrid_StockQryDrawCell
        end
        object StringGrid_StockLock: TStringGrid
          Left = 0
          Top = 143
          Width = 805
          Height = 142
          Align = alClient
          BevelInner = bvNone
          BevelOuter = bvNone
          Color = clWhite
          Ctl3D = False
          DefaultColWidth = 80
          DefaultRowHeight = 20
          DefaultDrawing = False
          FixedColor = clWhite
          FixedCols = 0
          RowCount = 10
          Options = [goFixedVertLine, goHorzLine]
          ParentCtl3D = False
          ScrollBars = ssVertical
          TabOrder = 1
          OnDrawCell = StringGrid_StockLockDrawCell
        end
      end
    end
  end
  object Panel_Top: TPanel
    Left = 0
    Top = 0
    Width = 1214
    Height = 49
    Align = alTop
    BevelOuter = bvLowered
    TabOrder = 1
    object Label4: TLabel
      Left = 334
      Top = 7
      Width = 91
      Height = 14
      Caption = 'ServerAddress'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
      Transparent = False
    end
    object Label3: TLabel
      Left = 225
      Top = 7
      Width = 56
      Height = 14
      Caption = 'Password'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
      Transparent = False
    end
    object Label2: TLabel
      Left = 116
      Top = 7
      Width = 42
      Height = 14
      Caption = 'UserID'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
      Transparent = False
    end
    object Label1: TLabel
      Left = 7
      Top = 7
      Width = 56
      Height = 14
      Caption = 'BrokerID'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
      Transparent = False
    end
    object Label5: TLabel
      Left = 566
      Top = 7
      Width = 49
      Height = 14
      Caption = 'FrontID'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
      Transparent = False
    end
    object Label6: TLabel
      Left = 672
      Top = 7
      Width = 63
      Height = 14
      Caption = 'SessionID'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
      Transparent = False
    end
    object Label10: TLabel
      Left = 778
      Top = 7
      Width = 77
      Height = 14
      Caption = 'MaxOrderRef'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold, fsUnderline]
      ParentFont = False
      Transparent = False
    end
    object TxtServerAddress: TEdit
      Left = 334
      Top = 21
      Width = 203
      Height = 22
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 0
    end
    object TxtPassword: TEdit
      Left = 225
      Top = 21
      Width = 103
      Height = 22
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 1
    end
    object TxtUserID: TEdit
      Left = 116
      Top = 21
      Width = 103
      Height = 22
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 2
    end
    object TxtBrokerID: TEdit
      Left = 7
      Top = 21
      Width = 103
      Height = 22
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 3
    end
    object TxtFrontID: TEdit
      Left = 566
      Top = 21
      Width = 100
      Height = 22
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 4
    end
    object TxtSessionID: TEdit
      Left = 672
      Top = 21
      Width = 100
      Height = 22
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 5
    end
    object TxtMaxOrderRef: TEdit
      Left = 778
      Top = 21
      Width = 100
      Height = 22
      Color = clBtnFace
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -12
      Font.Name = 'Consolas'
      Font.Style = [fsBold]
      ParentFont = False
      TabOrder = 6
    end
  end
  object Panel_Left: TPanel
    Left = 0
    Top = 49
    Width = 139
    Height = 639
    Align = alLeft
    BevelKind = bkTile
    BevelOuter = bvNone
    TabOrder = 2
    object BtnConn_Login: TButton
      Left = 14
      Top = 49
      Width = 103
      Height = 50
      Caption = 'Connect + Login'
      TabOrder = 0
      OnClick = BtnConn_LoginClick
    end
    object BtnDisconnect: TButton
      Left = 14
      Top = 103
      Width = 103
      Height = 50
      Caption = 'Disconnect'
      TabOrder = 1
      OnClick = BtnDisconnectClick
    end
    object Button2: TButton
      Left = 14
      Top = 250
      Width = 103
      Height = 36
      Caption = 'Settlement Info'
      TabOrder = 2
      OnClick = Button2Click
    end
    object Button3: TButton
      Left = 14
      Top = 291
      Width = 103
      Height = 37
      Caption = 'Investor Position'
      TabOrder = 3
      OnClick = Button3Click
    end
    object Button4: TButton
      Left = 14
      Top = 374
      Width = 103
      Height = 37
      Caption = 'QryTradingAccount'
      TabOrder = 4
      OnClick = Button4Click
    end
    object Button5: TButton
      Left = 14
      Top = 534
      Width = 103
      Height = 37
      Caption = 'Chang Password'
      TabOrder = 5
      OnClick = Button5Click
    end
    object Button6: TButton
      Left = 14
      Top = 332
      Width = 103
      Height = 37
      Caption = 'In... OpenPosition'
      TabOrder = 6
      OnClick = Button6Click
    end
  end
  object Panel_Bottom: TPanel
    Left = 0
    Top = 688
    Width = 1214
    Height = 174
    Align = alBottom
    TabOrder = 3
    object PageControl1: TPageControl
      Left = 1
      Top = 1
      Width = 1212
      Height = 172
      ActivePage = TabSheet1
      Align = alClient
      TabOrder = 0
      OnChange = PageControl1Change
      object TabSheet1: TTabSheet
        Caption = 'New Order'
        object RadioGroup_BS: TRadioGroup
          Left = 3
          Top = 60
          Width = 38
          Height = 50
          ItemIndex = 1
          Items.Strings = (
            'B'
            'S')
          TabOrder = 0
        end
        object RadioGroup_CombHedge: TRadioGroup
          Left = 47
          Top = 60
          Width = 50
          Height = 50
          ItemIndex = 0
          Items.Strings = (
            #25237#27231
            #20633#20812)
          TabOrder = 1
        end
        object TxtExchangeID: TEdit
          Left = 183
          Top = 5
          Width = 75
          Height = 22
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 2
        end
        object TxtInstrumentID: TEdit
          Left = 183
          Top = 33
          Width = 75
          Height = 22
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 3
        end
        object TxtQty: TEdit
          Left = 183
          Top = 61
          Width = 75
          Height = 22
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 4
        end
        object TxtPx: TEdit
          Left = 183
          Top = 89
          Width = 75
          Height = 22
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 5
        end
        object Button1: TButton
          Left = 277
          Top = 3
          Width = 75
          Height = 26
          Caption = 'New Order'
          TabOrder = 6
          OnClick = Button1Click
        end
      end
      object TabSheet2: TTabSheet
        Caption = 'Cancel Order'
        ImageIndex = 1
        object Label7: TLabel
          Left = 5
          Top = 60
          Width = 70
          Height = 14
          Caption = 'OrderSysID'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold, fsUnderline]
          ParentFont = False
          Transparent = False
        end
        object Label9: TLabel
          Left = 5
          Top = 102
          Width = 84
          Height = 14
          Caption = 'OrderLocalID'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold, fsUnderline]
          ParentFont = False
          Transparent = False
        end
        object TxtOrderSysID: TEdit
          Left = 5
          Top = 74
          Width = 120
          Height = 22
          Color = clBtnFace
          Enabled = False
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 0
        end
        object TxtOrderLocalID: TEdit
          Left = 5
          Top = 116
          Width = 120
          Height = 22
          Color = clBtnFace
          Enabled = False
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
        end
        object Button7: TButton
          Left = 201
          Top = 10
          Width = 75
          Height = 26
          Caption = 'Cancel Order'
          TabOrder = 2
          OnClick = Button7Click
        end
        object Panel_Ref: TPanel
          Left = 5
          Top = 5
          Width = 165
          Height = 48
          BevelKind = bkFlat
          BevelOuter = bvNone
          ParentColor = True
          TabOrder = 3
          object Label8: TLabel
            Left = 7
            Top = 4
            Width = 140
            Height = 14
            Caption = ' OrderRef / LockRef '
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Consolas'
            Font.Style = [fsBold, fsUnderline]
            ParentFont = False
            Transparent = False
          end
          object TxtOrderRef: TEdit
            Left = 7
            Top = 18
            Width = 120
            Height = 22
            Color = clWhite
            Font.Charset = ANSI_CHARSET
            Font.Color = clWindowText
            Font.Height = -12
            Font.Name = 'Consolas'
            Font.Style = [fsBold]
            ParentFont = False
            TabOrder = 0
          end
        end
      end
      object TabSheet3: TTabSheet
        Caption = 'Covered'
        ImageIndex = 2
        object Label12: TLabel
          Left = 340
          Top = 27
          Width = 63
          Height = 14
          Caption = 'LockSysID'
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold, fsUnderline]
          ParentFont = False
          Transparent = False
        end
        object Button8: TButton
          Left = 145
          Top = 69
          Width = 86
          Height = 25
          Caption = 'Lock Insert'
          TabOrder = 0
          OnClick = Button8Click
        end
        object Button9: TButton
          Left = 340
          Top = 72
          Width = 75
          Height = 25
          Caption = 'Qry Lock'
          TabOrder = 1
          OnClick = Button9Click
        end
        object Button10: TButton
          Left = 340
          Top = 101
          Width = 111
          Height = 25
          Caption = 'Qry LockPosition'
          TabOrder = 2
          OnClick = Button10Click
        end
        object RadioGroup_Lock: TRadioGroup
          Left = 3
          Top = 70
          Width = 66
          Height = 70
          ItemIndex = 0
          Items.Strings = (
            'Lock'
            'UnLock')
          TabOrder = 3
        end
        object TxtInstrumentStockID: TEdit
          Left = 268
          Top = 73
          Width = 66
          Height = 22
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 4
        end
        object TxtLockSysID: TEdit
          Left = 340
          Top = 41
          Width = 120
          Height = 22
          Color = clWhite
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 5
        end
        object TxtLockVolume: TEdit
          Left = 74
          Top = 70
          Width = 66
          Height = 22
          Font.Charset = ANSI_CHARSET
          Font.Color = clWindowText
          Font.Height = -12
          Font.Name = 'Consolas'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 6
        end
      end
    end
  end
end
