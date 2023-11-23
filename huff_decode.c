#include <stdio.h>
#include "arbrebin.h"
#include "assert.h"

#include "bfile.h"

void Decoder(FILE *fichier_encode, Arbre ArbreHuffman) {
        AfficherArbre(ArbreHuffman);    
        // Initialisation de la lecture bit à bit
    BFILE *bfile = bstart(fichier_encode, "r");
    // Nœud courant dans l'arbre de Huffman
    Arbre noeudCourant = ArbreHuffman;

    // Lire les bits du fichier encodé et décoder
    int bit;
    while ((bit = bitread(bfile)) != -1) {
        // Suivre le chemin dans l'arbre en fonction du bit
        if (bit == 0) {
            noeudCourant = FilsGauche(noeudCourant);
        } else {
            noeudCourant = FilsDroit(noeudCourant);
        }

        // Vérifier si le nœud courant est une feuille
        if (EstVide(FilsGauche(noeudCourant)) && EstVide(FilsDroit(noeudCourant))) {
            // Écrire le caractère associé dans la sortie standard
            printf("%c", Etiq(noeudCourant));

            // Revenir à la racine de l'arbre pour le prochain caractère
            noeudCourant = ArbreHuffman;
        }
    }

    // Fermer la lecture bit à bit
    bstop(bfile);
}


int main(int argc, char *argv[]) {
    FILE *fichier_encode = fopen(argv[1], "r");

    Arbre ArbreHuffman = LireArbre(fichier_encode);

    Decoder(fichier_encode, ArbreHuffman);

    fclose(fichier_encode);

    return 0;
}
