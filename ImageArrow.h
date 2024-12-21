#ifndef IMAGEARROW_H_INCLUDED
#define IMAGEARROW_H_INCLUDED

#include "mathplot.h"

// Function to calculate the distance between two wxPoints
double DistanceBetweenPoints(const wxPoint& point1, const wxPoint& point2)
{
    return std::sqrt((point1.x - point2.x) * (point1.x - point2.x) + (point1.y - point2.y) * (point1.y - point2.y));
}

bool IsPointNear(const wxPoint& point, const wxPoint& target, int tolerance)
{
    return std::abs(point.x - target.x) <= tolerance && std::abs(point.y - target.y) <= tolerance;
}

bool IsPointOnLine(const wxPoint& point, const wxPoint& start, const wxPoint& end, int tolerance)
{
    double dx = end.x - start.x;
    double dy = end.y - start.y;
    double lengthSquared = dx * dx + dy * dy;

    if (lengthSquared == 0)
        return IsPointNear(point, start, tolerance);

    double t = ((point.x - start.x) * dx + (point.y - start.y) * dy) / lengthSquared;
    if (t < 0.0 || t > 1.0)
        return false;

    wxPoint projection(start.x + t * dx, start.y + t * dy);
    return IsPointNear(point, projection, tolerance);
}

double DistanceToLine(double mouseX, double mouseY, double startX, double startY, double endX, double endY)
{
    // Calculate the length of the line segment
    double lineLength = std::sqrt((endX - startX) * (endX - startX) + (endY - startY) * (endY - startY));

    // If the line segment is actually a point, return the distance to this point
    if (lineLength == 0)
    {
        return std::sqrt((mouseX - startX) * (mouseX - startX) + (mouseY - startY) * (mouseY - startY));
    }

    // Project the point onto the line to find the closest point on the line segment
    double t = ((mouseX - startX) * (endX - startX) + (mouseY - startY) * (endY - startY)) / (lineLength * lineLength);

    // If the projection is outside the line segment, clamp to the closest endpoint
    if (t < 0.0)
    {
        return std::sqrt((mouseX - startX) * (mouseX - startX) + (mouseY - startY) * (mouseY - startY));
    }
    else if (t > 1.0)
    {
        return std::sqrt((mouseX - endX) * (mouseX - endX) + (mouseY - endY) * (mouseY - endY));
    }

    // Otherwise, find the point on the line and compute the distance
    double closestX = startX + t * (endX - startX);
    double closestY = startY + t * (endY - startY);
    return std::sqrt((mouseX - closestX) * (mouseX - closestX) + (mouseY - closestY) * (mouseY - closestY));
}

class mpArrow : public mpLayer {
public:
    mpArrow(const wxRealPoint& start, const wxRealPoint& end, const wxString& label)
        : m_start(start), m_end(end), m_label(label),
          mpLayer(mpLAYER_TEXT)
    {
        SetName("Arrow"); // the layer's
    }

    // Override the virtual method to draw on the plot
    virtual void DoPlot(wxDC& dc, mpWindow& w) override {
        // Transform coordinates to screen space
        wxCoord x1, y1, x2, y2;
        x1 = w.x2p(m_start.x);
        y1 = w.y2p(m_start.y);
        x2 = w.x2p(m_end.x);
        y2 = w.y2p(m_end.y);

        // Draw the main line
        dc.DrawLine(x1, y1, x2, y2);

        // Draw the arrowhead
        DrawArrowHead(dc, wxPoint(x1, y1), wxPoint(x2, y2));

        // Draw the label at the start point
        dc.DrawText(m_label, x1, y1);
    }

    // Interface Functions to Modify Start, End, and Label
    void SetStartPoint(const wxRealPoint& start) {
        m_start = start;
    }

    void SetEndPoint(const wxRealPoint& end) {
        m_end = end;
    }

    void SetLabel(const wxString& label) {
        m_label = label;
    }

    wxRealPoint GetStartPoint() const {
        return m_start;
    }

    wxRealPoint GetEndPoint() const {
        return m_end;
    }

    wxString GetLabel() const {
        return m_label;
    }

private:
    wxRealPoint m_start; // Start point of the arrow
    wxRealPoint m_end;   // End point of the arrow
    wxString m_label;    // Label at the start point

    void DrawArrowHead(wxDC& dc, const wxPoint& start, const wxPoint& end) {
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
        const double arrowAngle = M_PI / 6.0;

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
};


#endif // IMAGEARROW_H_INCLUDED
