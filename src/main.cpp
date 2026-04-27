#include <iostream>
#include <string>
#include <limits>
#include "core/grammar.h"
#include "io/parser.h"
#include "analysis/validator.h"
#include "analysis/first_set.h"
#include "analysis/follow_set.h"
#include "io/display.h"

// ── Single session ────────────────────────────────────────────────────────────
static void runSession() {
    std::cout << "\n══════════════════════════  Nueva sesion  ══════════════════════════\n";
    std::cout << "  Como desea ingresar la gramatica?\n";
    std::cout << "    [1]  Desde archivo de texto\n";
    std::cout << "    [2]  Interactivo (escribir en consola)\n";
    std::cout << "  Opcion: ";

    int option = 0;
    std::cin >> option;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // ── Parse ──────────────────────────────────────────────────────────────
    std::optional<Grammar> grammarOpt;

    if (option == 1) {
        std::cout << "  Ruta del archivo (ej: input/ejemplo.txt): ";
        std::string filename;
        std::getline(std::cin, filename);
        grammarOpt = Parser::fromFile(filename);
    } else {
        grammarOpt = Parser::fromConsole();
    }

    if (!grammarOpt.has_value()) {
        std::cerr << "\n[Error] No se pudo procesar la gramatica. Revise el formato.\n";
        return;
    }

    Grammar& g = grammarOpt.value();

    // ── Show parsed grammar ────────────────────────────────────────────────
    Display::printGrammar(g);

    // ── Validate ───────────────────────────────────────────────────────────
    std::cout << "\n  Validando gramatica...\n";
    ValidationResult vr = Validator::validate(g);

    for (const auto& w : vr.warnings)
        std::cout << "  [Advertencia] " << w << "\n";

    for (const auto& e : vr.errors)
        std::cout << "  [Error]       " << e << "\n";

    if (!vr.valid) {
        std::cout << "\n  La gramatica contiene errores; no es posible calcular los conjuntos.\n";
        return;
    }

    std::cout << "  Gramatica valida. Calculando conjuntos...\n";

    // ── Compute FIRST ──────────────────────────────────────────────────────
    FirstFollowMap firstSets  = FirstSetCalculator::compute(g);

    // ── Compute FOLLOW ─────────────────────────────────────────────────────
    FirstFollowMap followSets = FollowSetCalculator::compute(g, firstSets);

    // ── Display results ────────────────────────────────────────────────────
    Display::printTable(g, firstSets, followSets);
}

// ── Main loop ─────────────────────────────────────────────────────────────────
int main() {
    Display::printBanner();

    char again = 's';
    do {
        runSession();

        std::cout << "\n  Desea analizar otra gramatica? (s/n): ";
        std::cin >> again;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    } while (again == 's' || again == 'S');

    std::cout << "\n  Gracias por usar GrammarFlow. Hasta luego!\n\n";
    return 0;
}
