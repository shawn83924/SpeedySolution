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
USEFORM("SeqSettingForm.cpp", SequenceForm);
USEFORM("SettingForm.cpp", FormSetting);
USEFORM("SimTFXMain.cpp", SimTFXForm);
USEFORM("ProfileGraph.cpp", ProfileFrame);
USEFORM("SearchSpeedyForm.cpp", SearchForm);
USEFORM("SelChannelForm.cpp", ChannelForm);
USEFORM("TerminalForm.cpp", TermForm);
USEFORM("ThroughputLimit.cpp", ThroughputForm);
USEFORM("VersionUnit.cpp", VersionForm);
USEFORM("WarningDialog.cpp", WarningForm);
USEFORM("..\VCL\NewValueForm.cpp", AddValueForm);
USEFORM("SpeedyServerForm.cpp", ServerForm);
USEFORM("StarWaveFrame.cpp", SWFrame); /* TFrame: File Type */
USEFORM("SystemInfoForm.cpp", SystemForm);
USEFORM("TagViewer.cpp", TagForm);
USEFORM("ExceptionProd.cpp", ExceptionProdForm);
USEFORM("ExecptionAccount.cpp", ExceptionAccForm);
USEFORM("FormAbout.cpp", AboutForm);
USEFORM("GroupServer.cpp", ServerGroupForm);
USEFORM("ItemListForm.cpp", ItemsForm);
USEFORM("AccountInput.cpp", AccountInputForm);
USEFORM("AlarmForm.cpp", AlarmSettingForm);
USEFORM("AppInfo.cpp", AppInfoForm);
USEFORM("ChangeSessionStatus.cpp", ChangeSessionStatusForm);
USEFORM("EventViewer.cpp", EventForm);
USEFORM("NewAccountForm.cpp", AccountForm);
USEFORM("NewSession.cpp", SessionForm);
USEFORM("NewUser.cpp", NewUserForm);
USEFORM("PATSFrame.cpp", ConnectionFrame); /* TFrame: File Type */
USEFORM("PrcoessSettingForm.cpp", ProcessForm);
USEFORM("LineEditForm.cpp", PVCSettingForm);
USEFORM("Login.cpp", LoginForm);
USEFORM("MDIMain.cpp", MDIForm);
USEFORM("MessageDialog.cpp", MessageForm);
USEFORM("APIForm.cpp", APISettingForm);
USEFORM("UsersForm.cpp", UsersList);
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
		Application->CreateForm(__classid(TUsersList), &UsersList);
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
