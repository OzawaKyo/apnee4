#include "arbrebin.h"
#include "bfile.h"
#include "fap.h"
#include "huffman_code.h"
#include <assert.h>
#include <stdio.h>

typedef struct {
    int tab[256];
} TableOcc_t;

struct code_char HuffmanCode[256];

void ConstruireTableOcc(FILE *fichier, TableOcc_t *TableOcc) {

    int c;

    for (int i = 0; i < 256; i++) {
        TableOcc->tab[i] = 0;
    }
    c = fgetc(fichier);
    while (c != EOF) {
        if (c != ' ') {
            TableOcc->tab[c]++;
        }
        c = fgetc(fichier);
    };

    for (int i = 127; i < 256; i++) {
        TableOcc->tab[i] = 0;
    }

    int i;
    for (i = 0; i < 256; i++) {
        if (TableOcc->tab[i] != 0)
            printf("Occurences du caractere %c (code %d) : %d\n", i, i,TableOcc->tab[i]);
    }
}

fap InitHuffman(TableOcc_t *TableOcc) {
    fap file = creer_fap_vide();
    for (int i = 0; i < 256; i++) {
        if (TableOcc->tab[i] > 0) {
            Arbre a = NouveauNoeud(ArbreVide(), i, ArbreVide());
            file = inserer(file, a, TableOcc->tab[i]);
        }
    }   
    return file;
}

Arbre ConstruireArbre(fap file) {
    while (!est_fap_vide(file) && file->prochain != NULL)
    {
        Arbre a1, a2;
        int p1, p2;
        file = extraire(file, &a1, &p1);    
        file = extraire(file, &a2, &p2);
        Arbre a = NouveauNoeud(a1, p1+p2, a2);
        file = inserer(file, a, p1 + p2);

    }
    Arbre a;
    int p;
    file = extraire(file, &a, &p);    
    return a;
}


void ConstruireCode(Arbre huff) {
    /* A COMPLETER */
    printf("Programme non realise (ConstruireCode)\n");
}

void Encoder(FILE *fic_in, FILE *fic_out, Arbre ArbreHuffman) {
    /* A COMPLETER */
    printf("Programme non realise (Encoder)\n");
}

// int main(int argc, char *argv[]) {

    // TableOcc_t TableOcc;
    // FILE *fichier;
    // // FILE *fichier_encode;

    // fichier = fopen(argv[1], "r");
    // /* Construire la table d'occurences */
    // ConstruireTableOcc(fichier, &TableOcc);
    // fclose(fichier);

    // /* Initialiser la FAP */
    // fap file = InitHuffman(&TableOcc);

    // /* Construire l'arbre d'Huffman */
    // Arbre ArbreHuffman = ConstruireArbre(file);

    //     AfficherArbre(ArbreHuffman);

    /* Construire la table de codage */
    // ConstruireCode(ArbreHuffman);

    // /* Encodage */
    // fichier = fopen(argv[1], "r");
    // fichier_encode = fopen(argv[2], "w");

    // Encoder(fichier, fichier_encode, ArbreHuffman);

    // fclose(fichier_encode);
    // fclose(fichier);
    // return 0;
// }


int main() {
    // Exemple de fichier (à remplacer par le chemin de votre fichier)
    FILE *fichier = fopen("gargantua.txt", "r");
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return 1;
    }

    // Initialisation de la table d'occurrences
    TableOcc_t TableOcc;
    ConstruireTableOcc(fichier, &TableOcc);

    // Affichage de la table d'occurrences
    printf("Table d'occurrences :\n");
    for (int i = 0; i < 256; i++) {
        if (TableOcc.tab[i] != 0) {
            printf("Caractere %c (code %d) : %d\n", (char)i, i, TableOcc.tab[i]);
        }
    }

    // Initialisation de la file de priorités
    fap file = InitHuffman(&TableOcc);

    // Construction de l'arbre de Huffman
    Arbre arbreHuffman = ConstruireArbre(file);

    // Affichage de l'arbre de Huffman
    printf("\nArbre de Huffman :\n");
    AfficherArbre(arbreHuffman);

    // Libération de la mémoire allouée pour l'arbre
    LibererArbre(arbreHuffman);

    // Fermeture du fichier
    fclose(fichier);

    return 0;
}
