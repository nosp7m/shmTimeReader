# shmTimeReader

A Linux C program to read and display NTP (Network Time Protocol) shared memory segments. This utility allows you to inspect the time data that NTP daemons expose through System V shared memory using the SHM driver interface.

## Description

The NTP daemon can share time information with other applications through shared memory segments. This program reads those segments and displays the timing data in both human-readable and raw formats.

The program reads the NTP shared memory structure which includes:

- Clock and receive timestamps (seconds, microseconds, nanoseconds)
- Validity flags and mode information
- Leap second indicators
- Precision and sample count data

## Requirements

- Linux operating system
- GCC compiler (or compatible C compiler)
- Make build tool
- NTP daemon configured with SHM driver (to have data to read)

## Compilation

### Standard Build

To compile the program, simply run:

```bash
make
```

This will create the `shmTimeReader` executable in the current directory.

### Custom Compiler Options

You can override the default compiler or flags:

```bash
make CC=clang
make CFLAGS="-Wall -O3 -march=native"
```

### Clean Build

To remove compiled objects and start fresh:

```bash
make clean
```

## Installation

### System-wide Installation

To install the program system-wide (requires root/sudo):

```bash
sudo make install
```

By default, this installs to `/usr/local/bin/shmTimeReader`.

### Custom Installation Path

To install to a different location:

```bash
make PREFIX=/opt/local install
```

Or for user-local installation:

```bash
make PREFIX=$HOME/.local install
```

Make sure the installation directory is in your PATH.

### Uninstallation

To remove the installed program:

```bash
sudo make uninstall
```

## Usage

### Basic Usage

```bash
./shmTimeReader <shm_unit> [--format]
```

Where `<shm_unit>` is the NTP shared memory unit number (typically 0-3).

### Examples

Read NTP shared memory unit 0 (outputs raw bytes to stdout):

```bash
./shmTimeReader 0
```

Read unit 0 and display human-readable formatted output:

```bash
./shmTimeReader 0 --format
```

Save raw bytes to a file:

```bash
./shmTimeReader 0 > shm_data.bin
```

Pipe to hexdump for analysis:

```bash
./shmTimeReader 0 | hexdump -C
```

Pipe to od for octal/hex display:

```bash
./shmTimeReader 0 | od -A x -t x1z -v
```

### Command Line Options

- `<shm_unit>`: Required. The NTP SHM unit number to read (non-negative integer)
- `--format`: Optional. Display human-readable formatted output with timestamps
- Default (no option): Output raw bytes directly to stdout

### Sample Output

```text
NTP Shared Memory Contents:
===========================
Mode:                    1
Count:                   12345
Valid:                   1
Clock Timestamp (sec):   1703520123
Clock Timestamp (usec):  456789
Clock Timestamp (nsec):  456789000
Receive Timestamp (sec): 1703520123
Receive Timestamp (usec):456790
Receive Timestamp (nsec):456790000
Leap:                    0
Precision:               -20
Number of samples:       8

Clock Time (readable):   2023-12-25 12:15:23.456789000
Receive Time (readable): 2023-12-25 12:15:23.456790000
```

## Shared Memory Key Calculation

The program uses the NTP standard shared memory key calculation:

- Base key: `0x4e545030` (NTP0)
- Unit 0: `0x4e545030`
- Unit 1: `0x4e545031`
- Unit 2: `0x4e545032`
- Unit 3: `0x4e545033`

## Troubleshooting

### "Failed to get shared memory segment"

This error means the shared memory segment doesn't exist. Common causes:

- Refclock or NTP/Chrony daemon(s) are not running
- Refclock or NTP/Chrony are not configured with the SHM driver for that unit number
- Wrong unit number specified

**Solution**:

1. Check if daemon(s) are running
2. Verify configuration includes SHM driver
3. Use `ipcs -m` to list available shared memory segments

### Permission Denied

The shared memory segment may have restricted permissions.

**Solution**:

- Run with appropriate permissions
- Check shared memory permissions with `ipcs -m`
- May need to adjust NTP daemon user/group settings

### Invalid Data

If the `valid` field is 0, the time data is not valid yet.

**Solution**:

- Wait for NTP to acquire time data from its sources
- Check NTP daemon logs for issues
- Verify time source is available

## Development

### File Structure

- `shmTimeReader.c` - Main program implementation
- `shmTimeReader.h` - Header file with structure definitions
- `Makefile` - Build configuration
- `README.md` - This documentation

### NTP SHM Structure

The program uses the standard NTP shared memory structure with fields for:

- Mode and count (for synchronization)
- Clock and receive timestamps (sec, usec, nsec)
- Leap second indicator
- Precision and sample count
- Valid flag

## License

BSD-3-Clause License

Copyright (c) 2025, shmTimeReader contributors
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.

## References

- NTP Project: <https://www.ntp.org/>
- NTP SHM Driver Documentation: <https://www.ntp.org/documentation/drivers/driver28/>
- Chrony SHM Reference: <https://chrony.tuxfamily.org/>

## Author

Created for reading NTP shared memory segments on Linux systems.

## Version

Version 1.0 - Initial release
