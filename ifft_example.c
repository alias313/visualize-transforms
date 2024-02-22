#include <stdlib.h>
#include <math.h>
#include <fftw3.h>
#include <string.h>

#define M_PI 3.14159265358979323846

#define N 1000

#define a_argpos 1
#define b_argpos 2
#define sig_argpos 3
#define amp_argpos 4
#define freq_argpos 5
#define phase_argpos 6

int main(int argc, char *argv[]) {
    
  FILE *fptr;

  const float a = atof(argv[a_argpos]);
  const float b = atof(argv[b_argpos]);
  const float amp = atof(argv[amp_argpos]);
  const float freq_hz = atof(argv[freq_argpos]);
  const float phase_rad = atof(argv[phase_argpos]);

  const float sampling_interval = 0.005;
  const int total_samples = ceil((b-a)/sampling_interval)+1;
  printf("Total samples: %d\n", total_samples);

  fftw_complex in[total_samples], out[total_samples], in2[total_samples]; /* double [2] */
  fftw_plan p, q;
  int i;

  /* prepare signal */
  if (!strcmp(argv[sig_argpos], "cos")) {
    for (i = 0; i < total_samples; i++) {
      float input = a + i*sampling_interval;
      in[i][0] = amp * cos(freq_hz * 2*M_PI*input + phase_rad);
      printf("%5.2f %+5.2f\n", input, in[i][0]);
      in[i][1] = 0;
    }
  }
  else if (!strcmp(argv[sig_argpos], "sin")) {
    for (i = 0; i < total_samples; i++) {
      float input = a + i*sampling_interval;
      in[i][0] = amp * sin(freq_hz * 2*M_PI*input+phase_rad);
      in[i][1] = 0;
  }
  }
  else if (!strcmp(argv[sig_argpos], "sinc")) {
    for (i = 0; i < total_samples; i++) {
      float input = a + i*sampling_interval;
      if (input == 0) { // change in the future to 2*M_PI_input + sinc_phase_rad ~ 0 (if it's between sampling interval, for example -0,05 < x < 0,05 is sinc_amp if interval is 0.1)
        in[i][0] = amp;
        in[i][1] = 0;
      } else {
        in[i][0] = amp * sin(freq_hz * 2*M_PI*input)/(freq_hz * 2*M_PI*input);
        in[i][1] = 0;
      }
    }
  }
  else if (!strcmp(argv[sig_argpos], "square")) {
    for (i = 0; i < total_samples; i++) {
      float input = a + i*sampling_interval;
      if (abs(input) < 1) {
        in[i][0] = 1;
        in[i][1] = 0;
      } else {
        in[i][0] = 0;
        in[i][1] = 0;
      }
    }
  }
  else {
    printf("%s isn't recognized as an implemented signal to compute\n", argv[sig_argpos]);
    return 1;
  }

  /* forward Fourier transform, save the result in 'out' */
  p = fftw_plan_dft_1d(total_samples, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(p);

  fptr = fopen("fft_out.csv", "w");

  // fprintf(fptr, "%d\n", total_samples);
  // fprintf(fptr, "(00000) Freq\tFast Fourier Transform\n");
  fprintf(fptr, "Freq,re(FFT),im(FFT),input,re(signal)\n");
  for (i = 0; i < total_samples; i++) {
    float input = a+i*sampling_interval;
    float freq = i/(total_samples*sampling_interval);
    // fprintf(fptr, "(%05d) %+3.2f    | %+9.5f j%+9.5f\n", i+1,freq, out[i][0], out[i][1]);
    fprintf(fptr, "%05.2f,%+08.5f,%+08.5f,%+08.5f,%+08.5f\n", freq, out[i][0], out[i][1], 
                                                              input, in[i][0]);
  }

  fclose(fptr);

  fftw_destroy_plan(p);

  /* backward Fourier transform, save the result in 'in2' */
  printf("\nInverse transform:\n");
  q = fftw_plan_dft_1d(total_samples, out, in2, FFTW_BACKWARD, FFTW_ESTIMATE);
  fftw_execute(q);
  /* normalize */
  for (i = 0; i < total_samples; i++) {
    in2[i][0] *= 1./total_samples;
    in2[i][1] *= 1./total_samples;
  }
  
  printf("(00000) Freq\tInput Signal\t\t   Inverse FFT\n");
  for (i = 0; i < total_samples; i++) {
    float input = a+i*sampling_interval;
    printf("(%05d) %+4.3f %+9.5f %+9.5f I vs. %+9.5f %+9.5f I  difference: %+24.22f\n",
        i, input, in[i][0], in[i][1], in2[i][0], in2[i][1], in[i][0]-in2[i][0]);
  }
  fftw_destroy_plan(q);

  fftw_cleanup();

  return 0;
}
