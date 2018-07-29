#include "wave/optimization/ceres/local_params/line_parameterization.hpp"

namespace wave {

bool LineParameterization::Plus(const double *x, const double *delta, double *x_plus_delta) const {
    Eigen::Map<const Vec3> normal(x);
    Eigen::Map<const Vec3> pt0(x + 3);
    Eigen::Map<const Vec2> Dnormal(delta);
    Eigen::Map<const Vec2> Dpt0(delta + 2);

    Eigen::Map<Vec3> normalpD(x_plus_delta);
    Eigen::Map<Vec3> pt0pD(x_plus_delta + 3);

    // Cross product of unit z vector with p
    Vec2 axis;
    axis << -normal(1), normal(0); //, 0.0;

    double c = normal(2);

    Mat3 skew = Mat3::Zero();
    skew(0, 2) = axis(1);
    skew(1, 2) = -axis(0);
    skew(2, 0) = -axis(1);
    skew(2, 1) = axis(0);

    Mat3 R = Mat3::Identity() + skew + (1.0 / (1.0 + c)) * skew * skew;

    Vec3 v = R.block<3, 2>(0, 0) * Dnormal;

    double a_mag = ceres::sqrt(Dnormal(0) * Dnormal(0) + Dnormal(1) * Dnormal(1));

    if (a_mag < 1e-5) {
        normalpD = normal + v;
    } else {
        normalpD = ceres::cos(a_mag) * normal + ceres::sin(a_mag) * (v / a_mag);
    }

    pt0pD = pt0 + R.block<3, 2>(0,0) * Dpt0;

    return true;
}

bool LineParameterization::ComputeJacobian(const double *x, double *jacobian) const {
    Eigen::Map<const Vec3> X(x);
    Eigen::Map<Eigen::Matrix<double, 6, 4, Eigen::RowMajor>> J(jacobian);
    J.setZero();

    Vec2 axis;
    axis << -X(1), X(0); //, 0.0;

    double c = X(2);

    Mat3 skew = Mat3::Zero();
    skew(0, 2) = axis(1);
    skew(1, 2) = -axis(0);
    skew(2, 0) = -axis(1);
    skew(2, 1) = axis(0);

    Mat3 R = Mat3::Identity() + skew + (1.0 / (1.0 + c)) * skew * skew;

    J.block<3, 2>(0,0) = R.block<3, 2>(0,0);
    J.block<3, 2>(3,2) = R.block<3, 2>(0,0);

    return true;
}

}
