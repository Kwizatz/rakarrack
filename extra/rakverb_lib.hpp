/*
 * rakverb library - Convert reverb impulse response WAV files
 * to the rakarrack .rvb format.
 *
 * Copyright (C) 2008-2010 Josep Andreu
 * Copyright (C) 2010 Ryan Billing
 */

#pragma once

/// Convert a reverb impulse response WAV file to rakarrack .rvb format.
/// If output_file is nullptr, the output filename is derived from the input
/// by replacing the extension with .rvb.
/// Returns 0 on success, non-zero on failure.
int rakverb_convert(const char* input_file, const char* output_file = nullptr);
