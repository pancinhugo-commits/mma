/**
 * @file main.cpp
 * @brief Exemple d'utilisation de l'algorithme MMA pour résoudre un problème d'optimisation sous contraintes.
 * 
 * Ce fichier illustre comment utiliser la classe MMASolver
 * pour minimiser une fonction objectif sous des contraintes non linéaires.
 * 
 * Problème test :
 *   - Fonction objectif : f(x) = x1^2 + x2^2 + x3^2 (minimiser)
 *   - Contraintes :
 *       g1(x) = (x1-5)^2 + (x2-2)^2 + (x3-1)^2 - 9 <= 0
 *       g2(x) = (x1-3)^2 + (x2-4)^2 + (x3-3)^2 - 9 <= 0
 *   - Bornes : 0 <= xi <= 5 pour i = 1, 2, 3
 * 
 * @author Inspiré du code fourni par l'utilisateur
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <iomanip>
#include "optimisation.h"

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

/**
 * @brief Calcule le carré d'un nombre.
 * @param x Nombre à élever au carré.
 * @return x^2
 */
inline double Squared(double x) {
    return x * x;
}

/**
 * @brief Affiche un vecteur sur la console.
 * @param x Vecteur à afficher.
 * @param n Taille du vecteur.
 * @param name Nom du vecteur (pour l'affichage).
 */
void PrintVector(const double* x, int n, const std::string& name = "x") {
    std::cout << std::setw(10) << name << ": [";
    for (int i = 0; i < n; i++) {
        std::cout << std::setw(10) << x[i];
        if (i < n - 1) std::cout << ", ";
    }
    std::cout << "]" << std::endl;
}

// ============================================================================
// PROBLEM DEFINITION
// ============================================================================

/**
 * @brief Classe représentant un problème d'optimisation sous contraintes.
 * 
 * Ce problème est utilisé pour tester l'algorithme MMA.
 * Il définit :
 *   - Une fonction objectif quadratique.
 *   - Deux contraintes non linéaires (sphères en 3D).
 *   - Des bornes sur les variables de conception.
 */
class OptimizationProblem {
public:
    int n;                  ///< Nombre de variables de conception
    int m;                  ///< Nombre de contraintes
    std::vector<double> x0; ///< Point de départ
    std::vector<double> xmin; ///< Bornes inférieures
    std::vector<double> xmax; ///< Bornes supérieures

    /**
     * @brief Constructeur du problème.
     * 
     * Initialise un problème avec 3 variables et 2 contraintes.
     * Le point de départ est (4, 3, 2).
     */
    OptimizationProblem()
        : n(3)
        , m(2)
        , x0({4.0, 3.0, 2.0})
        , xmin(n, 0.0)
        , xmax(n, 5.0)
    {}

    /**
     * @brief Calcule la fonction objectif et les contraintes.
     * 
     * @param x Vecteur des variables de conception.
     * @param f0x Valeur de la fonction objectif (sortie).
     * @param fx Vecteur des contraintes (sortie).
     */
    void Evaluate(const double* x, double* f0x, double* fx) const {
        // Fonction objectif : f(x) = x1^2 + x2^2 + x3^2
        f0x[0] = 0.0;
        for (int i = 0; i < n; ++i) {
            f0x[0] += x[i] * x[i];
        }

        // Contrainte 1 : (x1-5)^2 + (x2-2)^2 + (x3-1)^2 - 9 <= 0
        fx[0] = Squared(x[0] - 5.0) + Squared(x[1] - 2.0) + Squared(x[2] - 1.0) - 9.0;
        
        // Contrainte 2 : (x1-3)^2 + (x2-4)^2 + (x3-3)^2 - 9 <= 0
        fx[1] = Squared(x[0] - 3.0) + Squared(x[1] - 4.0) + Squared(x[2] - 3.0) - 9.0;
    }

    /**
     * @brief Calcule la fonction objectif, les contraintes et leurs gradients.
     * 
     * @param x Vecteur des variables de conception.
     * @param f0x Valeur de la fonction objectif (sortie).
     * @param fx Vecteur des contraintes (sortie).
     * @param df0dx Gradient de la fonction objectif (sortie).
     * @param dfdx Gradient des contraintes (sortie, matrice n x m).
     */
    void EvaluateWithSensitivities(const double* x, double* f0x, double* fx, 
                                   double* df0dx, double* dfdx) const {
        // Évaluer la fonction objectif et les contraintes
        Evaluate(x, f0x, fx);

        // Gradient de la fonction objectif : df0/dxi = 2 * xi
        for (int i = 0; i < n; ++i) {
            df0dx[i] = 2.0 * x[i];
        }

        // Gradient des contraintes
        // Contrainte 1 : dg1/dx1 = 2*(x1-5), dg1/dx2 = 2*(x2-2), dg1/dx3 = 2*(x3-1)
        // Contrainte 2 : dg2/dx1 = 2*(x1-3), dg2/dx2 = 2*(x2-4), dg2/dx3 = 2*(x3-3)
        int k = 0;
        dfdx[k++] = 2.0 * (x[0] - 5.0);  // dg1/dx1
        dfdx[k++] = 2.0 * (x[0] - 3.0);  // dg2/dx1
        dfdx[k++] = 2.0 * (x[1] - 2.0);  // dg1/dx2
        dfdx[k++] = 2.0 * (x[1] - 4.0);  // dg2/dx2
        dfdx[k++] = 2.0 * (x[2] - 1.0);  // dg1/dx3
        dfdx[k++] = 2.0 * (x[2] - 3.0);  // dg2/dx3
    }
};

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main() {
    std::cout << "===================================================" << std::endl;
    std::cout << "| Test de l'algorithme MMA                       |" << std::endl;
    std::cout << "===================================================" << std::endl << std::endl;

    // Initialiser le problème
    OptimizationProblem problem;
    
    std::cout << "Problème :" << std::endl;
    std::cout << "  - Variables : " << problem.n << std::endl;
    std::cout << "  - Contraintes : " << problem.m << std::endl;
    std::cout << "  - Point de départ : ";
    PrintVector(problem.x0.data(), problem.n, "x0");
    std::cout << "  - Bornes : [";
    PrintVector(problem.xmin.data(), problem.n, "min");
    std::cout << "            ";
    PrintVector(problem.xmax.data(), problem.n, "max");
    std::cout << std::endl;

    // Variables pour l'optimisation
    std::vector<double> x = problem.x0;  // Variables de conception courantes
    std::vector<double> x_old = x;       // Variables de l'itération précédente
    
    double f;       // Valeur de la fonction objectif
    std::vector<double> g(problem.m);      // Valeurs des contraintes
    std::vector<double> df0dx(problem.n);  // Gradient de la fonction objectif
    std::vector<double> dfdx(problem.n * problem.m); // Gradient des contraintes

    // Évaluer le point initial
    problem.Evaluate(x.data(), &f, g.data());
    std::cout << "Valeur initiale :" << std::endl;
    std::cout << "  - f(x0) = " << f << std::endl;
    std::cout << "  - Contraintes : ";
    PrintVector(g.data(), problem.m, "g");
    std::cout << std::endl;

    // ========================================================================
    // TEST : ALGORITHME MMA
    // ========================================================================
    std::cout << "===================================================" << std::endl;
    std::cout << "| Test : Algorithme MMA                          |" << std::endl;
    std::cout << "===================================================" << std::endl << std::endl;

    // Initialiser le solveur MMA
    // Paramètres : n (variables), m (contraintes), a0, c0, d0
    // Ici, on utilise a0=0, c0=1000, d0=1 (valeurs typiques)
    MMASolver mma(problem.n, problem.m, 0.0, 1000.0, 1.0);
    
    // Paramètres de l'algorithme
    const int max_iter = 20;  // Nombre maximal d'itérations
    const double tol = 1e-5; // Tolérance pour la convergence
    double ch = 1.0;         // Changement maximal dans les variables
    
    // Boucle d'optimisation MMA
    for (int iter = 0; iter < max_iter && ch > tol; ++iter) {
        std::cout << "--- Itération " << iter + 1 << " ---" << std::endl;

        // Évaluer les sensibilités (gradient de f et des contraintes)
        problem.EvaluateWithSensitivities(x.data(), &f, g.data(), df0dx.data(), dfdx.data());

        // Mettre à jour les variables avec MMA
        mma.Update(x.data(), df0dx.data(), g.data(), dfdx.data(), 
                   problem.xmin.data(), problem.xmax.data());

        // Calculer le changement maximal dans les variables
        ch = 0.0;
        for (int i = 0; i < problem.n; ++i) {
            ch = std::max(ch, std::abs(x[i] - x_old[i]));
            x_old[i] = x[i];
        }

        // Afficher les résultats
        std::cout << "  - f(x) = " << f << std::endl;
        std::cout << "  - Changement maximal : " << ch << std::endl;
        std::cout << "  - Variables : ";
        PrintVector(x.data(), problem.n, "x");
        std::cout << "  - Contraintes : ";
        PrintVector(g.data(), problem.m, "g");
        std::cout << std::endl;
    }

    std::cout << "Résultat final (MMA) :" << std::endl;
    std::cout << "  - f(x) = " << f << std::endl;
    std::cout << "  - Variables : ";
    PrintVector(x.data(), problem.n, "x");
    std::cout << "  - Contraintes : ";
    PrintVector(g.data(), problem.m, "g");
    std::cout << std::endl;

    return 0;
}
