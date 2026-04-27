#pragma once
#include "../core/grammar.h"
#include <optional>
#include <string>
#include <vector>

// ── Parser ───────────────────────────────────────────────────────────────────
// Accepts grammars in the format:
//   NonTerminal -> sym1 sym2 sym3
//   NonTerminal -> alt1 | alt2 alt3
//
// Rules for the same NT can span multiple lines.
// EPS or null represent the empty string (ε).
// Lines starting with '#' are treated as comments.
// ─────────────────────────────────────────────────────────────────────────────
class Parser {
public:
    static std::optional<Grammar> fromFile   (const std::string& filename);
    static std::optional<Grammar> fromConsole();

private:
    static std::optional<Grammar>          parseLines(const std::vector<std::string>& lines);
    static std::vector<std::string>        tokenize  (const std::string& str);
    static void                            trim      (std::string& s);
};
