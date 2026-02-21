// EmbeddedResource.hpp — Helpers for accessing compiled-in data resources.
#ifndef EMBEDDED_RESOURCE_HPP
#define EMBEDDED_RESOURCE_HPP

#include <cstddef>
#include <cstring>
#include <string_view>

// Simple in-memory "FILE" for parsing text resources line by line.
struct MemStream {
    const char* data;
    std::size_t size;
    std::size_t pos = 0;

    MemStream(const unsigned char* d, std::size_t s)
        : data(reinterpret_cast<const char*>(d)), size(s) {}

    // Read one line into buf (like fgets). Returns buf on success, nullptr at EOF.
    char* gets(char* buf, int bufsize) {
        if (pos >= size) return nullptr;
        int i = 0;
        while (pos < size && i < bufsize - 1) {
            char c = data[pos++];
            buf[i++] = c;
            if (c == '\n') break;
        }
        buf[i] = '\0';
        return buf;
    }
};

// Declarations for all embedded WAV resources (Convolotron)
extern unsigned char res_1_wav[];  extern unsigned int res_1_wav_len;
extern unsigned char res_2_wav[];  extern unsigned int res_2_wav_len;
extern unsigned char res_3_wav[];  extern unsigned int res_3_wav_len;
extern unsigned char res_4_wav[];  extern unsigned int res_4_wav_len;
extern unsigned char res_5_wav[];  extern unsigned int res_5_wav_len;
extern unsigned char res_6_wav[];  extern unsigned int res_6_wav_len;
extern unsigned char res_7_wav[];  extern unsigned int res_7_wav_len;
extern unsigned char res_8_wav[];  extern unsigned int res_8_wav_len;
extern unsigned char res_9_wav[];  extern unsigned int res_9_wav_len;

// Declarations for all embedded RVB resources (Reverbtron)
extern unsigned char res_1_rvb[];  extern unsigned int res_1_rvb_len;
extern unsigned char res_2_rvb[];  extern unsigned int res_2_rvb_len;
extern unsigned char res_3_rvb[];  extern unsigned int res_3_rvb_len;
extern unsigned char res_4_rvb[];  extern unsigned int res_4_rvb_len;
extern unsigned char res_5_rvb[];  extern unsigned int res_5_rvb_len;
extern unsigned char res_6_rvb[];  extern unsigned int res_6_rvb_len;
extern unsigned char res_7_rvb[];  extern unsigned int res_7_rvb_len;
extern unsigned char res_8_rvb[];  extern unsigned int res_8_rvb_len;
extern unsigned char res_9_rvb[];  extern unsigned int res_9_rvb_len;
extern unsigned char res_10_rvb[]; extern unsigned int res_10_rvb_len;

// Declarations for all embedded DLY resources (Echotron)
extern unsigned char res_1_dly[];  extern unsigned int res_1_dly_len;
extern unsigned char res_2_dly[];  extern unsigned int res_2_dly_len;
extern unsigned char res_3_dly[];  extern unsigned int res_3_dly_len;
extern unsigned char res_4_dly[];  extern unsigned int res_4_dly_len;
extern unsigned char res_5_dly[];  extern unsigned int res_5_dly_len;
extern unsigned char res_6_dly[];  extern unsigned int res_6_dly_len;
extern unsigned char res_7_dly[];  extern unsigned int res_7_dly_len;
extern unsigned char res_8_dly[];  extern unsigned int res_8_dly_len;
extern unsigned char res_9_dly[];  extern unsigned int res_9_dly_len;
extern unsigned char res_10_dly[]; extern unsigned int res_10_dly_len;
extern unsigned char res_11_dly[]; extern unsigned int res_11_dly_len;

// Declarations for embedded bank resources (fileio)
extern unsigned char res_Default_rkrb[]; extern unsigned int res_Default_rkrb_len;
extern unsigned char res_Extra_rkrb[];   extern unsigned int res_Extra_rkrb_len;
extern unsigned char res_Extra1_rkrb[];  extern unsigned int res_Extra1_rkrb_len;

// Declarations for embedded PNG resources (GUI backgrounds)
extern unsigned char res_bg_png[];            extern unsigned int res_bg_png_len;
extern unsigned char res_bg1_png[];           extern unsigned int res_bg1_png_len;
extern unsigned char res_bg2_png[];           extern unsigned int res_bg2_png_len;
extern unsigned char res_bg3_png[];           extern unsigned int res_bg3_png_len;
extern unsigned char res_bg4_png[];           extern unsigned int res_bg4_png_len;
extern unsigned char res_bg5_png[];           extern unsigned int res_bg5_png_len;
extern unsigned char res_bg6_png[];           extern unsigned int res_bg6_png_len;
extern unsigned char res_bg_gray_furr_png[];  extern unsigned int res_bg_gray_furr_png_len;

// Lookup tables indexed by Filenum (0-based)
struct ResourceEntry {
    unsigned char* data;
    unsigned int* len;
};

inline const ResourceEntry wav_resources[] = {
    {res_1_wav, &res_1_wav_len}, {res_2_wav, &res_2_wav_len},
    {res_3_wav, &res_3_wav_len}, {res_4_wav, &res_4_wav_len},
    {res_5_wav, &res_5_wav_len}, {res_6_wav, &res_6_wav_len},
    {res_7_wav, &res_7_wav_len}, {res_8_wav, &res_8_wav_len},
    {res_9_wav, &res_9_wav_len},
};

inline const ResourceEntry rvb_resources[] = {
    {res_1_rvb, &res_1_rvb_len}, {res_2_rvb, &res_2_rvb_len},
    {res_3_rvb, &res_3_rvb_len}, {res_4_rvb, &res_4_rvb_len},
    {res_5_rvb, &res_5_rvb_len}, {res_6_rvb, &res_6_rvb_len},
    {res_7_rvb, &res_7_rvb_len}, {res_8_rvb, &res_8_rvb_len},
    {res_9_rvb, &res_9_rvb_len}, {res_10_rvb, &res_10_rvb_len},
};

inline const ResourceEntry dly_resources[] = {
    {res_1_dly, &res_1_dly_len},  {res_2_dly, &res_2_dly_len},
    {res_3_dly, &res_3_dly_len},  {res_4_dly, &res_4_dly_len},
    {res_5_dly, &res_5_dly_len},  {res_6_dly, &res_6_dly_len},
    {res_7_dly, &res_7_dly_len},  {res_8_dly, &res_8_dly_len},
    {res_9_dly, &res_9_dly_len},  {res_10_dly, &res_10_dly_len},
    {res_11_dly, &res_11_dly_len},
};

inline constexpr int NUM_WAV_RESOURCES = 9;
inline constexpr int NUM_RVB_RESOURCES = 10;
inline constexpr int NUM_DLY_RESOURCES = 11;

// PNG background lookup by basename (e.g. "bg.png")
struct NamedResource {
    const char* name;
    unsigned char* data;
    unsigned int* len;
};

inline const NamedResource png_resources[] = {
    {"bg.png",           res_bg_png,           &res_bg_png_len},
    {"bg1.png",          res_bg1_png,          &res_bg1_png_len},
    {"bg2.png",          res_bg2_png,          &res_bg2_png_len},
    {"bg3.png",          res_bg3_png,          &res_bg3_png_len},
    {"bg4.png",          res_bg4_png,          &res_bg4_png_len},
    {"bg5.png",          res_bg5_png,          &res_bg5_png_len},
    {"bg6.png",          res_bg6_png,          &res_bg6_png_len},
    {"bg_gray_furr.png", res_bg_gray_furr_png, &res_bg_gray_furr_png_len},
};
inline constexpr int NUM_PNG_RESOURCES = 8;

// Find an embedded PNG by matching the basename at end of path.
inline const NamedResource* find_embedded_png(const char* path) {
    std::string_view sv(path);
    for (const auto& r : png_resources) {
        std::string_view name(r.name);
        if (sv.size() >= name.size() &&
            sv.substr(sv.size() - name.size()) == name &&
            (sv.size() == name.size() || sv[sv.size() - name.size() - 1] == '/' ||
             sv[sv.size() - name.size() - 1] == '\\')) {
            return &r;
        }
    }
    return nullptr;
}

#endif // EMBEDDED_RESOURCE_HPP
