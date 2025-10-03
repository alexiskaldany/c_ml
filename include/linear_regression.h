#ifndef LINEAR_REGRESSION_H
#define LINEAR_REGRESSION_H

#include <vector>
#include <utility>

struct LinearRegressionResult {
    double slope;
    double intercept;
    double r_squared;
};

class LinearRegression {
public:
    static LinearRegressionResult calculate(const std::vector<std::pair<double, double>>& points);
};

#endif // LINEAR_REGRESSION_H
