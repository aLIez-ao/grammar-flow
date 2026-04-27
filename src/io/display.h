#pragma once
#include "../core/grammar.h"

// ── Display ──────────────────────────────────────────────────────────────────
// Pretty-prints grammar information and FIRST/FOLLOW result tables
// to standard output.
// ─────────────────────────────────────────────────────────────────────────────
class Display {
public:
    static void printBanner ();
    static void printGrammar(const Grammar& g);
    static void printTable  (const Grammar&       g,
                             const FirstFollowMap& firstSets,
                             const FirstFollowMap& followSets);
private:
    static std::string     setToString      (const StringSet& s);
    static void            printSeparator   (int c1, int c2, int c3);
};
