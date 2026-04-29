#include "parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// ── Helpers ──────────────────────────────────────────────────────────────────
void Parser::trim(std::string& s) {
    s.erase(s.begin(),
            std::find_if(s.begin(), s.end(),
                         [](unsigned char c){ return !std::isspace(c); }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
                         [](unsigned char c){ return !std::isspace(c); }).base(),
            s.end());
}

std::vector<std::string> Parser::tokenize(const std::string& str) {
    std::vector<std::string> tokens;
    std::istringstream iss(str);
    std::string token;
    while (iss >> token) tokens.push_back(token);
    return tokens;
}

// ── Core parse logic ─────────────────────────────────────────────────────────
std::optional<Grammar> Parser::parseLines(const std::vector<std::string>& lines) {

    // ── Raw structures before symbol classification ──────────────────────────
    struct RawRule {
        std::string                        lhs;
        std::vector<std::vector<std::string>> prods;  // each prod = list of tokens
    };

    std::vector<RawRule> rawRules;
    StringSet            ntSet;
    std::vector<std::string> ntOrder;

    // ── Pass 1: collect LHS names and raw productions ────────────────────────
    for (const auto& rawLine : lines) {
        std::string line = rawLine;
        trim(line);
        if (line.empty() || line[0] == '#') continue;

        auto arrowPos = line.find("->");
        if (arrowPos == std::string::npos) {
            std::cerr << "[Error] Formato invalido (falta '->'): " << line << "\n";
            return std::nullopt;
        }

        std::string lhs = line.substr(0, arrowPos);
        trim(lhs);
        if (lhs.empty()) {
            std::cerr << "[Error] LHS vacio en: " << line << "\n";
            return std::nullopt;
        }

        // If NT already has a rule, we append productions to it
        RawRule* existing = nullptr;
        for (auto& r : rawRules) {
            if (r.lhs == lhs) { existing = &r; break; }
        }

        if (!existing) {
            ntOrder.push_back(lhs);
            ntSet.insert(lhs);
            rawRules.push_back({lhs, {}});
            existing = &rawRules.back();
        }

        // Split RHS by '|'
        std::string rhs = line.substr(arrowPos + 2);
        std::string cur;
        for (char c : rhs) {
            if (c == '|') {
                trim(cur);
                if (!cur.empty()) {
                    auto toks = tokenize(cur);
                    if (!toks.empty()) existing->prods.push_back(toks);
                }
                cur.clear();
            } else {
                cur += c;
            }
        }
        trim(cur);
        if (!cur.empty()) {
            auto toks = tokenize(cur);
            if (!toks.empty()) existing->prods.push_back(toks);
        }
    }

    if (rawRules.empty()) {
        std::cerr << "[Error] No se encontraron reglas en la entrada.\n";
        return std::nullopt;
    }

    // ── Pass 2: build Grammar ────────────────────────────────────────────────
    Grammar g;
    g.nonTerminals = ntSet;
    g.ntOrder      = ntOrder;
    g.startSymbol  = ntOrder[0];

    // Collect terminals (everything that is not a NT, EPS, or null)
    for (const auto& rule : rawRules) {
        for (const auto& prod : rule.prods) {
            for (const auto& sym : prod) {
                if (ntSet.count(sym) == 0 &&
                    sym != EPSILON_KW && sym != NULL_KW) {
                    g.terminals.insert(sym);
                }
            }
        }
    }

    // Build typed productions
    for (const auto& rule : rawRules) {
        for (const auto& prod : rule.prods) {
            Production typed;
            for (const auto& sym : prod) typed.push_back(g.makeSymbol(sym));
            g.rules[rule.lhs].push_back(typed);
        }
    }

    return g;
}

// ── Public interface ─────────────────────────────────────────────────────────
std::optional<Grammar> Parser::fromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[Error] No se pudo abrir el archivo: " << filename << "\n";
        return std::nullopt;
    }
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    return parseLines(lines);
}

std::optional<Grammar> Parser::fromConsole() {
    std::cout << "\n┌─────────────────────────────────────────────────────┐\n";
    std::cout << "│            Ingreso interactivo de gramatica         │\n";
    std::cout << "└─────────────────────────────────────────────────────┘\n";
    std::cout << "  Formato:  NoTerminal -> simbolo1 simbolo2 | alternativa\n";
    std::cout << "  Epsilon:  use 'EPS' o 'null'\n";
    std::cout << "  Comentarios: comience la linea con '#'\n";
    std::cout << "  Termine:  linea vacia (Enter) para finalizar\n\n";

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        lines.push_back(line);
    }

    if (lines.empty()) {
        std::cerr << "[Error] No se ingresaron reglas.\n";
        return std::nullopt;
    }
    return parseLines(lines);
}
