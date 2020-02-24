#ifndef DONNEES_H
#define DONNEES_H
#include <stdbool.h>

/*____________________________________________________________________________________________________________________________________________________*/
/*-------------------------------------------------------   Les STRUCTURES   --------------------------------------------------------------------------*/

typedef struct // structure de la matrice de donnees
{
	unsigned int nb_colonnes; // la 1ere <=> classe à prédire (Y). Autres colonnes <=> variables d'observatio (Xi)
	unsigned int nb_lignes;   // <=> les individus
	double** matrice;         // tableau de tableaux de réels (i.e. tableaux 2D de réels)
} matrice_donnees;


typedef struct _table_donnees // structure du tableau interne de chaque noeud
{
    double val_y;
    double * param_decision; // tableau contenant les valeurs de X1,X2,X3 et X4

}table_donnees;



typedef struct _noeud  // structure de chaque noeud de l'arbre de decision
{
    table_donnees * table;
    int taille_table;

    struct _noeud * fils_gauche;
    struct _noeud * fils_droite;
    struct _noeud * pere;
    int hauteur;

    double precision_sur_y;
    int parametre_decision; // correspond au parametre utilisé pour obtenir fils_droite et fils_gauche
    double mediane_utilisee; // la valeur utilisee pour separer fils_d et fils_g =>  (-1) pour la racine
    int branche; // le noeud est un fils gauche (=1) ou  un fils droit (=2) ; si racine : =0
}noeud;


typedef struct _individu // structure UNIQUEMENT utilisee lors du calcul de la mediane
{
    int val_y; // parametre caracteristique cad espece
    double Xi; // un parametre etudié
}individu;




/*____________________________________________________________________________________________________________________________________________________*/
/*-------------------------------------------------- FONCTIONS PROPRES A L'UTILISATION DES DONNEES   --------------------------------------------------*/

matrice_donnees* charger_donnnees(const char* nom_fichier);
// Usage : var = liberer_donnees(var);  => var devient NULL
matrice_donnees* liberer_donnees(matrice_donnees * data);

/*____________________________________________________________________________________________________________________________________________________*/
/*-------------------------------------------------  FONCTIONS PROPRES A L'UTILISATION DE L'ARBRE   --------------------------------------------------*/

noeud * creer_noeud(noeud * pere_du_noeud,table_donnees * table, int taille_table, int hauteur_parent_du_nouveau_noeud, double mediane_ut,double precision_y,int branche);
bool associer_fils_gauche(noeud * parent, noeud * enfant);
bool associer_fils_droite(noeud * parent, noeud * enfant);
bool est_feuille(noeud const* element);

/*____________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   PARTIE 0 : Création de l'arbre    ------------------------------------------------------------*/

void calcul_mediane (double * echantillon_trie,  int taille_ech, double * mediane);

bool val_ech_id (double * echantillon_trie,  int taille_ech);// renvoie TRUE si toutes le svaleurs de l'echantillon sont identiques

void calcul_mediane_corrigee(double * echantillon_trie,  int taille_ech, double * mediane_corrigee);

void permuter(double * p_nb1, double * p_nb2);   // il faut ecrire permuter_cor(&varable1, &variable2) car on permute les ADRESSES MEMOIRES

void tri_bulle (double * tab, int taille_tableau); // simple tri bulle

double max(double reel_1, double reel_2); // fonction max toute simple

int param_max_precision (double table_precision[], int taille_table_precision); // renvoie le parametre de tri ayant le max de precision

double calculer_precision(table_donnees * table,int taille_table,int val_y);

void choisir_parametre_decision (table_donnees * table_interne_du_noeud, int taille_table_interne, int val_y_choisie, int* parametre_decision_a_choisir, double* mediane);
 // choisir_parametre_decision  renvoie le parametre a utiliser(sinon 0), ainsi que la mediane (sinon0)

table_donnees * creer_un_element_de_la_table (matrice_donnees * matrice,int numero_ligne);

table_donnees * initialisation_donnees(const char* nom_fichier,  int * taille_table); // taille table permet de recuperer la taille du 1er tableau interne

noeud * initialisation_arbre(int val_Y);

int peut_on_diviser_arbre (noeud * noeud,int val_y,int hauteur_max,int nb_individu_min,double precision_min,double precision_max,double*mediane);
// peut_on_diviser_arbre renvoie 0 si on peut pas, sinon le n° du parametre a utiliser

noeud * creer_fils_gauche (noeud* parent,int parametre_decision_a_utiliser,double mediane_a_utiliser,int val_y);

noeud * creer_fils_droite (noeud* parent,int parametre_decision_a_utiliser,double mediane_a_utiliser,int val_y);

void creer_arbre_recursivement (noeud * noeud_,int val_y,int hauteur_max,int nb_individu_min,double precision_min,double precision_max);


/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 1 : afficher la hauteur de l'arbre    ------------------------------------------------------------*/

void rechercher_hauteur_arbre(noeud const * arbre,int * hauteur_max);

void fonctionnalite_afficher_hauteur (noeud* arbre);

/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 2 : afficher la largeur de l'arbre    ------------------------------------------------------------*/

void rechercher_largeur_arbre(noeud const * arbre,int * compteur_de_feuilles);

void fonctionnalite_afficher_largeur (noeud* arbre);

/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 3 : afficher l'arbre    ------------------------------------------------------------*/


// Affiche d'un certain nombre d'espaces, proportionnel à offset
void afficher_offset(int offset);

void affichage_en_fonction_du_type_de_noeud(noeud* noeud);

void affichage_arborescence(noeud * arbre, int offset);

void fonctionnalite_afficher_arbre(noeud* arbre);

/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 4 : afficher les feuilles de l'arbre    ------------------------------------------------------------*/

void retrouver_chemin_feuille(noeud const * noeud_);

void afficher_chemin_feuille (noeud const * arbre);

void fonctionnalite_afficher_chemins_feuilles(noeud * arbre);

/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 5 : prédire    ------------------------------------------------------------*/

double * element_a_predire();

void fonctionnalite_predire(noeud const * arbre,int val_y);

/*____________________________________________________________________________________________________________________________________________________*/
/*-----------------------------------------------------------    MENU     ----------------------------------------------------------------------*/

void afficher_menu(noeud * arbre,int val_y); // affichage du menu et des choix

void initialisation_parametres_utilisateur(int * val_y,int * hauteur_max,int * nb_individu_min, double * precision_min, double * precision_max);

void programe_complet(); // SEUL ET UNIQUE PROGRAMME A METTRE DANS LE MAIN



//         -   FIN    -


#endif
