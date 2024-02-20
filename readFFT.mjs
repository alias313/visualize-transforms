import { readFile } from 'node:fs/promises';

console.log('Leyendo el primer archivo...');
const fftText = await readFile('./fft_out.txt', 'utf-8');

const totalSamples = parseInt(fftText.slice(0, fftText.indexOf("\n")));
let foundPos = fftText.indexOf("\n") + 1;

let pos = fftText.indexOf("\n", foundPos) + 1;

const fft = []

for (let i = 0; i <= totalSamples; i++) {
  console.log(fftText.slice(pos, fftText.indexOf("\n", pos)));

  let startPosFreq = fftText.indexOf(")", pos) + 1;
  let endPosFreq = fftText.indexOf("|", pos);
  let startPosFFT = fftText.indexOf("|", pos)+1;
  let endPosFFT = fftText.indexOf("j", pos);
  fft[i] = [parseFloat(fftText.substring(startPosFreq, endPosFreq)),
            parseFloat(fftText.substring(startPosFFT, endPosFFT))];

  foundPos = fftText.indexOf("\n", pos);
  pos = foundPos + 1; // continue the search from the next position
};

console.log(fft)
