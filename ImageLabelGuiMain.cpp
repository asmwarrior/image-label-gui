/***************************************************************
 * Name:      ImageLabelGuiMain.cpp
 * Purpose:   Code for Application Frame
 * Author:     ()
 * Created:   2024-12-21
 * Copyright:  ()
 * License:
 **************************************************************/

#include "ImageLabelGuiMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(ImageLabelGuiFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/filedlg.h>  // wxFileDialog
#include <wx/textdlg.h>  // wxTextentryDialog
#include <wx/msgdlg.h>
#include <wx/overlay.h>
#include <wx/sstream.h>  // For wxStringOutputStream
#include <wx/txtstrm.h>  // For wxTextOutputStream
#include <wx/log.h>
#include <wx/dcclient.h> // wxClientDC


#include "ImageArrow.h"  // custom arrow shape for wxMathPlot

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(ImageLabelGuiFrame)
const wxWindowID ImageLabelGuiFrame::ID_BUTTON1 = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_CHECKBOX1 = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_PANEL1 = wxNewId();
const wxWindowID ImageLabelGuiFrame::idMenuQuit = wxNewId();
const wxWindowID ImageLabelGuiFrame::idMenuAbout = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ImageLabelGuiFrame,wxFrame)
    //(*EventTable(ImageLabelGuiFrame)
    //*)
END_EVENT_TABLE()



ImageLabelGuiFrame::ImageLabelGuiFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(ImageLabelGuiFrame)
    wxBoxSizer* BoxSizer1;
    wxMenu* Menu1;
    wxMenu* Menu2;
    wxMenuBar* MenuBar1;
    wxMenuItem* MenuItem1;
    wxMenuItem* MenuItem2;

    Create(parent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE, _T("wxID_ANY"));
    AuiManager1 = new wxAuiManager(this, wxAUI_MGR_DEFAULT);
    m_MathPlot = new mpWindow(this, wxID_ANY, wxPoint(181,113), wxDefaultSize, wxTAB_TRAVERSAL);
    m_MathPlot->UpdateAll();
    m_MathPlot->Fit();
    AuiManager1->AddPane(m_MathPlot, wxAuiPaneInfo().Name(_T("image")).CenterPane().Caption(_("Pane caption")));
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Panel1->SetMinSize(wxSize(150,0));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    m_ButtonLoadImage = new wxButton(Panel1, ID_BUTTON1, _("Load image"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer1->Add(m_ButtonLoadImage, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CheckBoxDrawArrow = new wxCheckBox(Panel1, ID_CHECKBOX1, _("Draw arrow"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_CheckBoxDrawArrow->SetValue(false);
    BoxSizer1->Add(m_CheckBoxDrawArrow, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Panel1->SetSizer(BoxSizer1);
    AuiManager1->AddPane(Panel1, wxAuiPaneInfo().Name(_T("control")).DefaultPane().Caption(_("Pane caption")).CaptionVisible().Right().MinSize(wxSize(150,0)));
    AuiManager1->Update();
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tAlt-F4"), _("Quit the application"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("Help"));
    SetMenuBar(MenuBar1);
    StatusBar1 = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[1] = { -1 };
    int __wxStatusBarStyles_1[1] = { wxSB_NORMAL };
    StatusBar1->SetFieldsCount(1,__wxStatusBarWidths_1);
    StatusBar1->SetStatusStyles(1,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar1);

    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageLabelGuiFrame::OnButtonLoadImageClick, this, ID_BUTTON1);
    Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &ImageLabelGuiFrame::OnCheckBoxDrawArrowClick, this, ID_CHECKBOX1);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ImageLabelGuiFrame::OnQuit, this, idMenuQuit);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ImageLabelGuiFrame::OnAbout, this, idMenuAbout);
    //*)
}

ImageLabelGuiFrame::~ImageLabelGuiFrame()
{
    //(*Destroy(ImageLabelGuiFrame)
    //*)
}

void ImageLabelGuiFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void ImageLabelGuiFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, _("Welcome to..."));
}

void ImageLabelGuiFrame::OnButtonLoadImageClick(wxCommandEvent& event)
{
    // Open file dialog to select an image file
    wxFileDialog openFileDialog(
        this,
        _("Open Image File"),
        "", "",
        "Image files (*.png;*.jpg;*.bmp)|*.png;*.jpg;*.bmp|All files (*.*)|*.*",
        wxFD_OPEN | wxFD_FILE_MUST_EXIST
    );

    if (openFileDialog.ShowModal() == wxID_CANCEL)
        return; // User cancelled

    // Load the selected image
    wxString filePath = openFileDialog.GetPath();
    wxString fileName = openFileDialog.GetFilename();
    wxImage image;
    if (!image.LoadFile(filePath))
    {
        wxLogError("Failed to load image: %s", filePath);
        return;
    }

    // Store the image resolution and filename in the member variables
    m_LoadedImageWidth = image.GetWidth();
    m_LoadedImageHeight = image.GetHeight();
    m_LoadedImageFilename = fileName;

    // CleanPlot(); // Remove any existing layers
    m_MathPlot->DelAllPlot(true);
    m_MathPlot->DelLayer(m_MathPlot->GetLayerByClassName("mpBitmapLayer"), true);

    // Create a bitmap layer
    mpBitmapLayer* bitmapLayer = new mpBitmapLayer();
    bitmapLayer->SetBitmap(image, 0.0, 0.0, 1.0, 1.0);

    // Add bitmap layer to the plot
    m_MathPlot->AddLayer(bitmapLayer);

    // Update the plot
    m_MathPlot->Fit();
    m_MathPlot->Refresh();
}

void ImageLabelGuiFrame::OnCheckBoxDrawArrowClick(wxCommandEvent& event)
{
    if(m_CheckBoxDrawArrow->GetValue())
    {
        m_MathPlot->SetOnUserMouseAction([this](void* Sender, wxMouseEvent & event, bool & cancel)
        {
            OnUserMouseActionDrawArrow(Sender, event, cancel);
        });
    }
    else
        m_MathPlot->UnSetOnUserMouseAction();
}

void ImageLabelGuiFrame::OnUserMouseActionDrawArrow(void* Sender, wxMouseEvent& event, bool& cancel)
{
    static wxOverlay m_overlay;
    static wxPoint startPointScreen, endPointScreen;
    static wxPoint dragOffset;
    static wxPoint currentPointScreen;
    static bool isDrawingArrow = false;
    static bool isModifyingArrow = false;
    static bool isResizing = false;
    static bool isMoving = false;
    static bool resizingStartPoint = false;

    static double startX, startY, endX, endY; // Graph coordinates for the arrow
    static mpArrow* selectedArrow = nullptr;

    wxPoint mousePosition = event.GetPosition(); // Mouse position in screen coordinates
    mpWindow* plotWindow = (mpWindow*)Sender;   // Cast Sender to mpWindow

    cancel = true;

    // Left mouse button down: Start drawing a new arrow
    if(event.LeftDown() && !isModifyingArrow)
    {
        isDrawingArrow = true;

        // Record the starting point of the arrow
        startPointScreen = mousePosition;
        startX = plotWindow->p2x(startPointScreen.x);
        startY = plotWindow->p2y(startPointScreen.y);

        cancel = true;
    }
    // Right mouse button down: Start modifying an existing arrow
    else if(event.RightDown())
    {
        // Attempt to find the closest arrow layer to the mouse position
        double mouseX = plotWindow->p2x(mousePosition.x);
        double mouseY = plotWindow->p2y(mousePosition.y);
        selectedArrow = dynamic_cast<mpArrow*>(FindClosestArrowLayer(plotWindow, mouseX, mouseY));

        if(selectedArrow)
        {
            // Get start and end points from the selected arrow
            wxRealPoint startPoint = selectedArrow->GetStartPoint();
            wxRealPoint endPoint = selectedArrow->GetEndPoint();

            startX = startPoint.x;
            startY = startPoint.y;
            endX = endPoint.x;
            endY = endPoint.y;

            startPointScreen = wxPoint(plotWindow->x2p(startX), plotWindow->y2p(startY));
            endPointScreen = wxPoint(plotWindow->x2p(endX), plotWindow->y2p(endY));

            // Determine if the mouse is near the start or end point
            if(DistanceBetweenPoints(mousePosition, startPointScreen) < 10)
            {
                isResizing = true;
                isModifyingArrow = true;
                cancel = true;
                dragOffset = mousePosition - startPointScreen; // Record offset for start point
                resizingStartPoint = true; // Resizing the start point
            }
            else if(DistanceBetweenPoints(mousePosition, endPointScreen) < 10)
            {
                isResizing = true;
                isModifyingArrow = true;
                cancel = true;
                dragOffset = mousePosition - endPointScreen; // Record offset for end point
                resizingStartPoint = false; // Resizing the end point
            }
            else if(IsPointOnLine(mousePosition, startPointScreen, endPointScreen, 5))
            {
                isMoving = true;
                isModifyingArrow = true;
                dragOffset = mousePosition - startPointScreen;
                cancel = true;
            }
        }

        cancel = true;
    }
    // Mouse dragging with the left button: Draw a new arrow
    else if(event.Dragging() && event.LeftIsDown() && isDrawingArrow)
    {
        currentPointScreen = mousePosition;

        // Draw the arrow dynamically on the overlay
        wxClientDC dc(plotWindow);
        PrepareDC(dc);
        wxDCOverlay overlay(m_overlay, &dc);
        overlay.Clear();

        dc.SetPen(wxPen(*wxBLACK, 2));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        dc.DrawLine(startPointScreen, currentPointScreen);
        DrawArrowHead(dc, startPointScreen, currentPointScreen);

        cancel = true;
    }
    // Mouse dragging with the right button: Modify an existing arrow
    else if(event.Dragging() && event.RightIsDown() && isModifyingArrow)
    {
        wxClientDC dc(plotWindow);
        PrepareDC(dc);
        wxDCOverlay overlay(m_overlay, &dc);
        overlay.Clear();

        dc.SetPen(wxPen(*wxBLUE, 2));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        if(isResizing)
        {
            // Dynamically update the selected point
            if(resizingStartPoint)
            {
                startPointScreen = mousePosition;
                startX = plotWindow->p2x(startPointScreen.x);
                startY = plotWindow->p2y(startPointScreen.y);
            }
            else
            {
                endPointScreen = mousePosition;
                endX = plotWindow->p2x(endPointScreen.x);
                endY = plotWindow->p2y(endPointScreen.y);
            }

            // Draw the dynamically updated arrow
            dc.DrawLine(startPointScreen, endPointScreen);
            DrawArrowHead(dc, startPointScreen, endPointScreen);
        }
        else if(isMoving)
        {
            // Update both start and endpoint positions dynamically
            wxPoint newStartPoint = mousePosition - dragOffset;
            wxPoint delta = newStartPoint - startPointScreen;

            startPointScreen = newStartPoint;
            endPointScreen += delta;

            startX = plotWindow->p2x(startPointScreen.x);
            startY = plotWindow->p2y(startPointScreen.y);
            endX = plotWindow->p2x(endPointScreen.x);
            endY = plotWindow->p2y(endPointScreen.y);

            dc.DrawLine(startPointScreen, endPointScreen);
            DrawArrowHead(dc, startPointScreen, endPointScreen);
        }

        cancel = true;
    }

    // Left mouse button released: Finalize a new arrow
    else if(event.LeftUp() && isDrawingArrow)
    {
        isDrawingArrow = false;
        m_overlay.Reset();

        currentPointScreen = mousePosition;
        endX = plotWindow->p2x(currentPointScreen.x);
        endY = plotWindow->p2y(currentPointScreen.y);

        // Open a text entry dialog to get the label for the arrow
        wxTextEntryDialog textDialog(
            this,
            "Enter the label for the arrow:",
            "Arrow Label",
            "Arrow"
        );

        wxString label = "Arrow"; // Default label
        if (textDialog.ShowModal() == wxID_OK)
        {
            label = textDialog.GetValue(); // Get the user-entered label
        }

        // Create and add a permanent arrow layer with the label
        mpArrow* arrow = new mpArrow(wxRealPoint(startX, startY), wxRealPoint(endX, endY), label);
        plotWindow->AddLayer(arrow, true);
        plotWindow->Refresh();

        cancel = true;
    }

    // Right mouse button released: Finalize arrow modifications
    else if(event.RightUp() && isModifyingArrow)
    {
        isModifyingArrow = false;
        isResizing = false;
        isMoving = false;
        m_overlay.Reset();

        // Update the selected arrow's start and end points
        if(selectedArrow)
        {
            selectedArrow->SetStartPoint(wxRealPoint(startX, startY));
            selectedArrow->SetEndPoint(wxRealPoint(endX, endY));
            plotWindow->Refresh();

            // Clear the selectedArrow reference
            selectedArrow = nullptr;
        }

        cancel = true;
    }
    // Right mouse double-click: Edit the label of the selected arrow
    else if(event.RightDClick())
    {
        double mouseX = plotWindow->p2x(mousePosition.x);
        double mouseY = plotWindow->p2y(mousePosition.y);

        // Find the closest arrow to the mouse position
        selectedArrow = dynamic_cast<mpArrow*>(FindClosestArrowLayer(plotWindow, mouseX, mouseY));

        if(selectedArrow)
        {
            // Prompt the user to edit the label
            wxTextEntryDialog labelDialog(plotWindow,
                                          "Edit Arrow Label:",
                                          "Arrow Label Editor",
                                          selectedArrow->GetLabel());

            if(labelDialog.ShowModal() == wxID_OK)
            {
                // Update the label with the user's input
                wxString newLabel = labelDialog.GetValue();
                selectedArrow->SetLabel(newLabel.ToStdString());
                plotWindow->Refresh(); // Refresh the plot to display the updated label
            }
            cancel = true;
        }
    }
}


// Helper function to draw the arrowhead
void ImageLabelGuiFrame::DrawArrowHead(wxDC& dc, const wxPoint& start, const wxPoint& end)
{
    // Calculate the direction vector of the line
    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double length = std::sqrt(dx * dx + dy * dy);

    if (length == 0) return;

    // Normalize the direction vector
    dx /= length;
    dy /= length;

    // Define the size and angle of the arrowhead
    const double arrowSize = 10.0;
    const double arrowAngle = M_PI / 6.0; // 30 degrees

    // Calculate the two arrowhead points
    wxPoint arrowPoint1(
        end.x - arrowSize * (dx * std::cos(arrowAngle) - dy * std::sin(arrowAngle)),
        end.y - arrowSize * (dy * std::cos(arrowAngle) + dx * std::sin(arrowAngle)));

    wxPoint arrowPoint2(
        end.x - arrowSize * (dx * std::cos(-arrowAngle) - dy * std::sin(-arrowAngle)),
        end.y - arrowSize * (dy * std::cos(-arrowAngle) + dx * std::sin(-arrowAngle)));

    // Draw the arrowhead
    dc.DrawLine(end, arrowPoint1);
    dc.DrawLine(end, arrowPoint2);
}

mpArrow* ImageLabelGuiFrame::FindClosestArrowLayer(mpWindow* plotWindow, double mouseX, double mouseY)
{
    mpArrow* closestArrow = nullptr;
    double minDistance = std::numeric_limits<double>::max();

    // Iterate over all layers to find the closest arrow
    for (unsigned int i = 0; i < plotWindow->CountAllLayers(); i++)
    {
        auto layer = plotWindow->GetLayer(i);
        mpArrow* arrowLayer = dynamic_cast<mpArrow*>(layer);
        if (arrowLayer && arrowLayer->GetName() == "Arrow")
        {
            // Assuming mpArrow has GetStartPoint() and GetEndPoint() methods
            auto startPoint = arrowLayer->GetStartPoint(); // Should return a std::pair<double, double>
            auto endPoint = arrowLayer->GetEndPoint();     // Should return a std::pair<double, double>

            double startX = startPoint.x;
            double startY = startPoint.y;
            double endX = endPoint.x;
            double endY = endPoint.y;

            double distance = DistanceToLine(mouseX, mouseY, startX, startY, endX, endY);
            if (distance < minDistance)
            {
                minDistance = distance;
                closestArrow = arrowLayer;
            }
        }
    }

    // Return the closest arrow layer if within a reasonable distance
    return (minDistance < 5.0) ? closestArrow : nullptr;
}

