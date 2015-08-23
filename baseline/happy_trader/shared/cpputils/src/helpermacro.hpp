#ifndef _CPPUTILS_HELPERMACRO_INCLUDED
#define _CPPUTILS_HELPERMACRO_INCLUDED

// concatenation
#define CAT(a, b) PRIMITIVE_CAT(a, b)
#define PRIMITIVE_CAT(a, b) a##b


// binary intermediate split
#define SPLIT(i, im) PRIMITIVE_CAT(SPLIT_, i)(im)
#define SPLIT_0(a, b) a
#define SPLIT_1(a, b) b

// saturating increment and decrement
#define DEC(x) SPLIT(0, PRIMITIVE_CAT(DEC_, x))
#define INC(x) SPLIT(1, PRIMITIVE_CAT(DEC_, x))

#define DEC_0 0,1
#define DEC_1 0,2
#define DEC_2 1,3
#define DEC_3 2,4
#define DEC_4 3,5
#define DEC_5 4,6
#define DEC_6 5,7
#define DEC_7 6,8
#define DEC_8 7,9
#define DEC_9 8,9
/*

// bit complement
#define COMPL(bit) PRIMITIVE_CAT(COMPL_, bit)
#define COMPL_0 1
#define COMPL_1 0

// nullary parentheses detection
#define IS_NULLARY(x) SPLIT(0, CAT(IS_NULLARY_R_, IS_NULLARY_C x))
#define IS_NULLARY_C() 1
#define IS_NULLARY_R_1 1, ~
#define IS_NULLARY_R_IS_NULLARY_C 0, ~

// boolean conversion
#define BOOL(x) COMPL(IS_NULLARY(PRIMITIVE_CAT(BOOL_, x)))
#define BOOL_0 ()

// recursion backend
#define EXPR(s) PRIMITIVE_CAT(EXPR_, s)
#define EXPR_0(x) x
#define EXPR_1(x) x
#define EXPR_2(x) x
#define EXPR_3(x) x
#define EXPR_4(x) x
#define EXPR_5(x) x
#define EXPR_6(x) x
#define EXPR_7(x) x
#define EXPR_8(x) x
#define EXPR_9(x) x

// bit-oriented if control structure
#define IIF(bit) PRIMITIVE_CAT(IIF_, bit)
#define IIF_0(t, f) f
#define IIF_1(t, f) t

// number-oriented if control structure
#define IF(cond) IIF(BOOL(cond))

// emptiness abstraction
#define EMPTY()

// 1x and 2x deferral macros
#define DEFER(macro) macro EMPTY()
#define OBSTRUCT() DEFER(EMPTY)()

// argument list eater
#define EAT(size) PRIMITIVE_CAT(EAT_, size)
#define EAT_0()
#define EAT_1(a)
#define EAT_2(a, b)
#define EAT_3(a, b, c)
#define EAT_4(a, b, c, d)
#define EAT_5(a, b, c, d, e)
#define EAT_6(a, b, c, d, e, f)

// comma abstractions
#define COMMA() ,
#define COMMA_IF(n) IF(n)(COMMA, EMPTY)()

// repetition construct
#define REPEAT(s, count, macro, data) \
EXPR(s)(REPEAT_I(INC(s), INC(s), count, macro, data)) \

#define REPEAT_INDIRECT() REPEAT_I
#define REPEAT_I(s, o, count, macro, data) \
IF(count)(REPEAT_II, EAT(6))(OBSTRUCT(), s, o, DEC(count), macro, data) \


#define REPEAT_II(_, s, o, count, macro, data) \
EXPR(s) _(REPEAT_INDIRECT _()( \
INC(s), o, count, macro, data \
)) \
EXPR OBSTRUCT()(o)(macro OBSTRUCT()(o, count, data)) \

*/


#endif