#include "ImageLabelGuiApp.h"

//(*AppHeaders
#include "ImageLabelGuiMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(ImageLabelGuiApp);

bool ImageLabelGuiApp::OnInit()
{
    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if(wxsOK)
    {
        ImageLabelGuiFrame* Frame = new ImageLabelGuiFrame(0);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
