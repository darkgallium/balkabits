#ifndef HAMMING_H
#define HAMMING_H

uint16_t data_to_hamming_buf(uint16_t data);
uint16_t hamming_buf_to_data(uint16_t buf);

uint8_t compute_one_positions(uint8_t *positions, uint16_t hamming_buf);
uint16_t compute_parity_bits(uint16_t *hamming_buf);
void fix_hamming_buf(uint16_t *recvd_hamming_buf);

#endif
