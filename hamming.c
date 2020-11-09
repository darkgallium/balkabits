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

uint16_t hamming_buf_to_data(uint16_t buf) {
  uint16_t data = 0;
  uint8_t skipped_bits_no = 0;
  
  // FIXME : we should invert the order because the numbers are stored in little endian on intel architecture and here we store them in big endian
  // just i discovered that i was using the wrong order a little late

  for (int i = 15; i > 0; i--) {

    // if i is not a power of two and lsb of data is 1
    if ((i & (i - 1)) != 0 && (buf & 1)) {
      data += (1 << ( (15 - i) - skipped_bits_no));
    }

    if ((i & (i - 1)) == 0) {
      skipped_bits_no++;
    }

    buf = buf >> 1;
  }

  return data;
}


uint8_t compute_one_positions(uint8_t *positions, uint16_t hamming_buf) {
  uint8_t c = 0;

  for (int i = 15; i > 0; i--) {

    if (hamming_buf & 1) {
      positions[c++] = i;
      //printf("positions[%d]=%d",c,i);
    }
    
    hamming_buf = hamming_buf >> 1;
  }

  return c;

}

uint16_t compute_parity_bits(uint16_t *hamming_buf) {
  uint8_t positions[16];
  uint8_t c = compute_one_positions(positions, *hamming_buf);

  // we XOR all the positions of the ones

  uint16_t xor = 0;
  for (int i = 0; i < c; i++) {
    xor = xor ^ positions[i];
  }
  
  printf("xor = %d\n", xor);

  // FIXME : bit 0 is a special parity bit on the whole buffer as per the definition of a 11/5 hamming code 
  // right now we set it to 1 arbitrarily and we don't check it, this NEEDS to be fixed but i'm tired of this for the moment
  *hamming_buf += (1 << 15);

  // we compute the four parity bits 

  int16_t xor_cpy = xor;
  for (int i = 0; i < 4; i++) {
    if (xor & 1) {
	  // 15 - (2 puissance i) -> 1er bit à set, le bit 1, puis 2, puis 4, puis 8
      *hamming_buf += (1 << (15 - (1 << i)));
    }
    xor = xor >> 1;
  }
  printf("finalbuf=%d\n", *hamming_buf);
  return xor_cpy;
}

void printBits(size_t const size, void const * const ptr)
{
    unsigned char *b = (unsigned char*) ptr;
    unsigned char byte;
    int i, j;
    
    for (i = size-1; i >= 0; i--) {
        for (j = 7; j >= 0; j--) {
            byte = (b[i] >> j) & 1;
            printf("%u", byte);
        }
    }
    puts("");
}


void fix_hamming_buf(uint16_t *recvd_hamming_buf) {
    int16_t computed_hamming_buf = *recvd_hamming_buf;
    // reusing the compute_parity_bits function to get the parity bits/xor, this is not really clever, we could extract 
    // these bits directly instead but rn i'm tired of bitwise operators
    int16_t xor = compute_parity_bits(&computed_hamming_buf);

    // FIXME check bit 0 (whole buf parity bit)

    if (xor != 0) {
        printf("error detected in recvd buffer at index %d\n", xor);
        // flip the bit at index xor, xor 1 always flips the bit
        computed_hamming_buf ^= (1 << (15-xor));
	printf("%u\n", computed_hamming_buf);
	
	*recvd_hamming_buf = computed_hamming_buf;
    }
}
