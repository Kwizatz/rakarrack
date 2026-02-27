/*
 * rakverb library - Convert reverb impulse response WAV files
 * to the rakarrack .rvb format.
 *
 * Copyright (C) 2008-2010 Josep Andreu
 * Copyright (C) 2010 Ryan Billing
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 */

#include "rakverb_lib.hpp"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
#include <string>
#include <sndfile.h>

namespace {

constexpr int LIB_PERIOD = 128;

#define RND (rand() / ((RAND_MAX) + 1.0f))

} // anonymous namespace


int rakverb_convert(const char* input_file, const char* output_file)
{
    if (!input_file) return 1;

    char wbuf[2048];
    int i, j;
    int step;
    int readcount;
    FILE* fn;
    SNDFILE* infile = nullptr;
    SF_INFO sfinfo;
    char Outputfile[512];
    char tempfile[256];
    float* buf;
    float* index;
    float* data;
    float sample{0.0f};
    float testzero{0.0f}, time{0.0f}, tmp{0.0f}, iSR{0.0f}, lastbuf{0.0f};
    float skip{0.0f};
    int indexx{0};
    float chunk{0.0f}, incr{0.0f}, findex{0.0f};
    float compress{0.0f}, quality{0.0f};
    int x = 0;

    // Open input WAV file
    memset(&sfinfo, 0, sizeof(sfinfo));
    if (!(infile = sf_open(input_file, SFM_READ, &sfinfo))) {
        fprintf(stderr, "rakverb: failed to open input file: %s\n", input_file);
        return 1;
    }

    // Determine output filename
    if (output_file && output_file[0] != '\0') {
        snprintf(Outputfile, sizeof(Outputfile), "%s", output_file);
    } else {
        strncpy(tempfile, input_file, sizeof(tempfile) - 1);
        tempfile[sizeof(tempfile) - 1] = '\0';
        // Strip last 4 chars (e.g. ".wav") and add .rvb
        size_t len = strlen(tempfile);
        if (len > 4) tempfile[len - 4] = 0;
        snprintf(Outputfile, sizeof(Outputfile), "%s.rvb", tempfile);
    }

    // Allocate buffers
    buf   = (float*)malloc(sizeof(float) * LIB_PERIOD * sfinfo.channels);
    index = (float*)malloc(sizeof(float) * sfinfo.frames * sfinfo.channels);
    data  = (float*)malloc(sizeof(float) * sfinfo.frames * sfinfo.channels);

    if (!buf || !index || !data) {
        fprintf(stderr, "rakverb: memory allocation failed\n");
        sf_close(infile);
        free(buf);
        free(index);
        free(data);
        return 1;
    }

    // Open output and write header
    if ((fn = fopen(Outputfile, "w")) == nullptr) {
        fprintf(stderr, "rakverb: failed to open output file: %s\n", Outputfile);
        sf_close(infile);
        free(buf);
        free(index);
        free(data);
        return 1;
    }

    memset(wbuf, 0, sizeof(wbuf));
    sprintf(wbuf, "%s\n", input_file);
    fputs(wbuf, fn);

    // Process audio data
    readcount = sf_seek(infile, 0, SEEK_SET);
    readcount = 1;
    time = 0.0f;
    tmp = 0.0f;
    lastbuf = 0.0f;
    iSR = 1.0f / ((float)sfinfo.samplerate);
    if (sfinfo.channels == 1) step = 1;
    if (sfinfo.channels == 2) step = 2;

    while (readcount > 0) {
        readcount = sf_readf_float(infile, buf, LIB_PERIOD);

        for (i = 0; i < (LIB_PERIOD * sfinfo.channels); i += step) {
            if (step == 1) sample = buf[i];
            if (step == 2) sample = (buf[i] + buf[i + 1]) * .5;

            tmp += sample;
            testzero = sample * lastbuf;
            if (testzero < 0.0f) {
                data[x] = tmp;
                index[x] = time;
                x++;
                tmp = 0.0f;
            }
            time += iSR;
            lastbuf = sample;
        }
    }

    incr = 1500.0f / ((float)x);
    printf("incr: %f\n", incr);
    if (x < 1500) incr = 1.0f;

    compress = 100.0f * ((float)x) / ((float)sfinfo.frames);
    printf("Compression: %3.2f%%\n", compress);
    quality = incr * compress;
    printf("Quality : %3.2f%%\n", quality);
    sf_close(infile);

    memset(wbuf, 0, sizeof(wbuf));
    sprintf(wbuf, "%f,%f\n", compress, quality);
    fputs(wbuf, fn);

    skip = 0.0f;
    indexx = 1500;
    chunk = 11;

    memset(wbuf, 0, sizeof(wbuf));
    sprintf(wbuf, "%d\n", indexx);
    fputs(wbuf, fn);

    skip = 0.0f;
    indexx = 0;
    double dchunk = 10.0;
    chunk = 25;
    if (x < 1500) x = 1500;
    for (i = 0; i < x; i++) {
        skip += incr;
        findex = (float)indexx;
        if (findex < skip) {
            for (j = 0; j <= chunk; j++) {
                if (indexx < 1500) {
                    memset(wbuf, 0, sizeof(wbuf));
                    sprintf(wbuf, "%f,%f\n", index[i + j], data[i + j]);
                    fputs(wbuf, fn);
                    indexx++;
                }
            }
            chunk = (int)(dchunk * RND);
            dchunk *= 0.99;
        }
    }

    fclose(fn);
    free(buf);
    free(index);
    free(data);
    return 0;
}
