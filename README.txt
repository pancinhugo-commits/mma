================================================================================
BIBLIOTHÈQUE MMA (Method of Moving Asymptotes) - README
================================================================================

Ce dépôt contient une implémentation en C++ de l'algorithme MMA (Method of 
Moving Asymptotes) pour l'optimisation sous contraintes, ainsi que des outils 
pour l'exécuter en boucle jusqu'à convergence.

--------------------------------------------------------------------------------
STRUCTURE DU PROJET
--------------------------------------------------------------------------------

Le projet est organisé comme suit :

1. BIBLIOTHÈQUE ET EN-TÊTES
   -----------------------
   - libMma.h
     * Fichier d'en-tête principal contenant la classe MMASolver.
     * Implémente l'algorithme MMA pour résoudre des problèmes d'optimisation
       sous contraintes.
     * Contient également des fonctions utilitaires pour l'algèbre linéaire
       (factorisation LU, résolution de systèmes linéaires).

2. PROGRAMMES PRINCIPAUX
   --------------------
   - optimisation.cpp
     * Programme qui utilise la classe MMASolver pour mettre à jour les variables
       de conception.
     * Lit les fichiers x_old.csv (variables) et gradf.csv (gradient).
     * Écrit le résultat dans x_new.csv.

   - fcout.cpp
     * Programme pour calculer la valeur de la fonction coût et son gradient.
     * Lit le vecteur x depuis x.csv.
     * Écrit la valeur de la fonction coût dans result.csv.
     * Écrit le gradient dans gradf.csv.
     * Par défaut, utilise f(x) = x1^2 + x2^2 + ... + xn^2.

   - main.cpp
     * Exemple d'utilisation de la classe MMASolver avec un problème d'optimisation
       sous contraintes.
     * Montre comment initialiser le solveur MMA et exécuter des itérations.

3. SCRIPTS D'EXÉCUTION
   -------------------
   - run_mma.sh
     * Script Bash pour exécuter MMA en boucle jusqu'à convergence.
     * À chaque itération :
       1. Appelle fcout pour recalculer le gradient.
       2. Appelle optimisation pour mettre à jour x.
       3. Vérifie la convergence en comparant x_old.csv et x_new.csv.
     * Utilise Python pour les calculs en virgule flottante.

   - run_mma.py
     * Script Python alternatif pour exécuter MMA en boucle.
     * Fonctionne de manière similaire à run_mma.sh.

4. FICHIERS CSV (DONNÉES D'ENTRÉE/SORTIE)
   --------------------------------------
   - x.csv
     * Fichier d'entrée pour fcout.cpp.
     * Contient les variables de conception (ex: 4.0, 3.0, 2.0).

   - x_old.csv
     * Fichier d'entrée pour optimisation.cpp.
     * Contient les variables de conception de l'itération précédente.

   - gradf.csv
     * Fichier d'entrée pour optimisation.cpp.
     * Contient le gradient de la fonction coût (ex: 8.0, 6.0, 4.0).
     * Généré par fcout.cpp.

   - x_new.csv
     * Fichier de sortie pour optimisation.cpp.
     * Contient les nouvelles variables de conception après une itération de MMA.

   - result.csv
     * Fichier de sortie pour fcout.cpp.
     * Contient la valeur de la fonction coût (ex: 29.0).

5. BINAIRES COMPILÉS
   ------------------
   - fcout
     * Binaire compilé de fcout.cpp.

   - optimisation
     * Binaire compilé de optimisation.cpp.

   - optimisation_example
     * Binaire compilé de main.cpp.

--------------------------------------------------------------------------------
COMMENT UTILISER LE PROJET
--------------------------------------------------------------------------------

1. COMPILATION
   ------------
   Pour compiler les programmes, exécutez :

   g++ -std=c++11 -o fcout fcout.cpp
   g++ -std=c++11 -o optimisation optimisation.cpp -I.
   g++ -std=c++11 -o optimisation_example main.cpp -I.

2. EXÉCUTION D'UNE ITÉRATION SIMPLE
   --------------------------------
   a. Préparez un fichier x.csv avec vos variables initiales (ex: 4.0, 3.0, 2.0).
   b. Exécutez fcout pour calculer le gradient :
      ./fcout
   c. Copiez x.csv vers x_old.csv :
      cp x.csv x_old.csv
   d. Exécutez optimisation pour mettre à jour x :
      ./optimisation
   e. Le résultat sera dans x_new.csv.

3. EXÉCUTION EN BOUCLE JUSQU'À CONVERGENCE
   ---------------------------------------
   Utilisez le script run_mma.sh ou run_mma.py :

   chmod +x run_mma.sh
   ./run_mma.sh

   ou

   python3 run_mma.py

   Ces scripts :
   - Recalculent le gradient à chaque itération avec fcout.
   - Appliquent MMA avec optimisation.
   - Vérifient la convergence (changement maximal < 1e-5 par défaut).

4. EXÉCUTION DE L'EXEMPLE (main.cpp)
   -----------------------------------
   ./optimisation_example

   Cet exemple montre comment utiliser MMASolver avec un problème d'optimisation
   sous contraintes (2 contraintes non linéaires).

--------------------------------------------------------------------------------
PERSONNALISATION
--------------------------------------------------------------------------------

1. CHANGER LA FONCTION COÛT
   -------------------------
   Modifiez les fonctions ComputeCostFunction et ComputeGradient dans fcout.cpp
   pour implémenter votre propre fonction coût et son gradient.

2. CHANGER LES PARAMÈTRES DE MMA
   ------------------------------
   Dans optimisation.cpp ou main.cpp, vous pouvez ajuster les paramètres de
   MMASolver :
   - asyminit : Distance initiale des asymptotes.
   - asymdec : Facteur de décroissance des asymptotes.
   - asyminc : Facteur d'augmentation des asymptotes.
   - move : Limite de déplacement des variables.

3. AJOUTER DES CONTRAINTES
   ------------------------
   Si votre problème a des contraintes, modifiez optimisation.cpp pour :
   - Lire les contraintes depuis un fichier CSV.
   - Passer les contraintes et leurs gradients à MMASolver::Update.

--------------------------------------------------------------------------------
DÉPENDANCES
--------------------------------------------------------------------------------

- Compilateur C++ (g++ ou clang++) avec support C++11.
- Python 3 (pour run_mma.py et les calculs dans run_mma.sh).
- Git (pour cloner le dépôt).

--------------------------------------------------------------------------------
AUTEUR
--------------------------------------------------------------------------------

Ce projet est inspiré du code MMA original et a été développé pour fournir une
implémentation modulaire et facile à utiliser de l'algorithme MMA en C++.

--------------------------------------------------------------------------------
LICENCE
--------------------------------------------------------------------------------

Ce projet est distribué sous licence libre. Vous êtes libre de l'utiliser, de le
modifier et de le redistribuer selon vos besoins.
