Public Class Form1
    Friend WithEvents FConnection As OrderConnection.TaifexConnection

    Private Sub Form1_Load(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles MyBase.Load
        '建立一個期交所連線物件
        FConnection = New OrderConnection.TaifexConnection()
        FConnection.Create("SpeedySampleVB Client")
        FConnection.BrokerID = "S889999"   '設定期貨商代號
        FConnection.ClearMemberID = "S889" '設定結算會員
    End Sub

    Private Sub Button1_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles ConnectButton.Click
        '連線到Speedy Order Server
        FConnection.Connect("192.168.168.120", 23456)
    End Sub

    Private Sub FConnection_OnConnected() Handles FConnection.OnConnected
        ListBox.Items.Add("連線成功!")
    End Sub

    Private Sub FConnection_OnDisonnected() Handles FConnection.OnDisconnected
        ListBox.Items.Add("斷線")
    End Sub

    Private Sub Button2_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles LogonButton.Click
        '登入Speedy Order Server
        FConnection.Logon("0001", "123456", "0000001") ' AE, 密碼,投資人帳號
    End Sub

    Private Sub FConnection_OnLogonReply(ByVal Msg As String, ByVal IsAccepted As Boolean, ByVal ConnectionID As Integer) Handles FConnection.OnLogonReply
        If IsAccepted = True Then
            ListBox.Items.Add("登入成功!" + Msg)
        Else
            ListBox.Items.Add("登入失敗!" + Msg)
        End If
    End Sub

    Private Sub FutButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles FutButton.Click
        '產生一個新委託單物件
        Dim NewOrder As OrderMessage.NewOrderMessage = New OrderMessage.NewOrderMessage()
        NewOrder.Market = OrderMessage.MarketEnum.mFutures '市場別 Futures/Option
        NewOrder.AE = "0001"         '填入營業原代號
        NewOrder.Account = "0000001" '填入投資人帳號
        NewOrder.Symbol = "FITX"     '商品代號   
        NewOrder.OrderType = OrderMessage.OrderTypeEnum.otLimit '市價,限價單
        NewOrder.Price = 7000.0
        NewOrder.OrderQty = 5
        NewOrder.Side = OrderMessage.SideEnum.sBuy '買賣別
        NewOrder.MaturityMonthYear = "200612" '到期月份
        ListBox.Items.Add("期貨委託,網路單號:" + NewOrder.NID.ToString())
        '將此新委託單物件透過連線物件送到Order Server
        FConnection.NewOrder(NewOrder)
    End Sub

    Private Sub OptButton_Click(ByVal sender As System.Object, ByVal e As System.EventArgs) Handles OptButton.Click
        '產生一個新委託單物件
        Dim NewOrder As OrderMessage.NewOrderMessage = New OrderMessage.NewOrderMessage()
        NewOrder.Market = OrderMessage.MarketEnum.mOptions   '市場別 Futures/Option
        NewOrder.AE = "0001"         '填入營業原代號
        NewOrder.Account = "0000001" '填入投資人帳號
        NewOrder.Symbol = "TXO07100K6"     '商品代號   
        NewOrder.OrderType = OrderMessage.OrderTypeEnum.otLimit '市價,限價單
        NewOrder.Price = 99.0
        NewOrder.OrderQty = 5
        NewOrder.Side = OrderMessage.SideEnum.sBuy '買賣別
        NewOrder.TimeInForce = OrderMessage.TimeInForceEnum.tifFOK '委託方式ROD,IOC,FOK
        ListBox.Items.Add("選擇權委託,網路單號:" + NewOrder.NID.ToString())
        '將此新委託單物件透過連線物件送到Order Server
        FConnection.NewOrder(NewOrder)
    End Sub

    '回報的事件,所有的委託回報,成交回報,刪改單回報,都透過這個事件通知用戶端
    Private Sub FConnection_OnExecutionReport(ByVal Msg As OrderMessage.ExecutionReportMessage, ByVal PossDup As Boolean) Handles FConnection.OnExecutionReport
        Select Case Msg.OrderStatus
            Case OrderMessage.OrderStatusEnum.osPendingNew
                ListBox.Items.Add("Speedy收到新委託,網路單號:" + Msg.NID.ToString())
            Case OrderMessage.OrderStatusEnum.osNew
                ListBox.Items.Add("交易所委託回報,網路單號:" + Msg.NID.ToString() + " 單號:" + Msg.OrderID)
            Case OrderMessage.OrderStatusEnum.osPartiallyFilled
                ListBox.Items.Add("交易所成交回報,網路單號:" + Msg.NID.ToString() + " 單號:" + Msg.OrderID + " 口數:" + Msg.OrderQty.ToString() + " 成交價:" + Msg.Price.ToString())
            Case OrderMessage.OrderStatusEnum.osCanceled
                ListBox.Items.Add("交易所刪單成功,網路單號:" + Msg.NID.ToString() + " 單號:" + Msg.OrderID)
            Case OrderMessage.OrderStatusEnum.osRejected
                ListBox.Items.Add("下單失敗!網路單號:" + Msg.NID.ToString() + " 訊息:" + Msg.Text)
        End Select
    End Sub
End Class
