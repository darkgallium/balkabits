#include <stdio.h>
#include <stdint.h>

uint16_t data_to_hamming_buf(uint16_t data) {
  uint16_t buf = 0;

  for (int i = 15; i > 0; i--) {
    // if i is not a power of two and lsb of data is 1
    if ((i & (i - 1)) != 0 && (data & 1)) {
      buf += (1 << (15-i)); 
    }

    data = data >> 1;
  }

  return buf;
}

uint8_t compute_one_positions(uint8_t *positions, uint16_t hamming_buf) {
  uint8_t c = 0;

  for (int i = 15; i > 0; i--) {

    if (hamming_buf & 1) {
      positions[c++] = i;
    }
    
    hamming_buf = hamming_buf >> 1;
  }

  return c;

}

uint16_t compute_parity_bits(uint16_t hamming_buf) {
  uint8_t positions[16];
  uint8_t c = compute_one_positions(positions, hamming_buf);

  uint16_t xor = 0;
  for (int i = 0; i < c; i++) {
    xor = xor ^ positions[i];
  }
  
  printf("xor = %d\n", xor);
  hamming_buf += (1 << 15);

  for (int i = 0; i < 4; i++) {
    if (xor & (1 << i)) {
      hamming_buf += (1 << (15-(1 << i)));
    }
  }
  printf("finalbuf=%d\n", hamming_buf);
  return hamming_buf;
}

/*int main(void) {
  uint16_t buf = data_to_hamming_buf(0xff);
  compute_parity_bits(buf);
  return 0;
}*/
