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
            TableOcc->tab[c]++;
        c = fgetc(fichier);
    };
    // enlever tout les caracteres special au desus de 127
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
        Arbre a = NouveauNoeud(a1,'-', a2);
        file = inserer(file, a, p1 + p2);

    }
    Arbre a;
    int p;
    file = extraire(file, &a, &p);    
    return a;
}

void ConstruireCodeRec(Arbre huff, int code[], int lg, int indice) {
    if (EstVide(huff->fg) && EstVide(huff->fd)) {
        HuffmanCode[huff->etiq].lg = lg;
        for (int i = 0; i < lg; i++) {
            HuffmanCode[huff->etiq].code[i] = code[i];
        }
    } else {
        code[lg] = 0;
        ConstruireCodeRec(huff->fg, code, lg + 1, indice);
        code[lg] = 1;
        ConstruireCodeRec(huff->fd, code, lg + 1, indice);
    }
}

void ConstruireCode(Arbre huff) {
    int code[256];
    ConstruireCodeRec(huff, code, 0, 0);
}

void Encoder(FILE *fic_in, FILE *fic_out, Arbre ArbreHuffman) {
    EcrireArbre(fic_out, ArbreHuffman);

    int c;
    while ((c = fgetc(fic_in)) != EOF) {
            if (c == ' ') {
            fprintf(fic_out, " ");}
            int *code = HuffmanCode[c].code;
            int lg = HuffmanCode[c].lg;

            for (int i = 0; i < lg; i++) {
                fprintf(fic_out, "%d", code[i]);
            }
    }
}

int main(int argc, char *argv[]) {

    TableOcc_t TableOcc;
    FILE *fichier;
    FILE *fichier_encode;

    fichier = fopen(argv[1], "r");
    /* Construire la table d'occurences */
    ConstruireTableOcc(fichier, &TableOcc);
    fclose(fichier);

    /* Initialiser la FAP */
    fap file = InitHuffman(&TableOcc);

    /* Construire l'arbre d'Huffman */
    Arbre ArbreHuffman = ConstruireArbre(file);

        AfficherArbre(ArbreHuffman);

    /* Construire la table de codage */
    ConstruireCode(ArbreHuffman);
    printf("\nTable de codage :\n");
    for (int i = 0; i < 256; i++) {
        if (HuffmanCode[i].lg > 0) {
            printf("Caractere %c (code %d) : Longueur %d, Code ", i, i, HuffmanCode[i].lg);
            for (int j = 0; j < HuffmanCode[i].lg; j++) {
                printf("%d", HuffmanCode[i].code[j]);
            }
            printf("\n");
        }
    }
    /* Encodage */
    fichier = fopen(argv[1], "r");
    fichier_encode = fopen(argv[2], "w");

    Encoder(fichier, fichier_encode, ArbreHuffman);

    fclose(fichier_encode);
    fclose(fichier);

    return 0;
}

