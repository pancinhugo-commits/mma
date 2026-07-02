#!/bin/bash

# ============================================================================
# Script pour exécuter MMA en boucle jusqu'à convergence
# Intègre fcout pour recalculer le gradient à chaque itération.
# ============================================================================

# Paramètres
MAX_ITER=100          # Nombre maximal d'itérations
TOL=0.00001           # Tolérance pour la convergence (changement maximal dans x)
ITER=0                # Compteur d'itérations

# Fichiers
X_FILE="x.csv"        # Fichier principal pour x (utilisé par fcout)
X_OLD="x_old.csv"     # Fichier d'entrée pour optimisation (copie de x.csv)
X_NEW="x_new.csv"     # Fichier de sortie pour optimisation
GRADF="gradf.csv"     # Fichier du gradient (généré par fcout)
RESULT="result.csv"   # Fichier du résultat (généré par fcout, non utilisé ici)

# Vérifier que les fichiers d'entrée existent
if [ ! -f "$X_FILE" ]; then
    echo "Erreur : Le fichier $X_FILE n'existe pas."
    exit 1
fi

# Vérifier que les programmes sont compilés
if [ ! -f "./fcout" ]; then
    echo "Compilation de fcout.cpp..."
    g++ -std=c++11 -o fcout fcout.cpp
    if [ $? -ne 0 ]; then
        echo "Erreur : La compilation de fcout.cpp a échoué."
        exit 1
    fi
fi

if [ ! -f "./optimisation" ]; then
    echo "Compilation de optimisation.cpp..."
    g++ -std=c++11 -o optimisation optimisation.cpp -I.
    if [ $? -ne 0 ]; then
        echo "Erreur : La compilation de optimisation.cpp a échoué."
        exit 1
    fi
fi

echo "==================================================="
echo "| Exécution de MMA en boucle jusqu'à convergence   |"
echo "==================================================="
echo ""
echo "Paramètres :"
echo "  - Tolérance : $TOL"
echo "  - Itérations maximales : $MAX_ITER"
echo ""

# Initialiser x_old.csv avec x.csv
cp "$X_FILE" "$X_OLD"

# Boucle principale
while [ $ITER -lt $MAX_ITER ]; do
    ITER=$((ITER + 1))
    echo "--- Itération $ITER ---"

    # 1. Recalculer le gradient avec fcout
    echo "  - Recalcul du gradient avec fcout..."
    ./fcout
    if [ $? -ne 0 ]; then
        echo "Erreur : L'exécution de fcout a échoué."
        exit 1
    fi

    # 2. Exécuter MMA avec optimisation
    echo "  - Mise à jour de x avec optimisation..."
    ./optimisation
    if [ $? -ne 0 ]; then
        echo "Erreur : L'exécution de optimisation a échoué."
        exit 1
    fi

    # 3. Calculer le changement maximal entre x_old.csv et x_new.csv
    # Lire x_old.csv
    IFS=',' read -ra X_OLD_ARRAY < "$X_OLD"
    # Lire x_new.csv
    IFS=',' read -ra X_NEW_ARRAY < "$X_NEW"

    # Calculer le changement maximal
    CH=0.0
    for i in "${!X_OLD_ARRAY[@]}"; do
        # Calculer la différence absolue
        DIFF=$(python3 -c "print(abs(${X_NEW_ARRAY[$i]} - ${X_OLD_ARRAY[$i]}))")
        # Comparer avec CH
        if python3 -c "print($DIFF > $CH)" | grep -q "True"; then
            CH=$DIFF
        fi
    done

    # Afficher le changement avec 6 décimales
    echo "  - Changement maximal : $CH"

    # 4. Vérifier la convergence
    if python3 -c "print($CH < $TOL)" | grep -q "True"; then
        echo "Convergence atteinte !"
        break
    fi

    # 5. Mettre à jour x.csv et x_old.csv pour la prochaine itération
    cp "$X_NEW" "$X_FILE"
    cp "$X_NEW" "$X_OLD"
done

if [ $ITER -ge $MAX_ITER ]; then
    echo "Nombre maximal d'itérations atteint ($MAX_ITER)."
fi

echo ""
echo "Résultat final :"
cat "$X_NEW"
echo ""
echo "Fichier de sortie : $X_NEW"
