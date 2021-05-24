#include "Utils.h"

double epsilon(double a, double b) {
    return fabs(a + b) * numeric_limits<double>::epsilon() * 15;
};

bool improve(double value_1, double value_2) {
    return (value_1 - value_2) > epsilon(value_1, value_2);
}

bool assertEqual(double value_1, double value_2) {
    if (value_1 > PENAL_COST) {
        return abs(value_1 - value_2) <= 1e-4;
    }
    return abs(value_1 - value_2) <= 1e-8;
}