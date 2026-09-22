#ifndef IMAGELABEL_LAYER_H_INCLUDED
#define IMAGELABEL_LAYER_H_INCLUDED

#include "mathplot.h"

/**
 * A label which can be placed at an arbitrary point of the image. It is exported as a
 * "coordinate label" of the tikz-imagelabels package:
 *
 *     \draw[coordinate label = {text at (0.32,0.45)}];
 *
 * The position is stored in normalized image coordinates, i.e. (0,0) is the bottom left
 * corner and (1,1) is the top right corner of the loaded image. Those are exactly the
 * coordinates used inside the "annotationimage" environment of the package, see the
 * scope definition in tikz-imagelabels.dtx:
 *
 *     \begin{scope}[x={(image.south east)},y={(image.north west)}]
 */
class mpLabel : public mpLayer
{
public:
    mpLabel(const wxRealPoint& position, const wxString& text)
        : mpLayer(mpLAYER_TEXT), m_position(position), m_text(text)
    {
        SetName("Label");
    }

    /// The label is not part of the plotted data, so it must not influence the bounding
    /// box which mpWindow::Fit() computes from all the layers.
    virtual bool HasBBox() override
    {
        return false;
    }

    /// Draw a preview of the label. The package draws the label as a black box with white
    /// text, centered on the given coordinates, see "coordinate label style" in the dtx file.
    virtual void DoPlot(wxDC& dc, mpWindow& w) override
    {
        const wxCoord x = w.x2p(m_position.x);
        const wxCoord y = w.y2p(m_position.y);

        // The DC is shared with all the other layers, so its state has to be restored
        const wxFont oldFont = dc.GetFont();
        const wxColour oldTextColour = dc.GetTextForeground();
        const wxPen oldPen = dc.GetPen();
        const wxBrush oldBrush = dc.GetBrush();

        // roughly "font = \sffamily\bfseries\scriptsize" of the package
        dc.SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

        wxCoord textWidth = 0;
        wxCoord textHeight = 0;
        if(!m_text.IsEmpty())
        {
            dc.GetTextExtent(m_text, &textWidth, &textHeight);
        }

        // the package style uses "minimum width/height = 3mm"
        const wxCoord minSide = 12;
        const wxCoord boxWidth = wxMax(textWidth + 6, minSide);
        const wxCoord boxHeight = wxMax(textHeight + 4, minSide);

        // the label is anchored at its center, like "anchor=center" in the package style
        const wxRect box(x - boxWidth / 2, y - boxHeight / 2, boxWidth, boxHeight);

        dc.SetPen(*wxTRANSPARENT_PEN);
        dc.SetBrush(wxBrush(*wxBLACK));
        dc.DrawRectangle(box);

        dc.SetTextForeground(*wxWHITE);
        dc.DrawText(m_text, x - textWidth / 2, y - textHeight / 2);

        dc.SetFont(oldFont);
        dc.SetTextForeground(oldTextColour);
        dc.SetPen(oldPen);
        dc.SetBrush(oldBrush);
    }

    // Interface functions to modify the position and the text of the label
    wxRealPoint GetPosition() const
    {
        return m_position;
    }

    void SetPosition(const wxRealPoint& position)
    {
        m_position = position;
    }

    wxString GetText() const
    {
        return m_text;
    }

    void SetText(const wxString& text)
    {
        m_text = text;
    }

private:
    wxRealPoint m_position; // Position in normalized image coordinates
    wxString m_text;        // Text of the label
};

#endif // IMAGELABEL_LAYER_H_INCLUDED
