#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <getopt.h>
#include <error.h>
#include <string.h>
#include <errno.h>
#include <dialog.h>
#include "config.h"


struct option long_options[] = {
        {"decode",         no_argument, 0,    'd'},
        {"ignore-garbage", no_argument, 0,    'i'},

        {"help",           no_argument, NULL, 'h'},
        {"version",        no_argument, NULL, 'v'},

        {NULL, 0,                       NULL, 0}
};

const char *program_name;
const char *charset = "mpf";

void usage(int status) {
    if (status != EXIT_SUCCESS) {
        fprintf(stderr, "Try '%s --help' for more information.\n",
                program_name);
    } else {
        printf(
                "Usage: %s [OPTION]... [FILE]\n"
                "Kenny encode or decode FILE, or standard input, to standard output.\n"
                "\n"
                "With no FILE, or when FILE is -, read standard input.\n"
                "\n",
                program_name
        );

        fputs(
                "  -d, --decode          decode data\n",
                stdout
        );
    }

    exit(status);
}

void encode(FILE *in, FILE *out) {
    int c;
    unsigned short isUpper;
    char buff[4];

    buff[3] = '\0';

    while ((c = fgetc(in)) != EOF && !ferror(in)) {
        if (!isalpha(c)) {
            fputc(c, out);
            continue;
        }

        isUpper = isupper(c);
        c = tolower(c);
        c -= 'a';

        buff[2] = charset[c % 3];
        c /= 3;
        buff[1] = charset[c % 3];
        c /= 3;
        buff[0] = charset[c % 3];

        if (isUpper) {
            buff[0] = (char) toupper(buff[0]);
        }

        fputs(buff, out);
    }
}

void decode(FILE *in, FILE *out) {
    int c, i = 9, ord = 0;
    unsigned short isUpper = 0
            ;
    char *ptr = NULL;

    while ((c = fgetc(in)) != EOF && !ferror(in)) {
        if (!isalpha(c)) {
            fputc(c, out);
            continue;
        }

        if (i == 9) {
            isUpper = isupper(c);
        }
        c = tolower(c);
        ptr = strchr(charset, c);
        if (ptr == NULL) {
            fputc(c, out);
            continue;
        }

        ord += i * (ptr - charset);

        if (i == 1) {
            fputc(ord + (isUpper ? 'A' : 'a'), out);

            i = 9;
            ord = 0;
            isUpper = 0;
        } else {
            i /= 3;
        }
    }
}

int main(int argc, char **argv) {
    int opt;
    bool doDecode = false;
    const char *inputFile;
    FILE *inputHandle;

    program_name = argv[0];

    while ((opt = getopt_long(argc, argv, "dhv", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
                doDecode = true;
                break;

            case 'h':
                usage(EXIT_SUCCESS);
                break;

            case 'v':
                printf("kenny %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
                exit(EXIT_SUCCESS);

            default:
                usage(EXIT_FAILURE);
                break;
        }
    }

    if (argc - optind > 1) {
        error(0, 0, "extra operand '%s'", argv[optind]);
        usage(EXIT_FAILURE);
    }

    if (optind < argc)
        inputFile = argv[optind];
    else
        inputFile = "-";

    if (strcmp(inputFile, "-") == 0) {
        inputHandle = stdin;
    } else {
        inputHandle = fopen(inputFile, "r");
        if (inputHandle == NULL)
            error(EXIT_FAILURE, errno, "%s", inputFile);
    }

    if (doDecode)
        decode(inputHandle, stdout);
    else
        encode(inputHandle, stdout);

    if (fclose(inputHandle) == EOF) {
        if (strcmp(inputFile, "-") == 0)
            error(EXIT_FAILURE, errno, "closing standard input");
        else
            error(EXIT_FAILURE, errno, "%s", inputFile);
    }

    return EXIT_SUCCESS;
}
