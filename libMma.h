#ifndef OPTIMISATION_H
#define OPTIMISATION_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <iostream>
#include <stdexcept>

// ============================================================================
// UTILITY FUNCTIONS FOR LINEAR ALGEBRA
// ============================================================================

namespace Optimisation {

// Factorize a matrix K (n x n) using LU decomposition (in-place)
void Factorize(std::vector<double>& K, int n);

// Solve K * x = b (K is pre-factorized, n x n)
void Solve(std::vector<double>& K, std::vector<double>& x, int n);

} // namespace Optimisation

// ============================================================================
// MMA (Method of Moving Asymptotes) Solver
// ============================================================================

class MMASolver {
public:
    // Constructor
    // nn: number of design variables
    // mm: number of constraints
    // ai: initial value for a (constraint coefficients)
    // ci: initial value for c (constraint coefficients)
    // di: initial value for d (constraint coefficients)
    MMASolver(int nn, int mm, double ai = 1.0, double ci = 1.0, double di = 1.0);

    // Set asymptote parameters
    void SetAsymptotes(double init, double decrease, double increase);

    // Update the MMA subproblem and solve for new x
    // xval: current design variables (input/output)
    // dfdx: gradient of the objective function (n)
    // gx: constraint values (m)
    // dgdx: gradient of the constraints (n x m)
    // xmin: lower bounds for design variables (n)
    // xmax: upper bounds for design variables (n)
    void Update(double* xval, const double* dfdx, const double* gx, const double* dgdx,
                const double* xmin, const double* xmax);

    // Get the current iteration count
    int GetIterationCount() const { return iter; }

protected:
    // Problem dimensions
    int n; // number of design variables
    int m; // number of constraints
    int iter; // iteration counter

    // Parameters
    double xmamieps; // small epsilon for numerical stability
    double epsimin; // tolerance for dual problem
    double raa0; // small regularization parameter
    double move; // move limit parameter
    double albefa; // alpha/beta parameter
    double asyminit; // initial asymptote distance
    double asymdec; // asymptote decrease factor
    double asyminc; // asymptote increase factor

    // Vectors for MMA
    std::vector<double> a; // constraint coefficients (m)
    std::vector<double> c; // constraint coefficients (m)
    std::vector<double> d; // constraint coefficients (m)
    std::vector<double> y; // auxiliary variables (m)
    std::vector<double> lam; // Lagrange multipliers (m)
    std::vector<double> mu; // auxiliary multipliers (m)
    std::vector<double> s; // search direction (2*m)
    std::vector<double> low; // lower asymptotes (n)
    std::vector<double> upp; // upper asymptotes (n)
    std::vector<double> alpha; // lower bounds for subproblem (n)
    std::vector<double> beta; // upper bounds for subproblem (n)
    std::vector<double> p0; // objective coefficients (n)
    std::vector<double> q0; // objective coefficients (n)
    std::vector<double> pij; // constraint coefficients (n x m)
    std::vector<double> qij; // constraint coefficients (n x m)
    std::vector<double> b; // constraint constants (m)
    std::vector<double> grad; // gradient of dual problem (m)
    std::vector<double> hess; // Hessian of dual problem (m x m)
    std::vector<double> xold1; // previous design variables (n)
    std::vector<double> xold2; // design variables from two iterations ago (n)
    double z; // auxiliary variable for dual problem

    // Generate the MMA subproblem
    void GenSub(const double* xval, const double* dfdx, const double* gx, const double* dgdx,
                const double* xmin, const double* xmax);

    // Solve the dual problem using Interior Point Method
    void SolveDIP(double* x);

    // Solve the dual problem using Steepest Ascent Method
    void SolveDSA(double* x);

    // Compute the dual residual
    double DualResidual(double* x, double epsi);

    // Perform line search for dual problem
    void DualLineSearch();

    // Compute the Hessian of the dual problem
    void DualHess(double* x);

    // Compute the gradient of the dual problem
    void DualGrad(double* x);

    // Compute x, y, z from lambda
    void XYZofLAMBDA(double* x);
};

// ============================================================================
// IMPLEMENTATION OF UTILITY FUNCTIONS
// ============================================================================

namespace Optimisation {

inline void Factorize(std::vector<double>& K, int n) {
    for (int s = 0; s < n - 1; s++) {
        for (int i = s + 1; i < n; i++) {
            K[i * n + s] = K[i * n + s] / K[s * n + s];
            for (int j = s + 1; j < n; j++) {
                K[i * n + j] = K[i * n + j] - K[i * n + s] * K[s * n + j];
            }
        }
    }
}

inline void Solve(std::vector<double>& K, std::vector<double>& x, int n) {
    // Forward substitution
    for (int i = 1; i < n; i++) {
        double a = 0.0;
        for (int j = 0; j < i; j++) {
            a = a - K[i * n + j] * x[j];
        }
        x[i] = x[i] + a;
    }

    // Backward substitution
    x[n - 1] = x[n - 1] / K[(n - 1) * n + (n - 1)];
    for (int i = n - 2; i >= 0; i--) {
        double a = x[i];
        for (int j = i + 1; j < n; j++) {
            a = a - K[i * n + j] * x[j];
        }
        x[i] = a / K[i * n + i];
    }
}

} // namespace Optimisation

// ============================================================================
// IMPLEMENTATION OF MMASolver
// ============================================================================

inline MMASolver::MMASolver(int nn, int mm, double ai, double ci, double di)
    : n(nn)
    , m(mm)
    , iter(0)
    , xmamieps(1.0e-5)
    , epsimin(std::sqrt(n + m) * 1e-9)
    , raa0(0.00001)
    , move(0.5)
    , albefa(0.1)
    , asyminit(0.5)
    , asymdec(0.7)
    , asyminc(1.2)
    , a(m, ai)
    , c(m, ci)
    , d(m, di)
    , y(m)
    , lam(m)
    , mu(m)
    , s(2 * m)
    , low(n)
    , upp(n)
    , alpha(n)
    , beta(n)
    , p0(n)
    , q0(n)
    , pij(n * m)
    , qij(n * m)
    , b(m)
    , grad(m)
    , hess(m * m)
    , xold1(n)
    , xold2(n)
    , z(0.0)
{}

inline void MMASolver::SetAsymptotes(double init, double decrease, double increase) {
    asyminit = init;
    asymdec = decrease;
    asyminc = increase;
}

inline void MMASolver::Update(double* xval, const double* dfdx, const double* gx, const double* dgdx,
                              const double* xmin, const double* xmax) {
    // Generate the subproblem
    GenSub(xval, dfdx, gx, dgdx, xmin, xmax);

    // Update xolds
    xold2 = xold1;
    std::copy_n(xval, n, xold1.data());

    // Solve the dual with an interior point method
    SolveDIP(xval);
}

inline void MMASolver::GenSub(const double* xval, const double* dfdx, const double* gx, const double* dgdx,
                               const double* xmin, const double* xmax) {
    // Forward the iterator
    iter++;

    // Set asymptotes
    if (iter < 3) {
        for (int i = 0; i < n; i++) {
            low[i] = xval[i] - asyminit * (xmax[i] - xmin[i]);
            upp[i] = xval[i] + asyminit * (xmax[i] - xmin[i]);
        }
    } else {
        for (int i = 0; i < n; i++) {
            double zzz = (xval[i] - xold1[i]) * (xold1[i] - xold2[i]);
            double gamma;
            if (zzz < 0.0) {
                gamma = asymdec;
            } else if (zzz > 0.0) {
                gamma = asyminc;
            } else {
                gamma = 1.0;
            }
            low[i] = xval[i] - gamma * (xold1[i] - low[i]);
            upp[i] = xval[i] + gamma * (upp[i] - xold1[i]);

            double xmami = std::max(xmamieps, xmax[i] - xmin[i]);
            low[i] = std::max(low[i], xval[i] - 100.0 * xmami);
            low[i] = std::min(low[i], xval[i] - 1.0e-5 * xmami);
            upp[i] = std::max(upp[i], xval[i] + 1.0e-5 * xmami);
            upp[i] = std::min(upp[i], xval[i] + 100.0 * xmami);

            double xmi = xmin[i] - 1.0e-6;
            double xma = xmax[i] + 1.0e-6;
            if (xval[i] < xmi) {
                low[i] = xval[i] - (xma - xval[i]) / 0.9;
                upp[i] = xval[i] + (xma - xval[i]) / 0.9;
            }
            if (xval[i] > xma) {
                low[i] = xval[i] - (xval[i] - xmi) / 0.9;
                upp[i] = xval[i] + (xval[i] - xmi) / 0.9;
            }
        }
    }

    // Set bounds and the coefficients for the approximation
    for (int i = 0; i < n; ++i) {
        // Compute bounds alpha and beta
        alpha[i] = std::max(xmin[i], low[i] + albefa * (xval[i] - low[i]));
        alpha[i] = std::max(alpha[i], xval[i] - move * (xmax[i] - xmin[i]));
        alpha[i] = std::min(alpha[i], xmax[i]);
        beta[i] = std::min(xmax[i], upp[i] - albefa * (upp[i] - xval[i]));
        beta[i] = std::min(beta[i], xval[i] + move * (xmax[i] - xmin[i]));
        beta[i] = std::max(beta[i], xmin[i]);

        // Objective function
        double dfdxp = std::max(0.0, dfdx[i]);
        double dfdxm = std::max(0.0, -1.0 * dfdx[i]);
        double xmamiinv = 1.0 / std::max(xmamieps, xmax[i] - xmin[i]);
        double pq = 0.001 * std::abs(dfdx[i]) + raa0 * xmamiinv;
        p0[i] = std::pow(upp[i] - xval[i], 2.0) * (dfdxp + pq);
        q0[i] = std::pow(xval[i] - low[i], 2.0) * (dfdxm + pq);

        // Constraints
        for (int j = 0; j < m; j++) {
            double dgdxp = std::max(0.0, dgdx[i * m + j]);
            double dgdxm = std::max(0.0, -1.0 * dgdx[i * m + j]);
            xmamiinv = 1.0 / std::max(xmamieps, xmax[i] - xmin[i]);
            pq = 0.001 * std::abs(dgdx[i * m + j]) + raa0 * xmamiinv;
            pij[i * m + j] = std::pow(upp[i] - xval[i], 2.0) * (dgdxp + pq);
            qij[i * m + j] = std::pow(xval[i] - low[i], 2.0) * (dgdxm + pq);
        }
    }

    // The constant for the constraints
    for (int j = 0; j < m; j++) {
        b[j] = -gx[j];
        for (int i = 0; i < n; i++) {
            b[j] += pij[i * m + j] / (upp[i] - xval[i]) + qij[i * m + j] / (xval[i] - low[i]);
        }
    }
}

inline void MMASolver::SolveDIP(double* x) {
    for (int j = 0; j < m; j++) {
        lam[j] = c[j] / 2.0;
        mu[j] = 1.0;
    }

    const double tol = epsimin;
    double epsi = 1.0;
    double err = 1.0;
    int loop;

    while (epsi > tol) {
        loop = 0;
        while (err > 0.9 * epsi && loop < 100) {
            loop++;

            // Set up Newton system
            XYZofLAMBDA(x);
            DualGrad(x);
            for (int j = 0; j < m; j++) {
                grad[j] = -1.0 * grad[j] - epsi / lam[j];
            }
            DualHess(x);

            // Solve Newton system
            if (m > 1) {
                Optimisation::Factorize(hess, m);
                Optimisation::Solve(hess, grad, m);
                for (int j = 0; j < m; j++) {
                    s[j] = grad[j];
                }
            } else if (m > 0) {
                s[0] = grad[0] / hess[0];
            }

            // Get the full search direction
            for (int i = 0; i < m; i++) {
                s[m + i] = -mu[i] + epsi / lam[i] - s[i] * mu[i] / lam[i];
            }

            // Perform linesearch and update lam and mu
            DualLineSearch();

            XYZofLAMBDA(x);

            // Compute KKT residual
            err = DualResidual(x, epsi);
        }
        epsi = epsi * 0.1;
    }
}

inline void MMASolver::SolveDSA(double* x) {
    for (int j = 0; j < m; j++) {
        lam[j] = 1.0;
    }

    const double tol = epsimin;
    double err = 1.0;
    int loop = 0;

    while (err > tol && loop < 500) {
        loop++;
        XYZofLAMBDA(x);
        DualGrad(x);
        double theta = 1.0;
        err = 0.0;
        for (int j = 0; j < m; j++) {
            lam[j] = std::max(0.0, lam[j] + theta * grad[j]);
            err += grad[j] * grad[j];
        }
        err = std::sqrt(err);
    }
}

inline double MMASolver::DualResidual(double* x, double epsi) {
    std::vector<double> res(2 * m);

    for (int j = 0; j < m; j++) {
        res[j] = -b[j] - a[j] * z - y[j] + mu[j];
        res[j + m] = mu[j] * lam[j] - epsi;
        for (int i = 0; i < n; i++) {
            res[j] += pij[i * m + j] / (upp[i] - x[i]) + qij[i * m + j] / (x[i] - low[i]);
        }
    }

    double nrI = 0.0;
    for (int i = 0; i < 2 * m; i++) {
        if (nrI < std::abs(res[i])) {
            nrI = std::abs(res[i]);
        }
    }

    return nrI;
}

inline void MMASolver::DualLineSearch() {
    double theta = 1.005;
    for (int i = 0; i < m; i++) {
        if (theta < -1.01 * s[i] / lam[i]) {
            theta = -1.01 * s[i] / lam[i];
        }
        if (theta < -1.01 * s[i + m] / mu[i]) {
            theta = -1.01 * s[i + m] / mu[i];
        }
    }
    theta = 1.0 / theta;

    for (int i = 0; i < m; i++) {
        lam[i] = lam[i] + theta * s[i];
        mu[i] = mu[i] + theta * s[i + m];
    }
}

inline void MMASolver::DualHess(double* x) {
    std::vector<double> df2(n);
    std::vector<double> PQ(n * m);

    for (int i = 0; i < n; i++) {
        double pjlam = p0[i];
        double qjlam = q0[i];
        for (int j = 0; j < m; j++) {
            pjlam += pij[i * m + j] * lam[j];
            qjlam += qij[i * m + j] * lam[j];
            PQ[i * m + j] = pij[i * m + j] / std::pow(upp[i] - x[i], 2.0) - qij[i * m + j] / std::pow(x[i] - low[i], 2.0);
        }
        df2[i] = -1.0 / (2.0 * pjlam / std::pow(upp[i] - x[i], 3.0) + 2.0 * qjlam / std::pow(x[i] - low[i], 3.0));
        double xp = (std::sqrt(pjlam) * low[i] + std::sqrt(qjlam) * upp[i]) / (std::sqrt(pjlam) + std::sqrt(qjlam));
        if (xp < alpha[i]) {
            df2[i] = 0.0;
        }
        if (xp > beta[i]) {
            df2[i] = 0.0;
        }
    }

    // Create the matrix/matrix/matrix product: PQ^T * diag(df2) * PQ
    std::vector<double> tmp(n * m);
    for (int j = 0; j < m; j++) {
        for (int i = 0; i < n; i++) {
            tmp[j * n + i] = 0.0;
            tmp[j * n + i] += PQ[i * m + j] * df2[i];
        }
    }

    for (int i = 0; i < m; i++) {
        for (int j = 0; j < m; j++) {
            hess[i * m + j] = 0.0;
            for (int k = 0; k < n; k++) {
                hess[i * m + j] += tmp[i * n + k] * PQ[k * m + j];
            }
        }
    }

    double lamai = 0.0;
    for (int j = 0; j < m; j++) {
        if (lam[j] < 0.0) {
            lam[j] = 0.0;
        }
        lamai += lam[j] * a[j];
        if (lam[j] > c[j]) {
            hess[j * m + j] += -1.0;
        }
        hess[j * m + j] += -mu[j] / lam[j];
    }

    if (lamai > 0.0) {
        for (int j = 0; j < m; j++) {
            for (int k = 0; k < m; k++) {
                hess[j * m + k] += -10.0 * a[j] * a[k];
            }
        }
    }

    // Positive definiteness check
    double HessTrace = 0.0;
    for (int i = 0; i < m; i++) {
        HessTrace += hess[i * m + i];
    }
    double HessCorr = 1e-4 * HessTrace / m;

    if (-1.0 * HessCorr < 1.0e-7) {
        HessCorr = -1.0e-7;
    }

    for (int i = 0; i < m; i++) {
        hess[i * m + i] += HessCorr;
    }
}

inline void MMASolver::DualGrad(double* x) {
    for (int j = 0; j < m; j++) {
        grad[j] = -b[j] - a[j] * z - y[j];
        for (int i = 0; i < n; i++) {
            grad[j] += pij[i * m + j] / (upp[i] - x[i]) + qij[i * m + j] / (x[i] - low[i]);
        }
    }
}

inline void MMASolver::XYZofLAMBDA(double* x) {
    double lamai = 0.0;
    for (int i = 0; i < m; i++) {
        if (lam[i] < 0.0) {
            lam[i] = 0;
        }
        y[i] = std::max(0.0, lam[i] - c[i]); // Note: y=(lam-c)/d, but d is fixed at 1
        lamai += lam[i] * a[i];
    }
    z = std::max(0.0, 10.0 * (lamai - 1.0)); // SINCE a0 = 1.0

    for (int i = 0; i < n; i++) {
        double pjlam = p0[i];
        double qjlam = q0[i];
        for (int j = 0; j < m; j++) {
            pjlam += pij[i * m + j] * lam[j];
            qjlam += qij[i * m + j] * lam[j];
        }
        x[i] = (std::sqrt(pjlam) * low[i] + std::sqrt(qjlam) * upp[i]) / (std::sqrt(pjlam) + std::sqrt(qjlam));
        if (x[i] < alpha[i]) {
            x[i] = alpha[i];
        }
        if (x[i] > beta[i]) {
            x[i] = beta[i];
        }
    }
}

#endif // OPTIMISATION_H
