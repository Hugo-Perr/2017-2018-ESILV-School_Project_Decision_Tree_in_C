#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "arbre.h"






/*____________________________________________________________________________________________________________________________________________________*/
/*----------------------------------   PARTIE 0 : FONCTIONS PROPRES A L'UTILISATION DES DONNEES   --------------------------------------------------*/

matrice_donnees* charger_donnnees(const char* nom_fichier)
{
	FILE* fichier = fopen(nom_fichier,"r");
	if( fichier != NULL )
	{
		unsigned int nb_lignes;
		unsigned int nb_colonnes;

		// Etape 1 - traitement première ligne
		fscanf(fichier, "%u %u", &nb_lignes, &nb_colonnes); // %u <=> unsigned int
		fgetc(fichier); // lecture du retour chariot ('\n'). Valeur ignorée => passage à la ligne suivante

		// Etape 2 - allocation des lignes de la matrice
		double** matrice = (double**) malloc( nb_lignes * sizeof(double*) );

		// Etape 3 - remplissage de la matrice
		for(int ligne = 0 ; ligne < nb_lignes ; ligne++)
		{
			// allocation des colonnes de la matrice (pour chaque ligne)
			matrice[ligne] = (double*) malloc( nb_colonnes * sizeof(double) );

			for(int colonne = 0 ; colonne < nb_colonnes ; colonne++)
			{
				if(colonne != nb_colonnes-1)
				{
					fscanf(fichier, "%lg ", &matrice[ligne][colonne]);
				}
				else // dernière colonne (où le réel n'est pas suivi d'un espace)
				{
					fscanf(fichier, "%lg" , &matrice[ligne][colonne]);
				}
			}

			fgetc(fichier); // lecture du retour chariot ('\n'). Valeur ignorée => passage à la ligne suivante
		}

		matrice_donnees * data = (matrice_donnees*) malloc(sizeof(matrice_donnees));
		data->nb_colonnes = nb_colonnes;
		data->nb_lignes = nb_lignes;
		data->matrice = matrice;

		fclose(fichier);
		return data;
	}

	printf("Fichier %s inconnu.\n", nom_fichier);
	return NULL;
}

// Usage : var = liberer_donnees(var);  => var devient NULL
matrice_donnees* liberer_donnees(matrice_donnees * data)
{
	if(data != NULL)
	{
		for(int ligne = 0 ; ligne < data->nb_lignes ; ligne++)
		{
			if(data->matrice[ligne] != NULL) // normalement toujours vrai
				free(data->matrice[ligne]);
		}
		free(data->matrice);
		free(data);
	}
	return NULL;
}

/*____________________________________________________________________________________________________________________________________________________*/
/*----------------------------------   PARTIE 0 (bis) : FONCTIONS PROPRES A L'UTILISATION DE L'ARBRE   --------------------------------------------------*/

noeud * creer_noeud(noeud * pere_du_noeud,table_donnees * table, int taille_table, int hauteur_parent_du_nouveau_noeud, double mediane_ut,double precision_y,int branche)
{
    noeud * nouveau_noeud = (noeud*) malloc (sizeof(noeud));
    if (nouveau_noeud!=NULL)
    {
        nouveau_noeud->table=table;
        nouveau_noeud->taille_table=taille_table;

        nouveau_noeud->fils_droite=NULL;
        nouveau_noeud->fils_gauche=NULL;
        nouveau_noeud->pere = pere_du_noeud;
        nouveau_noeud->hauteur=hauteur_parent_du_nouveau_noeud+1;

        nouveau_noeud->parametre_decision=0; // CAD pas encore defini
        nouveau_noeud->precision_sur_y=precision_y;
        nouveau_noeud->mediane_utilisee=mediane_ut;
        nouveau_noeud->branche=branche;
        return nouveau_noeud;
    }
}


bool associer_fils_gauche(noeud * parent, noeud * enfant)
{
    bool reussite = false ;

    if (parent != NULL && enfant!=NULL && parent->fils_gauche==NULL)
    {
        parent->fils_gauche = enfant;
        reussite= true;
    }
    return reussite;
}


bool associer_fils_droite(noeud * parent, noeud * enfant)
{
    bool reussite = false ;

    if (parent != NULL && enfant!=NULL && parent->fils_droite==NULL)
    {
        parent->fils_droite=enfant;
        reussite= true;
    }
    return reussite;
}


bool est_feuille(noeud const* element)
{
    bool reussite = false;
    if (element!=NULL && element->fils_droite==NULL && element->fils_gauche==NULL)
    {
        reussite=true;
    }
    return reussite;
}

/*____________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   PARTIE 0 : Création de l'arbre    ------------------------------------------------------------*/


void calcul_mediane (double * echantillon_trie,  int taille_ech, double * mediane)
{
     int p;
    int n=taille_ech; // n est le nombre d'elements
    if ( n%2 == 0) // si le nombre d'elements de l'echantillon est pair
    {
        p= n/2 ;
        *mediane = ( *(echantillon_trie+ p-1) + *(echantillon_trie+p) )/2;
    }

    else // si le nombre d'elements de l'echantillon est impair
    {
        p=(n+1)/2;
        *mediane = *(echantillon_trie + p-1);
    }
}


bool val_ech_id (double * echantillon_trie,  int taille_ech)// renvoie TRUE si toutes le svaleurs de l'echantillon sont identiques
{
    bool identique = true;
    for (int i=0;i<taille_ech;i++)
    {
        if (echantillon_trie[i]!=echantillon_trie[0])
        {
            identique = false;
        }
    }
    return identique;
}

void calcul_mediane_corrigee(double * echantillon_trie,  int taille_ech, double * mediane_corrigee)
{
    *mediane_corrigee = -1 ;
    int n=taille_ech+1; // n est le nombre d'elements

    double mediane;
    calcul_mediane(echantillon_trie,taille_ech,&mediane);
    if (n>=2 || val_ech_id(echantillon_trie,taille_ech)==false )
    {
        if (mediane != echantillon_trie[taille_ech-1]) // si la mediane n'est PAS la valeur maximale de l'echantillon
        {
            *mediane_corrigee = mediane;
        }
        else
        {
            int i = taille_ech-2;
            while (i>=0 && echantillon_trie[i]==echantillon_trie[taille_ech-1])
            {
                i--;
            }
            double mediane_inter=echantillon_trie[i];
            *mediane_corrigee=mediane_inter; // soit la 2ème plus grande valeur de l'echantillon
        }
    }
}

void permuter(double * p_nb1, double * p_nb2)   // il faut ecrire permuter_cor(&varable1, &variable2) car on permute les ADRESSES MEMOIRES
{
    if((p_nb1 != NULL) && (p_nb2 != NULL))
    {
        double temp = *p_nb1;
        *p_nb1=*p_nb2;    // "*" signifie la valeur de .....  , ce qu'on modifie ici c'est la VALEUR associee au pointeur ...
        *p_nb2= temp ;
    }
}


void tri_bulle (double * tab, int taille_tableau)
{
    int born_inf = 0;
    for(int i=(taille_tableau-1); i>=0; i--)
    {
        for (int j=0; j<i;j++)
        {
            if (*(tab+j+1) < *(tab+j))
            {
                permuter(&tab[j+1],&tab[j]);
            }
        }
    }
}

double max(double reel_1, double reel_2)
{
    double maximum=reel_1;
    if (reel_2>maximum)
    {
        maximum=reel_2;
    }
    return maximum;
}

int param_max_precision (double table_precision[], int taille_table_precision) // renvoie le parametre de tri ayant le max de precision
{
    double maximum=table_precision[0];
    double maximum_inter=0;
    int param_decision_du_max = 0;

    for (int i=0;i<taille_table_precision;i++)
    {
        maximum_inter=max(maximum,table_precision[i]);

        if (maximum_inter>maximum)
        {
            maximum=maximum_inter;
            param_decision_du_max=i;
        }
    }
    return (param_decision_du_max + 1);
}

double calculer_precision(table_donnees * table,int taille_table,int val_y)
{
    double precision=0;
    double compteur_val_y=0;
    double val_y_float = val_y;
    for (int i=0; i<taille_table; i++)
    {
        if (table[i].val_y == val_y_float)
        {
            compteur_val_y++;
        }
    }
    double taille_table_float = taille_table;
    precision=(compteur_val_y)/(taille_table_float);
    return precision;
}


void choisir_parametre_decision (table_donnees * table_interne_du_noeud, int taille_table_interne, int val_y_choisie, int* parametre_decision_a_choisir, double* mediane) // renvoie le parametre a utiliser(sinon 0), ainsi que la mediane (sinon0)
{
    double val_y = val_y_choisie; // ceci permet de comparer le numero avec les reels du tableau interne
    double taille_table_interne_float = taille_table_interne;
    double table_des_precisions_max[] = {0,0,0,0};
    double table_des_medianes[] = {0,0,0,0};
    double mediane_test;

    double compteur_y_gauche; // comptera le nombre d'elements du tableau correspondant à la valeur Y choisie
    double compteur_y_droite;
    double compteur_element_gauche;
    double precision_max_test;
    double precision_gauche_test;
    double precision_droite_test;

    for (int parametre_teste = 1; parametre_teste<5 ; parametre_teste++)
    {
        mediane_test = 0; // initialisation parametres  test
        compteur_y_gauche=0;
        compteur_y_droite=0;
        compteur_element_gauche=0;
        precision_max_test=0;
        precision_gauche_test=0;
        precision_droite_test=0;

        double table_de_test[taille_table_interne]; // contiendra toutes les valeurs du Xi testé
        for (int i=0 ; i<taille_table_interne ; i++)
        {
            table_de_test[i] = table_interne_du_noeud[i].param_decision[parametre_teste-1]; // (-1) car le tab interne commence à 0
        }
        tri_bulle(&table_de_test,taille_table_interne);
        calcul_mediane_corrigee(&table_de_test,taille_table_interne,&mediane_test);
        for (int i=0; i<taille_table_interne ; i++)
        {

            if (table_interne_du_noeud[i].param_decision[parametre_teste-1] <= mediane_test) // c.a.d   ferait parti du fils_gauche
            {
                compteur_element_gauche = compteur_element_gauche + 1 ;
                if (table_interne_du_noeud[i].val_y == val_y)
                {
                    compteur_y_gauche+=1;
                }
            }
            else
            {
                if (table_interne_du_noeud[i].val_y == val_y)
                {
                    compteur_y_droite+=1;
                }
            }
        }
        precision_gauche_test= ( compteur_y_gauche ) / ( compteur_element_gauche ) ;
        precision_droite_test = compteur_y_droite/(taille_table_interne_float - compteur_element_gauche);
        precision_max_test = max(precision_gauche_test,precision_droite_test);
        table_des_precisions_max[parametre_teste-1]=precision_max_test;
        table_des_medianes[parametre_teste-1]=mediane_test; // permet de retenir les medianes calculees pour pas a le refaire ensuite
    }
    *parametre_decision_a_choisir = 0 ;
    * mediane = 0;
    if (val_ech_id(&table_des_precisions_max,4)== false) // si un parametre permet d'obtenir une meilleure precision (sinon pas de division car tous param identiques)
    {
        int param_max =  param_max_precision(table_des_precisions_max,4);
        *parametre_decision_a_choisir = param_max;
        *mediane= table_des_medianes[param_max-1];
    }

}



table_donnees * creer_un_element_de_la_table (matrice_donnees * matrice,int numero_ligne)
{
    table_donnees * nouvel_elment = (table_donnees*) malloc(sizeof(table_donnees));
    double * elements_decision = (double*) malloc(4*sizeof(double));
    if (nouvel_elment!=NULL && elements_decision!=NULL)
    {
        nouvel_elment->val_y = matrice->matrice[numero_ligne][0];
        for (int i=0;i<4;i++)
        {
            *(elements_decision+i)=matrice->matrice[numero_ligne][i+1];
        }
        nouvel_elment->param_decision=elements_decision;

        return nouvel_elment;
    }
}


table_donnees * initialisation_donnees(const char* nom_fichier,  int * taille_table) // taille table permet de recuperer la taille du 1er tableau interne
{
    matrice_donnees * matrice = NULL;
    matrice = charger_donnnees("iris.txt");

    table_donnees    * table = (table_donnees*) malloc((matrice->nb_lignes)*sizeof(table_donnees));
    if (table!=NULL) // malloc OK
    {
        table_donnees element_ajoute ;
        for (int i=0;i<matrice->nb_lignes;i++)
        {
            element_ajoute = *(  creer_un_element_de_la_table(matrice,i) );
            //printf("L'element ajoute NUMERO %d a VAL Y : \n", i, element_ajoute.val_y);
            *(table+i)= element_ajoute ;

        }
        *taille_table=matrice->nb_lignes;
        return table;
    }

}


noeud * initialisation_arbre(int val_Y)
{
    int taille_1er_tableau=0;
    table_donnees * tableau_interne = NULL;
    tableau_interne = initialisation_donnees("iris.txt",&taille_1er_tableau);
    double precision_table = calculer_precision(tableau_interne,taille_1er_tableau,val_Y);

    noeud * arbre = NULL; // initialisation
    noeud * pere_de_la_racine = NULL; // evidement
    arbre = creer_noeud(pere_de_la_racine,tableau_interne,taille_1er_tableau,0,0,precision_table,0);

    arbre->branche=0; // puisque c'est la racine
    return arbre;
}


int peut_on_diviser_arbre (noeud * noeud,int val_y,int hauteur_max,int nb_individu_min,double precision_min,double precision_max,double*mediane) // renvoie 0 si on peut pas, sinon le n° du parametre a utiliser
{
    int resultat = -1;
    *mediane = 0;

    if(noeud->hauteur>=hauteur_max  || noeud->taille_table<=nb_individu_min  ||  noeud->precision_sur_y<precision_min  || noeud->precision_sur_y>precision_max)
    {
        resultat=0;
    }

    else
    {
        choisir_parametre_decision(noeud->table,noeud->taille_table,val_y,&resultat,mediane);
    }

    return resultat;
}


noeud * creer_fils_gauche (noeud* parent,int parametre_decision_a_utiliser,double mediane_a_utiliser,int val_y)
{
    table_donnees    * table_g = (table_donnees*) malloc((parent->taille_table)*sizeof(table_donnees));
    int taille_g=0;
    double precision_g=0;
    if (table_g!=NULL)
    {
        for (int i=0; i<parent->taille_table ; i++)
        {
            if (parent->table[i].param_decision[parametre_decision_a_utiliser-1]<= mediane_a_utiliser ) // remplissage tableau de gauche
            {
                table_g[taille_g] = parent->table[i];
                taille_g++;
            }
        }
        free(table_g+taille_g);// liberation espace memoire non necessaire pour stocker les tableaux internes
        precision_g=calculer_precision(table_g,taille_g,val_y);
        noeud * fils_g = NULL;
        fils_g = creer_noeud(parent,table_g,taille_g,parent->hauteur,mediane_a_utiliser,precision_g,1);
        bool associer_gauche = associer_fils_gauche(parent,fils_g);
        return fils_g;
    }
}

noeud * creer_fils_droite (noeud* parent,int parametre_decision_a_utiliser,double mediane_a_utiliser,int val_y)
{
    table_donnees    * table_g = (table_donnees*) malloc((parent->taille_table)*sizeof(table_donnees));
    int taille_g=0;
    double precision_g=0;
    if (table_g!=NULL)
    {
        for (int i=0; i<parent->taille_table ; i++)
        {
            if (parent->table[i].param_decision[parametre_decision_a_utiliser-1]> mediane_a_utiliser ) // remplissage tableau de gauche
            {
                table_g[taille_g] = parent->table[i];
                taille_g++;
            }
        }
        free(table_g+taille_g);// liberation espace memoire non necessaire pour stocker les tableaux internes
        precision_g=calculer_precision(table_g,taille_g,val_y);
        noeud * fils_d = NULL;
        fils_d = creer_noeud(parent,table_g,taille_g,parent->hauteur,mediane_a_utiliser,precision_g,2);
        bool associer_gauche = associer_fils_gauche(parent,fils_d);
        return fils_d;
    }
}


void creer_arbre_recursivement (noeud * noeud_,int val_y,int hauteur_max,int nb_individu_min,double precision_min,double precision_max)
{
    double mediane=0;
    int hauteur = noeud_->hauteur;

    int parametre_division_a_utiliser = peut_on_diviser_arbre(noeud_,val_y,hauteur_max,nb_individu_min,precision_min,precision_max,&mediane);

    if (parametre_division_a_utiliser>0 && parametre_division_a_utiliser<5)
    {
        noeud * fils_g = NULL;
        noeud * fils_d = NULL;
        fils_g = creer_fils_gauche(noeud_,parametre_division_a_utiliser,mediane,val_y);
        fils_d = creer_fils_droite(noeud_,parametre_division_a_utiliser,mediane,val_y);
        fils_g->parametre_decision=parametre_division_a_utiliser;
        fils_d->parametre_decision=parametre_division_a_utiliser;
        bool asso_1 = associer_fils_gauche(noeud_,fils_g);
        bool asso_2 = associer_fils_droite(noeud_,fils_d);

        creer_arbre_recursivement(fils_g,val_y,hauteur_max,nb_individu_min,precision_min,precision_max);
        creer_arbre_recursivement(fils_d,val_y,hauteur_max,nb_individu_min,precision_min,precision_max);
    }
}

/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 1 : afficher la hauteur de l'arbre    ------------------------------------------------------------*/

void rechercher_hauteur_arbre(noeud const * arbre,int * hauteur_max)
{
	noeud * tmp = arbre;
	if (tmp != NULL)
	{
		// on affiche le premier noeud
		if (tmp->hauteur > *hauteur_max)
        {
            *hauteur_max = tmp->hauteur;
        }
		rechercher_hauteur_arbre(tmp -> fils_gauche,hauteur_max);
		rechercher_hauteur_arbre(tmp -> fils_droite,hauteur_max);
	}
}

void fonctionnalite_afficher_hauteur (noeud* arbre)
{
    int hauteur = 0;
    rechercher_hauteur_arbre(arbre,&hauteur);
    printf("La hauteur de l'arbre est : %d.\n",hauteur);
}


/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 2 : afficher la largeur de l'arbre    ------------------------------------------------------------*/

void rechercher_largeur_arbre(noeud const * arbre,int * compteur_de_feuilles)
{
	noeud * tmp = arbre;
	if (tmp != NULL)
	{
		bool est_une_feuille = est_feuille(tmp);
		if (est_une_feuille==true)
        {
            *compteur_de_feuilles +=1; // on incremente le compteur dès que l'on trouve une feuille
        }
		rechercher_largeur_arbre(tmp -> fils_gauche,compteur_de_feuilles);
		rechercher_largeur_arbre(tmp -> fils_droite,compteur_de_feuilles);
	}
}

void fonctionnalite_afficher_largeur (noeud* arbre)
{
    int largeur = 0;
    rechercher_largeur_arbre(arbre,&largeur);
    printf("La largeur de l'arbre est : %d.\n",largeur);
}

/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 3 : afficher l'arbre    ------------------------------------------------------------*/


// Affiche d'un certain nombre d'espaces, proportionnel à offset
void afficher_offset(int offset)
{
	for(int i = 0 ; i < offset ; i++)
	{
		printf("  "); // 2 espaces
	}
}

void affichage_en_fonction_du_type_de_noeud(noeud* noeud)
{
    if (noeud->branche==1)//si le noeud est un fils_gauche
    {
        printf("taille : %d, precision=%f%%,  chemin : X%d <= %f ", noeud->taille_table,noeud->precision_sur_y,noeud->parametre_decision,noeud->mediane_utilisee);
    }
    if (noeud->branche==2)//si le noeud est un fils_droite
    {
        printf("taille : %d, precision=%f%%,  chemin : X%d >  %f  ", noeud->taille_table,noeud->precision_sur_y,noeud->parametre_decision,noeud->mediane_utilisee);
    }
    if (noeud->branche==0)
    {
        printf("taille : %d, precision=%f%% ", noeud->taille_table,noeud->precision_sur_y);
    }

}


// Affichage récursif <=> parcours préfixé
// Précondition : l'arbre peut-être NULL (dans ce cas rien ne s'affiche)
// Initialisation : offset = 0
void affichage_arborescence(noeud * arbre, int offset)
{
	if(arbre != NULL)
	{
		// Etape 1 - afficher la valeur
		printf("\n");
		afficher_offset(offset);
		if( offset != 0 ) // tous les éléments sauf la racine
		{
			printf("|-");
		}
		affichage_en_fonction_du_type_de_noeud(arbre);

		// Etape 2 - appel récursif avec sous-arbre gauche
			// Si à gauche (et uniquement à gauche) c'est NULL on affiche "|-x"
			if( !est_feuille(arbre) && (arbre->fils_gauche == NULL) )
			{
				printf("\n");
				afficher_offset(offset+1);
				printf("|-x");
			}
		affichage_arborescence(arbre->fils_gauche, offset+1);


		// Etape 2 - appel récursif avec sous-arbre de droite

			// Si à droite (et uniquement à droite) c'est NULL on affiche "|-x"
			if( !est_feuille(arbre) && (arbre->fils_droite == NULL) )
			{
				printf("\n");
				afficher_offset(offset+1);
				printf("|-x");
			}
		affichage_arborescence(arbre->fils_droite, offset+1);
	}
	//else <=> arrêt de la récursivité
}

void fonctionnalite_afficher_arbre(noeud* arbre)
{
    printf("Affichage de l'arbre : \n");
    affichage_arborescence(arbre,2);
}

/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 4 : afficher les feuilles de l'arbre    ------------------------------------------------------------*/

void retrouver_chemin_feuille(noeud const * noeud_)
{
    noeud * tmp = noeud_;
    int compteur_remplissage_tab = 0;
    double tab_medianes_utilisees[noeud_->hauteur-1]; // contiendra le chemin en medianes de la racine à la feuille
    int tab_branches_utilisees[noeud_->hauteur-1];
    int tab_param_utilise[noeud_->hauteur-1];

    while (tmp->pere != NULL) // tant qu'on arrive pas a la racine
    {
        tab_medianes_utilisees[compteur_remplissage_tab]=tmp->mediane_utilisee;
        tab_branches_utilisees[compteur_remplissage_tab]=tmp->branche;
        tab_param_utilise[compteur_remplissage_tab]=tmp->parametre_decision;
        tmp=tmp->pere;
        compteur_remplissage_tab++;
    }
    for (int i= (noeud_->hauteur-2); i>=0 ; i--)  // on lit les tableaux a l'envers pour retrouve rle chemin dans le bon sens
    {
        if (tab_branches_utilisees[i]==1)
        {
            printf("X%d <= %f\n",tab_param_utilise[i],tab_medianes_utilisees[i]);
        }
        else
        {
            printf("X%d >  %f\n",tab_param_utilise[i],tab_medianes_utilisees[i]);
        }
    }
}



void afficher_chemin_feuille (noeud const * arbre)
{
	noeud * tmp = arbre;
	if (tmp != NULL)
	{
		bool est_une_feuille = est_feuille(tmp);
		if (est_une_feuille == true)
        {
            retrouver_chemin_feuille(tmp);
            printf("\n");

        }
		afficher_chemin_feuille(tmp -> fils_gauche);
		afficher_chemin_feuille(tmp -> fils_droite);
	}
}


void fonctionnalite_afficher_chemins_feuilles(noeud * arbre)
{
    printf("Affichage des chemins des feuilles (par bloc) :\n");
    afficher_chemin_feuille(arbre);
}

/*________________________________________________________________________________________________________________________________________________________________________*/
/*---------------------------------------------------   FONCTIONNALITE 5 : prédire    ------------------------------------------------------------*/

double * element_a_predire()
{
    double * tab_val_x = (double*) malloc (4*sizeof(double));
    float val_tmp=0;
    double tmp_conv = 0;

    if (tab_val_x!=NULL)
    {
       for (int i=0; i<4 ; i++)
       {
           printf("Entrez la valeur de la variable X%d : ",i+1);
           scanf("%f",&val_tmp);
           tmp_conv = val_tmp;
           *(tab_val_x+i)=tmp_conv;
           val_tmp=0;
        }
        return tab_val_x;
    }

}



void fonctionnalite_predire(noeud const * arbre,int val_y)
{
    double * element_inconnu = NULL;
    element_inconnu = element_a_predire();
    noeud * tmp = arbre;
    while (tmp->fils_gauche != NULL) // on divise chaque fois en deux donc pas besoin de verifier avec fils_droite
    {
        if (element_inconnu[1+tmp->fils_gauche->parametre_decision]   <=  tmp->fils_gauche->mediane_utilisee)
        {
            tmp = tmp->fils_gauche ;
        }
        else
        {
            tmp = tmp->fils_droite;
        }
    }
    double pourcentage_d_etre_type_y = tmp->precision_sur_y * 100;
    printf("L'element inconnu a %f%% de chance d'etre un type %d.\n",pourcentage_d_etre_type_y,val_y);



}


/*____________________________________________________________________________________________________________________________________________________*/
/*-----------------------------------------------------------    MENU     ----------------------------------------------------------------------*/

void afficher_menu(noeud * arbre,int val_y) // affichage du menu et des choix
{
    printf(" - MENU - \n");
    printf("1. Afficher la hauteur de l'arbre  \n2. Afficher la largeur de l'arbre  \n");
    printf("3. Afficher l'arbre  \n4. Afficher les feuilles  \n5. Predire \n6. Quitter\n\n");
    int choix;
    printf("Quel est votre choix ? : ");
    scanf("%d",&choix);

    if (choix==1)
    {
        fonctionnalite_afficher_hauteur(arbre);
    }

    if (choix==2)
    {
        fonctionnalite_afficher_largeur(arbre);
    }

    if (choix==3)
    {
        fonctionnalite_afficher_arbre(arbre);
    }

    if (choix==4)
    {
        fonctionnalite_afficher_chemins_feuilles(arbre);
    }

    if (choix==5)
    {
        fonctionnalite_predire(arbre,val_y);
    }
    if (choix==6)
    {
        free(arbre);
        exit(0);
    }
    printf("\n\n");
    afficher_menu(arbre,val_y);  // reafficher le menu apres chaque execution de commande

}

void initialisation_parametres_utilisateur(int * val_y,int * hauteur_max,int * nb_individu_min, double * precision_min, double * precision_max)
{
    while (*val_y!=1 && *val_y!=2  && *val_y!=3 )
    {
        printf("Quelle est le numero de l'espece a predire ?  : ");
        scanf("%d",val_y);
    }
    while (*hauteur_max<=0)
    {
        printf("Quelle est la hauteur maximale voulue ?  : ");
        scanf("%d",hauteur_max);
    }
    while (*nb_individu_min<=0)
    {
        printf("Quelle est le nombre d'individus minimum par noeud ?  : ");
        scanf("%d",nb_individu_min);
    }
    int precisionmin_en_pourcentage = 0;
    while (precisionmin_en_pourcentage<=0 || precisionmin_en_pourcentage>100)
    {
        printf("Quelle est la precision minimale (en %%) voulue (nombre entier) ?  : ");
        scanf("%d",&precisionmin_en_pourcentage);
        double precision_convertie_1 = precisionmin_en_pourcentage;
        *precision_min= (precision_convertie_1/100.0);
    }
    int precisionmax_en_pourcentage = 0;
    while (precisionmax_en_pourcentage<=0 || precisionmax_en_pourcentage>100 || precisionmax_en_pourcentage<precisionmin_en_pourcentage)
    {
        printf("Quelle est la precision maximale (en %%) voulue (nombre entier) ?  : ");
        scanf("%d",&precisionmax_en_pourcentage);
        double precision_convertie_2 = precisionmax_en_pourcentage;
        *precision_max= (precision_convertie_2/100.0);
    }

}


void programe_complet()
{

    int val_y = 0;
    int hauteur_max = 0;
    int nb_individu_min = 0;
    double precision_min = 0;
    double precision_max =0;
    initialisation_parametres_utilisateur(&val_y,&hauteur_max,&nb_individu_min,&precision_min,&precision_max);


    double mediane=1;
    noeud * arbre = NULL;

    arbre=initialisation_arbre(val_y);
    creer_arbre_recursivement(arbre,val_y,hauteur_max,nb_individu_min,precision_min,precision_max);

    afficher_menu(arbre,val_y);





}




