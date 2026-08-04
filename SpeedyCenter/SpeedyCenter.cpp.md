# SpeedyCenter.cpp

## 概述

`SpeedyCenter.cpp` 是 **SpeedyCenter.exe** 應用程式的主程式檔（`MainSource`），對應 C++Builder VCL 應用程式的標準進入點模式：註冊專案內所有表單（`USEFORM`），並在 `WinMain` 中初始化 `Application`、設定樣式、建立需要「預先建立」的表單，最後進入訊息迴圈 `Application->Run()`。

檔案位置：[SpeedyCenter.cpp](SpeedyCenter.cpp)

## 逐段說明

### 1. Header 與連結（第 1–9 行）

```cpp
#include <vcl.h>
#pragma hdrstop
#include "reinit.hpp"
#include "SCStrings.hpp"
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
#pragma link "c:\\MBus\\lib\\Win32\\UFC_XE7.lib"
```

- `reinit.hpp`：其他單元的 reinit 相關宣告（隨附輸出的 Pascal/hpp 標頭，非本專案原始碼）。
- `SCStrings.hpp`：字串資源（多語系文字）標頭。
- `Vcl.Styles.hpp` / `Vcl.Themes.hpp`：VCL 視覺樣式（VCL Styles）支援，供後續 `TStyleManager::TrySetStyle` 使用。
- `#pragma link` 直接以絕對路徑連結 `UFC_XE7.lib`（期交所底層通訊/委託函式庫，由 `..\BC Projects\UFC_XE7.cbproj` 編譯產生），這是**寫死的本機路徑**，代表建置環境需在 `c:\MBus\lib\Win32\` 下備妥此 lib。

### 2. USEFORM 清單（第 11–45 行）

透過 `USEFORM("XxxForm.cpp", XxxInstance)` 巨集，將專案內所有表單／Frame 單元登記進工程檔，讓 IDE 與連結器知道要編譯、連結哪些單元。共登記 **35 個表單/Frame**，涵蓋：

- 主框架與登入：`MDIMain`、`Login`
- 伺服器/連線管理：`SpeedyServerForm`、`GroupServer`、`SelChannelForm`、`SeqSettingForm`、`ThroughputLimit`、`LineEditForm`（PVC 設定）
- 通路 Frame：`StarWaveFrame`（`TFrame`）、`PATSFrame`（`TFrame`）
- 例外/告警/紀錄：`ExceptionProd`、`ExecptionAccount`、`AlarmForm`、`EventViewer`、`TagViewer`、`MessageDialog`、`WarningDialog`
- 帳號/使用者：`NewAccountForm`、`NewUser`、`AccountInput`、`ChangeSessionStatus`、`NewSession`
- 商品/設定：`ItemListForm`、`SettingForm`、`PrcoessSettingForm`、`APIForm`、`SearchSpeedyForm`
- 其他：`SimTFXMain`、`ProfileGraph`、`TerminalForm`、`VersionUnit`、`SystemInfoForm`、`FormAbout`、`AppInfo`
- 共用元件：`..\VCL\NewValueForm`（跨專案共用的數值輸入表單）

> 注意：`USEFORM` 只是註冊單元，**不代表**該表單一定會在程式啟動時被建立——實際建立時機（Auto-create 或延遲建立）取決於 `WinMain` 內是否呼叫 `Application->CreateForm`，其餘表單多半在使用者操作時（如點選選單/工具列）才動態 `new` 出來。

### 3. 全域變數（第 47 行）

```cpp
LCID FCurrentLCID;
```

紀錄目前使用中的語系代碼（Locale ID），供多語系（CHS/CHT/ENU）切換使用，實際的載入/儲存邏輯在 `MDIMain.cpp` 的 `LoadLanguage`/`SaveLanguage`。

### 4. `WinMain`：程式進入點（第 49–93 行）

```cpp
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        Application->Initialize();
        TStyleManager::TrySetStyle("Sapphire Kamri");
        Application->Title = "Speedy Center";
        Application->CreateForm(__classid(TMDIForm), &MDIForm);
        ...
        Application->Run();
    }
    catch( Exception &exception ) { Application->ShowException(&exception); }
    catch (...) { /* 轉為 Exception 後顯示 */ }
    return 0;
}
```

執行流程：

1. **`Application->Initialize()`**：VCL 應用程式初始化（初始化訊息佇列、Screen 物件等）。
2. **`TStyleManager::TrySetStyle("Sapphire Kamri")`**：套用自訂 VCL 視覺樣式（對應 `.cbproj` 中 `VCL_Custom_Styles` 指向的 `SapphireKamri.vsf`），讓介面呈現統一的深色系主題外觀。
3. **`Application->Title = "Speedy Center"`**：設定工作列/工作管理員顯示名稱。
4. **依序 `CreateForm` 建立「常駐」表單**（啟動時即建立，非用到才建立）：
   - `TMDIForm`（主 MDI 視窗，必為第一個，決定 `Application->MainForm`）
   - `TSearchForm`（搜尋）
   - `TWarningForm`（警告對話框）
   - `TEventForm`（事件檢視）
   - `TTagForm`（Tag 檢視）
   - `TAccountForm` / `TAccountInputForm`（帳號、帳號輸入）
   - `TFormSetting`（一般設定）
   - `TServerForm`（Speedy Server 監控主畫面）
   - `TSystemForm`（系統資訊）
   - `TServerGroupForm`（伺服器群組）
   - `TAddValueForm`（共用數值輸入表單）
   - `TAppInfoForm`（應用程式資訊）
   - `TChannelForm`（通路選擇）
   - `TItemsForm`（商品清單）
   - `TExceptionAccForm` / `TExceptionProdForm`（帳號別／商品別例外設定）
   - `TMessageForm`（訊息對話框）
   - `TThroughputForm`（流量限制）

   這些表單多為**全域單例、常駐記憶體**的功能視窗，設計上讓 `MDIMain` 可隨時直接參照其全域實體（如 `ServerForm`、`EventForm`）並呼叫其方法/顯示，不需每次動態建立，符合此類監控主控台「開機即備妥所有子面板」的慣用模式。其餘在 USEFORM 中列出但未在此 `CreateForm` 的表單（如 `Login`、`NewSession`、`NewUser` 等），則屬於**依需求動態建立**（例如登入時才 `new TLoginForm`）。

5. **`Application->Run()`**：進入標準 Windows 訊息迴圈，直到使用者關閉主視窗。

6. **例外處理**：
   - 捕捉 VCL `Exception`：交給 `Application->ShowException` 顯示標準錯誤對話框。
   - 捕捉其餘任何例外（`catch(...)`，例如非 VCL 的 C++ 例外）：重新包裝為空白 `Exception("")` 再顯示，確保任何未預期的例外都不會讓程式無聲當掉，而是彈出錯誤視窗。

## 小結

此檔案的角色單純而關鍵：它是整個 SpeedyCenter 監控程式的**啟動骨架**——註冊所有表單單元、套用視覺樣式、建立一組常駐的核心監控/管理視窗、進入訊息迴圈，並在最外層攔截未處理例外以避免程式無提示崩潰。實際的業務邏輯（伺服器監控、通路連線、委託回報處理等）分散在各表單/單元中，本檔案不包含商業邏輯。
