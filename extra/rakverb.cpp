/*
 * rakverb - Convert reverb impulse response WAV files to rakarrack .rvb format.
 *
 * Copyright (C) 2008-2010 Josep Andreu
 * Copyright (C) 2010 Ryan Billing
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 */

#include <cstdio>
#include <cstring>
#include <getopt.h>
#include "rakverb_lib.hpp"

static void
show_help()
{
    fprintf(stderr, "Usage: rakverb -i <input.wav> [-o <output.rvb>]\n\n");
    fprintf(stderr, "Convert a reverb impulse response WAV file to rakarrack .rvb format.\n\n");
    fprintf(stderr, "  -i, --input <file>     input WAV file (impulse response)\n");
    fprintf(stderr, "  -o, --output <file>    output .rvb file (default: <input>.rvb)\n");
    fprintf(stderr, "  -h, --help             display this help and exit\n\n");
}

int
main(int argc, char* argv[])
{
    int option_index = 0, opt;
    int exitwithhelp = 0;
    const char* input_file = nullptr;
    const char* output_file = nullptr;

    fprintf(stderr,
        "\nrackverb convert Reverb IR wav files to the rakarrack file format.\n"
        "rackverb - Copyright (c) Josep Andreu - Ryan Billing \n\n");

    struct option opts[] = {
        {"input", 1, nullptr, 'i'},
        {"output", 1, nullptr, 'o'},
        {"help", 0, nullptr, 'h'},
        {0, 0, 0, 0}
    };

    while (1) {
        opt = getopt_long(argc, argv, "i:o:h", opts, &option_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 'h':
            exitwithhelp = 1;
            break;
        case 'i':
            if (optarg != nullptr)
                input_file = optarg;
            break;
        case 'o':
            if (optarg != nullptr)
                output_file = optarg;
            break;
        }
    }

    if (exitwithhelp != 0) {
        show_help();
        return 0;
    }

    if (input_file == nullptr) {
        fprintf(stderr, "Try 'rakverb --help' for usage options.\n");
        return 0;
    }

    return rakverb_convert(input_file, output_file);
}
