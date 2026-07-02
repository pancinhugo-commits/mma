#!/usr/bin/env python3
"""
Script pour exécuter MMA en boucle jusqu'à convergence.

Ce script :
1. Lit les fichiers x_old.csv et gradf.csv.
2. Exécute le programme optimisation pour mettre à jour x.
3. Vérifie la convergence en comparant x_old.csv et x_new.csv.
4. Répète jusqu'à ce que le changement soit inférieur à une tolérance.
"""

import subprocess
import csv
import os
import sys
import math

# ============================================================================
# PARAMÈTRES
# ============================================================================

MAX_ITER = 100       # Nombre maximal d'itérations
TOL = 1e-5           # Tolérance pour la convergence (changement maximal dans x)

# Fichiers
X_OLD_FILE = "x_old.csv"  # Fichier d'entrée pour x
X_NEW_FILE = "x_new.csv"  # Fichier de sortie pour x
GRADF_FILE = "gradf.csv" # Fichier d'entrée pour le gradient

# ============================================================================
# FONCTIONS UTILITAIRES
# ============================================================================

def read_vector_from_csv(filename):
    """Lit un vecteur depuis un fichier CSV."""
    with open(filename, 'r') as f:
        reader = csv.reader(f)
        row = next(reader)
        return [float(val.strip()) for val in row]

def write_vector_to_csv(filename, vec):
    """Écrit un vecteur dans un fichier CSV."""
    with open(filename, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(vec)

def compute_max_change(x_old, x_new):
    """Calcule le changement maximal entre deux vecteurs."""
    if len(x_old) != len(x_new):
        raise ValueError("Les vecteurs doivent avoir la même taille.")
    return max(abs(x_new[i] - x_old[i]) for i in range(len(x_old)))

# ============================================================================
# VÉRIFIER LES FICHIERS D'ENTRÉE
# ============================================================================

if not os.path.exists(X_OLD_FILE):
    print(f"Erreur : Le fichier {X_OLD_FILE} n'existe pas.")
    sys.exit(1)

if not os.path.exists(GRADF_FILE):
    print(f"Erreur : Le fichier {GRADF_FILE} n'existe pas.")
    sys.exit(1)

# Vérifier que le programme optimisation est compilé
if not os.path.exists("./optimisation"):
    print("Compilation de optimisation.cpp...")
    result = subprocess.run(["g++", "-std=c++11", "-o", "optimisation", "optimisation.cpp", "-I."], 
                           capture_output=True, text=True)
    if result.returncode != 0:
        print("Erreur : La compilation a échoué.")
        print(result.stderr)
        sys.exit(1)

# ============================================================================
# BOUCLE PRINCIPALE
# ============================================================================

print("=" * 60)
print("Exécution de MMA en boucle jusqu'à convergence")
print("=" * 60)
print(f"Paramètres :")
print(f"  - Tolérance : {TOL}")
print(f"  - Itérations maximales : {MAX_ITER}")
print()

# Lire x_old initial
x_old = read_vector_from_csv(X_OLD_FILE)
print(f"Vecteur x initial : {x_old}")

for iter in range(1, MAX_ITER + 1):
    print(f"--- Itération {iter} ---")

    # Exécuter le programme MMA
    result = subprocess.run(["./optimisation"], capture_output=True, text=True)
    if result.returncode != 0:
        print("Erreur : L'exécution de optimisation a échoué.")
        print(result.stderr)
        sys.exit(1)

    # Lire x_new
    x_new = read_vector_from_csv(X_NEW_FILE)

    # Calculer le changement maximal
    ch = compute_max_change(x_old, x_new)
    print(f"  - Changement maximal : {ch}")

    # Vérifier la convergence
    if ch < TOL:
        print("Convergence atteinte !")
        break

    # Mettre à jour x_old pour la prochaine itération
    x_old = x_new
    write_vector_to_csv(X_OLD_FILE, x_old)

else:
    print(f"Nombre maximal d'itérations atteint ({MAX_ITER}).")

# ============================================================================
# AFFICHER LE RÉSULTAT FINAL
# ============================================================================

print()
print("Résultat final :")
print(f"  - Vecteur x : {x_new}")
print(f"  - Fichier de sortie : {X_NEW_FILE}")
