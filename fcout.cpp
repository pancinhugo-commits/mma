/**
 * @file fcout.cpp
 * @brief Programme pour calculer la fonction coût et son gradient.
 * 
 * Ce programme :
 *   1. Lit le vecteur x depuis un fichier x.csv.
 *   2. Calcule la valeur de la fonction coût f(x).
 *   3. Calcule le gradient de la fonction coût ∇f(x).
 *   4. Écrit f(x) dans result.csv.
 *   5. Écrit ∇f(x) dans gradf.csv.
 * 
 * Fonction coût par défaut : f(x) = x1^2 + x2^2 + x3^2
 * Gradient par défaut : ∇f(x) = [2*x1, 2*x2, 2*x3]
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

// ============================================================================
// FONCTION COÛT ET GRADIENT
// ============================================================================

/**
 * @brief Calcule la fonction coût f(x) = x1^2 + x2^2 + ... + xn^2.
 * @param x Vecteur des variables.
 * @return Valeur de la fonction coût.
 */
double ComputeCostFunction(const std::vector<double>& x) {
    double f = 0.0;
    for (double xi : x) {
        f += xi * xi;
    }
    return f;
}

/**
 * @brief Calcule le gradient de la fonction coût ∇f(x) = [2*x1, 2*x2, ..., 2*xn].
 * @param x Vecteur des variables.
 * @return Vecteur du gradient.
 */
std::vector<double> ComputeGradient(const std::vector<double>& x) {
    std::vector<double> grad(x.size());
    for (size_t i = 0; i < x.size(); ++i) {
        grad[i] = 2.0 * x[i];
    }
    return grad;
}

// ============================================================================
// FONCTIONS POUR LECTURE/ÉCRITURE CSV
// ============================================================================

/**
 * @brief Lit un vecteur depuis un fichier CSV.
 * @param filename Nom du fichier CSV.
 * @param vec Vecteur de sortie.
 * @return true si la lecture a réussi, false sinon.
 */
bool ReadVectorFromCSV(const std::string& filename, std::vector<double>& vec) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << std::endl;
        return false;
    }

    std::string line;
    if (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string token;
        vec.clear(); // Vider le vecteur avant de le remplir
        while (std::getline(iss, token, ',')) {
            // Supprimer les espaces
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            
            try {
                vec.push_back(std::stod(token));
            } catch (const std::exception& e) {
                std::cerr << "Erreur : Impossible de convertir " << token << " en double." << std::endl;
                return false;
            }
        }
    } else {
        std::cerr << "Erreur : Le fichier " << filename << " est vide." << std::endl;
        return false;
    }

    file.close();
    return true;
}

/**
 * @brief Écrit une valeur dans un fichier CSV.
 * @param filename Nom du fichier CSV.
 * @param value Valeur à écrire.
 * @return true si l'écriture a réussi, false sinon.
 */
bool WriteValueToCSV(const std::string& filename, double value) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << " en écriture." << std::endl;
        return false;
    }
    file << value << std::endl;
    file.close();
    return true;
}

/**
 * @brief Écrit un vecteur dans un fichier CSV.
 * @param filename Nom du fichier CSV.
 * @param vec Vecteur à écrire.
 * @return true si l'écriture a réussi, false sinon.
 */
bool WriteVectorToCSV(const std::string& filename, const std::vector<double>& vec) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << " en écriture." << std::endl;
        return false;
    }

    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) {
            file << ", ";
        }
        file << vec[i];
    }
    file << std::endl;

    file.close();
    return true;
}

// ============================================================================
// FONCTION PRINCIPALE
// ============================================================================

int main() {
    // Lire le vecteur x depuis x.csv
    std::vector<double> x;
    if (!ReadVectorFromCSV("x.csv", x)) {
        std::cerr << "Échec de la lecture de x.csv." << std::endl;
        return 1;
    }

    std::cout << "Vecteur x lu : [";
    for (size_t i = 0; i < x.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << x[i];
    }
    std::cout << "]" << std::endl;

    // Calculer la fonction coût f(x)
    double f = ComputeCostFunction(x);
    std::cout << "Valeur de la fonction coût : " << f << std::endl;

    // Écrire f(x) dans result.csv
    if (!WriteValueToCSV("result.csv", f)) {
        std::cerr << "Échec de l'écriture de result.csv." << std::endl;
        return 1;
    }

    // Calculer le gradient ∇f(x)
    std::vector<double> grad = ComputeGradient(x);
    std::cout << "Gradient : [";
    for (size_t i = 0; i < grad.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << grad[i];
    }
    std::cout << "]" << std::endl;

    // Écrire le gradient dans gradf.csv
    if (!WriteVectorToCSV("gradf.csv", grad)) {
        std::cerr << "Échec de l'écriture de gradf.csv." << std::endl;
        return 1;
    }

    std::cout << "Résultats écrits dans result.csv et gradf.csv." << std::endl;

    return 0;
}
