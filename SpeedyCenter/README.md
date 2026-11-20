# SpeedyCenter

## 專案簡介

**SpeedyCenter**（產品名稱 *Speedy Center*，公司內部代號 *Speedy Solution*）是 MDBS Development 開發的 Windows 監控管理程式（VCL Application，C++Builder），用於監控與管理下單/報價相關的伺服器程序、連線與帳務狀態，涵蓋 **TWSE（證交所）、OTC（櫃買）、TAIFEX（期交所）、PATS、FIX、StarWave** 等交易通路。

- 應用程式標題：`Speedy Center`
- 產品說明（VerInfo）：`Monitoring program for Speedy Solution`
- 公司：`MDBS Development Inc.`
- 支援通路（Comments）：`TWSE,OTC,TAIFEX,PATS,FIX and StarWave`
- 技術棧：Embarcadero C++Builder（RAD Studio, XE7 世代專案格式）、VCL Framework、`char`-based TCHAR mapping（ANSI）

## 專案進入點

專案群組（Group Project）進入點為：

```
SpeedyCenter_XE7Group.groupproj
```

此 groupproj 定義了整個建置順序與相依關係，內含以下子專案：

| 專案 | 路徑 | 說明 |
|---|---|---|
| `UFC_XE7.cbproj` | `..\BC Projects\UFC_XE7.cbproj` | 期交所（TAIFEX）底層通訊/下單函式庫（UFC = 統一委託格式相關函式庫），編譯為 `UFC_XE7.lib` 供 SpeedyCenter 連結 |
| `MDComponent_XE7.cbproj` | `..\MDComponent\vcl\MDComponent_XE7.cbproj` | 內部共用 VCL 元件套件（自訂控制項、Grid、Graph 按鈕等），供多個專案共用 |
| `MBusComponent_XE7.cbproj` | `..\VCL\MBusComponent_XE7.cbproj` | 內部訊息匯流排（MBus）元件，負責跨程序/跨主機訊息傳遞 |
| `SpeedyCenter.cbproj` | `.\SpeedyCenter.cbproj` | **主程式專案**，相依於 `UFC_XE7` 與 `MBusComponent_XE7` |
| `SpeedyCenter_CHS.bdsproj` | `CHS\SpeedyCenter_CHS.bdsproj` | 簡體中文語言資源 DLL |
| `SpeedyCenter_CHT.bdsproj` | `CHT\SpeedyCenter_CHT.bdsproj` | 繁體中文語言資源 DLL |
| `SpeedyCenter_ENU.bdsproj` | `ENU\SpeedyCenter_ENU.bdsproj` | 英文語言資源 DLL |

groupproj 內建的 MSBuild Targets：`Build`、`Clean`、`Make`（各自會依序呼叫上述子專案的對應 Target）。

主程式 `SpeedyCenter.cbproj`：
- `MainSource`：`SpeedyCenter.cpp`
- `ProjectType`：`CppVCLApplication`（VCL, Win32）
- 版本資訊：`FileVersion 3.0.0.73` / `ProductName Speedy Solution`
- 主要相依 Lib：`MDComponent_XE7.lib`、`MBusComponent_XE7.lib`、`UFC_XE7.lib`（透過 `#pragma link` 於 `SpeedyCenter.cpp` 引入）

## 目錄結構

```
SpeedyCenter/
├── SpeedyCenter_XE7Group.groupproj   # 專案群組進入點
├── SpeedyCenter.cbproj / .cpp        # 主程式專案
├── Speedy.h                          # 全域常數：Process 狀態碼、Speedy/Agent/CMD 通訊協定碼、WM_* 自訂訊息
├── DataStruct.h / .cpp               # TAIFEX 委託回報封包結構（ControlHeader, R010/R010F/R020/R070/R090 …）
├── *.cpp / *.h                       # 各功能表單（詳見下方「主要模組」）
├── CHS/  CHT/  ENU/                  # 簡中 / 繁中 / 英文語言資源專案（多語系 DLL）
├── Debug/ Debug_obj/                 # Debug 組態輸出與中繼檔
├── Release/ Release_obj/             # Release 組態輸出與中繼檔
└── SpeedyCenter_Install/             # 安裝程式相關檔案
```

## 主要模組（表單／單元）

以 `SpeedyCenter.cpp` 內的 `USEFORM` 清單為準，依功能分類：

### 主框架 / 登入
- `MDIMain` (`TMDIForm`)：MDI 主視窗，管理登入/登出、多語系切換、視窗排列（Cascade/Tile）、SysTray（最小化到系統列）、各子視窗的分頁（`ServerTabSet`）
- `Login` (`TLoginForm`)：使用者登入表單
- `NewUser` / `NewAccountForm` / `AccountInput` / `ChangeSessionStatus`：使用者與帳號管理

### 伺服器 / 群組 / Session 監控
- `SpeedyServerForm` (`TServerForm`)：Speedy Server 狀態監控主畫面
- `GroupServer` (`TServerGroupForm`)：伺服器群組管理
- `NewSession`：新增交易 Session
- `SelChannelForm`：通路/channel 選擇
- `SystemInfoForm`：系統資訊監控
- `TerminalForm`：終端機/CMD 監控（另見 `CMDform`）
- `LineEditForm` (`PVCSettingForm`)：PVC（實體/虛擬電路連線）設定
- `SeqSettingForm`：序號（Sequence No）設定
- `ThroughputLimit`：流量限制設定
- `ChangeSessionStatus`：Session 狀態變更

### 通路 Frame（各交易通路串接）
- `StarWaveFrame` (`TFrame`)：StarWave 通路
- `PATSFrame` (`TFrame`)：PATS 通路

### 例外 / 警示 / 監控
- `ExceptionProd` / `ExecptionAccount`：商品別／帳號別的例外（風控）設定
- `AlarmForm`：告警設定
- `EventViewer`：事件記錄檢視
- `TagViewer`：Tag（標記）檢視
- `ProfileGraph`：效能／流量圖表
- `WarningDialog`：警告對話框
- `MessageDialog`：訊息對話框

### 商品 / 設定
- `ItemListForm`：商品清單
- `SettingForm` (`TFormSetting`)：一般設定
- `PrcoessSettingForm` (`TProcessForm`)：程序（Process）設定，對應 `Speedy.h` 中的 `PROCESS_*` 狀態
- `APIForm`：API 設定
- `SearchSpeedyForm`：搜尋

### 其他
- `SimTFXMain`：TAIFEX 模擬相關主畫面
- `AppInfo` / `FormAbout` / `VersionUnit`：關於／版本資訊
- `Unit2`：輔助單元
- `CMDform`：命令列/指令視窗
- `..\VCL\NewValueForm`：共用數值輸入表單（來自共用 VCL 元件庫）

## 通訊協定與資料結構

`Speedy.h` 定義了核心通訊協定常數，供 Speedy 監控端與各 Agent／程序間溝通：

- **Process 狀態碼**：`PROCESS_STOPED / STARTING / STOPING / RESTARTING / RUNNING`
- **SPEEDY_* 指令**：程序清單查詢、啟動/停止/重啟程序、設定檔讀寫、期貨/選擇權委託全部刪單等
- **IMFIX_* 指令**：FIX Session 狀態查詢/回報、序號設定、EOD、斷線/重連、啟用 Session
- **AGENT_* 指令**：Agent 心跳（`AGENT_HEARTBEAT_INTERVAL = 6000`）、效能回報、PVC 資料、事件、KillSwitch（緊急停止交易開關）等
- **CMD_PROXY_* / CMD_* 指令**：登入/登出、使用者管理、Log 查詢、Proxy 使用者鎖定/流量控管
- **WM_* 自訂視窗訊息**：`WM_USER + 100~126`，供內部表單間非同步更新 UI（如 `WM_UPDATE_PVC`、`WM_FIX_STATE_CHANGE`、`WM_PROCESS_STATE` 等）

`DataStruct.h` 定義了 TAIFEX 委託／回報封包格式（固定長度欄位、C-style struct 風格）：
- `ControlHeader`：共用封包表頭（系統別、功能碼、訊息型態、時間、狀態碼）
- `R010` / `R010F`：新單委託（一般 / 選擇權）
- `R020`：委託回報
- `R070`：查詢請求
- `R090`：改量/改價委託

## 多國語言支援

專案採資源 DLL 方式做多語系，對應 `Language` 設定與 `Resource_DLL_Projects`：

| 語言 | 目錄 | LCID |
|---|---|---|
| 簡體中文（中國） | `CHS/SpeedyCenter_CHS.bdsproj` | `$00000804` |
| 繁體中文（台灣，預設 ActiveLang） | `CHT/SpeedyCenter_CHT.bdsproj` | `$00000404` |
| 英文（美國） | `ENU/SpeedyCenter_ENU.bdsproj` | `$00000409` |

`MDIMain` 內的 `LoadLanguage` / `SaveLanguage` 負責語系載入與切換。

## 建置輸出

| 組態 | 輸出目錄 | 中繼目錄 |
|---|---|---|
| Debug | `Debug\SpeedyCenter.exe` | `Debug_obj` |
| Release | `Release\SpeedyCenter.exe` | `Release_obj` |

安裝程式相關檔案位於 `SpeedyCenter_Install/`。

## 相依的外部/內部函式庫

- **UFC_XE7**（`..\BC Projects\UFC_XE7.cbproj`）：期交所底層通訊/委託函式庫
- **MDComponent_XE7**（`..\MDComponent\vcl`）：內部共用 VCL 元件
- **MBusComponent_XE7**（`..\VCL`）：內部訊息匯流排（跨程序通訊）元件
- VCL 標準套件：`vcl`, `rtl`, `dbrtl`, `vcldb`, `vclx`, `bcbie` 等（詳見 `.cbproj` 的 `PackageImports` / `LinkPackageStatics`）
