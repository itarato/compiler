# Compiler

(at least it tries O.O)

Learning project to:
- be able to parse grammars
- tokenizing a source
- building AST from tokens using the grammar

# Grammar reader

Grammar reader can read a grammar from file or string - that follows a strict format:

```
<grammar> ::= <rule-line>*
<rule-line> ::= <rule-name>":" <rule> ( "|" <rule> )* <line-end>
<rule> ::= <token>*
<rule-name> ::= <character>+
<token> ::= <character>+
```

The top level rule has to be `PROG`.

Example:

```
PROG: EXPR
EXPR: T_NUM | T_PAREN_OPEN EXPR OP EXPR T_PAREN_CLOSE
OP: T_ADD | T_SUB
```

Available terminal tokens are hardcoded for now in https://github.com/itarato/compiler/blob/master/util.cpp#L9 .

More examples in: https://github.com/itarato/compiler/tree/master/sample

# AST builder

There are 2 parsers (and AST builders) in the project:

- bactrack parser (requires context free, non left recursive, premature match free grammar - done by grammar normalizer)
- LL(1) parser (requires LL(1) grammar)

# Build

Requires Clang++.

```
make && ./main test
```

# Usage

- Normalize grammar:

`./main grammar GRAMMAR_FILE`

- Use backtrack parser:

`./main parse GRAMMAR_FILE SOURCE_FILE`

- Use LL(1) parser:

`./main llparse GRAMMAR_FILE SOURCE_FILE`

- Run tests:

`./main test`
