#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//configuration des ecrans
#define SCREEN_MENU  0
#define SCREEN_LISTE 1
#define SCREEN_AJOUT 2

// structure bibliotheque
typedef struct Livre {
    int id;
    char titre[50];
    char auteur[50];
    int annee;
    int disponible;
} tlivre;

tlivre biblio[100];
int nbLivres = 0;

// Variables de saisie graph
char txtTitre[50] = "";
char txtAuteur[50] = "";
char txtAnnee[10] = "";
bool editTitre = false;
bool editAuteur = false;
bool editAnnee = false;

// Variables de recherche
char txtRecherche[50] = "";
bool editRecherche = false;

// État de l'application (Initialisé au Menu)
int currentScreen = SCREEN_MENU;

void Sauvegarder() {
    FILE *f = fopen("bibliotheque.txt", "w"); //ouvre le fichier en ecriture
    if (f) {
        fprintf(f, "%d\n", nbLivres);// ecrit le nombre total de livre 
        for (int i = 0; i < nbLivres; i++) {
            fprintf(f, "%d|%s|%s|%d|%d\n", 
                    biblio[i].id, biblio[i].titre, biblio[i].auteur, biblio[i].annee, biblio[i].disponible); // un livre par ligne
        }
        fclose(f);
    }
}

void Charger() {
    FILE *f = fopen("bibliotheque.txt", "r");// ouvre le fichier en lecture
    if (f) { //verifie que le fichier existe
        char ligne[256];
        if (fgets(ligne, sizeof(ligne), f)) {
            sscanf(ligne, "%d", &nbLivres);// met le nombre de livre dans la variable nblivres
        }
        if (nbLivres > 100) nbLivres = 100;// verifie qu'il y a moins de 100 livres

        for (int i = 0; i < nbLivres; i++) {
            if (fgets(ligne, sizeof(ligne), f)) {
                ligne[strcspn(ligne, "\n")] = '\0';//remplace le /n de la fin recuperer par fgets par un caratère vide
                sscanf(ligne, "%d|%[^|]|%[^|]|%d|%d", 
                       &biblio[i].id, biblio[i].titre, biblio[i].auteur, &biblio[i].annee, &biblio[i].disponible);//range les données recup du fichier dans des variables
            }
        }
        fclose(f);
    }
}

void SupprimerLivre(int index) {
        // Décalage pour combler le vide
        for (int i = index; i < nbLivres - 1; i++) {
            biblio[i] = biblio[i + 1];
        }
        
        // On diminue le nombre de livres total
        nbLivres--; 
        
        // reassigne les numéros de 1 à n
        for (int i = 0; i < nbLivres; i++) {
            biblio[i].id = i + 1; 
        }
    }

// ----------------------------------------------------------------------------------
int main() {
    InitWindow(1000, 700, "Ma Bibliotheque GUI");
    SetTargetFPS(60);
    Charger();

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

        // Titre permanent
        GuiLabel((Rectangle){ 400, 20, 200, 40 }, "#102# GESTION BIBLIOTHEQUE");

        if (currentScreen == SCREEN_MENU) {
            // menu principal avec changement page
            if (GuiButton((Rectangle){ 400, 150, 200, 40 }, "#019# Afficher/Gerer Livres")) currentScreen = SCREEN_LISTE;
            if (GuiButton((Rectangle){ 400, 210, 200, 40 }, "#033# Ajouter un Livre")) currentScreen = SCREEN_AJOUT;
            if (GuiButton((Rectangle){ 400, 270, 200, 40 }, "#002# Sauvegarder")) Sauvegarder();
            if (GuiButton((Rectangle){ 400, 330, 200, 40 }, "#158# Quitter")) break;
        } 
        
        else if (currentScreen == SCREEN_LISTE) {
            // ECRAN LISTE RECHERCHE SUPPRESSION
            GuiGroupBox((Rectangle){ 50, 80, 900, 500 }, "Gestion de la liste");
            
            // Barre de recherche
            GuiLabel((Rectangle){ 70, 100, 160, 25 }, "Titre exact recherché :");
            if (GuiTextBox((Rectangle){ 240, 100, 300, 25 }, txtRecherche, 50, editRecherche)) {
                editRecherche = !editRecherche;
            }

            int ligneAffichees = 0;
            
            for (int i = 0; i < nbLivres; i++) {
                bool correspond = false;
                
                if (strlen(txtRecherche) == 0) {//si barre de recherche vide
                    correspond = true; 
                } 
                else if (strcmp(biblio[i].titre, txtRecherche) == 0) {//si titre correspond a recherche
                    correspond = true;
                }

                if (correspond) {
                    float yPos = 145.0f + (ligneAffichees * 35);
                    
                    if (yPos < 550) {
                        char statut[20];
                        strcpy(statut, biblio[i].disponible ? "[Dispo]" : "[Prete]");//
                        
                        // Affichage du livre
                        DrawText(TextFormat("%d - %s par %s (%d) %s", 
                                    biblio[i].id, biblio[i].titre, biblio[i].auteur, biblio[i].annee, statut), 
                                    70, yPos + 2, 18, DARKGRAY);
                        
                        // Bouton Preter / Rendre
                        if (GuiButton((Rectangle){ 720, yPos, 100, 25 }, biblio[i].disponible ? "Preter" : "Rendre")) { //condition si vraie alors preter sinon rendre
                            biblio[i].disponible = !biblio[i].disponible;//  inverse l'tat de la valeur
                        }
                        
                        // Bouton Supprimer
                        if (GuiButton((Rectangle){ 840, yPos, 80, 25 }, "Suppr.")) {
                            SupprimerLivre(i);
                            i--; // On recule l'index car le tableau vient de bouger
                        }
                        
                        ligneAffichees++;
                    }
                }
            }

            if (ligneAffichees == 0 && strlen(txtRecherche) > 0) {
                DrawText("Aucun livre ne correspond exactement à ce titre.", 70, 160, 20, GRAY);
            }

            if (GuiButton((Rectangle){ 400, 600, 200, 40 }, "Retour Menu")) {
                txtRecherche[0] = '\0'; 
                editRecherche = false;
                currentScreen = SCREEN_MENU;
            }
        } 
        
        else if (currentScreen == SCREEN_AJOUT) {
            // ÉCRAN AJOUT
            GuiGroupBox((Rectangle){ 300, 100, 400, 400 }, "Nouveau Livre");

            GuiLabel((Rectangle){ 320, 130, 100, 20 }, "Titre:");
            if (GuiTextBox((Rectangle){ 320, 155, 360, 30 }, txtTitre, 50, editTitre)) {
                editTitre = !editTitre;
                if (editTitre) { editAuteur = false; editAnnee = false; } 
            }

            GuiLabel((Rectangle){ 320, 200, 100, 20 }, "Auteur:");
            if (GuiTextBox((Rectangle){ 320, 225, 360, 30 }, txtAuteur, 50, editAuteur)) {
                editAuteur = !editAuteur;
                if (editAuteur) { editTitre = false; editAnnee = false; } 
            }

            GuiLabel((Rectangle){ 320, 270, 100, 20 }, "Annee:");
            if (GuiTextBox((Rectangle){ 320, 295, 100, 30 }, txtAnnee, 10, editAnnee)) {
                editAnnee = !editAnnee;
                if (editAnnee) { editTitre = false; editAuteur = false; } 
            }

            if (GuiButton((Rectangle){ 350, 380, 140, 40 }, "Valider")) {
                if (nbLivres < 100 && strlen(txtTitre) > 0) {
                    //le nouvel ID correspond au nombre actuel de livres + 1
                    biblio[nbLivres].id = nbLivres + 1;          
                    
                    strcpy(biblio[nbLivres].titre, txtTitre);    
                    strcpy(biblio[nbLivres].auteur, txtAuteur);  
                    biblio[nbLivres].annee = atoi(txtAnnee);      
                    biblio[nbLivres].disponible = 1;
                    nbLivres++;
                    
                    txtTitre[0] = '\0'; txtAuteur[0] = '\0'; txtAnnee[0] = '\0';
                    editTitre = editAuteur = editAnnee = false;
                    currentScreen = SCREEN_LISTE; 
                }
            }
            
            if (GuiButton((Rectangle){ 510, 380, 140, 40 }, "Annuler")) {
                txtTitre[0] = '\0'; txtAuteur[0] = '\0'; txtAnnee[0] = '\0';
                editTitre = editAuteur = editAnnee = false;
                currentScreen = SCREEN_MENU;
            }
        }

        EndDrawing();
    }

    Sauvegarder();
    CloseWindow();
    return 0;
}
