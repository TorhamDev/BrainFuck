/*
 * obfc.c
 * The Brainfuck compiler
 * A simple BrainFuck to C interpreter and Compiler.
 * This program is free software;
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CC "/usr/bin/gcc"

void usage(char **argv);
void generate(void);

FILE *outfile;
FILE *infile;

int main(int argc, char **argv) {

  int c;
  char *output_name = NULL;    /* name of the executable that will be made */
  char *c_output_name = NULL;  /* name of the C source file that is generated */
  char compile_opts[512] = CC; /* array containing compiler options */
  int keep_file = 0;           /* Keep the generated C source file? */
  int verbose = 0;             /* Use verbose output? */
  int no_compile = 0;          /* Don't compile, just generate C source file */

  if (argc < 2) {
    usage(argv);
    exit(1);
  }

  while ((c = getopt(argc, argv, "o:c:knvh")) != -1) {
    switch (c) {
    case 'o':
      output_name = optarg;
      break;
    case 'v':
      verbose = 1;
      break;
    case 'h':
      usage(argv);
      exit(0);
    case 'k':
      keep_file = 1;
      break;
    case 'c':
      c_output_name = optarg;
      break;
    case 'n':
      no_compile = 1;
      keep_file = 1;
      break;
    default:
      usage(argv);
      exit(1);
    }
  }

  if ((optind >= argc) || (strcmp(argv[optind], "-") == 0)) {
    fprintf(stderr, "%s: no input file specified\n", argv[0]);
    usage(argv);
    exit(1);
  }

  if (verbose)
    printf("[+] Opening %s...", argv[optind]);
  if ((infile = fopen(argv[optind], "r")) == NULL) {
    if (verbose)
      printf("failed\n");
    fprintf(stderr, "%s: could not open %s\n", argv[0], argv[optind]);
    exit(1);
  }
  if (verbose)
    printf("Ok\n");

  if (!c_output_name)
    c_output_name = "bf.out.c";

  if (!output_name)
    output_name = "bf.out";

  if (verbose)
    printf("[+] Opening the output file %s...", c_output_name);
  if ((outfile = fopen(c_output_name, "w")) == NULL) {
    if (verbose)
      printf("failed\n");
    fprintf(stderr, "%s: error opening output file %s\n", argv[0],
            c_output_name);
    fclose(infile);
    exit(1);
  }
  if (verbose)
    printf("Ok\n");

  if (verbose)
    printf("[+] Generating C source...");
  generate();
  if (verbose)
    printf("Ok\n");

  fclose(outfile);

  snprintf(compile_opts, sizeof(compile_opts), "%s -o %s %s", CC, output_name,
           c_output_name);

  if (!no_compile) {
    if (verbose) {
      printf("[+] Compiling...\n");
      printf("Compiler: " CC "\n");
      printf("Compile Options: %s\n", compile_opts);
    }
    int status = system(compile_opts);
    if (status != 0) {
      fprintf(stderr, "[!] Compilation failed.\n");
    } else if (verbose) {
      printf("Compiling Complete\n");
    }
  }

  if (!keep_file) {
    if (verbose)
      printf("[+] Deleting intermediate file %s...", c_output_name);
    unlink(c_output_name);
    if (verbose)
      printf("Ok\n");
  } else if (verbose) {
    printf("[+] Keeping intermediate file %s...Ok\n", c_output_name);
  }

  fclose(infile);
  return 0;
}

void generate(void) {
  int c;

  fprintf(outfile, "/* Automatically generated with obfc */\n");
  fprintf(outfile, "#include <stdio.h>\n");
  fprintf(outfile, "#include <stdlib.h>\n\n");
  fprintf(outfile, "int main(void) {\n");
  fprintf(outfile, "    char a[30000] = {0};\n");
  fprintf(outfile, "    char *ptr = a;\n\n");

  while ((c = fgetc(infile)) != EOF) {
    switch (c) {
    case '>':
      fprintf(outfile, "    ptr++;\n");
      break;
    case '<':
      fprintf(outfile, "    ptr--;\n");
      break;
    case '+':
      fprintf(outfile, "    ++*ptr;\n");
      break;
    case '-':
      fprintf(outfile, "    --*ptr;\n");
      break;
    case '[':
      fprintf(outfile, "    while (*ptr) {\n");
      break;
    case ']':
      fprintf(outfile, "    }\n");
      break;
    case '.':
      fprintf(outfile, "    putchar(*ptr);\n");
      break;
    case ',':
      fprintf(outfile, "    *ptr = getchar();\n");
      break;
    }
  }
  fprintf(outfile, "\n    return 0;\n}\n");
}

void usage(char *argv[]) {
  printf("obfc - the \"brainfuck compiler\"\n");
  printf("Usage: %s [OPTIONS] [FILE]\n", argv[0]);
  printf("Available Options:\n");
  printf("  -o outfile\tspecify output file name\n");
  printf("  -k\t\tkeep the generated C source (normally bf.out.c)\n");
  printf("  -c c_outfile\tspecify C source file name\n");
  printf("  -n\t\tDon't compile the C source, just output a copy of it\n");
  printf("  -v\t\tverbose output\n");
  printf("  -h\t\tdisplay help\n");
}
