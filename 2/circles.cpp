//
//  main.cpp
//  Circles
//
//  Created by Александр Марков on 20/10/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <cmath>

struct IntervalPoint {
    long double x_coord;
    int type;
};

const bool CompareIntervalPoints(const IntervalPoint& first,
                                 const IntervalPoint& second)
{
    return first.x_coord < second.x_coord;
}

struct Point {
    int x_coord;
    int y_coord;
};

std::vector<IntervalPoint> GetInterval(const Point& point,
                                       long double radii) {
    std::vector<IntervalPoint> ends;
    long double radi = (radii - point.y_coord)*(point.y_coord + radii);
    if (radi <= 0.0) {
        return ends;
    }
    radi = sqrt(radi);
    ends.push_back({point.x_coord - radi, 1});
    ends.push_back({point.x_coord + radi, -1});
    return ends;
}

bool DoesCover(const std::vector<IntervalPoint>& intervals, int min_points) {
    int current_cover = 0;
    for (const IntervalPoint& point: intervals) {
        current_cover += point.type;
        if (current_cover >= min_points) {
            return true;
        }
    }
    return false;
}

long double FindRadii(const std::vector<Point>& points, int min_points) {
    long double max_rad = 1000.0 * sqrt(2) + 5;
    long double min_rad = 0.0;
    
    while (max_rad - min_rad > 0.000001) {
        long double current_rad = (max_rad + min_rad) / 2;
        std::vector<IntervalPoint> intervals;
        for (const Point& point: points) {
            std::vector<IntervalPoint> interval = GetInterval(point, current_rad);
            if (interval.size() > 0) {
                intervals.push_back(interval[0]);
                intervals.push_back(interval[1]);
            }
        }
        std::sort(intervals.begin(), intervals.end(), CompareIntervalPoints);
        if (DoesCover(intervals, min_points)) {
            max_rad = current_rad;
        } else {
            min_rad = current_rad;
        }
    }
    return max_rad;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(0);
    
    size_t n_points;
    int min_cover;
    std::cin >> n_points >> min_cover;
    std::vector<Point> points;
    for (size_t i = 0; i < n_points; ++i) {
        int first, second;
        std::cin >> first >> second;
        points.push_back({first, second});
    }
    long double answer = FindRadii(points, min_cover);
    std::cout << std::fixed << std::setprecision(6) << answer;
    return 0;
}
