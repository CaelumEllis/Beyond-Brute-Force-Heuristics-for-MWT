#ifndef POINT_H
#define POINT_H

struct Point {
    double x, y;
    Point() : x(0), y(0) {};
    Point(double x_, double y_): x(x_), y(y_) {}

    // Operator to check equality of two points
    bool operator==(const Point& t) const {
        return x == t.x && y == t.y;
    }

    Point operator-(const Point & p) const {
        return Point(x - p.x, y - p.y);
    }

    double cross (const Point & p) const {
        return x * p.y - y * p.x;
    }

    double cross (const Point & p, const Point & q) const {
        return (p - *this).cross(q - *this);
    }

    int half() const {
        return int(y < 0 || (y == 0 && x < 0));
    }

    // Function to calculate the squared distance between two points
    static double distSq(Point a, Point b) {
        return (a.x - b.x) * (a.x - b.x) + 
            (a.y - b.y) * (a.y - b.y);
    }
};

#endif
