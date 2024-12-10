#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef struct {
    char repas[200];
    int calories;
} DietPlan;
typedef struct {
    char exercices[200];
    int duree;
} WorkoutPlan;
typedef struct Personne {
    char nom[50];
    char prenom[50];
    float taille;
    float poids;
    float imc;
    char categorie[20];
    struct Personne* suivant;
} Personne;
typedef struct PilePersonne {
    Personne personne;
    struct PilePersonne* suivant;
} PilePersonne;

Personne* liste = NULL;
PilePersonne* Pile = NULL;
void empiler(Personne personne) {
    PilePersonne* nouveau = (PilePersonne*)malloc(sizeof(PilePersonne));
    nouveau->personne = personne;
    nouveau->suivant = Pile;
    Pile = nouveau;
}
void afficher_historique() {
    PilePersonne* courant = Pile;
    printf("\n--- Historique des consultations/modifications ---\n");
    while (courant) {
        printf("Nom: %s, Prénom: %s, Taille: %.2f m, Poids: %.2f kg, IMC: %.2f, Catégorie: %s\n",
               courant->personne.nom, courant->personne.prenom,
               courant->personne.taille, courant->personne.poids,
               courant->personne.imc, courant->personne.categorie);
        courant = courant->suivant;
    }
}
float calculer_imc_et_categorie(Personne* personne) {
    personne->imc = personne->poids / (personne->taille * personne->taille);
    if (personne->imc < 18.5) {
        strcpy(personne->categorie, "Sous-poids");
    } else if (personne->imc >= 18.5 && personne->imc <= 24.9) {
        strcpy(personne->categorie, "Normal");
    } else {
        strcpy(personne->categorie, "Surpoids");
    }
    return personne->imc;
}
void generer_diet(float imc, DietPlan* diet) {
    if (imc < 18.5) {
        strcpy(diet->repas, "Petit déjeuner : Yaourt grec, granola, fruits\nDéjeuner : Salade de quinoa, avocat, pois chiches\nDîner : Saumon, légumes rôtis, riz\n");
        diet->calories = 3000;
    } else if (imc >= 18.5 && imc <= 24.9) {
        strcpy(diet->repas, "Petit déjeuner : Flocons d'avoine\nDéjeuner : Protéines (viande, poisson)\nDîner : Poulet grillé ou tofu\n");
        diet->calories = 2000;
    } else {
        strcpy(diet->repas, "Petit déjeuner : Flocons d'avoine, fruits\nDéjeuner : Salade avec poulet grillé\nDîner : Poisson grillé, brocoli, patates douces\n");
        diet->calories = 1500;
    }
}
void generer_workout(float imc, WorkoutPlan* workout) {
    if (imc < 18.5) {
        strcpy(workout->exercices, "Course à pied, natation, HIIT, musculation, yoga, vélo\n");
        workout->duree = 30;
    } else if (imc >= 18.5 && imc <= 24.9) {
        strcpy(workout->exercices, "Pompes, squats, tractions, dips, Mountain Climbers\n");
        workout->duree = 45;
    } else {
        strcpy(workout->exercices, "Cardio (course, vélo), musculation, exercices au poids corporel\n");
        workout->duree = 90;
    }
}
void ajouter_personne(Personne* nouvelle) {
    nouvelle->suivant = liste;
    liste = nouvelle;
}
Personne* rechercher_personne(char* nom, char* prenom) {
    Personne* courant = liste;
    while (courant) {
        if (strcmp(courant->nom, nom) == 0 && strcmp(courant->prenom, prenom) == 0) {
            return courant;
        }
        courant = courant->suivant;
    }
    return NULL;
}
void sauvegarder_dans_fichier(const char* fichier) {
    FILE* file = fopen(fichier, "w");
    if (!file) {
        printf("Erreur d'accès au fichier.\n");
        return;
    }

    Personne* courant = liste;
    while (courant) {
        fprintf(file, "%s;%s;%.2f;%.2f;%.2f;%s\n",
                courant->nom, courant->prenom, courant->taille, courant->poids, courant->imc, courant->categorie);
        courant = courant->suivant;
    }

    fclose(file);
}
void charger_depuis_fichier(const char* fichier) {
    FILE* file = fopen(fichier, "r");
    if (!file) return;

    char ch[256];
    while (fgets(ch, sizeof(ch), file)) {
        Personne* nouvelle = (Personne*)malloc(sizeof(Personne));
        if (sscanf(ch, "%[^;];%[^;];%f;%f;%f;%s", nouvelle->nom, nouvelle->prenom, &nouvelle->taille, &nouvelle->poids, &nouvelle->imc, nouvelle->categorie) == 6) {
            nouvelle->suivant = NULL;
            ajouter_personne(nouvelle);
        }
    }
    fclose(file);
}
int main() {
    const char* fichier = "database.txt";
    char nom[50], prenom[50];
    char continuer = 'o';

    charger_depuis_fichier(fichier);

    while (continuer == 'o' || continuer == 'O') {
        printf("Entrez le nom : ");
        scanf("%s", nom);
        printf("Entrez le prénom : ");
        scanf("%s", prenom);

        Personne* p = rechercher_personne(nom, prenom);
        DietPlan diet;
        WorkoutPlan workout;

        if (p) {
            printf("\nLes informations suivantes ont été trouvées :\n");
            printf("Nom : %s, Prénom : %s, Taille : %.2f m, Poids : %.2f kg, IMC : %.2f, Catégorie : %s\n",
                   p->nom, p->prenom, p->taille, p->poids, p->imc, p->categorie);
            printf("Est-ce vous ? (o/n) : ");
            char choix;
            scanf(" %c", &choix);

            if (choix == 'o' || choix == 'O') {
                printf("Modifiez le poids (%.2f kg) : ", p->poids);
                scanf("%f", &p->poids);
                printf("Modifiez la taille (%.2f m) : ", p->taille);
                scanf("%f", &p->taille);

                calculer_imc_et_categorie(p);
                empiler(*p);
                generer_diet(p->imc, &diet);
                generer_workout(p->imc, &workout);

                printf("\nDiet:\nCalories: %d\nRepas: %s", diet.calories, diet.repas);
                printf("\nWorkout:\nExercices: %sDurée: %d minutes\n", workout.exercices, workout.duree);
            }
        } else {
            printf("\nNouvelle personne.\n");
            Personne* nouvelle = (Personne*)malloc(sizeof(Personne));
            strcpy(nouvelle->nom, nom);
            strcpy(nouvelle->prenom, prenom);
            printf("Entrez la taille (m) : ");
            scanf("%f", &nouvelle->taille);
            printf("Entrez le poids (kg) : ");
            scanf("%f", &nouvelle->poids);

            calculer_imc_et_categorie(nouvelle);
            empiler(*nouvelle);
            ajouter_personne(nouvelle);
            generer_diet(nouvelle->imc, &diet);
            generer_workout(nouvelle->imc, &workout);

            printf("\nDiet:\nCalories: %d\nRepas: %s", diet.calories, diet.repas);
            printf("\nWorkout:\nExercices: %sDurée: %d minutes\n", workout.exercices, workout.duree);
        }

        printf("Voulez-vous continuer ? (o/n) : ");
        scanf(" %c", &continuer);
    }

    sauvegarder_dans_fichier(fichier);
    afficher_historique();
    printf("Données sauvegardées. Au revoir !\n");

    return 0;
}

