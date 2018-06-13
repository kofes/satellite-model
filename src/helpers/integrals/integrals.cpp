#include "integrals.h"

namespace helper {
namespace integral {
double rect(function f, double from, double to, double dh) {
    double sum = 0;
    double acc = 0;

    for (size_t i = 0; i < std::fabs(to - from) / dh; ++i) {
        double y = f(from + dh * i) * dh - acc;
        double t = sum + y;
        acc = (t - sum) - y;
        sum = t;
    }

    return sum;
}

double trapez(function f, double from, double to, double dh) {
    double sum = 0;
    double acc = 0;

    for (size_t i = 0; i < std::fabs(to - from)/dh - 1; ++i) {
        double y = (f(from + dh * i) + f(from + dh * (i + 1))) * dh / 2 - acc;
        double t = sum + y;
        acc = (t - sum) - y;
        sum = t;
    }

    return sum;
}

double simpson(function f, double from, double to, double dh) {
    double sum = 0;
    double acc = 0;

    sum += f(from);

    for (size_t i = 0; i < std::fabs(to - from)/dh; ++i) {
        double y = 4 * f(from + dh * (2 * i + 1) / 2) - acc;
        double t = sum + y;
        acc = (t - sum) - y;
        sum = t;
    }
    for (size_t i = 1; i < std::fabs(to - from)/dh; ++i) {
        double y = 2 * f(from + dh * i) - acc;
        double t = sum + y;
        acc = (t - sum) - y;
        sum = t;
    }

    sum += f(to);

    sum *= dh / 6;

    return sum;
}

linear_algebra::Vector rk4(
    multifunction f, const linear_algebra::Vector& initialValue,
    double from,
    double to,
    double dh
) {
     size_t N = std::fabs(to - from) / dh;
     N += (std::fabs(to - from) / dh - N) > 1e-5 ? 1 : 0;
     char direction = ((to - from > 0) ? 1 : -1);

     linear_algebra::Vector y(initialValue);

     for (size_t i = 0; i < N; ++i) {
         double x = direction * i * dh + from;

         auto k1 = dh * f(y, x);
         auto k2 = dh * f(y + k1 / 2., x + dh / 2.);
         auto k3 = dh * f(y + k2 / 2., x + dh / 2.);
         auto k4 = dh * f(y + k3, x + dh);

         y += (k1 + 2*k2 + 2*k3 + k4) / 6;
     }

     return y - initialValue;
}

linear_algebra::Vector rk5(
    multifunction f, const linear_algebra::Vector& initialValue,
    double from,
    double to,
    double dh
) {
     size_t N = std::fabs(to - from) / dh;
     N += (std::fabs(to - from) / dh - N) > 1e-5 ? 1 : 0;
     char direction = ((to - from > 0) ? 1 : -1);

     linear_algebra::Vector y(initialValue);

     for (size_t i = 0; i < N; ++i) {
         double x = direction * i * dh + from;
         auto k1 = dh * f(y, x);
         auto k2 = dh * f(y + k1 / 3., x + dh / 3.);
         auto k3 = dh * f(y + k1 / 6. + k2 / 6., x + dh / 3.);
         auto k4 = dh * f(y + k1 / 8. + k3 * 3. / 8., x + dh / 2.);
         auto k5 = dh * f(y + k1 / 2. - k3 * 3. / 2. + k4 * 18, x + dh);
         y += (k1 + 4*k4 + k5) / 6;
     }

     return y - initialValue;
}

linear_algebra::Vector rkf45(
    multifunction f, const linear_algebra::Vector& initialValue,
    double from,
    double to,
    double dh
) {
     size_t N = std::fabs(to - from) / dh;
     N += (std::fabs(to - from) / dh - N) > 1e-5 ? 1 : 0;
     char direction = ((to - from > 0) ? 1 : -1);

     linear_algebra::Vector y(initialValue);

     for (size_t i = 0; i < N; ++i) {
         double x = direction * i * dh + from;

         auto k1 = dh * f(y, x);
         auto k2 = dh * f(y + k1 / 4., x + dh / 4.);
         auto k3 = dh * f(y
             + k1 * 3. / 32.
             + k2 * 9. / 32.,
             x + dh * 3. / 8.);
         auto k4 = dh * f(y
             + k1 * 1932 / 2197.
             - k2 * 7200. / 2197.
             + k3 * 7296. / 2197.,
             x + dh * 12 / 13.);
         auto k5 = dh * f(y
             + k1 * 439. / 216.
             - k2 * 8
             + k3 * 3680. / 513
             - k4 * 845. / 4104.,
             x + dh);
         auto k6 = dh * f(y
             - k1 * 8. / 27.
             + k2 * 2
             - k3 * 3544. / 2565.
             + k4 * 1859. / 4104.
             - k5 * 11. / 40.,
             x + dh / 2.
         );

         y += (k1 * 16. / 135.
             + k3 * 6656. / 12825.
             + k4 * 28561. / 56430.
             - k5 * 9. / 50.
             + k6 * 2. / 55.
         );
     }

     return y - initialValue;
}

linear_algebra::Vector rkf78(
    multifunction f, const linear_algebra::Vector& initialValue,
    double from,
    double to,
    double dh
) {
     size_t N = std::fabs(to - from) / dh;
     N += (std::fabs(to - from) / dh - N) > 1e-5 ? 1 : 0;
     char direction = ((to - from > 0) ? 1 : -1);

     linear_algebra::Vector y(initialValue);
     std::vector<linear_algebra::Vector> k(RKF78_A_TABLE.size());

      for (size_t i = 0; i < N; ++i) {
          double x = direction * i * dh + from;
          // compute k
          for (size_t j = 0; j < RKF78_A_TABLE.size(); ++j) { // 13
              linear_algebra::Vector summaryAK(y.size(), 0);

              for (size_t l = 0; l < j; ++l) { // 12
                summaryAK += RKF78_A_TABLE[j][l] * k[l];
              }

              k[j] = dh * f(y + summaryAK, x + RKF78_C_TABLE[j] * dh);
          }
          // compute y
          for (size_t j = 0; j < RKF78_A_TABLE.size(); ++j) {
              y += k[j] * RKF78_B_TABLE[j];
          }
      }

     return y - initialValue;
}

}   // namespace integral
}   // namespace helper
