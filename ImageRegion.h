#ifndef IMAGEREGION_H_INCLUDED
#define IMAGEREGION_H_INCLUDED

#include "mathplot.h"

#include <cmath>

// Distance from a point to a screen rectangle. The distance is 0 for points
// inside the rectangle, which makes it easy to test "is the mouse on or near
// the region".
inline double DistanceToRectangle(const wxPoint& point, const wxRect& rect)
{
    const int dx = wxMax(wxMax(rect.GetLeft() - point.x, 0), point.x - rect.GetRight());
    const int dy = wxMax(wxMax(rect.GetTop() - point.y, 0), point.y - rect.GetBottom());
    return std::sqrt(static_cast<double>(dx) * dx + static_cast<double>(dy) * dy);
}

/**
 * A rectangular region annotation. It is exported as a "region label" of the
 * tikz-imagelabels package (a user defined extension, see the \imagelabelset
 * definition in the LaTeX preamble):
 *
 *     \draw[region label = {Text at (0.20,0.10) to (0.50,0.60)}];
 *
 * The two corners are stored in normalized image coordinates, i.e. (0,0) is the
 * bottom left corner and (1,1) is the top right corner of the loaded image.
 * The text is drawn centered below the rectangle, which corresponds to the
 * "anchor = north" placement at the south anchor of the fitted node.
 *
 * The corners are normalized when they are read, so it does not matter whether
 * the user dragged from the bottom left to the top right corner or the other
 * way round.
 */
class mpRegion : public mpLayer
{
public:
    mpRegion(const wxRealPoint& firstCorner, const wxRealPoint& secondCorner, const wxString& text)
        : mpLayer(mpLAYER_TEXT), m_firstCorner(firstCorner), m_secondCorner(secondCorner), m_text(text)
    {
        SetName("Region");
    }

    /// The region does not change the bounding box computed by mpWindow::Fit()
    virtual bool HasBBox() override
    {
        return false;
    }

    virtual void DoPlot(wxDC& dc, mpWindow& w) override
    {
        const wxCoord x1 = w.x2p(GetLeft());
        const wxCoord x2 = w.x2p(GetRight());
        const wxCoord yTop = w.y2p(GetTop());
        const wxCoord yBottom = w.y2p(GetBottom());

        // The DC is shared with all the other layers, so its state has to be restored
        const wxFont oldFont = dc.GetFont();
        const wxColour oldTextColour = dc.GetTextForeground();
        const wxPen oldPen = dc.GetPen();
        const wxBrush oldBrush = dc.GetBrush();

        // The rectangle itself
        dc.SetPen(wxPen(*wxBLACK, 2));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawRectangle(x1, yTop, x2 - x1, yBottom - yTop);

        // The text below the rectangle, centered horizontally
        dc.SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

        wxCoord textWidth = 0;
        wxCoord textHeight = 0;
        if(!m_text.IsEmpty())
        {
            dc.GetTextExtent(m_text, &textWidth, &textHeight);
        }

        dc.SetTextForeground(*wxBLACK);
        dc.DrawText(m_text, (x1 + x2) / 2 - textWidth / 2, yBottom + 3);

        dc.SetFont(oldFont);
        dc.SetTextForeground(oldTextColour);
        dc.SetPen(oldPen);
        dc.SetBrush(oldBrush);
    }

    // Normalized bounds, always ordered so that GetLeft() <= GetRight() and
    // GetBottom() <= GetTop(), independent of the corner order.
    double GetLeft() const
    {
        return wxMin(m_firstCorner.x, m_secondCorner.x);
    }

    double GetRight() const
    {
        return wxMax(m_firstCorner.x, m_secondCorner.x);
    }

    double GetBottom() const
    {
        return wxMin(m_firstCorner.y, m_secondCorner.y);
    }

    double GetTop() const
    {
        return wxMax(m_firstCorner.y, m_secondCorner.y);
    }

    // Interface functions to modify the corners and the text of the region
    wxRealPoint GetFirstCorner() const
    {
        return m_firstCorner;
    }

    wxRealPoint GetSecondCorner() const
    {
        return m_secondCorner;
    }

    wxString GetText() const
    {
        return m_text;
    }

    void SetFirstCorner(const wxRealPoint& corner)
    {
        m_firstCorner = corner;
    }

    void SetSecondCorner(const wxRealPoint& corner)
    {
        m_secondCorner = corner;
    }

    void SetText(const wxString& text)
    {
        m_text = text;
    }

private:
    wxRealPoint m_firstCorner;  // One corner in normalized image coordinates
    wxRealPoint m_secondCorner; // The opposite corner in normalized image coordinates
    wxString m_text;            // Text which is drawn below the rectangle
};

#endif // IMAGEREGION_H_INCLUDED
