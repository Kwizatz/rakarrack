/*
 * rakconvert - Convert old-format rakarrack bank files to the current format.
 *
 * Copyright (C) 2008-2010 Josep Andreu
 * Copyright (C) 2010 Ryan Billing
 * Copyright (C) 2010 Douglas McClendon
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of version 2 of the GNU General Public License
 * as published by the Free Software Foundation.
 */

#include <cstdio>
#include <getopt.h>
#include "rakconvert_lib.hpp"

static void
show_help()
{
    fprintf(stderr, "Usage: rakconvert -c <bankfile>\n\n");
    fprintf(stderr, "Convert old-format rakarrack bank files (pre-0.5.0) to the current format.\n");
    fprintf(stderr, "Also imports the associated .ml MIDI-learn file if present.\n\n");
    fprintf(stderr, "  -c, --convert <file>   old .rkrb bank file to convert\n");
    fprintf(stderr, "  -h, --help             display this help and exit\n");
    fprintf(stderr, "\nOutput: <name>01_050.rkrb and <name>02_050.rkrb\n\n");
}

int
main(int argc, char* argv[])
{
    int option_index = 0, opt;
    int exitwithhelp = 0;
    const char* bank_file = nullptr;

    fprintf(stderr,
        "\nrakconvert convert old data bank files to the new file format "
        "and store the new ones in the home user directory.\n"
        "rackconvert - Copyright (c) Josep Andreu - Ryan Billing - Douglas McClendon\n\n");

    struct option opts[] = {
        {"convert", 1, nullptr, 'c'},
        {"help", 0, nullptr, 'h'},
        {0, 0, 0, 0}
    };

    while (1) {
        opt = getopt_long(argc, argv, "c:h", opts, &option_index);
        if (opt == -1)
            break;

        switch (opt) {
        case 'h':
            exitwithhelp = 1;
            break;
        case 'c':
            if (optarg != nullptr)
                bank_file = optarg;
            break;
        }
    }

    if (exitwithhelp != 0) {
        show_help();
        return 0;
    }

    if (bank_file == nullptr) {
        fprintf(stderr, "Try 'rakconvert --help' for usage options.\n");
        return 0;
    }

    return rakconvert_convert_bank(bank_file);
}
