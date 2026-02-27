/*
 * rakconvert library - Convert old-format rakarrack bank files
 * to the current format.
 *
 * Copyright (C) 2008-2010 Josep Andreu
 * Copyright (C) 2010 Ryan Billing
 * Copyright (C) 2010 Douglas McClendon
 */

#pragma once

/// Convert an old-format rakarrack bank file to the current format.
/// Produces two new files: <basename>01_050.rkrb and <basename>02_050.rkrb
/// Also reads the associated .ml MIDI-learn file if present.
/// Returns 0 on success, non-zero on failure.
int rakconvert_convert_bank(const char* old_bank_file);
