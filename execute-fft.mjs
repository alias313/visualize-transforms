import { execFile } from 'node:child_process'

const child = execFile('./ifft_example_exec', [process.argv[2]], (error, stdout, stderr) => {
  if (error) {
    throw error
  }
  console.log(stdout)
})
