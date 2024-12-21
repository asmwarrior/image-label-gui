/***************************************************************
 * Name:      ImageLabelGuiMain.h
 * Purpose:   Defines Application Frame
 * Author:     ()
 * Created:   2024-12-21
 * Copyright:  ()
 * License:
 **************************************************************/

#ifndef IMAGE_LABEL_GUIMAIN_H
#define IMAGE_LABEL_GUIMAIN_H

//(*Headers(ImageLabelGuiFrame)
#include <mathplot.h>
#include <wx/aui/aui.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/statusbr.h>
//*)

class ImageLabelGuiFrame: public wxFrame
{
    public:

        ImageLabelGuiFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~ImageLabelGuiFrame();

    private:

        //(*Handlers(ImageLabelGuiFrame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        //*)

        //(*Identifiers(ImageLabelGuiFrame)
        static const wxWindowID idMenuQuit;
        static const wxWindowID idMenuAbout;
        static const wxWindowID ID_STATUSBAR1;
        //*)

        //(*Declarations(ImageLabelGuiFrame)
        mpWindow* MathPlot1;
        wxAuiManager* AuiManager1;
        wxStatusBar* StatusBar1;
        //*)

        DECLARE_EVENT_TABLE()
};

#endif // IMAGE_LABEL_GUIMAIN_H
