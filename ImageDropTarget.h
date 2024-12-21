#ifndef IMAGEDROPTARGET_H_INCLUDED
#define IMAGEDROPTARGET_H_INCLUDED

#include <wx/dnd.h>
#include <wx/image.h>
#include <wx/msgdlg.h>

#include "ImageLabelGuiMain.h" // Include the header file for MainFrame

class ImageDropTarget : public wxFileDropTarget
{
public:
    explicit ImageDropTarget(ImageLabelGuiFrame* frame) : m_frame(frame) {}

    bool OnDropFiles(wxCoord x, wxCoord y, const wxArrayString& filenames) override
    {
        if (filenames.GetCount() > 0)
        {
            wxString filePath = filenames[0];
            if (IsImageFile(filePath))
            {
                // Simulate the open image action
                wxMessageBox("Image opened: " + filePath, "Open Image", wxOK | wxICON_INFORMATION, m_frame);
                // Here, you would call your image loading function instead
                m_frame->LoadImage(filePath);
            }
            else
            {
                wxMessageBox("The dropped file is not a valid image.", "Error", wxOK | wxICON_ERROR, m_frame);
            }
        }
        return true;
    }

private:
    ImageLabelGuiFrame* m_frame;

    bool IsImageFile(const wxString& filePath)
    {
        // Check for common image file extensions
        wxString ext = filePath.AfterLast('.').Lower();
        return ext == "jpg" || ext == "jpeg" || ext == "png" || ext == "bmp" || ext == "gif" || ext == "tiff";
    }
};




#endif // IMAGEDROPTARGET_H_INCLUDED
