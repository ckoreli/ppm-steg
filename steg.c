#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

// GENERAL
FILE *safe_fopen(char *fajl, char *opt);
void print_usage(char *arg);
void read_header(FILE *file, int *P, int *width, int *height, int *depth);

// HIDING
int get_file_size(FILE *file);
void hide_char(FILE *in, FILE *out, int color, int c);
void hide_str(FILE *in, FILE *out, int color, char *str);

// UNHIDING
char unhide_char(FILE *in, int color);
int unhide_str(FILE *in, int color, char *str);
void read_str(FILE *in, int color, char *str);
void insert_char(char *str, char c);

int main (int argc, char **argv) {
    int option;
    int iflag = 0, color = 0, cflag = 0, oflag = 0;
    int hide = 0;
    char cleartext_filename[100];
    cleartext_filename[0] = '\0';

    FILE *in;
    FILE *cleartext;
    FILE *out;

    /* PROVERA ARGUMENATA */
    while ((option = getopt(argc, argv, "i:rgbc:o:")) != -1) {
        switch (option) {
            case 'i':
                if (iflag) {
                    printf("Dozvoljen samo jedan ulazni fajl.\n");
                    exit(EXIT_FAILURE);
                }

                iflag = 1;
                in = safe_fopen(optarg, "rb");
                break;
            case 'r':
                if (color) {
                    printf("Dozvoljen izbor samo jedne boje.\n");
                    exit(EXIT_FAILURE);
                }
                color = 1;
                break;
            case 'g':
                if (color)
                {
                    printf("Dozvoljen izbor samo jedne boje.\n");
                    exit(EXIT_FAILURE);
                }
                color = 2;
                break;
            case 'b':
                if (color)
                {
                    printf("Dozvoljen izbor samo jedne boje.\n");
                    exit(EXIT_FAILURE);
                }
                color = 3;
                break;
            case 'c':
                if (cflag)
                {
                    printf("Dozvoljen samo jedan tekstualni fajl.\n");
                    exit(EXIT_FAILURE);
                }

                cflag = 1;
                cleartext = safe_fopen(optarg, "r");
                strncpy(cleartext_filename, strtok(optarg, "."), 100);
                strcat(cleartext_filename, "\n");
                break;
            case 'o':
                if (oflag)
                {
                    printf("Dozvoljen samo jedan izlazni fajl.\n");
                    exit(EXIT_FAILURE);
                }

                oflag = 1;
                out = safe_fopen(optarg, "wb");
                break;
            default:
                printf("Uneta nepoznata opcija.\n");
                exit(EXIT_FAILURE);
        }
    }

    if (!iflag) {
        printf("Greska. Ulazna slika se mora navesti.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }

    if (!color) {
        printf("Greska. Boja se mora navesti.\n");
        print_usage(argv[0]);
        exit(EXIT_FAILURE);
    }
    /* KRAJ PROVERE */
    int P = 0, width = 0, height = 0, depth = 0;
    int im_size, ct_size;
    read_header(in, &P, &width, &height, &depth);
    char c;

    // Unet tekstualni fajl, HIDING MODE
    if (cflag)
    {
        hide = 1;
        if (!oflag) {
            out = safe_fopen("stegovan.ppm", "wb");
            strcpy(cleartext_filename, "stegovan\n");
        }

        // Broj slobodnih piksela za stekanje
        im_size = width * height;           
        // Broj karakterja/ bajtova u fajlu
        ct_size = get_file_size(cleartext);
        char x[100];
        sprintf(x, "%d\n", ct_size);

        //
        if ((ct_size + strlen(cleartext_filename) + strlen(x) + 5) * 8 > im_size) {
            printf("Ulazna slika je previse mala da bi se sakrio tekst.\n");
            exit(EXIT_FAILURE);
        }

        // printf("STEG\n");
        // printf("%s", cleartext_filename);
        // printf("%s", x);
        // return 0;

        fprintf(out, "P%d\n", P);
        fprintf(out, "%d %d\n", width, height);
        fprintf(out, "%d\n", depth);

        hide_str(in, out, color, "STEG\n");
        hide_str(in, out, color, cleartext_filename);
        hide_str(in, out, color, x);

        int c;
        while (1) {
            c = fgetc(cleartext);
            if (feof(cleartext))
                break;
            hide_char(in, out, color, c);
        }
        if (!feof(in)) {
            while (1) {
                c = fgetc(in);
                if (feof(in))
                    break;
                fputc(c, out);
            }
        }
    }
    // UNHIDING
    else {
        // printf("UNHIDING\n");
        // find_header(in, color, cleartext_filename, &ct_size);
        // strcat(cleartext_filename, ".txt");
        // out = safe_fopen(cleartext_filename, "w");
        if (oflag) {
            printf("Izlazna slika se ne treba navesti u UNHIDING modu.\n");
            exit(EXIT_FAILURE);
        }
        
        if (!unhide_str(in, color, "STEG\n")) {
            printf("Nista nije skriveno u toj boji.\n");
            exit(EXIT_FAILURE);
        }

        // Uzimam ime fajla
        // Idem do \n i napravim string, to mi je ime
        read_str(in, color, cleartext_filename);
        strcat(cleartext_filename, ".txt");
        out = safe_fopen(cleartext_filename, "w");

        // Uzimam velicinu fajla u bajtovima
        // Idem do \n i napravim string, strtol i dobijem int vrednost
        char s[20];
        s[0] = '\0';
        read_str(in, color, s);
        ct_size = strtol(s, NULL, 10);
        // Idem redom i uzimam x bajtova iz slidze
        // Picim i prepisujem u fajl karakter po karakter
        for (int i = 0; i < ct_size; ++i) {
            c = unhide_char(in, color);
            fputc(c, out);
        }
    }


    if (iflag) fclose(in);
    if (cflag) fclose(cleartext);
    if (oflag) fclose(out);
    return 0;
}

FILE *safe_fopen(char *file, char *opt)
{
    FILE *ret = fopen(file, opt);

    if (ret == NULL)
    {
        printf("Fajl %s nije pronadjen\n", file);
        exit(EXIT_FAILURE);
    }

    return ret;
}

void print_usage(char *arg)
{
    printf("Upotreba: %s -i IMAGE_FILE  { -r | -b | -g }  [ -c CLEARTEXT_FILE -o OUTPUT_IMAGE_FILE ]\n", arg);
}

void read_header(FILE *file, int *P, int *width, int *height, int *depth) {
    if (file == NULL) {
        printf("Greska pri citanju header-a.\n");
        return;
    }

    char buff[30];
    char *endptr;
    int lin = 0;
    
    while (lin < 3) {
        fgets(buff, 30, file);
        
        if (buff[0] == '#') continue;
        
        lin++;
        if (lin == 1) {
            *P = strtol(buff + 1, NULL, 10);
            if (*P != 6) {
                printf("Podrzava samo P6 ppm slike.\n");
                exit(EXIT_FAILURE);
            }
        }
        else if (lin == 2) {
            *width = strtol(buff, &endptr, 10);
            *height = strtol(endptr, NULL, 10);
        }
        else {
            *depth = strtol(buff, NULL, 10);
        }
    }
}

int get_file_size(FILE *file) {
    int ret = 0;

    while (1) {
        fgetc(file);
        if (feof(file))
            break;
        ret++;
    }

    fseek(file, 0, SEEK_SET);

    return ret;
}

void hide_char(FILE *in, FILE *out, int color, int c) {
    int B = 1;
    int x;
    int r, g, b;
    // printf("hideujem %c [%d]\n", c, c);
    for (int i = 0; i < 8; ++i) {
        r = fgetc(in);
        g = fgetc(in);
        b = fgetc(in);
        x = (c & B) > 0;
        // printf("B == %d\n", B);
        // printf("x == %d\n", x);
        if (color == 1) {
            // printf("pre   == %x\n", r);
            r = (r & 0xFE) | x;
            // printf("posle == %x\n", r);
        }
        else if (color == 2) {
            // printf("pre   == %x\n", g);
            g = (g & 0xFE) | x;
            // printf("posle == %x\n", g);
        }
        else {
            // printf("pre   == %x\n", b);
            b = (b & 0xFE) | x;
            // printf("posle == %x\n", b);
        }
        fputc(r, out);
        fputc(g, out);
        fputc(b, out);
        B <<= 1;
    }
}

void hide_str(FILE *in, FILE *out, int color, char *str) {
    for (int i = 0; i < strlen(str); ++i) {
        hide_char(in, out, color, str[i]);
    }
}

char unhide_char(FILE *in, int color) {
    int c = 0;
    int r, g, b;
    int B;

    for (int i = 0; i < 8; ++i) {
        r = fgetc(in);
        g = fgetc(in);
        b = fgetc(in);

        if (color == 1) {
            B = r & 1;
        }
        else if (color == 2) {
            B = g & 1;
        }
        else {
            B = b & 1;
        }

        c += (B << i);
    }

    return c;
}

int unhide_str(FILE *in, int color, char *str) {
    for (int i = 0; i < strlen(str); ++i) {
        // printf("TRAZIM %c --- NASAO %d\n", str[i], unhide_char(in, color));
        if (unhide_char(in, color) != str[i])
            return 0;
    }
    return 1;
}

void insert_char(char *str, char c) {
    str[strlen(str) + 1] = '\0';
    str[strlen(str)] = c;
}

void read_str(FILE *in, int color, char *str) {
    char c;
    while ((c = unhide_char(in, color)) != '\n') {
        insert_char(str, c);
    }
}