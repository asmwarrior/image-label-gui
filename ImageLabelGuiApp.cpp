/***************************************************************
 * Name:      ImageLabelGuiApp.cpp
 * Purpose:   Code for Application Class
 * Author:     ()
 * Created:   2024-12-21
 * Copyright:  ()
 * License:
 **************************************************************/

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
    if ( wxsOK )
    {
        ImageLabelGuiFrame* Frame = new ImageLabelGuiFrame(0);
        Frame->Show();
        SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}
