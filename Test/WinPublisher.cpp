//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#pragma link "\\MBus\\lib\\UFC.lib"
#pragma link "\\MBus\\lib\\Migo.lib"
//---------------------------------------------------------------------------
USEFORM("..\Test\MainForm.cpp", PublisherForm );
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
         Application->Initialize();
         Application->CreateForm(__classid(TPublisherForm), &PublisherForm);
         Application->Run();
    }
    catch (Exception &exception)
    {
         Application->ShowException(&exception);
    }
    catch (...)
    {
         try
         {
             throw Exception("");
         }
         catch (Exception &exception)
         {
             Application->ShowException(&exception);
         }
    }
    return 0;
}
//---------------------------------------------------------------------------
