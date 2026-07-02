/**
 * @file optimisation.cpp
 * @brief Programme pour lire des vecteurs depuis des fichiers CSV, appliquer MMA, et écrire le résultat.
 * 
 * Ce programme :
 *   1. Lit le vecteur x depuis un fichier x_old.csv.
 *   2. Lit le gradient de la fonction coût depuis un fichier gradf.csv.
 *   3. Applique une itération de l'algorithme MMA pour mettre à jour x.
 *   4. Écrit le nouveau vecteur x dans un fichier x_new.csv.
 * 
 * Format des fichiers CSV :
 *   - x_old.csv : x1, x2, ..., xn (ex: 4.0, 3.0, 2.0)
 *   - gradf.csv : df/dx1, df/dx2, ..., df/dxn (ex: 8.0, 6.0, 4.0)
 *   - x_new.csv : x1_new, x2_new, ..., xn_new (sortie)
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "optimisation.h"

// ============================================================================
// UTILITY FUNCTIONS FOR CSV I/O
// ============================================================================

/**
 * @brief Lit un vecteur depuis un fichier CSV.
 * @param filename Nom du fichier CSV.
 * @param vec Vecteur de sortie (doit être déjà alloué avec la bonne taille).
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
        size_t i = 0;
        while (std::getline(iss, token, ',')) {
            // Supprimer les espaces éventuels
            token.erase(0, token.find_first_not_of(" \t"));
            token.erase(token.find_last_not_of(" \t") + 1);
            
            if (i >= vec.size()) {
                std::cerr << "Erreur : Le fichier " << filename << " contient plus de valeurs que prévu." << std::endl;
                return false;
            }
            
            try {
                vec[i] = std::stod(token);
            } catch (const std::exception& e) {
                std::cerr << "Erreur : Impossible de convertir la valeur " << token << " en double." << std::endl;
                return false;
            }
            i++;
        }
        
        if (i != vec.size()) {
            std::cerr << "Erreur : Le fichier " << filename << " ne contient pas assez de valeurs." << std::endl;
            return false;
        }
    } else {
        std::cerr << "Erreur : Le fichier " << filename << " est vide." << std::endl;
        return false;
    }

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
// MAIN FUNCTION
// ============================================================================

int main() {
    // Dimensions du problème (à adapter selon votre cas)
    const int n = 3; // Nombre de variables de conception
    const int m = 0; // Nombre de contraintes (0 pour un problème sans contraintes)

    // Initialiser les vecteurs
    std::vector<double> x(n);          // Variables de conception
    std::vector<double> gradf(n);      // Gradient de la fonction coût
    std::vector<double> xmin(n, 0.0);   // Bornes inférieures (ex: 0.0 pour toutes les variables)
    std::vector<double> xmax(n, 5.0);   // Bornes supérieures (ex: 5.0 pour toutes les variables)

    // Lire x depuis x_old.csv
    if (!ReadVectorFromCSV("x_old.csv", x)) {
        std::cerr << "Échec de la lecture de x_old.csv." << std::endl;
        return 1;
    }
    std::cout << "Vecteur x initial : [";
    for (size_t i = 0; i < x.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << x[i];
    }
    std::cout << "]" << std::endl;

    // Lire le gradient depuis gradf.csv
    if (!ReadVectorFromCSV("gradf.csv", gradf)) {
        std::cerr << "Échec de la lecture de gradf.csv." << std::endl;
        return 1;
    }
    std::cout << "Gradient de la fonction coût : [";
    for (size_t i = 0; i < gradf.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << gradf[i];
    }
    std::cout << "]" << std::endl;

    // Initialiser MMASolver
    // Paramètres : n (variables), m (contraintes), a0, c0, d0
    // Pour un problème sans contraintes, on peut utiliser m=0, a0=0, c0=0, d0=0
    MMASolver mma(n, m, 0.0, 1000.0, 1.0);

    // Vecteurs vides pour les contraintes (puisque m=0)
    std::vector<double> g(m, 0.0);
    std::vector<double> dgdx(n * m, 0.0);

    // Appliquer une itération de MMA
    // Note : Si m=0, les contraintes sont ignorées
    mma.Update(x.data(), gradf.data(), g.data(), dgdx.data(), xmin.data(), xmax.data());

    // Écrire le nouveau vecteur x dans x_new.csv
    if (!WriteVectorToCSV("x_new.csv", x)) {
        std::cerr << "Échec de l'écriture de x_new.csv." << std::endl;
        return 1;
    }

    std::cout << "Nouveau vecteur x : [";
    for (size_t i = 0; i < x.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << x[i];
    }
    std::cout << "]" << std::endl;

    std::cout << "Résultat écrit dans x_new.csv." << std::endl;

    return 0;
}
