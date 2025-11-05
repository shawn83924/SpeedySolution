object SequenceForm: TSequenceForm
  Left = 1026
  Top = 204
  BorderStyle = bsDialog
  Caption = #35373#23450#35338#24687#24207#34399
  ClientHeight = 140
  ClientWidth = 410
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -17
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poMainFormCenter
  Scaled = False
  PixelsPerInch = 120
  TextHeight = 21
  object Bevel1: TBevel
    Left = 5
    Top = 5
    Width = 399
    Height = 80
  end
  object Label1: TLabel
    Left = 13
    Top = 53
    Width = 277
    Height = 21
    Caption = 'Incoming Message Sequence Number'
  end
  object Label2: TLabel
    Left = 13
    Top = 15
    Width = 276
    Height = 21
    Caption = 'Outgoing Message Sequence Number'
  end
  object InSeqEdit: TEdit
    Left = 299
    Top = 50
    Width = 96
    Height = 29
    ImeMode = imSAlpha
    TabOrder = 1
  end
  object OutSeqEdit: TEdit
    Left = 299
    Top = 12
    Width = 96
    Height = 29
    ImeMode = imSAlpha
    TabOrder = 0
  end
  object CancelBtn: TBitBtn
    Left = 209
    Top = 101
    Width = 97
    Height = 32
    DoubleBuffered = True
    Kind = bkCancel
    ParentDoubleBuffered = False
    TabOrder = 3
  end
  object OKBtn: TBitBtn
    Left = 105
    Top = 101
    Width = 97
    Height = 32
    Caption = 'OK'
    Default = True
    DoubleBuffered = True
    Glyph.Data = {
      DE010000424DDE01000000000000760000002800000024000000120000000100
      0400000000006801000000000000000000001000000000000000000000000000
      80000080000000808000800000008000800080800000C0C0C000808080000000
      FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF00333333333333
      3333333333333333333333330000333333333333333333333333F33333333333
      00003333344333333333333333388F3333333333000033334224333333333333
      338338F3333333330000333422224333333333333833338F3333333300003342
      222224333333333383333338F3333333000034222A22224333333338F338F333
      8F33333300003222A3A2224333333338F3838F338F33333300003A2A333A2224
      33333338F83338F338F33333000033A33333A222433333338333338F338F3333
      0000333333333A222433333333333338F338F33300003333333333A222433333
      333333338F338F33000033333333333A222433333333333338F338F300003333
      33333333A222433333333333338F338F00003333333333333A22433333333333
      3338F38F000033333333333333A223333333333333338F830000333333333333
      333A333333333333333338330000333333333333333333333333333333333333
      0000}
    NumGlyphs = 2
    ParentDoubleBuffered = False
    TabOrder = 2
    OnClick = OKBtnClick
  end
end
