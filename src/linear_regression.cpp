#include "linear_regression.h"
#include <cmath>
#include <stdexcept>

LinearRegressionResult LinearRegression::calculate(const std::vector<std::pair<double, double>>& points) {
    if (points.empty()) {
        throw std::invalid_argument("Cannot calculate linear regression with no points");
    }
    
    if (points.size() == 1) {
        throw std::invalid_argument("Cannot calculate linear regression with only one point");
    }
    
    double sum_x = 0.0, sum_y = 0.0, sum_xy = 0.0, sum_xx = 0.0;
    int n = points.size();
    
    for (const auto& point : points) {
        sum_x += point.first;
        sum_y += point.second;
        sum_xy += point.first * point.second;
        sum_xx += point.first * point.first;
    }
    
    double mean_x = sum_x / n;
    double mean_y = sum_y / n;
    
    // Calculate slope (m) and intercept (b) for y = mx + b
    double denominator = sum_xx - (sum_x * sum_x / n);
    if (std::abs(denominator) < 1e-10) {
        throw std::invalid_argument("Cannot calculate linear regression: all x values are the same");
    }
    
    double slope = (sum_xy - (sum_x * sum_y / n)) / denominator;
    double intercept = mean_y - slope * mean_x;
    
    // Calculate R-squared
    double ss_total = 0.0, ss_residual = 0.0;
    for (const auto& point : points) {
        double y_pred = slope * point.first + intercept;
        ss_residual += (point.second - y_pred) * (point.second - y_pred);
        ss_total += (point.second - mean_y) * (point.second - mean_y);
    }
    
    double r_squared = (ss_total > 1e-10) ? (1.0 - ss_residual / ss_total) : 1.0;
    
    return {slope, intercept, r_squared};
}
