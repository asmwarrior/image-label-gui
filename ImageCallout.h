#ifndef IMAGECALLOUT_H_INCLUDED
#define IMAGECALLOUT_H_INCLUDED

#include "mathplot.h"

#include <cmath>

/**
 * A rectangular region inside of the image together with a text and a leader
 * line which points from the text to the rectangle. It is exported as a
 * "region callout" of the tikz-imagelabels package (a user defined extension,
 * see the \imagelabelset definition in the LaTeX preamble):
 *
 *     \draw[region callout left = {Text at 0.85 to (0.35,0.70) to (0.55,0.90)}];
 *
 * It combines the behaviour of an "annotation" (the text sits outside of the
 * image, on one of its four borders) and of a "region label" (a rectangle
 * inside of the image).
 *
 * The anchor is the position of the text in normalized image coordinates. The
 * coordinate which lies outside of [0,1] decides the border, exactly like it is
 * done for the arrows:
 *
 *     anchor.x < 0  ->  left    (the value of anchor.y is written to the code)
 *     anchor.x > 1  ->  right   (the value of anchor.y is written to the code)
 *     anchor.y < 0  ->  below   (the value of anchor.x is written to the code)
 *     otherwise     ->  above   (the value of anchor.x is written to the code)
 *
 * The two corners are normalized when they are read, so it does not matter in
 * which direction the user has dragged the rectangle.
 */
class mpRegionCallout : public mpLayer
{
public:
    mpRegionCallout(const wxRealPoint& anchor, const wxRealPoint& firstCorner,
                    const wxRealPoint& secondCorner, const wxString& text)
        : mpLayer(mpLAYER_TEXT), m_anchor(anchor), m_firstCorner(firstCorner),
          m_secondCorner(secondCorner), m_text(text)
    {
        SetName("Callout");
    }

    /// The callout does not change the bounding box computed by mpWindow::Fit()
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

        const wxCoord anchorX = w.x2p(m_anchor.x);
        const wxCoord anchorY = w.y2p(m_anchor.y);

        // The point of the rectangle which the leader line points at
        const wxRealPoint connector = GetConnectorPoint();
        const wxCoord connectorX = w.x2p(connector.x);
        const wxCoord connectorY = w.y2p(connector.y);

        // The DC is shared with all the other layers, so its state has to be restored
        const wxFont oldFont = dc.GetFont();
        const wxColour oldTextColour = dc.GetTextForeground();
        const wxPen oldPen = dc.GetPen();
        const wxBrush oldBrush = dc.GetBrush();

        dc.SetPen(wxPen(*wxBLACK, 2));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);

        // The leader line from the text to the rectangle
        dc.DrawLine(anchorX, anchorY, connectorX, connectorY);

        // The rectangle itself
        dc.DrawRectangle(x1, yTop, x2 - x1, yBottom - yTop);

        // The text at the anchor. It is aligned so that it stays outside of the
        // image, on the border which the anchor belongs to.
        dc.SetFont(wxFont(8, wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

        wxCoord textWidth = 0;
        wxCoord textHeight = 0;
        if(!m_text.IsEmpty())
        {
            dc.GetTextExtent(m_text, &textWidth, &textHeight);
        }

        wxCoord textX = anchorX;
        if(m_anchor.x < 0.0)       // left border, the text ends at the anchor
            textX = anchorX - textWidth;
        else if(m_anchor.x > 1.0)  // right border, the text starts at the anchor
            textX = anchorX;
        else                       // above or below, the text is centred
            textX = anchorX - textWidth / 2;

        dc.SetTextForeground(*wxBLACK);
        dc.DrawText(m_text, textX, anchorY);

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

    /// The point where the leader line meets the rectangle. It is the point of
    /// the border which is closest to the anchor, which looks natural for all
    /// four borders.
    wxRealPoint GetConnectorPoint() const
    {
        const double left = GetLeft();
        const double right = GetRight();
        const double bottom = GetBottom();
        const double top = GetTop();

        if(m_anchor.x < left)
            return wxRealPoint(left, wxMin(wxMax(m_anchor.y, bottom), top));

        if(m_anchor.x > right)
            return wxRealPoint(right, wxMin(wxMax(m_anchor.y, bottom), top));

        if(m_anchor.y < bottom)
            return wxRealPoint(wxMin(wxMax(m_anchor.x, left), right), bottom);

        if(m_anchor.y > top)
            return wxRealPoint(wxMin(wxMax(m_anchor.x, left), right), top);

        // The anchor lies inside of the rectangle, point at its centre then
        return wxRealPoint((left + right) / 2, (bottom + top) / 2);
    }

    /// A rough screen rectangle around the text. It is used to decide whether the
    /// mouse is on the text, so that the anchor can be dragged. The exact width of
    /// the text is only known inside of DoPlot(), hence an estimate is used.
    wxRect GetTextScreenRect(mpWindow& w) const
    {
        const wxCoord anchorX = w.x2p(m_anchor.x);
        const wxCoord anchorY = w.y2p(m_anchor.y);

        const int estimatedWidth = 80;
        const int estimatedHeight = 20;

        wxCoord textLeft = anchorX;
        if(m_anchor.x < 0.0)
            textLeft = anchorX - estimatedWidth;      // the text ends at the anchor
        else if(m_anchor.x > 1.0)
            textLeft = anchorX;                       // the text starts at the anchor
        else
            textLeft = anchorX - estimatedWidth / 2;  // the text is centred

        return wxRect(textLeft, anchorY - 3, estimatedWidth, estimatedHeight);
    }

    // Interface functions to modify the anchor, the corners and the text
    wxRealPoint GetAnchor() const
    {
        return m_anchor;
    }

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

    void SetAnchor(const wxRealPoint& anchor)
    {
        m_anchor = anchor;
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
    wxRealPoint m_anchor;       // Position of the text, one coordinate is outside of [0,1]
    wxRealPoint m_firstCorner;  // One corner of the rectangle in normalized image coordinates
    wxRealPoint m_secondCorner; // The opposite corner of the rectangle
    wxString m_text;            // Text which is drawn at the anchor
};

#endif // IMAGECALLOUT_H_INCLUDED
