#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include <string.h>

#define N 1000

int main(int argc, char *argv[]) {

  fftw_complex in[N], out[N], in2[N]; /* double [2] */
  fftw_plan p, q;
  int i;

  /* prepare signal */
  if (!strcmp(argv[1], "cos")) {
    for (i = 0; i < N; i++) {
      in[i][0] = 3*cos(5 * 2*M_PI*i/N);
      in[i][1] = 0;
    }
  }
  else if (!strcmp(argv[1], "sin")) {
    for (i = 0; i < N; i++) {
      in[i][0] = 3*sin(5 * 2*M_PI*i/N);
      in[i][1] = 0;
  }
  }
  else if (!strcmp(argv[1], "sinc")) {
    for (i = 0; i < N; i++) {
      float interval = 0.1;
      float input = interval*i;
      if (input == 0) {
        in[i][0] = 1;
        in[i][1] = 0;
      } else {
        in[i][0] = sin(2*M_PI*input)/(2*M_PI*input);
        in[i][1] = 0;
      }
    }
  }
  else if (!strcmp(argv[1], "square")) {
    for (i = 0; i < N; i++) {
      float interval = 0.1;
      float input = interval*i;
      if (input < N*interval/2) {
        in[i][0] = 1;
        in[i][1] = 0;
      } else {
        in[i][0] = 0;
        in[i][1] = 0;
      }
    }
  }
  else {
    printf("%s isn't recognized as an implemented signal to compute\n", argv[1]);
    return 1;
  }

  /* forward Fourier transform, save the result in 'out' */
  p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);
  printf("Freq\tFast Fourier Transform\n");
  for (i = 0; i < floor(N/2); i++)
    printf("%3d\t%+9.5f%+9.5fI\n", i, out[i][0], out[i][1]);
  fftw_destroy_plan(p);

  /* backward Fourier transform, save the result in 'in2' */
  printf("\nInverse transform:\n");
  q = fftw_plan_dft_1d(N, out, in2, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(q);
  /* normalize */
  for (i = 0; i < N; i++) {
    in2[i][0] *= 1./N;
    in2[i][1] *= 1./N;
  }
  
  printf("Freq\tInput Signal\t\t   Inverse FFT\n");
  for (i = 0; i < N; i++)
    printf("%3d %+9.5f %+9.5f I vs. %+9.5f %+9.5f I  difference: %+24.22f\n",
        i, in[i][0], in[i][1], in2[i][0], in2[i][1], in[i][0]-in2[i][0]);
  fftw_destroy_plan(q);

  fftw_cleanup();

  return 0;
}
