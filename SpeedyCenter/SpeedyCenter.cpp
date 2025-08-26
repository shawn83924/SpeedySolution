//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "reinit.hpp"
#include "SCStrings.hpp"
//---------------------------------------------------------------------------
#include <Vcl.Styles.hpp>
#include <Vcl.Themes.hpp>
#pragma link "c:\\MBus\\lib\\Win32\\UFC_XE7.lib"
//---------------------------------------------------------------------------
USEFORM("SettingForm.cpp", FormSetting);
USEFORM("SeqSettingForm.cpp", SequenceForm);
USEFORM("SimTFXMain.cpp", SimTFXForm);
USEFORM("SelChannelForm.cpp", ChannelForm);
USEFORM("ProfileGraph.cpp", ProfileFrame);
USEFORM("SearchSpeedyForm.cpp", SearchForm);
USEFORM("VersionUnit.cpp", VersionForm);
USEFORM("TerminalForm.cpp", TermForm);
USEFORM("..\VCL\NewValueForm.cpp", AddValueForm);
USEFORM("WarningDialog.cpp", WarningForm);
USEFORM("TagViewer.cpp", TagForm);
USEFORM("SpeedyServerForm.cpp", ServerForm);
USEFORM("SystemInfoForm.cpp", SystemForm);
USEFORM("StarWaveFrame.cpp", SWFrame); /* TFrame: File Type */
USEFORM("PrcoessSettingForm.cpp", ProcessForm);
USEFORM("ExecptionAccount.cpp", ExceptionAccForm);
USEFORM("ExceptionProd.cpp", ExceptionProdForm);
USEFORM("GroupServer.cpp", ServerGroupForm);
USEFORM("FormAbout.cpp", AboutForm);
USEFORM("EventViewer.cpp", EventForm);
USEFORM("AlarmForm.cpp", AlarmSettingForm);
USEFORM("AccountInput.cpp", AccountInputForm);
USEFORM("ChangeSessionStatus.cpp", ChangeSessionStatusForm);
USEFORM("AppInfo.cpp", AppInfoForm);
USEFORM("ItemListForm.cpp", ItemsForm);
USEFORM("NewSession.cpp", SessionForm);
USEFORM("NewAccountForm.cpp", AccountForm);
USEFORM("PATSFrame.cpp", ConnectionFrame); /* TFrame: File Type */
USEFORM("NewUser.cpp", NewUserForm);
USEFORM("MessageDialog.cpp", MessageForm);
USEFORM("LineEditForm.cpp", PVCSettingForm);
USEFORM("MDIMain.cpp", MDIForm);
USEFORM("Login.cpp", LoginForm);
USEFORM("ThroughputLimit.cpp", ThroughputForm);
//---------------------------------------------------------------------------
LCID FCurrentLCID;
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		TStyleManager::TrySetStyle("Sapphire Kamri");
		Application->Title = "Speedy Center";
		Application->CreateForm(__classid(TMDIForm), &MDIForm);
		Application->CreateForm(__classid(TSearchForm), &SearchForm);
		Application->CreateForm(__classid(TWarningForm), &WarningForm);
		Application->CreateForm(__classid(TEventForm), &EventForm);
		Application->CreateForm(__classid(TTagForm), &TagForm);
		Application->CreateForm(__classid(TAccountForm), &AccountForm);
		Application->CreateForm(__classid(TAccountInputForm), &AccountInputForm);
		Application->CreateForm(__classid(TFormSetting), &FormSetting);
		Application->CreateForm(__classid(TServerForm), &ServerForm);
		Application->CreateForm(__classid(TSystemForm), &SystemForm);
		Application->CreateForm(__classid(TServerGroupForm), &ServerGroupForm);
		Application->CreateForm(__classid(TAddValueForm), &AddValueForm);
		Application->CreateForm(__classid(TAppInfoForm), &AppInfoForm);
		Application->CreateForm(__classid(TChannelForm), &ChannelForm);
		Application->CreateForm(__classid(TItemsForm), &ItemsForm);
		Application->CreateForm(__classid(TExceptionAccForm), &ExceptionAccForm);
		Application->CreateForm(__classid(TExceptionProdForm), &ExceptionProdForm);
		Application->CreateForm(__classid(TMessageForm), &MessageForm);
		Application->CreateForm(__classid(TThroughputForm), &ThroughputForm);
		Application->Run();
	}
	catch( Exception &exception )
	{
		 Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch( Exception &exception )
		{
			Application->ShowException(&exception);
		}
	}
	return 0;
}
//---------------------------------------------------------------------------
