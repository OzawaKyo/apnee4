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
        if (c != 10) {
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
    // Ouvrir un accès bit à bit en écriture pour le fichier de sortie
    BFILE *bf = bstart(fic_out, "w");

    // Écrire l'arbre binaire dans le fichier de sortie
    EcrireArbre(bf->fichier ,ArbreHuffman);
    // Revenir au début du fichier d'entrée
    rewind(fic_in);

    int c;
    while ((c = fgetc(fic_in)) != EOF) {
        for (int i = 0; i < HuffmanCode[c].lg; i++) {
            // Écrire chaque bit du code dans le fichier de sortie
            bitwrite(bf, HuffmanCode[c].code[i]);
        }
    }

    // Écrire un caractère spécial pour indiquer la fin du fichier
    for (int i = 0; i < HuffmanCode[4].lg; i++) {
        bitwrite(bf, HuffmanCode[4].code[i]);
    }

    // Fermer l'accès bit à bit
    bstop(bf);
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

    /* Encodage */
    fichier = fopen(argv[1], "r");
    fichier_encode = fopen(argv[2], "w");

    Encoder(fichier, fichier_encode, ArbreHuffman);

    fclose(fichier_encode);
    fclose(fichier);
    return 0;
}

