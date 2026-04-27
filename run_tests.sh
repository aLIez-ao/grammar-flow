#!/usr/bin/env bash
# ─────────────────────────────────────────────────────────────────────────────
# run_tests.sh  –  GrammarFlow test runner
# Uso: ./run_tests.sh
# ─────────────────────────────────────────────────────────────────────────────

BINARY="./build/bin/grammar-flow"
GRAMMARS_DIR="./tests/grammars"

# ── Colores ───────────────────────────────────────────────────────────────────
GREEN="\033[0;32m"
RED="\033[0;31m"
YELLOW="\033[0;33m"
CYAN="\033[0;36m"
RESET="\033[0m"

PASS=0
FAIL=0
TOTAL=0

# ── Helpers ───────────────────────────────────────────────────────────────────
print_header() {
    echo ""
    echo -e "${CYAN}══════════════════════════════════════════════════════${RESET}"
    echo -e "${CYAN}  GrammarFlow - Test Runner${RESET}"
    echo -e "${CYAN}══════════════════════════════════════════════════════${RESET}"
    echo ""
}

# run_test <archivo> <tipo: ok|error> <patron_esperado>
#   ok    → la gramatica debe pasar validacion y calcular conjuntos
#   error → la gramatica debe fallar con un mensaje que contenga <patron_esperado>
run_test() {
    local file="$1"
    local type="$2"
    local pattern="$3"
    local name
    name=$(basename "$file")
    TOTAL=$((TOTAL + 1))

    # Ejecutar el programa con la gramática dada
    output=$(printf "1\n%s\nn\n" "$file" | "$BINARY" 2>&1)

    if [[ "$type" == "ok" ]]; then
        # Debe calcular conjuntos sin errores
        if echo "$output" | grep -q "No Terminal"; then
            echo -e "  ${GREEN}✔ PASS${RESET}  $name"
            PASS=$((PASS + 1))
        else
            echo -e "  ${RED}✘ FAIL${RESET}  $name"
            echo -e "         Esperaba tabla de resultados pero no se encontro."
            echo -e "         Salida:\n$(echo "$output" | grep -E 'Error|Advertencia' | head -5)"
            FAIL=$((FAIL + 1))
        fi

    elif [[ "$type" == "error" ]]; then
        # Debe fallar con el patrón esperado en el mensaje de error
        if echo "$output" | grep -qi "$pattern"; then
            echo -e "  ${GREEN}✔ PASS${RESET}  $name  ${YELLOW}(error detectado correctamente)${RESET}"
            PASS=$((PASS + 1))
        else
            echo -e "  ${RED}✘ FAIL${RESET}  $name"
            echo -e "         Esperaba error con patron: '$pattern'"
            echo -e "         Salida obtenida:\n$(echo "$output" | grep -E 'Error|valida' | head -5)"
            FAIL=$((FAIL + 1))
        fi
    fi
}

# ── Verificar que el binario existe ──────────────────────────────────────────
print_header

if [[ ! -f "$BINARY" ]]; then
    echo -e "${RED}  [Error] No se encontro el binario: $BINARY${RESET}"
    echo -e "  Ejecuta primero:  cmake --build build"
    exit 1
fi

echo -e "  Binario   : $BINARY"
echo -e "  Gramáticas: $GRAMMARS_DIR"
echo ""
echo -e "  ── Casos válidos ────────────────────────────────────"

# ── Casos que deben PASAR ─────────────────────────────────────────────────────
run_test "$GRAMMARS_DIR/expresiones.txt"         ok
run_test "$GRAMMARS_DIR/epsilon_productions.txt" ok
run_test "$GRAMMARS_DIR/epsilon_chain.txt"       ok

echo ""
echo -e "  ── Casos que deben fallar (validación) ──────────────"

# ── Casos que deben FALLAR ────────────────────────────────────────────────────
run_test "$GRAMMARS_DIR/left_recursion.txt"   error "Recursion izquierda"
run_test "$GRAMMARS_DIR/undefined_symbol.txt" error "no tiene regla propia"

# ── Resumen ───────────────────────────────────────────────────────────────────
echo ""
echo -e "${CYAN}══════════════════════════════════════════════════════${RESET}"
echo -e "  Resultado: ${GREEN}$PASS pasaron${RESET}  /  ${RED}$FAIL fallaron${RESET}  /  $TOTAL total"
echo -e "${CYAN}══════════════════════════════════════════════════════${RESET}"
echo ""

[[ $FAIL -eq 0 ]] && exit 0 || exit 1
