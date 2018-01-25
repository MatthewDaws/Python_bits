"""

Adapted from Chapter 10 of Stroustrop, "The C++ Programming Language 4th ed"
"""

import enum, collections

class Kind(enum.Enum):
    name = 1
    number = 2
    end = 3
    plus = 4
    minus = 5
    mul = 6
    div = 7
    assign = 8
    lp = 9
    rp = 10


class Token():
    def __init__(self, kind, value):
        self._kind = kind
        self._value = value
    
    @property
    def kind(self):
        return self._kind

    @property
    def value(self):
        return self._value


def expr(token_stream, get_next):
    left = term(token_stream, get_next)
    while True:
        if token_stream.current.kind == Kind.plus:
            left += term(token_stream, True)
        elif token_stream.current.kind == Kind.minus:
            left -= term(token_stream, True)
        else:
            return left

def term(token_stream, get_next):
    left = prim(token_stream, get_next)
    while True:
        if token_stream.current.kind == Kind.mul:
            left *= prim(token_stream, True)
        elif token_stream.current.kind == Kind.div:
            # Catch divide by zero??
            left /= prim(token_stream, True)
        else:
            return left

def prim(token_stream, get_next):
    if get_next:
        token_stream.get()
    if token_stream.current.kind == Kind.number:
        v = float(token_stream.current.value)
        token_stream.get()
        return v
    elif token_stream.current.kind == Kind.name:
        name = token_stream.current.value
        v = lookup[name]
        token_stream.get()
        if token_stream.current.kind == Kind.assign:
            v = expr(token_stream, True)
            lookup[name] = v
        return v
    elif token_stream.current.kind == Kind.minus:
        return -prim(token_stream, True)
    elif token_stream.current.kind == Kind.lp:
        e = expr(token_stream, True)
        if token_stream.current.kind != Kind.rp:
            raise Exception("Unbalanced bracket")
        token_stream.get()
        return e
    else:
        raise Exception("Expected primary expression")

class TokenStream():
    def __init__(self, data):
        self._data = data
        self._index = 0
        self._current_token = Token(Kind.end, "")
    
    @property
    def current(self):
        return self._current_token

    _CHAR_MAP = {"*" : Kind.mul, "/" : Kind.div, "+" : Kind.plus, "-" : Kind.minus,
        "(" : Kind.lp, ")" : Kind.rp, "=" : Kind.assign}
    _DIGITS = "0123456789."
    _WHITESPACE = " \t"

    def get(self):
        while self._index < len(self._data) and self._data[self._index] in self._WHITESPACE:
            self._index += 1
 
        if self._index >= len(self._data):
            self._current_token = Token(Kind.end, "")
        elif self._data[self._index] in self._CHAR_MAP:
            self._current_token = Token(self._CHAR_MAP[self._data[self._index]], "")
            self._index += 1
        elif self._data[self._index] in self._DIGITS:
            numstring = ""
            while self._index < len(self._data) and self._data[self._index] in self._DIGITS:
                numstring += self._data[self._index]
                self._index += 1
            self._current_token = Token(Kind.number, numstring)
        else:
            name = ""
            while self._index < len(self._data) and self._data[self._index].isalpha():
                name += self._data[self._index]
                self._index += 1
            self._current_token = Token(Kind.name, name)

        return self._current_token

if __name__ == "__main__":
    lookup = collections.defaultdict(float)
    lookup["pi"] = 3.141592654
    while True:
        line = input("> ")
        ts = TokenStream(line)
        print(expr(ts, True))
