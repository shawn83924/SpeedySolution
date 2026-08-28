object AccountForm: TAccountForm
  Left = 980
  Top = 116
  BorderStyle = bsDialog
  Caption = 'AccountForm'
  ClientHeight = 759
  ClientWidth = 339
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -15
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesktopCenter
  Scaled = False
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 18
  object PwdPanel: TPanel
    Left = 0
    Top = 41
    Width = 339
    Height = 97
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 1
    object Label2: TLabel
      Left = 0
      Top = 8
      Width = 101
      Height = 18
      Alignment = taRightJustify
      AutoSize = False
      Caption = #23494#30908
    end
    object Label3: TLabel
      Left = 0
      Top = 41
      Width = 101
      Height = 18
      Alignment = taRightJustify
      AutoSize = False
      Caption = #30906#35469#23494#30908
    end
    object PwdConfirmEdit: TMaskEdit
      Left = 112
      Top = 38
      Width = 169
      Height = 26
      ImeMode = imSAlpha
      PasswordChar = '*'
      TabOrder = 1
      Text = ''
    end
    object PwdEdit: TMaskEdit
      Left = 112
      Top = 5
      Width = 169
      Height = 26
      ImeMode = imSAlpha
      PasswordChar = '*'
      TabOrder = 0
      Text = ''
    end
    object ModifyPasswordCheckBox: TCheckBox
      Left = 92
      Top = 69
      Width = 192
      Height = 17
      Caption = #35531#29992#25142#20462#25913#23494#30908
      Checked = True
      State = cbChecked
      TabOrder = 2
    end
  end
  object IDPanel: TPanel
    Left = 0
    Top = 0
    Width = 339
    Height = 41
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 0
    TabStop = True
    object Label1: TLabel
      Left = 0
      Top = 12
      Width = 101
      Height = 18
      Alignment = taRightJustify
      AutoSize = False
      Caption = #29151#26989#21729#20195#34399
    end
    object IDEdit: TEdit
      Left = 112
      Top = 9
      Width = 169
      Height = 26
      ImeMode = imSAlpha
      TabOrder = 0
    end
  end
  object BtnPanel: TPanel
    Left = 0
    Top = 712
    Width = 339
    Height = 44
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 3
    object OKBtn: TBitBtn
      Left = 73
      Top = 3
      Width = 97
      Height = 32
      Caption = #30906#23450
      Default = True
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
      TabOrder = 0
      OnClick = OKBtnClick
    end
    object CancelBtn: TBitBtn
      Left = 176
      Top = 2
      Width = 97
      Height = 32
      Caption = #21462#28040
      Kind = bkCancel
      NumGlyphs = 2
      TabOrder = 1
    end
  end
  object AttrPanel: TPanel
    Left = 0
    Top = 294
    Width = 339
    Height = 310
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 2
    object PageControl: TPageControl
      Left = 14
      Top = 5
      Width = 311
      Height = 297
      ActivePage = TFXTabSheet
      TabOrder = 0
      OnChange = PageControlChange
      object TFXTabSheet: TTabSheet
        Caption = #26399#20132#25152
        object Label9: TLabel
          Left = 11
          Top = 237
          Width = 120
          Height = 18
          Caption = #27298#26680#31243#24335#19981#23384#22312#26178
        end
        object Label6: TLabel
          Left = 11
          Top = 206
          Width = 60
          Height = 18
          Caption = #27298#26680#31243#24335
        end
        object Label11: TLabel
          Left = 205
          Top = 176
          Width = 30
          Height = 18
          Caption = #32676#32068
        end
        object Label24: TLabel
          Left = 11
          Top = 143
          Width = 60
          Height = 18
          Caption = #19979#21934#20358#28304
        end
        object Label25: TLabel
          Left = 185
          Top = 143
          Width = 60
          Height = 18
          Caption = #34892#24773#20358#28304
        end
        object OIDGenGroup: TRadioGroup
          Left = 4
          Top = 0
          Width = 286
          Height = 136
          Caption = #33258#21205#32232#21934#34399#21151#33021
          ItemIndex = 0
          Items.Strings = (
            #38364#38281
            #36899#32396#21312#38291
            #25351#23450#27331#34399
            #20351#29992#20849#29992#27331#34399)
          TabOrder = 0
          OnClick = OIDGenGroupClick
        end
        object BeginOIDEdit: TEdit
          Left = 112
          Top = 53
          Width = 62
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 5
          TabOrder = 1
        end
        object EndOIDEdit: TEdit
          Left = 191
          Top = 53
          Width = 62
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 5
          TabOrder = 2
        end
        object StaticText1: TStaticText
          Left = 176
          Top = 55
          Width = 15
          Height = 22
          Caption = '~'
          TabOrder = 3
        end
        object EditBtn: TBitBtn
          Left = 254
          Top = 83
          Width = 24
          Height = 27
          Glyph.Data = {
            06030000424D0603000000000000060100002800000020000000100000000100
            08000000000000020000120B0000120B0000340000003400000000000000FFFF
            FF00FE00FE00FEFCFA00FEDDB800FEDFBC00FEE3C500FEE4C700FEE6CB00FEED
            DA00FEF4E900FEF5EB00FDDBB400FEE1C000FEE8CF00FEE9D000FEEBD500FEEE
            DB00FEF0E000FEF3E600FEF2E300FEF5E900FEF7EE00FEFBF700FEF9F200FEFC
            F900D582000000980000FCFCFC00FBFBFB00FAFAFA00F8F8F800F6F6F600F4F4
            F400F3F3F300F2F2F200F0F0F000EFEFEF00ECECEC00E9E9E900E7E7E700E6E6
            E600E4E4E400E2E2E200E1E1E100DFDFDF00DDDDDD00DBDBDB00D8D8D8006A6A
            6A004C4C4C00FFFFFF0002020202020202020202020202020202020202020202
            0202020202020202020202021A1A1A1A1A1A1A1A1A1A1A1A0202020231313131
            31313131313131310202021A11100F08060D05040C0C0C0C1A0202312627282A
            2C2D2E2F303030303102021A1209100F08060D05040C0C0C1A02023125262728
            2A2C2D2E2F3030303102021A141B1B1B1B1B1B1B1B1B1B0C1A02023124323232
            32323232323232303102021A0B141211100F08060D05040C1A02023121242526
            27282A2C2D2E2F303102021A161B1B1B1B1B1B08070D05041A02023120323232
            3232322A2B2D2E2F3102021A181615131211100E08070D051A0202311F202223
            252627292A2B2D2E3102021A171B1B1B1B1B1B1B1B1B1B0D1A0202311E323232
            323232323232322D3102021A031718160A141211100E08071A0202311C1E1F20
            2224252627292A2B3102021A031B1B1B1B1B1B1209100E081A0202311C323232
            323232252627292A3102021A03031917181615131211100E1A0202311C1C1D1E
            1F202223252627293102021A031B1B1B1B1B1B1B1B1B1B101A0202311C323232
            32323232323232273102021A0303030319171816151312111A0202311C1C1C1C
            1D1E1F2022232526310202021A1A1A1A1A1A1A1A1A1A1A1A0202020231313131
            3131313131313131020202020202020202020202020202020202020202020202
            02020202020202020202}
          NumGlyphs = 2
          TabOrder = 4
          OnClick = TFXEditBtnClick
        end
        object TerminalsEdit: TEdit
          Left = 112
          Top = 83
          Width = 141
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 4096
          TabOrder = 5
        end
        object FUTBrokerIDCheckBox: TCheckBox
          Left = 11
          Top = 176
          Width = 109
          Height = 20
          Caption = #25351#23450#26399#36008#21830
          TabOrder = 7
          OnClick = FUTBrokerIDCheckBoxClick
        end
        object DefaultCheckerComboBox: TComboBox
          Left = 161
          Top = 234
          Width = 130
          Height = 26
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 8
          Text = #39361#22238#22996#35351#21934
          OnChange = GroupComboBoxChange
          Items.Strings = (
            #39361#22238#22996#35351#21934
            #25918#34892#22996#35351#21934)
        end
        object CheckerComboBox: TComboBox
          Left = 99
          Top = 203
          Width = 192
          Height = 26
          Style = csDropDownList
          TabOrder = 9
          OnChange = GroupComboBoxChange
        end
        object ComboBoxGroup: TComboBox
          Left = 248
          Top = 173
          Width = 41
          Height = 26
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 10
          Text = '0'
          OnChange = GroupComboBoxChange
          Items.Strings = (
            '0'
            '1'
            '2'
            '3'
            '4'
            '5'
            '6'
            '7'
            '8'
            '9')
        end
        object BitBtn3: TBitBtn
          Left = 254
          Top = 52
          Width = 24
          Height = 27
          Glyph.Data = {
            36050000424D3605000000000000360400002800000010000000100000000100
            0800000000000001000000000000000000000001000000010000000000000101
            0100020202000303030004040400050505000606060007070700080808000909
            09000A0A0A000B0B0B000C0C0C000D120D000F180F00111E1000122412001429
            1300152E140016331500173C160018451700194F18001A5718001B5C19001B62
            19001B6619001B6919001B6C19001B6E19001B7119001A7419001A7619001A78
            19001A7A19001A7D1900197F1900198019001980190019811900198019001980
            19001E801B0025801D002C802000338122003B82260044842A0057873200708C
            3B00869144009B944C00B2995600C19D5D00CCA06200D1A26300D5A46600D5A3
            6400D6A26200D69E5E00D69B5A00D5975500D3914E00D0894400D1843E00D181
            3800D3803300D47F2F00D27E3100CF7C3200CD7A3300C9763300C7743200C270
            3200BE6D3400BA6A3200B5653100B1612E00AF5E2D00AC5C2C00A95A2C00A658
            2C00A2552C009E522C009B4F2B00974C2A00954A290093492800934927009248
            2700904826008F4725008E4723008D4722008B4622008A452200884422008643
            22008542220085422300864224008743250088442B008846310087483B00854B
            4600834E54007F5161007B59740076608600726797006C6CA5006770AF006373
            B8005F75C0005977C9005577D1005377D5005277D800527CDC005280DE005283
            DF005182DD004F80DB004C7BD8004B7BD800497AD7004779D6004172D2003B6C
            CF003665CB003160C8002D5BC4002956C1002552BD00224FB800204FB5001F50
            B3001D4EB0001A4AAE001D50AB002156A6002556A4002957A3002E58A200335B
            A0003D5F9D0047649A0050689700586C9400627191006B768E00747A8B007C7E
            8800828186008483850085848500868584008785840087858400878684008886
            84008886840089878500898785008A8886008B8987008C8B89008F8D8B00918F
            8E009392900096949300989796009B9A99009E9D9C00A2A1A000A4A4A300A7A7
            A600AAAAA900ADADAD00B1B1B100B5B5B500B6B6B600B7B7B700B8B8B800B9B9
            B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
            C100C2C2C200C3C3C300C4C4C400C5C5C500CCCBCA00D3D1CF00D9D6D300E2C1
            DB00EBA1E300F185EA00F56DEE00F85AF200FA49F500FC37F700FD27FA00FD1C
            FB00FE13FC00FE0CFD00FE06FE00FE03FE00FE01FE00FE00FE00FE00FE00FE00
            FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
            FE00FE00FE00FE00FE00FE00FE00FE01FE00FE07FE00FE16FD00FE2AFC00FE51
            F900FE84F400FEA6F100FECAEF00FEE1ED00FEEBED00FEF1ED00FEF4EE00FEF6
            EE00FEF5EC00FEF5EA00FDF3E700FDF1E400FDF0E100FDEFDE00FDECD900FEE9
            D300FEE7CE00FEE5CA00FDE4C800FDE3C600FDE2C400FDE2C400DDDDDDDDDDDD
            DDDDDDDDDDDDDDDDDDDD5356585858585858585858585858555362F1F4A1F8F9
            A1FBFBA1FEFEA1FEFE5A62F5A1A1A1A1A1A1A1A1A1A1A1A1A15C62A1A1F3768B
            F7271AF93569FC798D5C62F1A1F1768BF7271AF93569FC798D5C62A1A1F1768B
            F7271AF9FDFDFC798D5C62F1A1F1768BF72A1AF9FDFDFC798D5C62A1A1F1768B
            F7271AF9FAFBFCFEFE5C62F1A1F1F1F3F72A1AF8F9FAFCFEFE5C63A1A1F1F1F3
            F7F7F7F7F8F9FBFDFE564B3A3939393939393939383A3838364A4F4343434343
            4343434338433834754EDD5050505050505050504E504D5253DDDDDDDDDDDDDD
            DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD}
          TabOrder = 11
          OnClick = BitBtn3Click
        end
        object FUTBrokerIDEdit: TEdit
          Left = 124
          Top = 173
          Width = 75
          Height = 26
          ImeMode = imSAlpha
          TabOrder = 6
        end
        object TermsComboBox: TComboBox
          Left = 122
          Top = 1
          Width = 110
          Height = 26
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 12
          Text = #20841#30908#27331#34399
          OnChange = TermsComboBoxChange
          Items.Strings = (
            #20841#30908#27331#34399
            #19977#30908#27331#34399
            #22235#30908#27331#34399
            #20116#30908#27331#34399)
        end
        object OrdSrcComboBox: TComboBox
          Left = 74
          Top = 140
          Width = 102
          Height = 26
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 13
          Text = #38928#35373#20540' '
          Items.Strings = (
            #38928#35373#20540' '
            'D:'#23560#32218
            'A:'#20351#29992'API'
            'M:'#34892#21205#36617#20855
            'W:'#32178#31449
            'P:'#38651#33126#36575#39636
            'V:'#35486#38899
            'G:'#19968#33324#22996#35351)
        end
        object InfoSrcEdit: TEdit
          Left = 248
          Top = 140
          Width = 41
          Height = 26
          AutoSize = False
          ImeMode = imSAlpha
          MaxLength = 3
          TabOrder = 14
        end
      end
      object TFXOffHourTabSheet: TTabSheet
        Caption = #26399#20132#25152#19979#21320#30436
        ImageIndex = 5
        object OHOIDGenGroup: TRadioGroup
          Left = 4
          Top = 0
          Width = 286
          Height = 136
          Caption = #33258#21205#32232#21934#34399#21151#33021
          ItemIndex = 0
          Items.Strings = (
            #38364#38281
            #36899#32396#21312#38291
            #25351#23450#27331#34399
            #20351#29992#20849#29992#27331#34399)
          TabOrder = 7
          OnClick = OHOIDGenGroupClick
        end
        object OHTermsComboBox: TComboBox
          Left = 122
          Top = 1
          Width = 110
          Height = 26
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 0
          Text = #20841#30908#27331#34399
          OnChange = OHTermsComboBoxChange
          Items.Strings = (
            #20841#30908#27331#34399
            #19977#30908#27331#34399
            #22235#30908#27331#34399
            #20116#30908#27331#34399)
        end
        object BitBtn1: TBitBtn
          Left = 254
          Top = 52
          Width = 24
          Height = 27
          Glyph.Data = {
            36050000424D3605000000000000360400002800000010000000100000000100
            0800000000000001000000000000000000000001000000010000000000000101
            0100020202000303030004040400050505000606060007070700080808000909
            09000A0A0A000B0B0B000C0C0C000D120D000F180F00111E1000122412001429
            1300152E140016331500173C160018451700194F18001A5718001B5C19001B62
            19001B6619001B6919001B6C19001B6E19001B7119001A7419001A7619001A78
            19001A7A19001A7D1900197F1900198019001980190019811900198019001980
            19001E801B0025801D002C802000338122003B82260044842A0057873200708C
            3B00869144009B944C00B2995600C19D5D00CCA06200D1A26300D5A46600D5A3
            6400D6A26200D69E5E00D69B5A00D5975500D3914E00D0894400D1843E00D181
            3800D3803300D47F2F00D27E3100CF7C3200CD7A3300C9763300C7743200C270
            3200BE6D3400BA6A3200B5653100B1612E00AF5E2D00AC5C2C00A95A2C00A658
            2C00A2552C009E522C009B4F2B00974C2A00954A290093492800934927009248
            2700904826008F4725008E4723008D4722008B4622008A452200884422008643
            22008542220085422300864224008743250088442B008846310087483B00854B
            4600834E54007F5161007B59740076608600726797006C6CA5006770AF006373
            B8005F75C0005977C9005577D1005377D5005277D800527CDC005280DE005283
            DF005182DD004F80DB004C7BD8004B7BD800497AD7004779D6004172D2003B6C
            CF003665CB003160C8002D5BC4002956C1002552BD00224FB800204FB5001F50
            B3001D4EB0001A4AAE001D50AB002156A6002556A4002957A3002E58A200335B
            A0003D5F9D0047649A0050689700586C9400627191006B768E00747A8B007C7E
            8800828186008483850085848500868584008785840087858400878684008886
            84008886840089878500898785008A8886008B8987008C8B89008F8D8B00918F
            8E009392900096949300989796009B9A99009E9D9C00A2A1A000A4A4A300A7A7
            A600AAAAA900ADADAD00B1B1B100B5B5B500B6B6B600B7B7B700B8B8B800B9B9
            B900BABABA00BBBBBB00BCBCBC00BDBDBD00BEBEBE00BFBFBF00C0C0C000C1C1
            C100C2C2C200C3C3C300C4C4C400C5C5C500CCCBCA00D3D1CF00D9D6D300E2C1
            DB00EBA1E300F185EA00F56DEE00F85AF200FA49F500FC37F700FD27FA00FD1C
            FB00FE13FC00FE0CFD00FE06FE00FE03FE00FE01FE00FE00FE00FE00FE00FE00
            FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00FE00
            FE00FE00FE00FE00FE00FE00FE00FE01FE00FE07FE00FE16FD00FE2AFC00FE51
            F900FE84F400FEA6F100FECAEF00FEE1ED00FEEBED00FEF1ED00FEF4EE00FEF6
            EE00FEF5EC00FEF5EA00FDF3E700FDF1E400FDF0E100FDEFDE00FDECD900FEE9
            D300FEE7CE00FEE5CA00FDE4C800FDE3C600FDE2C400FDE2C400DDDDDDDDDDDD
            DDDDDDDDDDDDDDDDDDDD5356585858585858585858585858555362F1F4A1F8F9
            A1FBFBA1FEFEA1FEFE5A62F5A1A1A1A1A1A1A1A1A1A1A1A1A15C62A1A1F3768B
            F7271AF93569FC798D5C62F1A1F1768BF7271AF93569FC798D5C62A1A1F1768B
            F7271AF9FDFDFC798D5C62F1A1F1768BF72A1AF9FDFDFC798D5C62A1A1F1768B
            F7271AF9FAFBFCFEFE5C62F1A1F1F1F3F72A1AF8F9FAFCFEFE5C63A1A1F1F1F3
            F7F7F7F7F8F9FBFDFE564B3A3939393939393939383A3838364A4F4343434343
            4343434338433834754EDD5050505050505050504E504D5253DDDDDDDDDDDDDD
            DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD}
          TabOrder = 1
          OnClick = BitBtn1Click
        end
        object OHTermEditBtn: TBitBtn
          Left = 254
          Top = 83
          Width = 24
          Height = 27
          Glyph.Data = {
            06030000424D0603000000000000060100002800000020000000100000000100
            08000000000000020000120B0000120B0000340000003400000000000000FFFF
            FF00FE00FE00FEFCFA00FEDDB800FEDFBC00FEE3C500FEE4C700FEE6CB00FEED
            DA00FEF4E900FEF5EB00FDDBB400FEE1C000FEE8CF00FEE9D000FEEBD500FEEE
            DB00FEF0E000FEF3E600FEF2E300FEF5E900FEF7EE00FEFBF700FEF9F200FEFC
            F900D582000000980000FCFCFC00FBFBFB00FAFAFA00F8F8F800F6F6F600F4F4
            F400F3F3F300F2F2F200F0F0F000EFEFEF00ECECEC00E9E9E900E7E7E700E6E6
            E600E4E4E400E2E2E200E1E1E100DFDFDF00DDDDDD00DBDBDB00D8D8D8006A6A
            6A004C4C4C00FFFFFF0002020202020202020202020202020202020202020202
            0202020202020202020202021A1A1A1A1A1A1A1A1A1A1A1A0202020231313131
            31313131313131310202021A11100F08060D05040C0C0C0C1A0202312627282A
            2C2D2E2F303030303102021A1209100F08060D05040C0C0C1A02023125262728
            2A2C2D2E2F3030303102021A141B1B1B1B1B1B1B1B1B1B0C1A02023124323232
            32323232323232303102021A0B141211100F08060D05040C1A02023121242526
            27282A2C2D2E2F303102021A161B1B1B1B1B1B08070D05041A02023120323232
            3232322A2B2D2E2F3102021A181615131211100E08070D051A0202311F202223
            252627292A2B2D2E3102021A171B1B1B1B1B1B1B1B1B1B0D1A0202311E323232
            323232323232322D3102021A031718160A141211100E08071A0202311C1E1F20
            2224252627292A2B3102021A031B1B1B1B1B1B1209100E081A0202311C323232
            323232252627292A3102021A03031917181615131211100E1A0202311C1C1D1E
            1F202223252627293102021A031B1B1B1B1B1B1B1B1B1B101A0202311C323232
            32323232323232273102021A0303030319171816151312111A0202311C1C1C1C
            1D1E1F2022232526310202021A1A1A1A1A1A1A1A1A1A1A1A0202020231313131
            3131313131313131020202020202020202020202020202020202020202020202
            02020202020202020202}
          NumGlyphs = 2
          TabOrder = 2
          OnClick = OHTermEditBtnClick
        end
        object OHTerminalsEdit: TEdit
          Left = 112
          Top = 83
          Width = 141
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 4096
          TabOrder = 3
        end
        object StaticText4: TStaticText
          Left = 176
          Top = 55
          Width = 15
          Height = 22
          Caption = '~'
          TabOrder = 4
        end
        object OHBeginOIDEdit: TEdit
          Left = 112
          Top = 53
          Width = 62
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 5
          TabOrder = 5
        end
        object OHEndOIDEdit: TEdit
          Left = 191
          Top = 53
          Width = 62
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 5
          TabOrder = 6
        end
        object OffHourCheckBox: TCheckBox
          Left = 12
          Top = 150
          Width = 140
          Height = 20
          Caption = #19979#21320#30436#21478#32232#21934#34399
          TabOrder = 8
          OnClick = OffHourCheckBoxClick
        end
      end
      object TSETabSheet: TTabSheet
        Caption = #35657#20132#25152
        ImageIndex = 1
        object Label7: TLabel
          Left = 11
          Top = 206
          Width = 60
          Height = 18
          Caption = #27298#26680#31243#24335
        end
        object Label10: TLabel
          Left = 11
          Top = 237
          Width = 120
          Height = 18
          Caption = #27298#26680#31243#24335#19981#23384#22312#26178
        end
        object TSEOIDGenGroup: TRadioGroup
          Left = 4
          Top = 0
          Width = 286
          Height = 136
          Caption = #33258#21205#32232#21934#34399#21151#33021
          ItemIndex = 0
          Items.Strings = (
            #38364#38281
            #36899#32396#21312#38291
            #25351#23450#27331#34399
            #20351#29992#20849#29992#27331#34399)
          TabOrder = 0
          OnClick = TSEOIDGenGroupClick
        end
        object TSEEditBtn: TBitBtn
          Left = 254
          Top = 77
          Width = 24
          Height = 27
          Glyph.Data = {
            06030000424D0603000000000000060100002800000020000000100000000100
            08000000000000020000120B0000120B0000340000003400000000000000FFFF
            FF00FE00FE00FEFCFA00FEDDB800FEDFBC00FEE3C500FEE4C700FEE6CB00FEED
            DA00FEF4E900FEF5EB00FDDBB400FEE1C000FEE8CF00FEE9D000FEEBD500FEEE
            DB00FEF0E000FEF3E600FEF2E300FEF5E900FEF7EE00FEFBF700FEF9F200FEFC
            F900D582000000980000FCFCFC00FBFBFB00FAFAFA00F8F8F800F6F6F600F4F4
            F400F3F3F300F2F2F200F0F0F000EFEFEF00ECECEC00E9E9E900E7E7E700E6E6
            E600E4E4E400E2E2E200E1E1E100DFDFDF00DDDDDD00DBDBDB00D8D8D8006A6A
            6A004C4C4C00FFFFFF0002020202020202020202020202020202020202020202
            0202020202020202020202021A1A1A1A1A1A1A1A1A1A1A1A0202020231313131
            31313131313131310202021A11100F08060D05040C0C0C0C1A0202312627282A
            2C2D2E2F303030303102021A1209100F08060D05040C0C0C1A02023125262728
            2A2C2D2E2F3030303102021A141B1B1B1B1B1B1B1B1B1B0C1A02023124323232
            32323232323232303102021A0B141211100F08060D05040C1A02023121242526
            27282A2C2D2E2F303102021A161B1B1B1B1B1B08070D05041A02023120323232
            3232322A2B2D2E2F3102021A181615131211100E08070D051A0202311F202223
            252627292A2B2D2E3102021A171B1B1B1B1B1B1B1B1B1B0D1A0202311E323232
            323232323232322D3102021A031718160A141211100E08071A0202311C1E1F20
            2224252627292A2B3102021A031B1B1B1B1B1B1209100E081A0202311C323232
            323232252627292A3102021A03031917181615131211100E1A0202311C1C1D1E
            1F202223252627293102021A031B1B1B1B1B1B1B1B1B1B101A0202311C323232
            32323232323232273102021A0303030319171816151312111A0202311C1C1C1C
            1D1E1F2022232526310202021A1A1A1A1A1A1A1A1A1A1A1A0202020231313131
            3131313131313131020202020202020202020202020202020202020202020202
            02020202020202020202}
          NumGlyphs = 2
          TabOrder = 1
          OnClick = TSEEditBtnClick
        end
        object TSETerminalsEdit: TEdit
          Left = 112
          Top = 78
          Width = 140
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 4096
          TabOrder = 2
        end
        object TSEEndOIDEdit: TEdit
          Left = 208
          Top = 47
          Width = 70
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 5
          TabOrder = 3
        end
        object StaticText2: TStaticText
          Left = 187
          Top = 49
          Width = 15
          Height = 22
          Caption = '~'
          TabOrder = 4
        end
        object TSEBeginOIDEdit: TEdit
          Left = 112
          Top = 47
          Width = 70
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 5
          TabOrder = 5
        end
        object TSECheckerComboBox: TComboBox
          Left = 99
          Top = 203
          Width = 192
          Height = 26
          Style = csDropDownList
          TabOrder = 6
          OnChange = GroupComboBoxChange
        end
        object TSEDefaultCheckerComboBox: TComboBox
          Left = 161
          Top = 234
          Width = 130
          Height = 26
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 7
          Text = #39361#22238#22996#35351#21934
          OnChange = GroupComboBoxChange
          Items.Strings = (
            #39361#22238#22996#35351#21934
            #25918#34892#22996#35351#21934)
        end
        object TSEBrokerIDCheckBox: TCheckBox
          Left = 11
          Top = 176
          Width = 159
          Height = 20
          Caption = #25351#23450#35388#21048#21830#20195#34399
          TabOrder = 8
          OnClick = TSEBrokerIDCheckBoxClick
        end
        object TSEBrokerIDEdit: TEdit
          Left = 177
          Top = 173
          Width = 113
          Height = 26
          ImeMode = imSAlpha
          TabOrder = 9
        end
        object TSETermsComboBox: TComboBox
          Left = 122
          Top = 1
          Width = 110
          Height = 26
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 10
          Text = #19968#30908#27331#34399
          OnChange = TSETermsComboBoxChange
          Items.Strings = (
            #19968#30908#27331#34399
            #20841#30908#27331#34399
            #19977#30908#27331#34399
            #22235#30908#27331#34399
            #20116#30908#27331#34399)
        end
      end
      object PATSTabSheet: TTabSheet
        Caption = #22806#26399
        ImageIndex = 4
        object Label20: TLabel
          Left = 11
          Top = 206
          Width = 60
          Height = 18
          Caption = #27298#26680#31243#24335
        end
        object Label21: TLabel
          Left = 11
          Top = 237
          Width = 120
          Height = 18
          Caption = #27298#26680#31243#24335#19981#23384#22312#26178
        end
        object Label22: TLabel
          Left = 11
          Top = 78
          Width = 90
          Height = 18
          Caption = #21487#19979#21934#30340#24115#34399
        end
        object Label23: TLabel
          AlignWithMargins = True
          Left = 11
          Top = 177
          Width = 215
          Height = 18
          Caption = #20840#37096#31227#38500','#34920#31034#20219#20309#24115#34399#37117#21487#19979#21934
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clRed
          Font.Height = -15
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
        end
        object PATSCheckerComboBox: TComboBox
          Left = 99
          Top = 203
          Width = 192
          Height = 26
          Style = csDropDownList
          TabOrder = 0
        end
        object PATSDefaultCheckerComboBox: TComboBox
          Left = 161
          Top = 234
          Width = 130
          Height = 26
          Style = csDropDownList
          ItemIndex = 0
          TabOrder = 1
          Text = #39361#22238#22996#35351#21934
          Items.Strings = (
            #39361#22238#22996#35351#21934
            #25918#34892#22996#35351#21934)
        end
        object PATSOIDGroup: TRadioGroup
          Left = 4
          Top = 3
          Width = 286
          Height = 75
          Caption = #33258#21205#32232#21934#34399#21151#33021
          ItemIndex = 0
          Items.Strings = (
            #33258#21205#32232#24460#22235#30908','#31532#19968#30908
            #36899#32396#21312#38291)
          TabOrder = 2
          OnClick = PATSOIDGroupClick
        end
        object PATSBeginOIDEdit: TEdit
          Left = 101
          Top = 48
          Width = 73
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 5
          TabOrder = 3
        end
        object StaticText3: TStaticText
          Left = 180
          Top = 49
          Width = 15
          Height = 22
          Caption = '~'
          TabOrder = 4
        end
        object PATSEndOIDEdit: TEdit
          Left = 194
          Top = 47
          Width = 73
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 5
          TabOrder = 5
        end
        object PATSOIDPrefixEdit: TEdit
          Left = 179
          Top = 18
          Width = 24
          Height = 26
          ImeMode = imSAlpha
          MaxLength = 1
          TabOrder = 6
        end
        object AccListBox: TListBox
          Left = 11
          Top = 99
          Width = 215
          Height = 69
          ItemHeight = 18
          TabOrder = 7
        end
        object AddAccButton: TButton
          Left = 228
          Top = 98
          Width = 27
          Height = 36
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -25
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 8
          OnClick = AddAccButtonClick
        end
        object DelAccButton: TButton
          Left = 228
          Top = 133
          Width = 27
          Height = 37
          Caption = '-'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -25
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 9
          OnClick = DelAccButtonClick
        end
      end
      object VersionSheet: TTabSheet
        Caption = #29256#26412#25511#21046
        ImageIndex = 2
        TabVisible = False
        object Label12: TLabel
          Left = 14
          Top = 15
          Width = 166
          Height = 18
          Caption = #21487#36899#32218#30340'Speedy API'#29256#26412
        end
        object Label13: TLabel
          AlignWithMargins = True
          Left = 3
          Top = 243
          Width = 297
          Height = 18
          Align = alBottom
          Alignment = taCenter
          AutoSize = False
          Caption = #20840#37096#31227#38500','#34920#31034#20219#20309#29256#26412#30340'API'#37117#21487#36899#32218
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clRed
          Font.Height = -15
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          ExplicitLeft = 0
          ExplicitTop = 238
          ExplicitWidth = 302
        end
        object AddVerButton: TButton
          Left = 229
          Top = 10
          Width = 27
          Height = 25
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 0
          OnClick = AddVerButtonClick
        end
        object DelVerButton: TButton
          Left = 258
          Top = 9
          Width = 27
          Height = 26
          Caption = '-'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -20
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
          OnClick = DelVerButtonClick
        end
        object VerListBox: TListBox
          Left = 11
          Top = 41
          Width = 277
          Height = 191
          ItemHeight = 18
          TabOrder = 2
        end
      end
      object WarningSheet: TTabSheet
        Caption = #35686#31034
        ImageIndex = 3
        TabVisible = False
        object Label14: TLabel
          Left = 5
          Top = 21
          Width = 90
          Height = 18
          Caption = #22996#35351#21934#34399#23569#26044
        end
        object Label15: TLabel
          Left = 189
          Top = 21
          Width = 75
          Height = 18
          Caption = #31532#19968#27425#35686#31034
        end
        object Label16: TLabel
          Left = 5
          Top = 55
          Width = 90
          Height = 18
          Caption = #22996#35351#21934#34399#23569#26044
        end
        object Label17: TLabel
          Left = 189
          Top = 55
          Width = 75
          Height = 18
          Caption = #31532#20108#27425#35686#31034
        end
        object Label18: TLabel
          Left = 29
          Top = 95
          Width = 69
          Height = 22
          Caption = 'Version:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clNavy
          Font.Height = -19
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Label19: TLabel
          Left = 9
          Top = 128
          Width = 89
          Height = 22
          Caption = 'Build date:'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clNavy
          Font.Height = -19
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object VersionLabel: TLabel
          Left = 106
          Top = 95
          Width = 140
          Height = 18
          AutoSize = False
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -19
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object BuildLabel: TLabel
          Left = 106
          Top = 128
          Width = 140
          Height = 18
          AutoSize = False
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clBlue
          Font.Height = -19
          Font.Name = 'Arial'
          Font.Style = []
          ParentFont = False
        end
        object Level1Spin: TCSpinEdit
          Left = 100
          Top = 18
          Width = 84
          Height = 28
          MaxValue = 9000000
          MinValue = 100
          TabOrder = 0
          Value = 500
          OnChange = Level1SpinChange
        end
        object Level2Spin: TCSpinEdit
          Left = 100
          Top = 52
          Width = 84
          Height = 28
          MaxValue = 9000000
          MinValue = 100
          TabOrder = 1
          Value = 100
        end
      end
    end
  end
  object SrcPanel: TPanel
    Left = 0
    Top = 138
    Width = 339
    Height = 156
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 4
    object Label4: TLabel
      Left = 0
      Top = 37
      Width = 101
      Height = 18
      Alignment = taRightJustify
      AutoSize = False
      Caption = #27402#38480
    end
    object Label5: TLabel
      Left = 0
      Top = 5
      Width = 101
      Height = 18
      Alignment = taRightJustify
      AutoSize = False
      Caption = #19979#21934#20358#28304
    end
    object Label8: TLabel
      Left = 0
      Top = 131
      Width = 101
      Height = 18
      Alignment = taRightJustify
      AutoSize = False
      Caption = #27599#31186
    end
    object Label26: TLabel
      Left = 176
      Top = 132
      Width = 105
      Height = 18
      AutoSize = False
      Caption = #31558'('#27969#37327#19978#38480')'
    end
    object UserLabel: TLabel
      Left = 0
      Top = 67
      Width = 101
      Height = 18
      Alignment = taRightJustify
      AutoSize = False
      Caption = #38989#22806#22238#22577
    end
    object IPEdit: TEdit
      Left = 112
      Top = 96
      Width = 168
      Height = 26
      ImeMode = imSAlpha
      TabOrder = 0
    end
    object IPCheckBox: TCheckBox
      Left = 21
      Top = 99
      Width = 82
      Height = 20
      Caption = #25351#23450'IP'
      TabOrder = 1
      OnClick = IPCheckBoxClick
    end
    object GroupComboBox: TComboBox
      Left = 112
      Top = 34
      Width = 169
      Height = 26
      Style = csDropDownList
      ItemIndex = 0
      TabOrder = 2
      Text = #31649#29702#32773
      OnChange = GroupComboBoxChange
      Items.Strings = (
        #31649#29702#32773
        #29151#26989#21729
        #19979#21934#31649#36947)
    end
    object SourceComboBox: TComboBox
      Left = 112
      Top = 2
      Width = 169
      Height = 26
      Style = csDropDownList
      TabOrder = 3
      Items.Strings = (
        'Speedy-API'
        'FIX-Orc'
        'FIX-GLTRADE'
        'FIX-RTS')
    end
    object ThroughputSpinEdit: TCSpinEdit
      Left = 108
      Top = 128
      Width = 60
      Height = 28
      MaxValue = 5000
      MinValue = 5
      TabOrder = 4
      Value = 300
    end
    object UsersEdit: TEdit
      Left = 112
      Top = 64
      Width = 168
      Height = 26
      Enabled = False
      ImeMode = imSAlpha
      TabOrder = 5
      Text = '123456'
    end
    object UsersBitBtn: TBitBtn
      Left = 280
      Top = 63
      Width = 27
      Height = 27
      Glyph.Data = {
        B6080000424DB608000000000000360400002800000030000000180000000100
        08000000000080040000130B0000130B00000001000000010000FF00FF000000
        0000C8C8C8000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000101010100000000000000000000000000000000000000000202020200
        0000000000000000000000000000000000000001010101010000000000000000
        0000000000000000000000020202020200000000000000000000000000000001
        0100000100000101000101010000000000000000000000020200000200000202
        0002020200000000000000000000010101010101000001010101010101000000
        0000000000000202020202020000020202020202020000000000000000010100
        0001000000000000010000000100000000000000000202000002000000000000
        0200000002000000000000000001010000000000000000000000000101000000
        0000000000020200000000000000000000000002020000000000000000000101
        0000000101010101000000010000000000000000000002020000000202020202
        0000000200000000000000000001010100000101000001010100000101010000
        0000000000020202000002020000020202000002020200000000000001010100
        0001010000000000010000010101010000000000020202000002020000000000
        0200000202020200000000000100000000010000000000000101000000000100
        0000000002000000000200000000000002020000000002000000000001000000
        0001000000000000010100000000010000000000020000000002000000000000
        0202000000000200000000000101010000010100000000000100000101010100
        0000000002020200000202000000000002000002020202000000000000010101
        0000010100000101010000010101000000000000000202020000020200000202
        0200000202020000000000000000010100000001010101010000000100000000
        0000000000000202000000020202020200000002000000000000000000010100
        0000000000000000000000010100000000000000000202000000000000000000
        0000000202000000000000000001010000010000000000000100000001000000
        0000000000020200000200000000000002000000020000000000000000000101
        0101010100000101010101010100000000000000000002020202020200000202
        0202020202000000000000000000000101000001000001010001010100000000
        0000000000000002020000020000020200020202000000000000000000000000
        0000000101010101000000000000000000000000000000000000000202020202
        0000000000000000000000000000000000000001010101000000000000000000
        0000000000000000000000020202020000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000000000000000
        0000000000000000000000000000000000000000000000000000}
      NumGlyphs = 2
      TabOrder = 6
      OnClick = UsersBitBtnClick
    end
  end
  object AccountPanel: TPanel
    Left = 0
    Top = 604
    Width = 339
    Height = 108
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 5
    object AccPageControl: TPageControl
      Left = 14
      Top = 0
      Width = 311
      Height = 106
      ActivePage = TAIFEXTabSheet
      TabOrder = 0
      object TAIFEXTabSheet: TTabSheet
        Caption = 'TAIFEXTabSheet'
        TabVisible = False
        object TAIFEXAccLabel: TLabel
          Left = 0
          Top = 41
          Width = 83
          Height = 18
          Alignment = taRightJustify
          AutoSize = False
          Caption = #26399#36008#24115#34399
        end
        object AccountListView: TListView
          Left = 94
          Top = 2
          Width = 180
          Height = 93
          Columns = <
            item
              Caption = #24115#34399
              Width = 90
            end
            item
              Caption = #36523#20998#30908
              Width = 60
            end>
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -15
          Font.Name = 'Tahoma'
          Font.Style = []
          RowSelect = True
          ParentFont = False
          TabOrder = 0
          ViewStyle = vsReport
        end
        object AddAccountButton: TButton
          Left = 276
          Top = 16
          Width = 27
          Height = 38
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -25
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 1
          OnClick = AddAccountButtonClick
        end
        object DeleteAccountButton: TButton
          Left = 276
          Top = 56
          Width = 27
          Height = 39
          Caption = '-'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -25
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 2
          OnClick = DeleteAccountButtonClick
        end
      end
      object TWSETabSheet: TTabSheet
        Caption = 'TWSETabSheet'
        ImageIndex = 1
        TabVisible = False
        object TWSEAccLabel: TLabel
          Left = 0
          Top = 41
          Width = 83
          Height = 18
          Alignment = taRightJustify
          AutoSize = False
          Caption = #35657#21048#24115#34399
        end
        object TWSEAccountListView: TListView
          Left = 94
          Top = 2
          Width = 180
          Height = 93
          Columns = <
            item
              Caption = #24115#34399
              Width = 90
            end
            item
              Caption = #36523#20998#30908
              Width = 60
            end>
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -15
          Font.Name = 'Tahoma'
          Font.Style = []
          RowSelect = True
          ParentFont = False
          TabOrder = 0
          ViewStyle = vsReport
        end
        object DelTWSEAccountButton: TButton
          Left = 276
          Top = 56
          Width = 27
          Height = 39
          Caption = '-'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -25
          Font.Name = 'Tahoma'
          Font.Style = [fsBold]
          ParentFont = False
          TabOrder = 1
          OnClick = DelTWSEAccountButtonClick
        end
        object AddTWSEAccountButton: TButton
          Left = 276
          Top = 16
          Width = 27
          Height = 38
          Caption = '+'
          Font.Charset = DEFAULT_CHARSET
          Font.Color = clWindowText
          Font.Height = -25
          Font.Name = 'Tahoma'
          Font.Style = []
          ParentFont = False
          TabOrder = 2
          OnClick = NewTWSEAccountButtonClick
        end
      end
    end
  end
end
