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
extern unsigned char __1_wav[];  extern unsigned int __1_wav_len;
extern unsigned char __2_wav[];  extern unsigned int __2_wav_len;
extern unsigned char __3_wav[];  extern unsigned int __3_wav_len;
extern unsigned char __4_wav[];  extern unsigned int __4_wav_len;
extern unsigned char __5_wav[];  extern unsigned int __5_wav_len;
extern unsigned char __6_wav[];  extern unsigned int __6_wav_len;
extern unsigned char __7_wav[];  extern unsigned int __7_wav_len;
extern unsigned char __8_wav[];  extern unsigned int __8_wav_len;
extern unsigned char __9_wav[];  extern unsigned int __9_wav_len;

// Declarations for all embedded RVB resources (Reverbtron)
extern unsigned char __1_rvb[];  extern unsigned int __1_rvb_len;
extern unsigned char __2_rvb[];  extern unsigned int __2_rvb_len;
extern unsigned char __3_rvb[];  extern unsigned int __3_rvb_len;
extern unsigned char __4_rvb[];  extern unsigned int __4_rvb_len;
extern unsigned char __5_rvb[];  extern unsigned int __5_rvb_len;
extern unsigned char __6_rvb[];  extern unsigned int __6_rvb_len;
extern unsigned char __7_rvb[];  extern unsigned int __7_rvb_len;
extern unsigned char __8_rvb[];  extern unsigned int __8_rvb_len;
extern unsigned char __9_rvb[];  extern unsigned int __9_rvb_len;
extern unsigned char __10_rvb[]; extern unsigned int __10_rvb_len;

// Declarations for all embedded DLY resources (Echotron)
extern unsigned char __1_dly[];  extern unsigned int __1_dly_len;
extern unsigned char __2_dly[];  extern unsigned int __2_dly_len;
extern unsigned char __3_dly[];  extern unsigned int __3_dly_len;
extern unsigned char __4_dly[];  extern unsigned int __4_dly_len;
extern unsigned char __5_dly[];  extern unsigned int __5_dly_len;
extern unsigned char __6_dly[];  extern unsigned int __6_dly_len;
extern unsigned char __7_dly[];  extern unsigned int __7_dly_len;
extern unsigned char __8_dly[];  extern unsigned int __8_dly_len;
extern unsigned char __9_dly[];  extern unsigned int __9_dly_len;
extern unsigned char __10_dly[]; extern unsigned int __10_dly_len;
extern unsigned char __11_dly[]; extern unsigned int __11_dly_len;

// Declarations for embedded bank resources (fileio)
extern unsigned char Default_rkrb[]; extern unsigned int Default_rkrb_len;
extern unsigned char Extra_rkrb[];   extern unsigned int Extra_rkrb_len;
extern unsigned char Extra1_rkrb[];  extern unsigned int Extra1_rkrb_len;

// Declarations for embedded PNG resources (GUI backgrounds)
extern unsigned char bg_png[];            extern unsigned int bg_png_len;
extern unsigned char bg1_png[];           extern unsigned int bg1_png_len;
extern unsigned char bg2_png[];           extern unsigned int bg2_png_len;
extern unsigned char bg3_png[];           extern unsigned int bg3_png_len;
extern unsigned char bg4_png[];           extern unsigned int bg4_png_len;
extern unsigned char bg5_png[];           extern unsigned int bg5_png_len;
extern unsigned char bg6_png[];           extern unsigned int bg6_png_len;
extern unsigned char bg_gray_furr_png[];  extern unsigned int bg_gray_furr_png_len;

// Lookup tables indexed by Filenum (0-based)
struct ResourceEntry {
    unsigned char* data;
    unsigned int* len;
};

inline const ResourceEntry wav_resources[] = {
    {__1_wav, &__1_wav_len}, {__2_wav, &__2_wav_len},
    {__3_wav, &__3_wav_len}, {__4_wav, &__4_wav_len},
    {__5_wav, &__5_wav_len}, {__6_wav, &__6_wav_len},
    {__7_wav, &__7_wav_len}, {__8_wav, &__8_wav_len},
    {__9_wav, &__9_wav_len},
};

inline const ResourceEntry rvb_resources[] = {
    {__1_rvb, &__1_rvb_len}, {__2_rvb, &__2_rvb_len},
    {__3_rvb, &__3_rvb_len}, {__4_rvb, &__4_rvb_len},
    {__5_rvb, &__5_rvb_len}, {__6_rvb, &__6_rvb_len},
    {__7_rvb, &__7_rvb_len}, {__8_rvb, &__8_rvb_len},
    {__9_rvb, &__9_rvb_len}, {__10_rvb, &__10_rvb_len},
};

inline const ResourceEntry dly_resources[] = {
    {__1_dly, &__1_dly_len},  {__2_dly, &__2_dly_len},
    {__3_dly, &__3_dly_len},  {__4_dly, &__4_dly_len},
    {__5_dly, &__5_dly_len},  {__6_dly, &__6_dly_len},
    {__7_dly, &__7_dly_len},  {__8_dly, &__8_dly_len},
    {__9_dly, &__9_dly_len},  {__10_dly, &__10_dly_len},
    {__11_dly, &__11_dly_len},
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
    {"bg.png",           bg_png,           &bg_png_len},
    {"bg1.png",          bg1_png,          &bg1_png_len},
    {"bg2.png",          bg2_png,          &bg2_png_len},
    {"bg3.png",          bg3_png,          &bg3_png_len},
    {"bg4.png",          bg4_png,          &bg4_png_len},
    {"bg5.png",          bg5_png,          &bg5_png_len},
    {"bg6.png",          bg6_png,          &bg6_png_len},
    {"bg_gray_furr.png", bg_gray_furr_png, &bg_gray_furr_png_len},
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
