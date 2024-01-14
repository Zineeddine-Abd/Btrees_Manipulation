#include <stdio.h>
#include <stdlib.h>

#define DEGRE 2 //its (Degre/2) and i name it degre , and i choose Degre/2 because its better to manipulate

// Structure noeud
typedef struct Noeud {
    int cles[2 * DEGRE - 1];
    struct Noeud* Fils[2 * DEGRE];
    int EstFeuille;
    int NbrCle;
} Noeud;

// Structure arbre
typedef struct BArbre {
    Noeud* racine;
} BArbre;

// Fonction pour créer un nouveau nœud
Noeud* creer_noeud(int est_feuille) {
    Noeud* noeud = (Noeud*)malloc(sizeof(Noeud));
    noeud->EstFeuille = est_feuille;
    noeud->NbrCle = 0;

    for (int i = 0; i < 2 * DEGRE; ++i) {
        noeud->cles[i] = 0;
        noeud->Fils[i] = NULL;
    }
    return noeud;
}

// Fonction pour créer un nouvel arbre B
BArbre* creer_b_arbre() {
    BArbre* b_arbre = (BArbre*)malloc(sizeof(BArbre));
    b_arbre->racine = creer_noeud(1); // 1 indique un nœud feuille
    return b_arbre;
}

// Fonction pour diviser un enfant d'un nœud plein
//tu lui donne le parent et l'index de l'enfant a diviser
void diviser_enfant(Noeud* parent, int index) {
    //le fils a droite
    Noeud* nouveau_noeud = creer_noeud(parent->Fils[index]->EstFeuille);
    //le fils a gauche
    Noeud* enfant = parent->Fils[index];

    //parceque on vas insere dans le noeud perent
    parent->NbrCle++;

    //decalages des cles et des fils
    for (int i = parent->NbrCle - 1; i > index; --i) {
        parent->cles[i] = parent->cles[i - 1];
        parent->Fils[i + 1] = parent->Fils[i];
    }

    //insertion du nouveau cles
    //insertion du cle mediane du fils
    parent->cles[index] = enfant->cles[DEGRE - 1];
    //insertion du noeud quil vas etre la partie droite du fils divisee
    parent->Fils[index + 1] = nouveau_noeud;

    //par exemple si on a un noeud de Degre 2 (DEGRE-1 = 1) et c'est l'element mediane et le nombre de cles dans chaque fils divisee
    nouveau_noeud->NbrCle = DEGRE - 1;

    for (int i = 0; i < DEGRE - 1; ++i) {
        //affectant les cles du nouveau noeud (fils divisee a droite)
        //i + DEGRE  = 0 + 2 = 2 donc la dernire case si le degre est 2 (ou il ya 3 case 1ere dans le fils gauche la mediane inseree dans le parent l'adroite dans le fils droite)
        nouveau_noeud->cles[i] = enfant->cles[i + DEGRE];
        // apres le transfere des cles en mes les clees a0 pour indiquer quil sont vide
        enfant->cles[i + DEGRE] = 0;

        //cette etape n'est pas necessaire pour pour le nouveau noeud parcequ'elle est par defaut 0 dans le nouveau noeud
    }

    //si le nouveau noeud n'est pas une feuille (== le fils aussi pas une feuille) donc necessite la mise a jour des fils aussi
    if (!enfant->EstFeuille) {
        for (int i = 0; i < DEGRE; ++i) {
            nouveau_noeud->Fils[i] = enfant->Fils[i + DEGRE];
            //mettre a null dans le fils a droite apres le transfere
            enfant->Fils[i + DEGRE] = NULL;
        }
    }

    //mise a jour le nombre de cles du fils a droite
    enfant->NbrCle = DEGRE - 1;
}

// Fonction pour insérer une clé dans un nœud non plein
void inserer_non_plein(Noeud* noeud, int cle) {
    int i = noeud->NbrCle - 1;

    if (noeud->EstFeuille) {
        //just inserer le cle et sortir de la fonction
        while (i >= 0 && cle < noeud->cles[i]) {
            noeud->cles[i + 1] = noeud->cles[i];
            i--;
        }
        noeud->cles[i + 1] = cle;
        noeud->NbrCle++;
    } else {//for exemple if it is the root (not a leaf)
        //searching for the right child to insert in it
        // because if its not a leaf than we should look for the right leaf to insert in
        while (i >= 0 && cle < noeud->cles[i]) {
            i--;
        }
        i++;

        if (noeud->Fils[i]->NbrCle == 2 * DEGRE - 1) {
            diviser_enfant(noeud, i);

            if (cle > noeud->cles[i]) {
                i++;
            }
        }

        inserer_non_plein(noeud->Fils[i], cle);
    }
}

// Fonction pour insérer une clé dans l'arbre B
void inserer_b_arbre(BArbre* b_arbre, int cle) {
    Noeud* racine = b_arbre->racine;

    if (racine->NbrCle == 2 * DEGRE - 1) {
        Noeud* nouvelle_racine = creer_noeud(0); // 0 indique un nœud interne
        nouvelle_racine->Fils[0] = racine;
        b_arbre->racine = nouvelle_racine;
        diviser_enfant(nouvelle_racine, 0);
        inserer_non_plein(nouvelle_racine, cle);
    } else {
        inserer_non_plein(racine, cle);
    }
}

// Fonction pour rechercher une clé dans l'arbre B
int rechercher_b_arbre(Noeud* noeud, int cle) {
    int i = 0;
    while (i < noeud->NbrCle && cle > noeud->cles[i]) {
        i++;
    }

    if (i < noeud->NbrCle && cle == noeud->cles[i]) {
        return 1; // Clé trouvée
    } else if (noeud->EstFeuille) {
        return 0; // Clé non trouvée
    } else {
        return rechercher_b_arbre(noeud->Fils[i], cle); // Récursion vers le fils approprié
    }
}

void print_b_tree(Noeud* noeud, int level) {
    if (noeud != NULL) {
        int i;
        for (i = 0; i < noeud->NbrCle; i++) {
            // Recursively print the subtree rooted with child i
            print_b_tree(noeud->Fils[i], level + 1);

            // Print the key at the current node
            printf("Level %d, Key %d\n", level, noeud->cles[i]);
        }

        // Print the subtree rooted with the last child
        print_b_tree(noeud->Fils[i], level + 1);
    }
}

// Fonction pour afficehr B-arbre
void print_b_tree_wrapper(BArbre* b_arbre) {
    printf("Content of the B-tree:\n");
    print_b_tree(b_arbre->racine, 0);
    printf("\n");
}


int main() {
    BArbre* b_arbre = creer_b_arbre();

    int choice;
    int key;

    do {
        printf("\nMenu:\n");
        printf("1. Ajouter une cle\n");
        printf("2. Rechercher une cle\n");
        printf("3. Afficher l'arbre\n");
        printf("0. Quitter\n");
        printf("Choix : ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                printf("Entrez la cle a ajouter : ");
                scanf("%d", &key);
                inserer_b_arbre(b_arbre, key);
                print_b_tree_wrapper(b_arbre);
                break;

            case 2:
                printf("Entrez la cle a rechercher : ");
                scanf("%d", &key);
                if (rechercher_b_arbre(b_arbre->racine, key)) {
                    printf("Cle %d existe\n", key);
                } else {
                    printf("Cle %d n'existe pas\n", key);
                }
                break;

            case 3:
                print_b_tree_wrapper(b_arbre);
                break;

            case 0:
                printf("Au revoir!\n");
                break;

            default:
                printf("Choix invalide. Veuillez entrer 1, 2, 3 ou 0.\n");
        }
    } while (choice != 0);

    return 0;
}
