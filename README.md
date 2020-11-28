# balkabits

This is a Flush+Reload based covert channel between two unprivileged processes.
It requires some shared library to be mapped by the sender and the receiver, which will be used as a pipe between the two processes using only memory accesses and timing measurements.
These programs allow to exchange between the two processes, via the covert channel, an arbitrary file.

This program is based of [this code](https://github.com/clementine-m/cache_template_attacks) from Clémentine Maurice and IAIK.

## Usage

1. Select a shared library to map, we tested successfully libgedit.so and libspell.so included in gedit.
2. Get your cache hit CPU threshold using Clémentine Maurice's calibration code, and change the `LIMIT` value accordingly in `utils.h`.
3. Compile the program: `make all`.
4. Launch first the sender: `./sender <path to the shared lib> <shared lib offset> <path to the file to send>`.
5. Then launch the receiver: `./listener <path to the shared lib> <shared lib offset> <path to save the file received>`.

If the result is corrupted, it means that there is a lot of noise generated in your caches by other programs. Verify the `LIMIT` threshold and increase the `REPEATS_FOR_SECURE` value progressively. The program will be slower but the result is less likely to be corrupted.

## Performance measurements

Setup:
* Transfer of a file of size 159134 bytes.
* Calm cache setting (a few programs running but < 20% CPU usage)
* `REPEATS_FOR_SECURE = 4` and `LIMIT = 190`

Results:
* 3 bit flips
* Raw capacity: C = 7623.19 bps
* Error rate: p = 2,4e-6
* True capacity: P = 7622.82 bps

## Architecture

![Schema of the architecture](https://raw.githubusercontent.com/darkgallium/balkabits/master/doc/architecture.png)
