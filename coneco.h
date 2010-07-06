#ifndef _CONECO_LISP_
#define _CONECO_LISP_ 1

#define CNL_NIL ((CNL_OBJ*)0)
#define CNL_NIL_P(x) ((x) == CNL_NIL)
#define CNL_TYPE(x) ((x)->type & 0xFF)
#define CNL_PAIR_P(x) (((x) == CNL_NIL) ? 0 : CNL_TYPE(x) == CNL_TYPE_PAIR)
#define CNL_SYMBOL_P(x) (((x) == CNL_NIL) ? 0 : CNL_TYPE(x) == CNL_TYPE_SYMBOL)
#define CNL_SYNTAX_P(x) (((x) == CNL_NIL) ? 0 : CNL_TYPE(x) == CNL_TYPE_SYNTAX)
#define CNL_NUMBER_P(x) (((x) == CNL_NIL) ? 0 : CNL_TYPE(x) == CNL_TYPE_NUMBER)
#define CNL_FUNC_P(x) (((x) == CNL_NIL) ? 0 : CNL_TYPE(x) == CNL_TYPE_FUNC)
#define CNL_PROC_P(x) (((x) == CNL_NIL) ? 0 : CNL_TYPE(x) == CNL_TYPE_PROC)
#define CNL_MACRO_P(x) (((x) == CNL_NIL) ? 0 : CNL_TYPE(x) == CNL_TYPE_MACRO)
#define CNL_CAR(x) ((CNL_PAIR_P(x) || CNL_PROC_P(x) || CNL_MACRO_P(x)) ? ((CNL_OBJ*)(x)->o.pair.car) : CNL_NIL)
#define CNL_CDR(x) ((CNL_PAIR_P(x) || CNL_PROC_P(x) || CNL_MACRO_P(x)) ? ((CNL_OBJ*)(x)->o.pair.cdr) : CNL_NIL)
#define CNL_CAAR(x) CNL_CAR(CNL_CAR(x))
#define CNL_CADR(x) CNL_CAR(CNL_CDR(x))
#define CNL_CDAR(x) CNL_CDR(CNL_CAR(x))
#define CNL_CDDR(x) CNL_CDR(CNL_CDR(x))
#define CNL_CADDR(x) CNL_CAR(CNL_CDR(CNL_CDR(x)))
#define CNL_CAADR(x) CNL_CAR(CNL_CAR(CNL_CDR(x)))
#define CNL_CADAR(x) CNL_CAR(CNL_CDR(CNL_CAR(x)))
#define CNL_CDDAR(x) CNL_CDR(CNL_CDR(CNL_CAR(x)))
#define CNL_CDDDR(x) CNL_CDR(CNL_CDR(CNL_CDR(x)))
#define CNL_SET_CAR(x,y) if(CNL_PAIR_P(x) || CNL_PROC_P(x)){ (x)->o.pair.car = (y); }
#define CNL_SET_CDR(x,y) if(CNL_PAIR_P(x) || CNL_PROC_P(x)){ (x)->o.pair.cdr = (y); }
#define CNL_NUMBER(x) ((x)->o.number)
#define CNL_SYNTAX(x) ((x)->o.number)

enum CNL_TYPE {
	CNL_TYPE_SYNTAX,
	CNL_TYPE_MACRO,
	CNL_TYPE_FUNC,
	CNL_TYPE_PROC,
	CNL_TYPE_PAIR,
	CNL_TYPE_SYMBOL,
	CNL_TYPE_NUMBER,
	CNL_TYPE_CHAR,
	CNL_TYPE_STRING };

enum CNL_SYNTAX {
	CNL_SYNTAX_BEGIN,
	CNL_SYNTAX_DEFINE,
	CNL_SYNTAX_IF,
	CNL_SYNTAX_LAMBDA,
	CNL_SYNTAX_QUOTE,
	CNL_SYNTAX_SWEEP };

typedef struct _CNL_PAIR {
	void *car;
	void *cdr;
} CNL_PAIR;

typedef struct _CNL_OBJ {
	enum CNL_TYPE type;
	union {
		long number;
		char *str;
		CNL_PAIR pair;
		void *func;
	} o;
} CNL_OBJ;

typedef struct _CNL_GC {
	unsigned int size;
	CNL_OBJ **buf;
} CNL_GC;

CNL_OBJ *cnl_bind;
CNL_GC *cnl_gc;

void cnl_debug_print(CNL_OBJ *obj);
CNL_OBJ* cnl_cons(CNL_GC *gc,CNL_OBJ *left,CNL_OBJ *right);
CNL_OBJ* cnl_make_number(CNL_GC *gc,long n);
CNL_OBJ* cnl_make_function(CNL_GC *gc,CNL_OBJ* (*f)(CNL_GC*,CNL_OBJ*));
CNL_OBJ* cnl_make_proc(CNL_GC *gc,CNL_OBJ* bind,CNL_OBJ* targ);
CNL_OBJ* cnl_make_symbol(CNL_GC *gc,char *s);
CNL_OBJ* cnl_make_syntax(CNL_GC *gc,enum CNL_SYNTAX s);
CNL_OBJ* cnl_make_default_binds(CNL_GC *gc);
int cnl_symbol_equal_p(CNL_OBJ *s1,CNL_OBJ *s2);
CNL_OBJ* cnl_func_call(CNL_GC *gc,CNL_OBJ *s);
CNL_OBJ* cnl_func_macro(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_cons(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_car(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_cdr(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_set_car(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_set_cdr(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_pair_p(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_null_p(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_number_add(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_number_sub(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_number_mul(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_number_div(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_number_equal_p(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_number_greater_p(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_func_number_less_p(CNL_GC *gc,CNL_OBJ *args);
CNL_OBJ* cnl_eval(CNL_GC *gc,CNL_OBJ *bind,CNL_OBJ *obj);

#endif /* _CONECO_LISP_ */
