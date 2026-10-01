#include "ImageLabelGuiMain.h"
#include <wx/msgdlg.h>

//(*InternalHeaders(ImageLabelGuiFrame)
#include <wx/intl.h>
#include <wx/string.h>
//*)

#include <wx/dialog.h>   // wxDialog
#include <wx/event.h>    // wxKeyEvent, wxEVT_CHAR_HOOK
#include <wx/filedlg.h>  // wxFileDialog
#include <wx/sizer.h>    // wxBoxSizer
#include <wx/textdlg.h>  // wxTextentryDialog
#include <wx/msgdlg.h>
#include <wx/overlay.h>
#include <wx/sstream.h>  // For wxStringOutputStream / wxStringInputStream
#include <wx/txtstrm.h>  // For wxTextOutputStream / wxTextInputStream
#include <wx/log.h>
#include <wx/dcclient.h> // wxClientDC
#include <wx/filename.h> // wxFileName


#include "ImageDropTarget.h" // drag and drop for opening the image file


#include "ImageArrow.h"  // custom arrow shape for wxMathPlot

#include "ImageLabelLayer.h"  // custom label shape for wxMathPlot

#include "ImageRegion.h"  // custom rectangular region shape for wxMathPlot

//helper functions
enum wxbuildinfoformat
{
    short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if(format == long_f)
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

// Escape the characters which have a special meaning in LaTeX
static wxString EscapeLatex(const wxString& text)
{
    wxString escaped = text;
    escaped.Replace("_", "\\_");
    escaped.Replace("&", "\\&");
    escaped.Replace("%", "\\%");
    escaped.Replace("#", "\\#");
    escaped.Replace("$", "\\$");
    escaped.Replace("^", "\\^{}");
    escaped.Replace("{", "\\{");
    escaped.Replace("}", "\\}");
    return escaped;
}

// Reverse the escaping applied by EscapeLatex
static wxString UnescapeLatex(const wxString& text)
{
    wxString unescaped = text;
    unescaped.Replace("\\_", "_");
    unescaped.Replace("\\&", "&");
    unescaped.Replace("\\%", "%");
    unescaped.Replace("\\#", "#");
    unescaped.Replace("\\$", "$");
    unescaped.Replace("\\^{}", "^");
    unescaped.Replace("\\{", "{");
    unescaped.Replace("\\}", "}");
    return unescaped;
}

// Restrict a point to the image area, which is the [0,1] x [0,1] square of the plot
static wxRealPoint ClampToImage(double x, double y)
{
    return wxRealPoint(wxMin(wxMax(x, 0.0), 1.0), wxMin(wxMax(y, 0.0), 1.0));
}

//(*IdInit(ImageLabelGuiFrame)
const wxWindowID ImageLabelGuiFrame::ID_BUTTON1 = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_CHECKBOX1 = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_CHECKBOX2 = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_CHECKBOX3 = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_BUTTON2 = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_PANEL1 = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_TEXTCTRL1 = wxNewId();
const wxWindowID ImageLabelGuiFrame::idMenuQuit = wxNewId();
const wxWindowID ImageLabelGuiFrame::idMenuAbout = wxNewId();
const wxWindowID ImageLabelGuiFrame::ID_STATUSBAR1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(ImageLabelGuiFrame, wxFrame)
    //(*EventTable(ImageLabelGuiFrame)
    //*)
END_EVENT_TABLE()



ImageLabelGuiFrame::ImageLabelGuiFrame(wxWindow* parent, wxWindowID id)
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
    AuiManager1->AddPane(m_MathPlot, wxAuiPaneInfo().Name(_T("image")).CenterPane().Caption(_("image")));
    Panel1 = new wxPanel(this, ID_PANEL1, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL, _T("ID_PANEL1"));
    Panel1->SetMinSize(wxSize(150,0));
    BoxSizer1 = new wxBoxSizer(wxVERTICAL);
    m_ButtonLoadImage = new wxButton(Panel1, ID_BUTTON1, _("Load image"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer1->Add(m_ButtonLoadImage, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CheckBoxDrawArrow = new wxCheckBox(Panel1, ID_CHECKBOX1, _("Draw arrow"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX1"));
    m_CheckBoxDrawArrow->SetValue(false);
    BoxSizer1->Add(m_CheckBoxDrawArrow, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CheckBoxDrawLabel = new wxCheckBox(Panel1, ID_CHECKBOX2, _("Draw label"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX2"));
    m_CheckBoxDrawLabel->SetValue(false);
    BoxSizer1->Add(m_CheckBoxDrawLabel, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_CheckBoxDrawRegion = new wxCheckBox(Panel1, ID_CHECKBOX3, _("Draw region"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_CHECKBOX3"));
    m_CheckBoxDrawRegion->SetValue(false);
    BoxSizer1->Add(m_CheckBoxDrawRegion, 0, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    m_ButtonGenerateLatexCode = new wxButton(Panel1, ID_BUTTON2, _("Generate latex code"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer1->Add(m_ButtonGenerateLatexCode, 0, wxALL|wxEXPAND, 5);
    ButtonImportLatexCode = new wxButton(Panel1, wxID_ANY, _("Import latex code"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator);
    BoxSizer1->Add(ButtonImportLatexCode, 0, wxALL|wxEXPAND, 5);
    Panel1->SetSizer(BoxSizer1);
    AuiManager1->AddPane(Panel1, wxAuiPaneInfo().Name(_T("control")).DefaultPane().Caption(_("control")).CaptionVisible().Right().MinSize(wxSize(150,0)));
    m_TextCtrlLog = new wxTextCtrl(this, ID_TEXTCTRL1, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_RICH|wxTE_RICH2, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    m_TextCtrlLog->SetMinSize(wxSize(0,150));
    AuiManager1->AddPane(m_TextCtrlLog, wxAuiPaneInfo().Name(_T("log")).DefaultPane().Caption(_("log")).CaptionVisible().Bottom().MinSize(wxSize(0,150)));
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
    Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &ImageLabelGuiFrame::OnCheckBoxDrawLabelClick, this, ID_CHECKBOX2);
    Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &ImageLabelGuiFrame::OnCheckBoxDrawRegionClick, this, ID_CHECKBOX3);
    Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageLabelGuiFrame::OnButtonGenerateLatexCodeClick, this, ID_BUTTON2);
    ButtonImportLatexCode->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &ImageLabelGuiFrame::OnButtonImportLatexCodeClick, this);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ImageLabelGuiFrame::OnQuit, this, idMenuQuit);
    Bind(wxEVT_COMMAND_MENU_SELECTED, &ImageLabelGuiFrame::OnAbout, this, idMenuAbout);
    //*)

    // The Del key removes the annotation which was touched last. wxEVT_CHAR_HOOK
    // is used because it is received no matter which child window has the focus.
    Bind(wxEVT_CHAR_HOOK, &ImageLabelGuiFrame::OnCharHook, this);

    InitializePlot();

    // Set the drop target
    SetDropTarget(new ImageDropTarget(this));
}

ImageLabelGuiFrame::~ImageLabelGuiFrame()
{
    //(*Destroy(ImageLabelGuiFrame)
    AuiManager1->UnInit();
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

    if(openFileDialog.ShowModal() == wxID_CANCEL)
        return; // User cancelled

    wxString filePath = openFileDialog.GetPath();

    LoadImage(filePath);
}

void ImageLabelGuiFrame::OnCheckBoxDrawArrowClick(wxCommandEvent& event)
{
    if(m_CheckBoxDrawArrow->GetValue())
    {
        // mpWindow has a single user mouse action callback, so only one drawing mode can be active
        m_CheckBoxDrawLabel->SetValue(false);
        m_CheckBoxDrawRegion->SetValue(false);

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

        // Calculate the length of the arrow
        double deltaX = endX - startX;
        double deltaY = endY - startY;
        double distance = std::sqrt(deltaX * deltaX + deltaY * deltaY);

        const double minimumDistanceThreshold = 0.1; // Set your threshold here

        if(distance < minimumDistanceThreshold)
        {
            // If the distance is too small, skip creating the arrow
            wxMessageBox(
                "The arrow is too short to be created. Please try again.",
                "Arrow Too Short",
                wxOK | wxICON_WARNING,
                this
            );
            return;
        }

        // Open a text entry dialog to get the label for the arrow
        wxTextEntryDialog textDialog(
            this,
            "Enter the label for the arrow:",
            "Arrow Label",
            "Arrow"
        );

        wxString label = "Arrow"; // Default label
        if(textDialog.ShowModal() == wxID_OK)
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

            // Remember the arrow, it can be removed again with the Del key
            m_pLastTouchedLayer = selectedArrow;

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

            // Remember the arrow, it can be removed again with the Del key
            m_pLastTouchedLayer = selectedArrow;

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

    if(length == 0) return;

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
    for(unsigned int i = 0; i < plotWindow->CountAllLayers(); i++)
    {
        auto layer = plotWindow->GetLayer(i);
        mpArrow* arrowLayer = dynamic_cast<mpArrow*>(layer);
        if(arrowLayer && arrowLayer->GetName() == "Arrow")
        {
            // Assuming mpArrow has GetStartPoint() and GetEndPoint() methods
            auto startPoint = arrowLayer->GetStartPoint(); // Should return a std::pair<double, double>
            auto endPoint = arrowLayer->GetEndPoint();     // Should return a std::pair<double, double>

            double startX = startPoint.x;
            double startY = startPoint.y;
            double endX = endPoint.x;
            double endY = endPoint.y;

            double distance = DistanceToLine(mouseX, mouseY, startX, startY, endX, endY);
            if(distance < minDistance)
            {
                minDistance = distance;
                closestArrow = arrowLayer;
            }
        }
    }

    // Return the closest arrow layer if within a reasonable distance
    return (minDistance < 5.0) ? closestArrow : nullptr;
}


void ImageLabelGuiFrame::InitializePlot(void)
{
    m_MathPlot->EnableDoubleBuffer(true);
    m_MathPlot->SetMargins(80, 80, 80, 80);

    bottomAxis = new mpScaleX(wxT("X"), mpALIGN_CENTERX, true, mpX_NORMAL);
    bottomAxis->SetLabelFormat("%g");
    leftAxis = new mpScaleY(wxT("Y"), mpALIGN_CENTERY, true);
    leftAxis->SetLabelFormat("%g");

    wxFont graphFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
    wxPen axispen(*wxRED, 2, wxPENSTYLE_SOLID);
    bottomAxis->SetFont(graphFont);
    leftAxis->SetFont(graphFont);
    bottomAxis->SetPen(axispen);
    leftAxis->SetPen(axispen);

    m_MathPlot->AddLayer(bottomAxis);
    m_MathPlot->AddLayer(leftAxis);
    mpTitle* plotTitle;
    m_MathPlot->AddLayer(plotTitle = new mpTitle(_("Image label tool for tikz-imagelabels package")));

    wxFont titleFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
    plotTitle->SetFont(titleFont);

    mpInfoCoords* info;
    m_MathPlot->AddLayer(info = new mpInfoCoords());
    info->SetVisible(true);

    mpInfoLegend* legend;
    m_MathPlot->AddLayer(legend = new mpInfoLegend());
    legend->SetItemDirection(mpHorizontal); // Note: Comment out this line to test mpVertical
    legend->SetVisible(true);

    m_MathPlot->Fit();
}

void ImageLabelGuiFrame::CleanPlot(void)
{
    m_MathPlot->DelAllPlot(true);
    bottomAxis->SetAlign(mpALIGN_CENTERX);
    bottomAxis->SetLogAxis(false);
    leftAxis->SetAlign(mpALIGN_CENTERY);
    leftAxis->SetLogAxis(false);
    bottomAxis->SetAuto(true);
    m_MathPlot->DelLayer(m_MathPlot->GetLayerByName(_T("BarChart")), true);
    m_MathPlot->DelLayer(m_MathPlot->GetLayerByClassName("mpBitmapLayer"), true);

    // Remove specific layers of type mpArrow and mpLabel
    // note the loop variable i and the CountAllLayers() will be changed if one layer get removed
    for (unsigned int i = 0; i < m_MathPlot->CountAllLayers(); /* no increment here */)
    {
        auto layer = m_MathPlot->GetLayer(i);
        mpArrow* arrowLayer = dynamic_cast<mpArrow*>(layer);
        mpLabel* labelLayer = dynamic_cast<mpLabel*>(layer);
        mpRegion* regionLayer = dynamic_cast<mpRegion*>(layer);
        if ((arrowLayer && arrowLayer->GetName() == "Arrow") ||
            (labelLayer && labelLayer->GetName() == "Label") ||
            (regionLayer && regionLayer->GetName() == "Region"))
        {
            m_MathPlot->DelLayer(layer, true);
        }
        else
        {
            i++; // Only increment if the current layer was not removed
        }
    }

    // The remembered annotation has just been removed as well
    m_pLastTouchedLayer = nullptr;
}

// Helper: check whether the given layer is still part of the plot. The pointer
// which was remembered by the last mouse action can be stale, for example when
// a new image was loaded or when Latex code was imported in the meantime.
static bool IsLayerInPlot(mpWindow* plotWindow, mpLayer* layer)
{
    for(unsigned int i = 0; i < plotWindow->CountAllLayers(); i++)
    {
        if(plotWindow->GetLayer(i) == layer)
            return true;
    }

    return false;
}

void ImageLabelGuiFrame::OnCharHook(wxKeyEvent& event)
{
    const int keyCode = event.GetKeyCode();

    if(keyCode == WXK_DELETE || keyCode == WXK_NUMPAD_DELETE)
    {
        // The log window is editable, so the Del key belongs to it while it has the focus
        wxWindow* focusedWindow = wxWindow::FindFocus();
        if(focusedWindow && dynamic_cast<wxTextCtrl*>(focusedWindow) != nullptr)
        {
            event.Skip();
            return;
        }

        // While a modal dialog is shown the main frame is disabled, the Del key
        // belongs to that dialog then
        if(IsEnabled() && TryDeleteLastTouchedAnnotation())
            return; // handled, the key must not reach the focused window
    }

    event.Skip();
}

bool ImageLabelGuiFrame::TryDeleteLastTouchedAnnotation(void)
{
    if(!m_pLastTouchedLayer)
        return false;

    // The annotation may already be gone, drop the stale pointer in that case
    if(!IsLayerInPlot(m_MathPlot, m_pLastTouchedLayer))
    {
        m_pLastTouchedLayer = nullptr;
        return false;
    }

    wxString typeName;
    wxString text;

    if(mpArrow* arrowLayer = dynamic_cast<mpArrow*>(m_pLastTouchedLayer))
    {
        typeName = "arrow";
        text = arrowLayer->GetLabel();
    }
    else if(mpLabel* labelLayer = dynamic_cast<mpLabel*>(m_pLastTouchedLayer))
    {
        typeName = "label";
        text = labelLayer->GetText();
    }
    else if(mpRegion* regionLayer = dynamic_cast<mpRegion*>(m_pLastTouchedLayer))
    {
        typeName = "region";
        text = regionLayer->GetText();
    }
    else
    {
        // Not one of our own layers, nothing to delete
        m_pLastTouchedLayer = nullptr;
        return false;
    }

    wxString message;
    if(text.IsEmpty())
    {
        message = wxString::Format("Do you want to delete the %s?", typeName);
    }
    else
    {
        message = wxString::Format("Do you want to delete the %s \"%s\"?", typeName, text);
    }

    const int answer = wxMessageBox(message,
                                    "Delete annotation",
                                    wxYES_NO | wxICON_QUESTION | wxNO_DEFAULT,
                                    this);

    // From here on the key is considered as handled, whatever the user has chosen
    if(answer != wxYES)
        return true;

    m_MathPlot->DelLayer(m_pLastTouchedLayer, true);
    m_pLastTouchedLayer = nullptr;
    m_MathPlot->Refresh();

    return true;
}

void ImageLabelGuiFrame::OnButtonGenerateLatexCodeClick(wxCommandEvent& event)
{
    wxStringOutputStream stringStream;
    wxTextOutputStream textStream(stringStream);

    // Write LaTeX code
    textStream << wxString::Format("\\begin{annotationimage}{width=0.7\\linewidth}{%s}\n", m_LoadedImageFilename);

    // Iterate over all layers to find the closest arrow
    for (unsigned int i = 0; i < m_MathPlot->CountAllLayers(); i++)
    {
        auto layer = m_MathPlot->GetLayer(i);
        mpArrow* arrowLayer = dynamic_cast<mpArrow*>(layer);
        if (arrowLayer && arrowLayer->GetName() == "Arrow")
        {
            // Start the LaTeX arrow generation
            wxRealPoint start = arrowLayer->GetStartPoint();
            wxRealPoint end = arrowLayer->GetEndPoint();
            wxString label = arrowLayer->GetLabel(); // Now wxString supports Unicode

            // Escape LaTeX special characters in the label
            label = EscapeLatex(label);

            // Calculate the annotation placement based on start position
            if (start.x < 0.0 && start.y > 0.0 && start.y < 1.0)
            {
                textStream << wxString::Format("    \\draw[annotation left = {%s at %.2f}] to (%.2f,%.2f);\n", label, start.y, end.x, end.y);
            }
            else if (start.x > 1.0 && start.y > 0.0 && start.y < 1.0)
            {
                textStream << wxString::Format("    \\draw[annotation right = {%s at %.2f}] to (%.2f,%.2f);\n", label, start.y, end.x, end.y);
            }
            else if (start.y < 0.0 && start.x > 0.0 && start.x < 1.0)
            {
                textStream << wxString::Format("    \\draw[annotation below = {%s at %.2f}] to (%.2f,%.2f);\n", label, start.x, end.x, end.y);
            }
            else
            {
                textStream << wxString::Format("    \\draw[annotation above = {%s at %.2f}] to (%.2f,%.2f);\n", label, start.x, end.x, end.y);
            }
        }
        else if (mpLabel* labelLayer = dynamic_cast<mpLabel*>(layer))
        {
            // A coordinate label can be placed at an arbitrary position of the image
            const wxRealPoint position = labelLayer->GetPosition();
            textStream << wxString::Format("    \\draw[coordinate label = {%s at (%.2f,%.2f)}];\n",
                                           EscapeLatex(labelLayer->GetText()), position.x, position.y);
        }
        else if (mpRegion* regionLayer = dynamic_cast<mpRegion*>(layer))
        {
            // A region is a rectangle with a text below it. It uses the user defined
            // "region label" key of the tikz-imagelabels package
            textStream << wxString::Format("    \\draw[region label = {%s at (%.2f,%.2f) to (%.2f,%.2f)}];\n",
                                           EscapeLatex(regionLayer->GetText()),
                                           regionLayer->GetLeft(), regionLayer->GetBottom(),
                                           regionLayer->GetRight(), regionLayer->GetTop());
        }
    }

    textStream << "\\end{annotationimage}\n";

    // Display the LaTeX code in the text control
    m_TextCtrlLog->SetValue(stringStream.GetString());
}

// Parse a coordinate pair like "(0.52,0.78)" or "0.52,0.78" into (x,y).
// The surrounding parentheses are optional, so both the old and the new
// LaTeX syntax can be read. Returns true on success.
static bool ParseCoordinatePair(const wxString& text, double& x, double& y)
{
    wxString trimmed = text;
    trimmed.Trim(true).Trim(false);

    // Strip the optional surrounding parentheses, e.g. "(0.52,0.78)"
    if(trimmed.StartsWith("(") && trimmed.EndsWith(")"))
    {
        trimmed = trimmed.Mid(1, trimmed.Length() - 2);
        trimmed.Trim(true).Trim(false);
    }

    int commaPos = trimmed.Find(',');
    if(commaPos == wxNOT_FOUND)
        return false;

    wxString xStr = trimmed.Mid(0, commaPos);
    wxString yStr = trimmed.Mid(commaPos + 1);
    xStr.Trim(true).Trim(false);
    yStr.Trim(true).Trim(false);

    if(xStr.IsEmpty() || yStr.IsEmpty())
        return false;

    if(!xStr.ToDouble(&x))
        return false;
    if(!yStr.ToDouble(&y))
        return false;
    return true;
}

// Helper: read the content between two markers, e.g. "...label at 0.12]..."
static wxString ExtractBetween(const wxString& text, const wxString& startMarker, const wxString& endMarker)
{
    int startPos = text.Find(startMarker);
    if(startPos == wxNOT_FOUND)
        return wxEmptyString;

    startPos += startMarker.Length();
    int endPos = text.Length();
    if(!endMarker.IsEmpty())
    {
        int foundEnd = text.Mid(startPos).Find(endMarker);
        if(foundEnd == wxNOT_FOUND)
            return wxEmptyString;
        endPos = startPos + foundEnd;
    }

    return text.Mid(startPos, endPos - startPos);
}

// Parse a single LaTeX line and create the corresponding arrow or label.
// Supported formats (matching the generator in OnButtonGenerateLatexCodeClick):
//   \\draw[annotation left  = {LABEL at POS}] to (X,Y);
//   \\draw[annotation right = {LABEL at POS}] to (X,Y);
//   \\draw[annotation below = {LABEL at POS}] to (X,Y);
//   \\draw[annotation above = {LABEL at POS}] to (X,Y);
//   \\draw[coordinate label = {TEXT at (X,Y)}];
//   \\draw[region label = {TEXT at (X1,Y1) to (X2,Y2)}];
static bool ParseLatexLine(const wxString& line, mpWindow* plotWindow)
{
    wxString trimmed = line;
    trimmed.Trim(true).Trim(false);

    if(!trimmed.StartsWith("\\draw["))
        return false;

    wxString content = ExtractBetween(trimmed, "\\draw[", "]");
    if(content.IsEmpty())
        return false;

    // Coordinate label: \draw[coordinate label = {text at (x,y)}];
    if(content.StartsWith("coordinate label"))
    {
        wxString arg = ExtractBetween(content, "= {", "}");
        if(arg.IsEmpty())
            return false;

        // arg format: "text at (x,y)"
        int atPos = arg.Find(" at ");
        if(atPos == wxNOT_FOUND)
            return false;

        wxString text = arg.Mid(0, atPos);
        text.Trim(true).Trim(false);

        wxString coords = ExtractBetween(arg, "(", ")");
        if(coords.IsEmpty())
            return false;

        double x = 0.0, y = 0.0;
        if(!ParseCoordinatePair(coords, x, y))
            return false;

        wxRealPoint position = ClampToImage(x, y);
        mpLabel* label = new mpLabel(position, UnescapeLatex(text));
        plotWindow->AddLayer(label, true);
        return true;
    }

    // Region label: \draw[region label = {text at (SW_x,SW_y) to (NE_x,NE_y)}];
    if(content.StartsWith("region label"))
    {
        wxString arg = ExtractBetween(content, "= {", "}");
        if(arg.IsEmpty())
            return false;

        // arg format: "text at (SW_x,SW_y) to (NE_x,NE_y)"
        const int atPos = arg.Find(" at ");
        const int toPos = arg.Find(" to ");
        if(atPos == wxNOT_FOUND || toPos == wxNOT_FOUND || toPos < atPos)
            return false;

        wxString text = arg.Mid(0, atPos);
        text.Trim(true).Trim(false);

        const wxString firstCornerStr = arg.Mid(atPos + 4, toPos - (atPos + 4));
        const wxString secondCornerStr = arg.Mid(toPos + 4);

        double firstX = 0.0, firstY = 0.0;
        double secondX = 0.0, secondY = 0.0;
        if(!ParseCoordinatePair(firstCornerStr, firstX, firstY))
            return false;
        if(!ParseCoordinatePair(secondCornerStr, secondX, secondY))
            return false;

        mpRegion* region = new mpRegion(ClampToImage(firstX, firstY),
                                        ClampToImage(secondX, secondY),
                                        UnescapeLatex(text));
        plotWindow->AddLayer(region, true);
        return true;
    }

    // Annotation arrows: annotation <direction> = {label at pos}
    wxString direction;
    if(content.StartsWith("annotation left"))
        direction = "left";
    else if(content.StartsWith("annotation right"))
        direction = "right";
    else if(content.StartsWith("annotation below"))
        direction = "below";
    else if(content.StartsWith("annotation above"))
        direction = "above";
    else
        return false;

    wxString arg = ExtractBetween(content, "= {", "}");
    if(arg.IsEmpty())
        return false;

    // arg format: "label at pos"
    int atPos = arg.Find(" at ");
    if(atPos == wxNOT_FOUND)
        return false;

    wxString labelText = arg.Mid(0, atPos);
    labelText.Trim(true).Trim(false);

    double pos = 0.0;
    {
        wxString posStr = arg.Mid(atPos + 4);
        posStr.Trim(true).Trim(false);
        if(!posStr.ToDouble(&pos))
            return false;
    }

    // Extract "to (x,y)" from the remaining part of the line
    wxString toPart = ExtractBetween(trimmed, "] to (", ");");
    if(toPart.IsEmpty())
        return false;

    double endX = 0.0, endY = 0.0;
    if(!ParseCoordinatePair(toPart, endX, endY))
        return false;

    // Reconstruct the start point from the annotation direction
    const double outsideOffset = 0.15; // Same visual offset used by the generator
    wxRealPoint start, end(endX, endY);
    if(direction == "left")
    {
        start = wxRealPoint(-outsideOffset, pos);
    }
    else if(direction == "right")
    {
        start = wxRealPoint(1.0 + outsideOffset, pos);
    }
    else if(direction == "below")
    {
        start = wxRealPoint(pos, -outsideOffset);
    }
    else // above
    {
        start = wxRealPoint(pos, 1.0 + outsideOffset);
    }

    mpArrow* arrow = new mpArrow(start, end, UnescapeLatex(labelText));
    plotWindow->AddLayer(arrow, true);
    return true;
}

void ImageLabelGuiFrame::OnButtonImportLatexCodeClick(wxCommandEvent& event)
{
    // Build a small dialog with a multiline text control so the user can paste
    // the generated (or hand-written) LaTeX code.
    wxDialog dialog(this, wxID_ANY, _("Import LaTeX annotations"),
                    wxDefaultPosition, wxSize(500, 400),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    wxTextCtrl* textCtrl = new wxTextCtrl(&dialog, wxID_ANY, wxEmptyString,
                                          wxDefaultPosition, wxDefaultSize,
                                          wxTE_MULTILINE | wxTE_DONTWRAP);
    textCtrl->SetMinSize(wxSize(480, 300));
    sizer->Add(textCtrl, 1, wxALL | wxEXPAND, 5);

    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxButton(&dialog, wxID_OK, _("Import")), 0, wxALL, 5);
    buttonSizer->Add(new wxButton(&dialog, wxID_CANCEL, _("Cancel")), 0, wxALL, 5);
    sizer->Add(buttonSizer, 0, wxALIGN_CENTER_HORIZONTAL, 5);

    dialog.SetSizerAndFit(sizer);

    if(dialog.ShowModal() != wxID_OK)
        return;

    wxString latexCode = textCtrl->GetValue();
    if(latexCode.IsEmpty())
        return;

    // Optional: try to extract the image filename from
    // \begin{annotationimage}{...}{filename}
    wxString filename = ExtractBetween(latexCode, "}{", "}");
    if(!filename.IsEmpty() && !filename.Contains("\\"))
    {
        wxFileName fileName(filename);
        wxString fullPath = fileName.GetFullPath();
        if(!fullPath.IsEmpty() && wxFileName::Exists(fullPath))
        {
            LoadImage(fullPath);
        }
    }

    // Remove existing arrows and labels but keep the bitmap/axes/title/legend
    for(unsigned int i = 0; i < m_MathPlot->CountAllLayers(); /* no increment */)
    {
        auto layer = m_MathPlot->GetLayer(i);
        mpArrow* arrowLayer = dynamic_cast<mpArrow*>(layer);
        mpLabel* labelLayer = dynamic_cast<mpLabel*>(layer);
        mpRegion* regionLayer = dynamic_cast<mpRegion*>(layer);
        if((arrowLayer && arrowLayer->GetName() == "Arrow") ||
           (labelLayer && labelLayer->GetName() == "Label") ||
           (regionLayer && regionLayer->GetName() == "Region"))
        {
            m_MathPlot->DelLayer(layer, true);
        }
        else
        {
            ++i;
        }
    }

    // The remembered annotation has just been removed as well
    m_pLastTouchedLayer = nullptr;

    // Parse each line independently
    wxStringInputStream inputStream(latexCode);
    wxTextInputStream textStream(inputStream);
    wxString line;
    size_t importedCount = 0;
    size_t failedCount = 0;

    while(!inputStream.Eof())
    {
        line = textStream.ReadLine();
        if(line.IsEmpty())
            continue;

        if(ParseLatexLine(line, m_MathPlot))
            ++importedCount;
        else if(line.Contains("\\draw["))
            ++failedCount;
    }

    m_MathPlot->Refresh();

    wxString message = wxString::Format("Imported %lu annotation(s).", static_cast<unsigned long>(importedCount));
    if(failedCount > 0)
        message += wxString::Format(" %lu line(s) could not be parsed.", static_cast<unsigned long>(failedCount));
    wxMessageBox(message, _("Import result"), wxOK | wxICON_INFORMATION, this);
}

    // Add your image loading function here
void ImageLabelGuiFrame::LoadImage(const wxString& filePath)
{
    // Code to load and display the image
    // wxMessageBox("Loading image: " + filePath, "Load Image", wxOK | wxICON_INFORMATION, this);

    // Use wxFileName to extract the file name
    wxFileName fileName(filePath);
    wxString name = fileName.GetFullName();

    wxImage image;
    if(!image.LoadFile(filePath))
    {
        wxLogError("Failed to load image: %s", filePath);
        return;
    }

    // Store the image resolution and filename in the member variables
    m_LoadedImageWidth = image.GetWidth();
    m_LoadedImageHeight = image.GetHeight();
    m_LoadedImageFilename = name;

    CleanPlot(); // Remove any existing layers

    // Create a bitmap layer
    mpBitmapLayer* bitmapLayer = new mpBitmapLayer();
    bitmapLayer->SetBitmap(image, 0.0, 0.0, 1.0, 1.0);

    // Add bitmap layer to the plot
    m_MathPlot->AddLayer(bitmapLayer);

    // Update the plot
    m_MathPlot->Fit();
    m_MathPlot->Refresh();
}

void ImageLabelGuiFrame::OnCheckBoxDrawLabelClick(wxCommandEvent& event)
{
    if(m_CheckBoxDrawLabel->GetValue())
    {
        // mpWindow has a single user mouse action callback, so only one drawing mode can be active
        m_CheckBoxDrawArrow->SetValue(false);
        m_CheckBoxDrawRegion->SetValue(false);

        m_MathPlot->SetOnUserMouseAction([this](void* Sender, wxMouseEvent & event, bool & cancel)
        {
            OnUserMouseActionDrawLabel(Sender, event, cancel);
        });
    }
    else
        m_MathPlot->UnSetOnUserMouseAction();
}

void ImageLabelGuiFrame::OnUserMouseActionDrawLabel(void* Sender, wxMouseEvent& event, bool& cancel)
{
    static wxOverlay m_overlay;
    static wxPoint pressedPointScreen; // Screen position where the left button was pressed
    static wxPoint dragOffset;         // Offset between the mouse and the label which is moved
    static bool isPlacingLabel = false;
    static bool isMovingLabel = false;
    static mpLabel* selectedLabel = nullptr;

    const wxPoint mousePosition = event.GetPosition();
    mpWindow* plotWindow = (mpWindow*)Sender;   // Cast Sender to mpWindow

    cancel = true;

    // Left mouse button down: remember the position, the label itself is added on release
    if(event.LeftDown())
    {
        isPlacingLabel = true;
        pressedPointScreen = mousePosition;
    }
    // Right mouse button down: start moving the label which is closest to the mouse
    else if(event.RightDown())
    {
        selectedLabel = FindClosestLabelLayer(plotWindow, mousePosition);

        if(selectedLabel)
        {
            const wxRealPoint position = selectedLabel->GetPosition();
            const wxPoint labelPointScreen(plotWindow->x2p(position.x), plotWindow->y2p(position.y));

            dragOffset = mousePosition - labelPointScreen;
            isMovingLabel = true;
        }
    }
    // Left mouse button released: a simple click adds a new label, a dragged mouse is ignored
    else if(event.LeftUp() && isPlacingLabel)
    {
        isPlacingLabel = false;

        if(DistanceBetweenPoints(pressedPointScreen, mousePosition) < 5)
        {
            AddLabelAtScreenPosition(plotWindow, mousePosition);
        }
    }
    // Right mouse button released: keep the new position of the moved label
    else if(event.RightUp() && isMovingLabel)
    {
        isMovingLabel = false;
        m_overlay.Reset();

        if(selectedLabel)
        {
            plotWindow->Refresh();

            // Remember the label, it can be removed again with the Del key
            m_pLastTouchedLayer = selectedLabel;

            selectedLabel = nullptr;
        }
    }
    // Mouse dragging with the right button: move the selected label
    else if(event.Dragging() && event.RightIsDown() && isMovingLabel && selectedLabel)
    {
        const wxPoint newPointScreen = mousePosition - dragOffset;

        // The label always stays inside the image
        selectedLabel->SetPosition(ClampToImage(plotWindow->p2x(newPointScreen.x),
                                                plotWindow->p2y(newPointScreen.y)));

        // Draw a preview rectangle at the new position
        wxClientDC dc(plotWindow);
        PrepareDC(dc);
        wxDCOverlay overlay(m_overlay, &dc);
        overlay.Clear();

        dc.SetPen(wxPen(*wxBLUE, 2));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(newPointScreen.x - 12, newPointScreen.y - 8, 24, 16);
    }
    // Right mouse double-click: edit the text of the selected label
    else if(event.RightDClick())
    {
        selectedLabel = FindClosestLabelLayer(plotWindow, mousePosition);

        if(selectedLabel)
        {
            wxTextEntryDialog textDialog(plotWindow,
                                         "Edit the text of the label:",
                                         "Label Text Editor",
                                         selectedLabel->GetText());

            if(textDialog.ShowModal() == wxID_OK && !textDialog.GetValue().IsEmpty())
            {
                selectedLabel->SetText(textDialog.GetValue());
                plotWindow->Refresh();
            }

            // Remember the label, it can be removed again with the Del key
            m_pLastTouchedLayer = selectedLabel;
        }

        selectedLabel = nullptr;
    }
}

void ImageLabelGuiFrame::AddLabelAtScreenPosition(mpWindow* plotWindow, const wxPoint& mouseScreenPosition)
{
    // Ask for the text of the label before creating it
    wxTextEntryDialog textDialog(this,
                                 "Enter the text of the label:",
                                 "Label Text",
                                 "Label");

    if(textDialog.ShowModal() != wxID_OK)
        return; // the user cancelled the dialog

    const wxString text = textDialog.GetValue();
    if(text.IsEmpty())
        return;

    // p2x() and p2y() return the coordinates of the plot, which are the normalized
    // image coordinates used by the annotationimage environment
    const wxRealPoint position = ClampToImage(plotWindow->p2x(mouseScreenPosition.x),
                                              plotWindow->p2y(mouseScreenPosition.y));

    mpLabel* label = new mpLabel(position, text);
    plotWindow->AddLayer(label, true);
    plotWindow->Refresh();
}

mpLabel* ImageLabelGuiFrame::FindClosestLabelLayer(mpWindow* plotWindow, const wxPoint& mouseScreenPosition)
{
    mpLabel* closestLabel = nullptr;
    double minDistance = std::numeric_limits<double>::max();

    // Iterate over all layers to find the closest label
    for(unsigned int i = 0; i < plotWindow->CountAllLayers(); i++)
    {
        auto layer = plotWindow->GetLayer(i);
        mpLabel* labelLayer = dynamic_cast<mpLabel*>(layer);
        if(labelLayer && labelLayer->GetName() == "Label")
        {
            const wxRealPoint position = labelLayer->GetPosition();
            const wxPoint labelPointScreen(plotWindow->x2p(position.x), plotWindow->y2p(position.y));

            // The distance is measured in screen pixels, such that it does not depend on the zoom level
            const double distance = DistanceBetweenPoints(mouseScreenPosition, labelPointScreen);
            if(distance < minDistance)
            {
                minDistance = distance;
                closestLabel = labelLayer;
            }
        }
    }

    // Return the closest label layer if it is close enough to the mouse
    return (minDistance < 10.0) ? closestLabel : nullptr;
}

void ImageLabelGuiFrame::OnUserMouseActionDrawRegion(void* Sender, wxMouseEvent& event, bool& cancel)
{
    static wxOverlay overlay;
    static wxPoint startPointScreen;   // Screen position where the left button was pressed
    static wxPoint currentPointScreen; // Current screen position while dragging
    static bool isDrawingRegion = false;
    static bool isModifyingRegion = false;
    static bool isMoving = false;
    static bool isResizing = false;
    static int resizingCorner = -1; // 0 = bottom left, 1 = bottom right, 2 = top left, 3 = top right
    static double grabGraphX = 0.0, grabGraphY = 0.0; // Graph coordinates where the right button was pressed
    static double originalLeft = 0.0, originalRight = 0.0, originalBottom = 0.0, originalTop = 0.0;
    static mpRegion* selectedRegion = nullptr;

    const wxPoint mousePosition = event.GetPosition();
    mpWindow* plotWindow = (mpWindow*)Sender; // Cast Sender to mpWindow

    cancel = true;

    // Left mouse button down: start drawing a new region
    if(event.LeftDown() && !isModifyingRegion)
    {
        isDrawingRegion = true;
        startPointScreen = mousePosition;
    }
    // Right mouse button down: start modifying an existing region
    else if(event.RightDown())
    {
        // Attempt to find the closest region layer to the mouse position
        selectedRegion = FindClosestRegionLayer(plotWindow, mousePosition);

        if(selectedRegion)
        {
            originalLeft = selectedRegion->GetLeft();
            originalRight = selectedRegion->GetRight();
            originalBottom = selectedRegion->GetBottom();
            originalTop = selectedRegion->GetTop();

            grabGraphX = plotWindow->p2x(mousePosition.x);
            grabGraphY = plotWindow->p2y(mousePosition.y);

            // Screen positions of the four corners of the region
            const wxPoint corners[4] =
            {
                wxPoint(plotWindow->x2p(originalLeft),  plotWindow->y2p(originalBottom)), // 0 bottom left
                wxPoint(plotWindow->x2p(originalRight), plotWindow->y2p(originalBottom)), // 1 bottom right
                wxPoint(plotWindow->x2p(originalLeft),  plotWindow->y2p(originalTop)),    // 2 top left
                wxPoint(plotWindow->x2p(originalRight), plotWindow->y2p(originalTop))     // 3 top right
            };

            // Determine if the mouse is near one of the corners
            resizingCorner = -1;
            double minDistance = 10.0;
            for(int i = 0; i < 4; i++)
            {
                const double distance = DistanceBetweenPoints(mousePosition, corners[i]);
                if(distance < minDistance)
                {
                    minDistance = distance;
                    resizingCorner = i;
                }
            }

            if(resizingCorner >= 0)
            {
                isResizing = true;
                isModifyingRegion = true;
            }
            else
            {
                // Otherwise the whole region is moved when the mouse is inside it
                wxRect regionScreenRect(corners[2], corners[1]);
                regionScreenRect.Inflate(5);
                if(regionScreenRect.Contains(mousePosition))
                {
                    isMoving = true;
                    isModifyingRegion = true;
                }
                else
                {
                    selectedRegion = nullptr;
                }
            }
        }
    }
    // Left mouse button released: finalize a new region
    else if(event.LeftUp() && isDrawingRegion)
    {
        isDrawingRegion = false;
        overlay.Reset();

        currentPointScreen = mousePosition;

        const double startX = plotWindow->p2x(startPointScreen.x);
        const double startY = plotWindow->p2y(startPointScreen.y);
        const double endX = plotWindow->p2x(currentPointScreen.x);
        const double endY = plotWindow->p2y(currentPointScreen.y);

        const double minimumSizeThreshold = 0.02;

        if(std::abs(endX - startX) < minimumSizeThreshold || std::abs(endY - startY) < minimumSizeThreshold)
        {
            // If the region is too small, skip creating it
            wxMessageBox(
                "The region is too small to be created. Please try again.",
                "Region Too Small",
                wxOK | wxICON_WARNING,
                this
            );
            return;
        }

        // Open a text entry dialog to get the text of the region
        wxTextEntryDialog textDialog(
            this,
            "Enter the text of the region:",
            "Region Text",
            "Region"
        );

        wxString text = "Region"; // Default text
        if(textDialog.ShowModal() == wxID_OK)
        {
            text = textDialog.GetValue(); // Get the user-entered text
        }

        // Create and add a permanent region layer with the text
        mpRegion* region = new mpRegion(ClampToImage(startX, startY),
                                        ClampToImage(endX, endY),
                                        text);
        plotWindow->AddLayer(region, true);
        plotWindow->Refresh();
    }
    // Mouse dragging with the left button: draw a new region
    else if(event.Dragging() && event.LeftIsDown() && isDrawingRegion)
    {
        currentPointScreen = mousePosition;

        // Draw the rectangle dynamically on the overlay
        wxClientDC dc(plotWindow);
        PrepareDC(dc);
        wxDCOverlay dcOverlay(overlay, &dc);
        dcOverlay.Clear();

        dc.SetPen(wxPen(*wxBLACK, 2));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        // Normalize the rectangle, the user may drag in any direction
        const wxCoord rectLeft = wxMin(startPointScreen.x, currentPointScreen.x);
        const wxCoord rectTop = wxMin(startPointScreen.y, currentPointScreen.y);
        const wxCoord rectRight = wxMax(startPointScreen.x, currentPointScreen.x);
        const wxCoord rectBottom = wxMax(startPointScreen.y, currentPointScreen.y);

        dc.DrawRectangle(rectLeft, rectTop, rectRight - rectLeft, rectBottom - rectTop);
    }
    // Mouse dragging with the right button: move or resize the selected region
    else if(event.Dragging() && event.RightIsDown() && isModifyingRegion && selectedRegion)
    {
        double left = originalLeft;
        double right = originalRight;
        double bottom = originalBottom;
        double top = originalTop;

        if(isResizing)
        {
            // The dragged corner follows the mouse, the opposite corner stays fixed
            const wxRealPoint corner = ClampToImage(plotWindow->p2x(mousePosition.x),
                                                    plotWindow->p2y(mousePosition.y));

            switch(resizingCorner)
            {
                case 0: left = corner.x;  bottom = corner.y; break; // bottom left
                case 1: right = corner.x; bottom = corner.y; break; // bottom right
                case 2: left = corner.x;  top = corner.y;    break; // top left
                default: right = corner.x; top = corner.y;   break; // top right
            }
        }
        else if(isMoving)
        {
            // Move the whole region, but keep it inside the image
            const double deltaX = plotWindow->p2x(mousePosition.x) - grabGraphX;
            const double deltaY = plotWindow->p2y(mousePosition.y) - grabGraphY;

            left = originalLeft + deltaX;
            right = originalRight + deltaX;
            bottom = originalBottom + deltaY;
            top = originalTop + deltaY;

            if(left < 0.0)
            {
                right -= left;
                left = 0.0;
            }
            if(right > 1.0)
            {
                left -= right - 1.0;
                right = 1.0;
            }
            if(bottom < 0.0)
            {
                top -= bottom;
                bottom = 0.0;
            }
            if(top > 1.0)
            {
                bottom -= top - 1.0;
                top = 1.0;
            }
        }

        // The corners are normalized here, so dragging a corner across the opposite one just flips the region
        selectedRegion->SetFirstCorner(wxRealPoint(wxMin(left, right), wxMin(bottom, top)));
        selectedRegion->SetSecondCorner(wxRealPoint(wxMax(left, right), wxMax(bottom, top)));
        plotWindow->Refresh();
    }
    // Right mouse button released: keep the new position of the region
    else if(event.RightUp() && isModifyingRegion)
    {
        isModifyingRegion = false;
        isMoving = false;
        isResizing = false;
        resizingCorner = -1;

        if(selectedRegion)
        {
            plotWindow->Refresh();

            // Remember the region, it can be removed again with the Del key
            m_pLastTouchedLayer = selectedRegion;

            selectedRegion = nullptr;
        }
    }
    // Right mouse double-click: edit the text of the selected region
    else if(event.RightDClick())
    {
        selectedRegion = FindClosestRegionLayer(plotWindow, mousePosition);

        if(selectedRegion)
        {
            wxTextEntryDialog textDialog(plotWindow,
                                         "Edit the text of the region:",
                                         "Region Text Editor",
                                         selectedRegion->GetText());

            if(textDialog.ShowModal() == wxID_OK)
            {
                selectedRegion->SetText(textDialog.GetValue());
                plotWindow->Refresh();
            }

            // Remember the region, it can be removed again with the Del key
            m_pLastTouchedLayer = selectedRegion;
        }

        selectedRegion = nullptr;
    }
}

mpRegion* ImageLabelGuiFrame::FindClosestRegionLayer(mpWindow* plotWindow, const wxPoint& mouseScreenPosition)
{
    mpRegion* closestRegion = nullptr;
    double minDistance = std::numeric_limits<double>::max();

    // Iterate over all layers to find the closest region
    for(unsigned int i = 0; i < plotWindow->CountAllLayers(); i++)
    {
        auto layer = plotWindow->GetLayer(i);
        mpRegion* regionLayer = dynamic_cast<mpRegion*>(layer);
        if(regionLayer && regionLayer->GetName() == "Region")
        {
            const wxPoint topLeft(plotWindow->x2p(regionLayer->GetLeft()),
                                  plotWindow->y2p(regionLayer->GetTop()));
            const wxPoint bottomRight(plotWindow->x2p(regionLayer->GetRight()),
                                      plotWindow->y2p(regionLayer->GetBottom()));

            // The distance is measured in screen pixels, such that it does not depend on the zoom level
            const double distance = DistanceToRectangle(mouseScreenPosition,
                                                        wxRect(topLeft, bottomRight));
            if(distance < minDistance)
            {
                minDistance = distance;
                closestRegion = regionLayer;
            }
        }
    }

    // Return the closest region layer if it is close enough to the mouse
    return (minDistance < 10.0) ? closestRegion : nullptr;
}

void ImageLabelGuiFrame::OnCheckBoxDrawRegionClick(wxCommandEvent& event)
{
    if(m_CheckBoxDrawRegion->GetValue())
    {
        // mpWindow has a single user mouse action callback, so only one drawing mode can be active
        m_CheckBoxDrawArrow->SetValue(false);
        m_CheckBoxDrawLabel->SetValue(false);

        m_MathPlot->SetOnUserMouseAction([this](void* Sender, wxMouseEvent & event, bool & cancel)
        {
            OnUserMouseActionDrawRegion(Sender, event, cancel);
        });
    }
    else
        m_MathPlot->UnSetOnUserMouseAction();
}
