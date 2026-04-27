#include "display.h"
#include <iostream>
#include <iomanip>
#include <sstream>

// ── Banner ───────────────────────────────────────────────────────────────────
void Display::printBanner() {
    std::cout << "\n";
    std::cout << "  ██████╗ ██████╗  █████╗ ███╗  ███╗███╗  ███╗ █████╗ ██████╗      ███████╗██╗      ██████╗ ██╗    ██╗\n";
    std::cout << " ██╔════╝ ██╔══██╗██╔══██╗████╗████║████╗████║██╔══██╗██╔══██╗     ██╔════╝██║     ██╔═══██╗██║    ██║\n";
    std::cout << " ██║  ███╗██████╔╝███████║██╔████╔██║██╔████╔██║███████║██████╔╝     █████╗  ██║     ██║   ██║██║ █╗ ██║\n";
    std::cout << " ██║   ██║██╔══██╗██╔══██║██║╚██╔╝██║██║╚██╔╝██║██╔══██║██╔══██╗     ██╔══╝  ██║     ██║   ██║██║███╗██║\n";
    std::cout << " ╚██████╔╝██║  ██║██║  ██║██║ ╚═╝ ██║██║ ╚═╝ ██║██║  ██║██║  ██║     ██║     ███████╗╚██████╔╝╚███╔███╔╝\n";
    std::cout << "  ╚═════╝ ╚═╝  ╚═╝╚═╝  ╚═╝╚═╝     ╚═╝╚═╝     ╚═╝╚═╝  ╚═╝╚═╝  ╚═╝     ╚═╝     ╚══════╝ ╚═════╝  ╚══╝╚══╝\n";
    std::cout << "\n  Calculadora de Conjuntos PRIMERO y SIGUIENTE  |  Teoria de Lenguajes y Compiladores\n";
    std::cout << "  ────────────────────────────────────────────────────────────────────────────────────\n\n";
}

// ── Grammar dump ─────────────────────────────────────────────────────────────
void Display::printGrammar(const Grammar& g) {
    std::cout << "\n┌── Gramatica reconocida ───────────────────────────────────────────────────┐\n";
    std::cout << "  Simbolo inicial : " << g.startSymbol << "\n";

    std::cout << "  No Terminales   : ";
    for (const auto& nt : g.ntOrder) std::cout << nt << "  ";
    std::cout << "\n";

    std::cout << "  Terminales      : ";
    for (const auto& t  : g.terminals) std::cout << t  << "  ";
    std::cout << "\n\n  Producciones:\n";

    for (const auto& nt : g.ntOrder) {
        auto it = g.rules.find(nt);
        if (it == g.rules.end()) continue;

        std::cout << "    " << std::left << std::setw(6) << nt << " -> ";

        bool firstProd = true;
        for (const auto& prod : it->second) {
            if (!firstProd) std::cout << "\n           | ";
            for (const auto& sym : prod) std::cout << sym.name << " ";
            firstProd = false;
        }
        std::cout << "\n";
    }
    std::cout << "└──────────────────────────────────────────────────────────────────────────┘\n";
}

// ── Helpers ──────────────────────────────────────────────────────────────────
std::string Display::setToString(const StringSet& s) {
    if (s.empty()) return "{ }";
    std::ostringstream oss;
    oss << "{ ";
    bool first = true;
    for (const auto& el : s) {
        if (!first) oss << ", ";
        oss << el;
        first = false;
    }
    oss << " }";
    return oss.str();
}

void Display::printSeparator(int c1, int c2, int c3) {
    std::cout << "+" << std::string(c1, '-')
              << "+" << std::string(c2, '-')
              << "+" << std::string(c3, '-') << "+\n";
}

// ── Results table ────────────────────────────────────────────────────────────
void Display::printTable(const Grammar&       g,
                          const FirstFollowMap& firstSets,
                          const FirstFollowMap& followSets) {
    constexpr int C1 = 14;   // NT column width
    constexpr int C2 = 40;   // FIRST column width
    constexpr int C3 = 40;   // FOLLOW column width

    std::cout << "\n";
    printSeparator(C1, C2, C3);
    std::cout << "| " << std::left << std::setw(C1 - 2) << "No Terminal"
              << "| " << std::setw(C2 - 2) << "PRIMERO"
              << "| " << std::setw(C3 - 2) << "SIGUIENTE" << "|\n";
    printSeparator(C1, C2, C3);

    for (const auto& nt : g.ntOrder) {
        static const StringSet empty;
        std::string fs = setToString(firstSets .count(nt) ? firstSets .at(nt) : empty);
        std::string fw = setToString(followSets.count(nt) ? followSets.at(nt) : empty);

        // Truncate if too wide for column
        auto fit = [](std::string& str, int maxW) {
            if ((int)str.size() > maxW - 1) str = str.substr(0, maxW - 4) + "...";
        };
        fit(fs, C2 - 2);
        fit(fw, C3 - 2);

        std::cout << "| " << std::left << std::setw(C1 - 2) << nt
                  << "| " << std::setw(C2 - 2) << fs
                  << "| " << std::setw(C3 - 2) << fw << "|\n";
    }

    printSeparator(C1, C2, C3);
    std::cout << "\n  Nota: EPS = cadena vacia (epsilon)   $ = fin de cadena\n";
}
