#ifndef IMAGE_LABEL_GUIMAIN_H
#define IMAGE_LABEL_GUIMAIN_H

//(*Headers(ImageLabelGuiFrame)
#include <mathplot.h>
#include <wx/aui/aui.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statusbr.h>
#include <wx/textctrl.h>
//*)

class mpArrow;

class ImageLabelGuiFrame: public wxFrame
{
public:

    ImageLabelGuiFrame(wxWindow* parent, wxWindowID id = -1);
    virtual ~ImageLabelGuiFrame();

    void LoadImage(const wxString& filePath);


private:

    //(*Handlers(ImageLabelGuiFrame)
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnButtonLoadImageClick(wxCommandEvent& event);
    void OnCheckBoxDrawArrowClick(wxCommandEvent& event);
    void OnButtonGenerateLatexCodeClick(wxCommandEvent& event);
    //*)

    //(*Identifiers(ImageLabelGuiFrame)
    static const wxWindowID ID_BUTTON1;
    static const wxWindowID ID_CHECKBOX1;
    static const wxWindowID ID_BUTTON2;
    static const wxWindowID ID_PANEL1;
    static const wxWindowID ID_TEXTCTRL1;
    static const wxWindowID idMenuQuit;
    static const wxWindowID idMenuAbout;
    static const wxWindowID ID_STATUSBAR1;
    //*)

    //(*Declarations(ImageLabelGuiFrame)
    mpWindow* m_MathPlot;
    wxAuiManager* AuiManager1;
    wxButton* m_ButtonGenerateLatexCode;
    wxButton* m_ButtonLoadImage;
    wxCheckBox* m_CheckBoxDrawArrow;
    wxPanel* Panel1;
    wxStatusBar* StatusBar1;
    wxTextCtrl* m_TextCtrlLog;
    //*)

    // user defined variables

    mpScaleX* bottomAxis = NULL;
    mpScaleY* leftAxis = NULL;
    void InitializePlot(void);
    void CleanPlot(void);

    int m_LoadedImageWidth;
    int m_LoadedImageHeight;
    wxString m_LoadedImageFilename;
    bool isDragging = false;          // Track if the user is dragging the mouse
    void OnUserMouseActionDrawArrow(void* Sender, wxMouseEvent& event, bool& cancel);
    void DrawArrowHead(wxDC& dc, const wxPoint& start, const wxPoint& end);
    mpArrow* FindClosestArrowLayer(mpWindow* plotWindow, double mouseX, double mouseY);



    DECLARE_EVENT_TABLE()
};

#endif // IMAGE_LABEL_GUIMAIN_H
