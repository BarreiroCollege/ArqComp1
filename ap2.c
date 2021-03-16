#include <pmmintrin.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Cache L1: 512 líneas
// Cache L2: 4096 líneas
// Cache L3: 131072 líneas

#define CLS 64

// Double: 8 bytes
// Tamaño de línea: 64
// En cada línea 8 doubles
#define NUMD 8

// C=4
// F = L * 2

// C=8
// F = L

// C=20
// F = (floor(L / 5) * 2) + if(L % 5 <= 3, 1, 2)

// C=40
// F = ceil(L / 5)

// #define L 65536 // 512 768 2048 6144 65536 98304 262144 524288
// #define F 65536
// #define C 8 // 4 8 20 40

void start_counter();
double get_counter();
double mhz();

/* Initialize the cycle counter */

static unsigned cyc_hi = 0;
static unsigned cyc_lo = 0;

/* Set *hi and *lo to the high and low order bits of the cycle counter.
 Implementation requires assembly code to use the rdtsc instruction. */
void access_counter(unsigned *hi, unsigned *lo)
{
    asm("rdtsc; movl %%edx,%0; movl %%eax,%1" /* Read cycle counter */
        : "=r"(*hi), "=r"(*lo)                /* and move results to */
        : /* No input */                      /* the two outputs */
        : "%edx", "%eax");
}

/* Record the current value of the cycle counter. */
void start_counter()
{
    access_counter(&cyc_hi, &cyc_lo);
}

/* Return the number of cycles since the last call to start_counter. */
double get_counter()
{
    unsigned ncyc_hi, ncyc_lo;
    unsigned hi, lo, borrow;
    double result;

    /* Get cycle counter */
    access_counter(&ncyc_hi, &ncyc_lo);

    /* Do double precision subtraction */
    lo = ncyc_lo - cyc_lo;
    borrow = lo > ncyc_lo;
    hi = ncyc_hi - cyc_hi - borrow;
    result = (double)hi * (1 << 30) * 4 + lo;
    if (result < 0)
    {
        fprintf(stderr, "Error: counter returns neg value: %.0f\n", result);
    }
    return result;
}

double mhz(int verbose, int sleeptime)
{
    double rate;

    start_counter();
    sleep(sleeptime);
    rate = get_counter() / (1e6 * sleeptime);
    if (verbose)
        printf("\n Processor clock rate = %.1f MHz\n", rate);
    return rate;
}

int main(int argc, char ** argv)
{
    if (argc != 3) {
        printf("Uso: %s [F] [C]\n", argv[0]);
        exit(1);
    }

    // int L = atoi(argv[1]);
    int F = atoi(argv[1]);
    int C = atoi(argv[2]);

    srand(time(0));

    double ck;

    // Matriz de valores con reserva dinámica
    double **M;
    M = (double **) _mm_malloc(sizeof(double *) * F, CLS);
    for (int i = 0; i < F; i++) *(M+i) = (double *) malloc(sizeof(double) * C);

    for (int i = 0; i < F; i++) {
        for (int j = 0; j < C; j++) {
            M[i][j] = rand();
        }
    }

    start_counter();

    /* Poñer aquí o código a medir */
    double red[10];
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < F; j++) {
            int start = 0;
            while (start < C) {
                red[i] += M[j][start];
                start += NUMD;
            }
        }
    }

    ck = get_counter();
   
    for (int i = 0; i < 10; i++){
        printf("%lf\t", red[i]);
    }

    printf("\n Clocks=%1.10lf \n", ck);

    /* Esta rutina imprime a frecuencia de reloxo estimada coas rutinas start_counter/get_counter */
    // mhz(1, 1);

    for (int i = 0; i < F; i++) free(M[i]);
    _mm_free(M);

    FILE *f = fopen("res.txt", "w");
    fprintf(f, "%lf", ck);
    fclose(f);

    return 0;
}