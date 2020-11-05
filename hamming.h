#ifndef HAMMING_H
#define HAMMING_H

uint16_t data_to_hamming_buf(uint16_t data);
uint8_t compute_one_positions(uint8_t *positions, uint16_t hamming_buf);
uint16_t compute_parity_bits(uint16_t hamming_buf);

#endif
