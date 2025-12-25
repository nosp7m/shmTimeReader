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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#include "shmTimeReader.h"

#define NTP_SHM_BASE 0x4e545030  /* NTP0 */

/**
 * Read NTP shared memory segment
 * @param unit: SHM unit number (0-3 typically)
 * @param shm_data: Pointer to structure to store the data
 * @return: 0 on success, -1 on failure
 */
int read_shm_time(int unit, struct shmTime *shm_data) {
    int shmid;
    struct shmTime *shm_ptr;
    key_t key;
    
    /* Calculate the shared memory key for this unit */
    key = NTP_SHM_BASE + unit;
    
    /* Get the shared memory segment ID */
    shmid = shmget(key, sizeof(struct shmTime), 0);
    if (shmid == -1) {
        fprintf(stderr, "Error: Failed to get shared memory segment for unit %d (key: 0x%x)\n", 
                unit, key);
        fprintf(stderr, "Error details: %s\n", strerror(errno));
        fprintf(stderr, "Make sure NTP is running and configured to use SHM driver unit %d\n", unit);
        return -1;
    }
    
    /* Attach to the shared memory segment */
    shm_ptr = (struct shmTime *)shmat(shmid, NULL, SHM_RDONLY);
    if (shm_ptr == (struct shmTime *)-1) {
        fprintf(stderr, "Error: Failed to attach to shared memory segment\n");
        fprintf(stderr, "Error details: %s\n", strerror(errno));
        return -1;
    }
    
    /* Copy the data from shared memory */
    memcpy(shm_data, shm_ptr, sizeof(struct shmTime));
    
    /* Detach from the shared memory segment */
    if (shmdt(shm_ptr) == -1) {
        fprintf(stderr, "Warning: Failed to detach from shared memory\n");
    }
    
    return 0;
}

/**
 * Print the contents of NTP shared memory structure
 * @param shm_data: Pointer to the SHM data structure
 */
void print_shm_time(const struct shmTime *shm_data) {
    printf("NTP Shared Memory Contents:\n");
    printf("===========================\n");
    printf("Mode:                    %d\n", shm_data->mode);
    printf("Count:                   %d\n", shm_data->count);
    printf("Valid:                   %d\n", shm_data->valid);
    printf("Clock Timestamp (sec):   %ld\n", (long)shm_data->clockTimeStampSec);
    printf("Clock Timestamp (usec):  %d\n", shm_data->clockTimeStampUSec);
    printf("Clock Timestamp (nsec):  %u\n", shm_data->clockTimeStampNSec);
    printf("Receive Timestamp (sec): %ld\n", (long)shm_data->receiveTimeStampSec);
    printf("Receive Timestamp (usec):%d\n", shm_data->receiveTimeStampUSec);
    printf("Receive Timestamp (nsec):%u\n", shm_data->receiveTimeStampNSec);
    printf("Leap:                    %d\n", shm_data->leap);
    printf("Precision:               %d\n", shm_data->precision);
    printf("Number of samples:       %d\n", shm_data->nsamples);
    printf("\n");
    
    /* Print human-readable time if valid */
    if (shm_data->valid) {
        char time_str[64];
        struct tm *tm_info = localtime(&shm_data->clockTimeStampSec);
        if (tm_info != NULL) {
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            printf("Clock Time (readable):   %s.%09u\n", time_str, shm_data->clockTimeStampNSec);
        }
        
        tm_info = localtime(&shm_data->receiveTimeStampSec);
        if (tm_info != NULL) {
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);
            printf("Receive Time (readable): %s.%09u\n", time_str, shm_data->receiveTimeStampNSec);
        }
    } else {
        printf("Note: Data is marked as INVALID\n");
    }
}

int main(int argc, char *argv[]) {
    int unit;
    struct shmTime shm_data;
    int show_format = 0;
    
    /* Parse command line arguments */
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s <shm_unit> [--format]\n", argv[0]);
        fprintf(stderr, "  <shm_unit>: NTP shared memory unit number (typically 0-3)\n");
        fprintf(stderr, "  --format:   Display human-readable formatted output\n");
        fprintf(stderr, "  (default):  Output raw bytes to stdout\n");
        fprintf(stderr, "\nExamples:\n");
        fprintf(stderr, "  %s 0          # Output raw bytes\n", argv[0]);
        fprintf(stderr, "  %s 0 --format # Display formatted output\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    /* Parse unit number */
    unit = atoi(argv[1]);
    if (unit < 0) {
        fprintf(stderr, "Error: Invalid unit number. Must be non-negative.\n");
        return EXIT_FAILURE;
    }
    
    /* Check for --format flag */
    if (argc == 3) {
        if (strcmp(argv[2], "--format") == 0) {
            show_format = 1;
        } else {
            fprintf(stderr, "Error: Unknown option '%s'\n", argv[2]);
            fprintf(stderr, "Valid option: --format\n");
            return EXIT_FAILURE;
        }
    }
    
    /* Read the shared memory */
    if (read_shm_time(unit, &shm_data) != 0) {
        return EXIT_FAILURE;
    }
    
    /* Print the data based on requested format */
    if (show_format) {
        /* Human-readable formatted output */
        print_shm_time(&shm_data);
    } else {
        /* Default: raw bytes to stdout */
        fwrite(&shm_data, sizeof(struct shmTime), 1, stdout);
    }
    
    return EXIT_SUCCESS;
}
