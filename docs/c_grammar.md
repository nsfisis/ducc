# C Grammar


## Notation

* `'A'`: Terminal symbol
* `A`: Non-terminal symbol
* `A / B`: A or B
* `A?`: Optional A
* `{ A }+`: 1 or more repetitions of A
* `{ A }*`: 0 or more repetitions of A
* `{ A | S }+`: 1 or more repetitions of A separated by S
    * Equivalent to `A { S A }*`
* `{ A | S }*`: 0 or more repetitions of A separated by S
    * Equivalent to `{ A | S }+?`
* `{ A |? S }+`: 1 or more repetitions of A separated by S, allowing optional trailing S
    * Equivalent to `A { S A }* S?`
* `{ A |? S }*`: 1 or more repetitions of A separated by S, allowing optional trailing S
    * Equivalent to `{ A |? S }+?`
* `( A )`: Grouping
* `# ...`: Additional constraints


## Expressions

```
primary-expr:
    identifier
    constant
    string-literal
    '(' expr ')'
    generic-selection

generic-selection:
    '_Generic' '(' assignment-expr ',' generic-assoc-list ')'

generic-assoc-list:
    { generic-association | ',' }*

generic-association:
    type-name ':' assignment-expr
    'default' ':' assignment-expr

postfix-expr:
    postfix-expr-stem { postfix-expr-postfix }*

postfix-expr-stem:
    primary-expr
    compound-literal

postfix-expr-postfix:
    '[' expr ']'
    '(' argument-expr-list? ')'
    '.' identifier
    '->' identifier
    '++'
    '--'

argument-expr-list:
    { assignment-expr | ',' }+

compound-literal:
    '(' storage-class-specifiers? type-name ')' braced-initializer

storage-class-specifiers:
    { storage-class-specifier }+

unary-expr:
    postfix-expr
    '++' unary-expr
    '--' unary-expr
    unary-operator cast-expr
    'sizeof' unary-expr
    'sizeof' '(' type-name ')'
    'alignof' '(' type-name ')'

unary-operator:
    '&'
    '*'
    '+'
    '-'
    '~'
    '!'

cast-expr:
    unary-expr
    '(' type-name ')' cast-expr

multiplicative-expr:
    cast-expr { ( '*' / '/' / '%' ) cast-expr }*

additive-expr:
    multiplicative-expr { ( '+' / '-' ) multiplicative-expr }*

shift-expr:
    additive-expr { ( '<<' / '>>' ) additive-expr }*

relational-expr:
    shift-expr { ( '<' / '>' / '<=' / '>=' ) shift-expr }*

equality-expr:
    relational-expr { ( '==' / '!=' ) relational-expr }*

bitwise-and-expr:
    equality-expr { '&' equality-expr }*

bitwise-xor-expr:
    bitwise-and-expr { '^' bitwise-and-expr }*

bitwise-or-expr:
    bitwise-xor-expr { '|' bitwise-xor-expr }*

logical-and-expr:
    bitwise-or-expr { '&&' bitwise-or-expr }*

logical-or-expr:
    logical-and-expr { '||' logical-and-expr }*

conditional-expr:
    logical-or-expr ( '?' expr ':' conditional-expr )?

assignment-expr:
    conditional-expr
    unary-expr assignment-operator assignment-expr

assignment-operator:
    '='
    '*='
    '/='
    '%='
    '+='
    '-='
    '<<='
    '>>='
    '&='
    '^='
    '|='

expr:
    assignment-expr { ',' assignment-expr }*

constant-expr:
    conditional-expr
```

## Declarations

```
declaration:
    static_assert-declaration
    attribute-specifier-sequence ';'
    attribute-specifier-sequence declaration-specifiers init-declarator-list ';'
    declaration-specifiers init-declarator-list ';'

declaration-specifiers:
    { declaration-specifier }+ attribute-specifier-sequence?

declaration-specifier:
    storage-class-specifier
    type-specifier-qualifier
    function-specifier

init-declarator-list:
    { init-declarator | ',' }+

init-declarator:
    declarator ( '=' initializer )?

storage-class-specifier:
    'auto'
    'constexpr'
    'extern'
    'register'
    'static'
    'thread_local'
    'typedef'

type-specifier:
    'void'
    'char'
    'short'
    'int'
    'long'
    'float'
    'double'
    'signed'
    'unsigned'
    '_BitInt' '(' constant-expr ')'
    'bool'
    '_Complex'
    '_Decimal32'
    '_Decimal64'
    '_Decimal128'
    atomic-type-specifier
    struct-or-union-specifier
    enum-specifier
    typeof-specifier
    typedef-name

struct-or-union-specifier:
    struct-or-union attribute-specifier-sequence? identifier? '{' member-declaration-list '}'
    struct-or-union attribute-specifier-sequence? identifier

struct-or-union:
    'struct'
    'union'

member-declaration-list:
    { member-declaration }+

member-declaration:
    attribute-specifier-sequence? specifier-qualifier-list member-declaration-list? ';'
    static_assert-declaration

specifier-qualifier-list:
    { type-specifier-qualifier }+ attribute-specifier-sequence?

type-specifier-qualifier:
    type-specifier
    type-qualifier
    alignment-specifier

member-declarator-list:
    { member-declarator | ',' }+

member-declarator:
    declarator
    declarator? ':' constant-expr

enum-specifier:
    'enum' attribute-specifier-sequence? identifier? enum-type-specifier? '{' enumerator-list '}'
    'enum' identifier enum-type-specifier?

enumerator-list:
    { enumerator |? ',' }+

enumerator:
    enumeration-constant attribute-specifier-sequence? ( '=' constant-expr )?

enum-type-specifier:
    ':' specifier-qualifier-list

atomic-type-specifier:
    '_Atomic' '(' type-name ')'

typeof-specifier:
    ( 'typeof' / 'typeof_unqual' ) '(' typeof-specifier-argument ')'

typeof-specifier-argument:
    expr
    type-name

type-qualifier:
    'const'
    'restrict'
    'volatile'
    '_Atomic'

function-specifier:
    'inline'
    '_Noreturn'

alignment-specifier:
    'alignas' '(' ( type-name / constant-expr ) ')'

declarator:
    pointer? direct-declarator

direct-declarator:
    identifier attribute-specifier-sequence?
    '(' declarator ')'
    array-declarator attribute-specifier-sequence?
    function-declarator attribute-specifier-sequence?

array-declarator:
    direct-declarator '[' type-qualifier-list? assignment-expr? ']'
    direct-declarator '[' 'static' type-qualifier-list? assignment-expr ']'
    direct-declarator '[' type-qualifier-list 'static' assignment-expr ']'
    direct-declarator '[' type-qualifier-list? '*' ']'

function-declarator:
    direct-declarator '(' parameter-type-list? ')'

pointer:
    { '*' attribute-specifier-sequence? type-qualifier-list? }+

type-qualifier-list:
    { type-qualifier }+

parameter-type-list:
    parameter-list
    parameter-list ',' '...'
    '...'

parameter-list:
    { parameter-declaration | ',' }+

parameter-declaration:
    attribute-specifier-sequence? declaration-specifiers ( declarator / abstract-declarator? )

type-name:
    specifier-qualifier-list abstract-declarator?

abstract-declarator:
    pointer
    pointer? direct-abstract-declarator

direct-abstract-declarator:
    '(' abstract-declarator ')'
    array-abstract-declarator attribute-specifier-sequence?
    function-abstract-declarator attribute-specifier-sequence?

array-abstract-declarator:
    direct-abstract-declarator? '[' type-qualifier-list? assignment-expr? ']'
    direct-abstract-declarator? '[' 'static' type-qualifier-list? assignment-expr ']'
    direct-abstract-declarator? '[' type-qualifier-list 'static' assignment-expr ']'
    direct-abstract-declarator? '[' '*' ']'

function-abstract-declarator:
    direct-abstract-declarator? '(' parameter-type-list? ')'

typedef-name:
    identifier

braced-initializer:
    '{' { designation? initializer |? ',' }* '}'

initializer:
    assignment-expr
    braced-initializer

designation:
    designator-list '='

designator-list:
    { designator }+

designator:
    '[' constant-expr ']'
    '.' identifier

static_assert-declaration:
    'static_assert' '(' constant-expr ( ',' string-literal )? ')' ';'

attribute-specifier-sequence:
    { attribute-specifier }+

attribute-specifier:
    '[' '[' attribute-list ']' ']'

attribute-list:
    { attribute? | ',' }

attribute:
    attribute-token attribute-argument-clause?

attribute-token:
    identifier ( '::' identifier )?

attribute-argument-clause:
    '(' balanced-token-sequence ')'

balanced-token-sequence:
    { balanced-token }+

balanced-token:
    '(' balanced-token-sequence? ')'
    '[' balanced-token-sequence? ']'
    '{' balanced-token-sequence? '}'
    any token other than a parenthesis, a bracket, or a brace
```


## Statements

```
stmt:
    labeled-stmt
    unlabaled-stmt

unlabaled-stmt:
    expr-stmt
    attribute-specifier-sequence? ( primary-block / jump-stmt )

primary-block:
    compound-stmt
    selection-stmt
    iteration-stmt

label:
    attribute-specifier-sequence? identifier ':'
    attribute-specifier-sequence? 'case' constant-expr ':'
    attribute-specifier-sequence? 'default' ':'

labeled-stmt:
    label stmt

compound-stmt:
    '{' block-item-list? '}'

block-item-list:
    { block-item }+

expr-stmt:
    attribute-specifier-sequence? expr? ';'

selection-stmt:
    'if' '(' expr ')' stmt ( 'else' stmt )?
    'switch' '(' expr ')' stmt

iteration-stmt:
    'while' '(' expr ')' stmt
    'do' stmt 'while' '(' expr ')' ';'
    'for' '(' expr? ';' expr? ';' expr? ')' stmt
    'for' '(' declaration expr? ';' expr? ')' stmt

jump-stmt:
    'goto' identifier ';'
    'continue' ';'
    'break' ';'
    'return' expr? ';'
```


## External definitions

```
translation-unit:
    { external-declaration }+

external-declaration:
    function-definition
    declaration

external-declaration:
    static_assert-declaration
    attribute-specifier-sequence ';'
    attribute-specifier-sequence function-definition-or-declaration-rest
    function-definition-or-declaration-rest

function-definition-or-declaration-rest:
    declaration-specifiers init-declarator-list ';'
    declaration-specifiers init-declarator-list compound-stmt
        # Each item of init-declarator-list must not have initializer.
        # The length of init-declarator-list must be one.
```
