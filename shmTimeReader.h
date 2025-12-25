/*
 * shmTimeReader - NTP Shared Memory Reader
 * 
 * Copyright (c) 2025, shmTimeReader contributors
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SHMTIMEREADER_H
#define SHMTIMEREADER_H

#include <stdint.h>
#include <time.h>

/* NTP SHM structure definition based on NTP documentation
 * This structure matches the shared memory interface used by NTP
 */
struct shmTime {
    int    mode;           /* 0 - if valid set
                            *       use values,
                            *       clear valid
                            * 1 - if valid set
                            *       if count before and after read of data is equal,
                            *         use values
                            *       clear valid
                            */
    volatile int    count;
    time_t          clockTimeStampSec;
    int             clockTimeStampUSec;
    time_t          receiveTimeStampSec;
    int             receiveTimeStampUSec;
    int             leap;
    int             precision;
    int             nsamples;
    volatile int    valid;
    unsigned        clockTimeStampNSec;     /* Unsigned ns timestamps */
    unsigned        receiveTimeStampNSec;
    int             dummy[8];
};

/* Function prototypes */
int read_shm_time(int unit, struct shmTime *shm_data);
void print_shm_time(const struct shmTime *shm_data);

#endif /* SHMTIMEREADER_H */
