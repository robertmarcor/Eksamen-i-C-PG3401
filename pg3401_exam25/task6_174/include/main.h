#ifndef MAIN_H
#define MAIN_H

// TEA decryption function declarations - using 32-bit unsigned int instead of long
void decipher(unsigned int *const v, unsigned int *const w, const unsigned int *const k);
void encipher(unsigned int *const v, unsigned int *const w, const unsigned int *const k);

#endif // MAIN_H