import re
import sys

START = 0

ACCEPT = {
    1: "SUMA",
    2: "INCR",
    3: "ID",
    4: "ID",
}

def is_upper(c: str) -> bool:
    return 'A' <= c <= 'Z'

def is_lower(c: str) -> bool:
    """Minúsculas o dígitos para [a-z0-9]* en ID."""
    return ('a' <= c <= 'z') or ('0' <= c <= '9')

def move(state: set[int], c: str) -> set[int]:
    next = set()
    for s in state:
        if s == 0:
            if c == '+':
                next.add(1)
            if is_upper(c):
                next.add(3)
        elif s == 1:
            if c == '+':
                next.add(2)
        elif s == 3:
            if is_lower(c):
                next.add(4)
        elif s == 4:
            if is_lower(c):
                next.add(4)
    return next
            
def best_token(current_state: set[int]) -> str | None:
    # Devuelve el token aceptado, con prioridad INCR > SUMA > ID
    if 2 in current_state:
        return "INCR"
    if 1 in current_state:
        return "SUMA"
    if 3 in current_state or 4 in current_state:
        return "ID"
    return None

def token(text: str):
    i = 0
    n = len(text)

    while i < n:
        # Saltar espacios
        if text[i].isspace():
            i += 1
            continue

        state = {START}
        j = i
        best = None
        best_end = i

        while j < n and not text[j].isspace():
            state = move(state, text[j])
            if not state:
                break

            tok = best_token(state)
            if tok is not None:
                best = tok
                best_end = j + 1
            
            j += 1

        if best is None:
            print("NO ACEPTA")
            return

        lexema = text[i:best_end]
        print(f"{best}\t{lexema}")
        i = best_end

def main():
    if len(sys.argv) != 2:
        print("Uso: python afopy.py <archivo.txt>")
        sys.exit(1)

    path = sys.argv[1]
    with open(path, "r", encoding="utf-8") as file:
        text = file.read()

    token(text)

if __name__ == "__main__":
    main()