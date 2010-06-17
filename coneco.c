#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "coneco.h"
#include "gc.h"
#include "reader.tab.h"

void cnl_debug_print(CNL_OBJ *obj){
	if(CNL_NIL_P(obj)){
		printf("NIL");
	}else{
		switch(CNL_TYPE(obj)){
			case CNL_TYPE_UNDEF : printf("UNDEF"); break;
			case CNL_TYPE_SYNTAX : printf("SYNTAX"); break;
			case CNL_TYPE_FUNC : printf("FUNC"); break;
			case CNL_TYPE_PROC : printf("PROC"); break;
			case CNL_TYPE_PAIR :
				printf("(");
				cnl_debug_print(CNL_CAR(obj));
				printf(".");
				cnl_debug_print(CNL_CDR(obj));
				printf(")"); break;
			case CNL_TYPE_SYMBOL : printf("%s",obj->o.str); break;
			case CNL_TYPE_NUMBER : printf("%ld",CNL_NUMBER(obj)); break;
		}
	}
}

CNL_OBJ* cnl_cons(CNL_GC *gc,CNL_OBJ *left,CNL_OBJ *right){
	CNL_OBJ *pair = cnl_make_obj(gc);
	pair->type = CNL_TYPE_PAIR;
	CNL_SET_CAR(pair,left);
	CNL_SET_CDR(pair,right);
	return pair;
}

CNL_OBJ* cnl_make_undef(CNL_GC *gc){
	CNL_OBJ *undef = cnl_make_obj(gc);
	undef->type = CNL_TYPE_UNDEF;
	return undef;
}

CNL_OBJ* cnl_make_number(CNL_GC *gc,long n){
	CNL_OBJ *num = cnl_make_obj(gc);
	num->type = CNL_TYPE_NUMBER;
	num->o.number = n;
	return num;
}

CNL_OBJ* cnl_make_function(CNL_GC *gc,CNL_OBJ* (*f)(CNL_GC*,CNL_OBJ*)){
	CNL_OBJ *func = cnl_make_obj(gc);
	func->type = CNL_TYPE_FUNC;
	func->o.func = f;
	return func;
}

CNL_OBJ* cnl_make_proc(CNL_GC *gc,CNL_OBJ* bind,CNL_OBJ* targ){
	CNL_OBJ *proc = cnl_make_obj(gc);
	proc->type = CNL_TYPE_PROC;
	CNL_SET_CAR(proc,bind);
	CNL_SET_CDR(proc,targ);
	return proc;
}

CNL_OBJ* cnl_make_symbol(CNL_GC *gc,char *s){
	CNL_OBJ *symbol = cnl_make_obj(gc);
	symbol->type = CNL_TYPE_SYMBOL;
	symbol->o.str = strcpy((char*)malloc(strlen(s) + 1),s);
	return symbol;
}

CNL_OBJ* cnl_make_syntax(CNL_GC *gc,enum CNL_SYNTAX s){
	CNL_OBJ *syntax = cnl_make_obj(gc);
	syntax->type = CNL_TYPE_SYNTAX;
	syntax->o.number = s;
	return syntax;
}

CNL_OBJ* cnl_make_default_binds(CNL_GC *gc){
	CNL_OBJ* bind;
	bind = CNL_NIL;
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"begin"),cnl_make_syntax(gc,CNL_SYNTAX_BEGIN)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"define"),cnl_make_syntax(gc,CNL_SYNTAX_DEFINE)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"if"),cnl_make_syntax(gc,CNL_SYNTAX_IF)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"lambda"),cnl_make_syntax(gc,CNL_SYNTAX_LAMBDA)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"quote"),cnl_make_syntax(gc,CNL_SYNTAX_QUOTE)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"sweep"),cnl_make_syntax(gc,CNL_SYNTAX_SWEEP)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"cons"),cnl_make_function(gc,cnl_func_cons)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"car"),cnl_make_function(gc,cnl_func_car)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"cdr"),cnl_make_function(gc,cnl_func_cdr)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"set-car!"),cnl_make_function(gc,cnl_func_set_car)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"set-cdr!"),cnl_make_function(gc,cnl_func_set_cdr)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"pair?"),cnl_make_function(gc,cnl_func_pair_p)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"null?"),cnl_make_function(gc,cnl_func_null_p)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"+"),cnl_make_function(gc,cnl_func_number_add)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"-"),cnl_make_function(gc,cnl_func_number_sub)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"*"),cnl_make_function(gc,cnl_func_number_mul)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"/"),cnl_make_function(gc,cnl_func_number_div)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"="),cnl_make_function(gc,cnl_func_number_equal_p)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,">"),cnl_make_function(gc,cnl_func_number_greater_p)),bind);
	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_symbol(gc,"<"),cnl_make_function(gc,cnl_func_number_less_p)),bind);
	return bind;
}

int cnl_symbol_equal_p(CNL_OBJ *s1,CNL_OBJ *s2){
	int ret = 0;
	if(CNL_SYMBOL_P(s1) && CNL_SYMBOL_P(s2)){
		if(s1 == s2){
			ret = 1;
		}else{
			if(strcmp(s1->o.str,s2->o.str) == 0){
				ret = 1;
			}
		}
	}
	return ret;
}

CNL_OBJ* cnl_func_call(CNL_GC *gc,CNL_OBJ *s){
	CNL_OBJ* func = CNL_CAR(s);
	CNL_OBJ* args = CNL_CDR(s);
	CNL_OBJ* (*f)(CNL_GC*,CNL_OBJ*) = func->o.func;
	return f(gc,args);
}

CNL_OBJ* cnl_func_cons(CNL_GC *gc,CNL_OBJ *args){
	if(CNL_NIL_P(args)){
		return CNL_NIL;
	}else{
		return cnl_cons(gc,CNL_CAR(args),CNL_CADR(args));
	}
}

CNL_OBJ* cnl_func_car(CNL_GC *gc,CNL_OBJ *args){
	return CNL_CAAR(args);
}

CNL_OBJ* cnl_func_cdr(CNL_GC *gc,CNL_OBJ *args){
	return CNL_CDAR(args);
}

CNL_OBJ* cnl_func_set_car(CNL_GC *gc,CNL_OBJ *args){
	if(CNL_PAIR_P(args)){
		CNL_OBJ* obj = CNL_CAR(args);
		if(CNL_PAIR_P(obj) || CNL_PROC_P(obj)){
			CNL_SET_CAR(obj,CNL_CADR(args));
			return obj;
		}
	}
	return CNL_NIL;
}

CNL_OBJ* cnl_func_set_cdr(CNL_GC *gc,CNL_OBJ *args){
	if(CNL_PAIR_P(args)){
		CNL_OBJ* obj = CNL_CAR(args);
		if(CNL_PAIR_P(obj) || CNL_PROC_P(obj)){
			CNL_SET_CDR(obj,CNL_CADR(args));
			return obj;
		}
	}
	return CNL_NIL;
}

CNL_OBJ* cnl_func_pair_p(CNL_GC *gc,CNL_OBJ *args){
	CNL_OBJ *ls = args;
	while(CNL_PAIR_P(ls)){
		if(! CNL_PAIR_P(CNL_CAR(ls))){
			return CNL_NIL;
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,1);
}

CNL_OBJ* cnl_func_null_p(CNL_GC *gc,CNL_OBJ *args){
	CNL_OBJ *ls = args;
	while(CNL_PAIR_P(ls)){
		if(! CNL_NIL_P(CNL_CAR(ls))){
			return CNL_NIL;
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,1);
}

CNL_OBJ* cnl_func_number_add(CNL_GC *gc,CNL_OBJ *args){
	int n = 0;
	CNL_OBJ *ls = args;
	while(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n += CNL_NUMBER(CNL_CAR(ls));
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,n);
}

CNL_OBJ* cnl_func_number_sub(CNL_GC *gc,CNL_OBJ *args){
	int n = 0;
	CNL_OBJ *ls = args;
	if(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n += CNL_NUMBER(CNL_CAR(ls));
		}
		ls = CNL_CDR(ls);
	}
	while(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n -= CNL_NUMBER(CNL_CAR(ls));
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,n);
}

CNL_OBJ* cnl_func_number_mul(CNL_GC *gc,CNL_OBJ *args){
	int n = 1;
	CNL_OBJ *ls = args;
	while(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n *= CNL_NUMBER(CNL_CAR(ls));
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,n);
}

CNL_OBJ* cnl_func_number_div(CNL_GC *gc,CNL_OBJ *args){
	int n = 1;
	CNL_OBJ *ls = args;
	if(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n *= CNL_NUMBER(CNL_CAR(ls));
		}
		ls = CNL_CDR(ls);
	}
	while(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n /= CNL_NUMBER(CNL_CAR(ls));
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,n);
}

CNL_OBJ* cnl_func_number_equal_p(CNL_GC *gc,CNL_OBJ* args){
	int n = 0;
	CNL_OBJ *ls = args;
	if(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n = CNL_NUMBER(CNL_CAR(ls));
		}else{
			return CNL_NIL;
		}
		ls = CNL_CDR(ls);
	}
	while(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			if(n != CNL_NUMBER(CNL_CAR(ls))){
				return CNL_NIL;
			}
		}else{
			return CNL_NIL;
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,1);
}

CNL_OBJ* cnl_func_number_greater_p(CNL_GC *gc,CNL_OBJ* args){
	int n = 0;
	CNL_OBJ *ls = args;
	if(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n = CNL_NUMBER(CNL_CAR(ls));
		}else{
			return CNL_NIL;
		}
		ls = CNL_CDR(ls);
	}
	while(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			if(n <= CNL_NUMBER(CNL_CAR(ls))){
				return CNL_NIL;
			}else{
				n = CNL_NUMBER(CNL_CAR(ls));
			}
		}else{
			return CNL_NIL;
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,1);
}

CNL_OBJ* cnl_func_number_less_p(CNL_GC *gc,CNL_OBJ* args){
	int n = 0;
	CNL_OBJ *ls = args;
	if(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			n = CNL_NUMBER(CNL_CAR(ls));
		}else{
			return CNL_NIL;
		}
		ls = CNL_CDR(ls);
	}
	while(CNL_PAIR_P(ls)){
		if(CNL_NUMBER_P(CNL_CAR(ls))){
			if(n >= CNL_NUMBER(CNL_CAR(ls))){
				return CNL_NIL;
			}else{
				n = CNL_NUMBER(CNL_CAR(ls));
			}
		}else{
			return CNL_NIL;
		}
		ls = CNL_CDR(ls);
	}
	return cnl_make_number(gc,1);
}

CNL_OBJ* cnl_eval(CNL_GC *gc,CNL_OBJ *bind,CNL_OBJ *obj){

	CNL_OBJ *ret;
	CNL_OBJ *env;
	CNL_OBJ *tmp;
	CNL_OBJ *targ;
	CNL_OBJ *args;

	ret = CNL_NIL;

	bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),bind);
	targ = obj;
	env = cnl_cons(gc,cnl_cons(gc,bind,targ),CNL_NIL);

	while(! CNL_NIL_P(env)){
		bind = CNL_CAAR(env);
		targ = CNL_CDAR(env);
		if(CNL_PAIR_P(targ)){
			if(CNL_NUMBER(CNL_CAAR(bind)) == 0){
				/* ENV PUSH BEGIN */
				bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
				env = cnl_cons(gc,cnl_cons(gc,bind,CNL_CAR(targ)),env);
				/* ENV PUSH END */
			}else if(CNL_SYNTAX_P(CNL_CADAR(bind))){
				if(CNL_SYNTAX(CNL_CADAR(bind)) == CNL_SYNTAX_BEGIN){
					/* SYNTAX begin BEGIN */
					int i;
					int n = CNL_NUMBER(CNL_CAAR(bind));
					for(i = 0; i < n; i++){
						if(CNL_PAIR_P(targ)){ targ = CNL_CDR(targ); }
					}
					if(CNL_PAIR_P(targ)){
						if(CNL_NIL_P(CNL_CDR(targ))){
							/* ENV POP&PUSH BEGIN */
							bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
							env = cnl_cons(gc,cnl_cons(gc,bind,CNL_CAR(targ)),CNL_CDR(env));
							/* ENV POP&PUSH END */
						}else{
							/* ENV PUSH BEGIN */
							bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
							env = cnl_cons(gc,cnl_cons(gc,bind,CNL_CAR(targ)),env);
							/* ENV PUSH END */
						}
					}else if(CNL_NIL_P(targ)){
						if(n == 1){
							ret = CNL_NIL;
						}else{
							args = CNL_CAR(bind);
							while(CNL_PAIR_P(CNL_CDR(args))){
								args = CNL_CDR(args);
							}
							ret = CNL_CAR(args);
						}
						/* ENV POP BEGIN */
						env = CNL_CDR(env);
						if(CNL_PAIR_P(env)){
							bind = CNL_CAAR(env);
							args = CNL_CAR(bind);
							CNL_NUMBER(CNL_CAR(args))++;
							while(CNL_PAIR_P(CNL_CDR(args))){
								args = CNL_CDR(args);
							}
							CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
							ret = CNL_NIL;
						}
						/* ENV POP END */
					}else{
						/* ENV POP&PUSH BEGIN */
						bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
						env = cnl_cons(gc,cnl_cons(gc,bind,targ),CNL_CDR(env));
						/* ENV POP&PUSH END */
					}
					/* SYNTAX begin END */
				}else if(CNL_SYNTAX(CNL_CADAR(bind)) == CNL_SYNTAX_DEFINE){
					/* SYNTAX define BEGIN */
					if(CNL_NUMBER(CNL_CAAR(bind)) == 1){
						/* ENV PUSH BEGIN */
						bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
						env = cnl_cons(gc,cnl_cons(gc,bind,CNL_CADDR(targ)),env);
						/* ENV PUSH END */
					}else{
						if(CNL_PAIR_P(CNL_CDR(env))){
							tmp = CNL_CDAR(CNL_CADR(env));
							if(CNL_PAIR_P(tmp)){
								tmp = cnl_cons(gc,CNL_CAR(tmp),CNL_CDR(tmp));
								CNL_SET_CDR(CNL_CDAR(CNL_CADR(env)),tmp);
								tmp = cnl_cons(gc,CNL_CADR(targ),CNL_CADDR(CNL_CAR(bind)));
								CNL_SET_CAR(CNL_CDAR(CNL_CADR(env)),tmp);
							}
						}else{
							tmp = CNL_CDR(bind);
							if(CNL_PAIR_P(tmp)){
								tmp = cnl_cons(gc,CNL_CAR(tmp),CNL_CDR(tmp));
								CNL_SET_CDR(CNL_CDR(bind),tmp);
								tmp = cnl_cons(gc,CNL_CADR(targ),CNL_CADDR(CNL_CAR(bind)));
								CNL_SET_CAR(CNL_CDR(bind),tmp);
							}
						}
						ret = cnl_make_undef(gc);
						/* ENV POP BEGIN */
						env = CNL_CDR(env);
						if(CNL_PAIR_P(env)){
							bind = CNL_CAAR(env);
							args = CNL_CAR(bind);
							CNL_NUMBER(CNL_CAR(args))++;
							while(CNL_PAIR_P(CNL_CDR(args))){
								args = CNL_CDR(args);
							}
							CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
							ret = CNL_NIL;
						}
						/* ENV POP END */
					}
					/* SYNTAX define END */
				}else if(CNL_SYNTAX(CNL_CADAR(bind)) == CNL_SYNTAX_IF){
					/* SYNTAX if BEGIN */
					if(CNL_NUMBER(CNL_CAAR(bind)) == 1){
						/* ENV PUSH BEGIN */
						bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
						env = cnl_cons(gc,cnl_cons(gc,bind,CNL_CADR(targ)),env);
						/* ENV PUSH END */
					}else{
						if(CNL_NIL_P(CNL_CADDR(CNL_CAR(bind)))){
							targ = CNL_CDDDR(targ);
							if(CNL_NIL_P(targ)){
								ret = cnl_make_undef(gc);
								/* ENV POP BEGIN */
								env = CNL_CDR(env);
								if(CNL_PAIR_P(env)){
									bind = CNL_CAAR(env);
									args = CNL_CAR(bind);
									CNL_NUMBER(CNL_CAR(args))++;
									while(CNL_PAIR_P(CNL_CDR(args))){
										args = CNL_CDR(args);
									}
									CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
									ret = CNL_NIL;
								}
								/* ENV POP END */
							}else{
								/* ENV POP&PUSH BEGIN */
								bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
								env = cnl_cons(gc,cnl_cons(gc,bind,CNL_CAR(targ)),CNL_CDR(env));
								/* ENV POP&PUSH END */
							}
						}else{
							/* ENV POP&PUSH BEGIN */
							bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
							env = cnl_cons(gc,cnl_cons(gc,bind,CNL_CADDR(targ)),CNL_CDR(env));
							/* ENV POP&PUSH END */
						}
					}
					/* SYNTAX if END */
				}else if(CNL_SYNTAX(CNL_CADAR(bind)) == CNL_SYNTAX_LAMBDA){
					/* SYNTAX lambda BEGIN */
					ret = cnl_make_proc(gc,CNL_CDR(bind),targ);
					/* ENV POP BEGIN */
					env = CNL_CDR(env);
					if(CNL_PAIR_P(env)){
						bind = CNL_CAAR(env);
						args = CNL_CAR(bind);
						CNL_NUMBER(CNL_CAR(args))++;
						while(CNL_PAIR_P(CNL_CDR(args))){
							args = CNL_CDR(args);
						}
						CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
						ret = CNL_NIL;
					}
					/* ENV POP END */
					/* SYNTAX lambda END */
				}else if(CNL_SYNTAX(CNL_CADAR(bind)) == CNL_SYNTAX_QUOTE){
					/* SYNTAX quote BEGIN */
					ret = CNL_CADR(targ);
					/* ENV POP BEGIN */
					env = CNL_CDR(env);
					if(CNL_PAIR_P(env)){
						bind = CNL_CAAR(env);
						args = CNL_CAR(bind);
						CNL_NUMBER(CNL_CAR(args))++;
						while(CNL_PAIR_P(CNL_CDR(args))){
							args = CNL_CDR(args);
						}
						CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
						ret = CNL_NIL;
					}
					/* ENV POP END */
					/* SYNTAX quote END */
				}else if(CNL_SYNTAX(CNL_CADAR(bind)) == CNL_SYNTAX_SWEEP){
					/* SYNTAX sweep BEGIN */
					cnl_gc_sweep(gc,env);
					ret = CNL_NIL;
					/* ENV POP BEGIN */
					env = CNL_CDR(env);
					if(CNL_PAIR_P(env)){
						bind = CNL_CAAR(env);
						args = CNL_CAR(bind);
						CNL_NUMBER(CNL_CAR(args))++;
						while(CNL_PAIR_P(CNL_CDR(args))){
							args = CNL_CDR(args);
						}
						CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
						ret = CNL_NIL;
					}
					/* ENV POP END */
					/* SYNTAX sweep END */
				}else{
					ret = cnl_make_undef(gc);
					/* ENV POP BEGIN */
					env = CNL_CDR(env);
					if(CNL_PAIR_P(env)){
						bind = CNL_CAAR(env);
						args = CNL_CAR(bind);
						CNL_NUMBER(CNL_CAR(args))++;
						while(CNL_PAIR_P(CNL_CDR(args))){
							args = CNL_CDR(args);
						}
						CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
						ret = CNL_NIL;
					}
					/* ENV POP END */
				}
			}else{
				int i;
				int n = CNL_NUMBER(CNL_CAAR(bind));
				for(i = 0; i < n; i++){
					if(CNL_PAIR_P(targ)){ targ = CNL_CDR(targ); }
				}
				if(CNL_PAIR_P(targ)){
					/* ENV PUSH BEGIN */
					bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
					env = cnl_cons(gc,cnl_cons(gc,bind,CNL_CAR(targ)),env);
					/* ENV PUSH END */
				}else if(CNL_NIL_P(targ)){
					tmp = CNL_CADAR(bind);
					if(CNL_PROC_P(tmp)){
						CNL_OBJ* proc = tmp;
						CNL_OBJ* names = CNL_CADDR(proc);
						CNL_OBJ* values = CNL_CDDAR(bind);
						bind = CNL_CAR(proc);
						args = CNL_NIL;
						int flag;
						while(CNL_PAIR_P(bind)){
							flag = 0;
							tmp = names;
							while(CNL_PAIR_P(tmp)){
								if(cnl_symbol_equal_p(CNL_CAAR(bind),CNL_CAR(tmp))){
									flag = 1;
								}
								tmp = CNL_CDR(tmp);
							}
							if(! flag){
								args = cnl_cons(gc,CNL_CAR(bind),args);
							}
							bind = CNL_CDR(bind);
						}
						while(CNL_PAIR_P(names)){
							if(CNL_PAIR_P(values)){
								args = cnl_cons(gc,cnl_cons(gc,CNL_CAR(names),CNL_CAR(values)),args);
								values = CNL_CDR(values);
							}else{
								args = cnl_cons(gc,cnl_cons(gc,CNL_CAR(names),CNL_NIL),args);
							}
							names = CNL_CDR(names);
						}
						if(! CNL_NIL_P(names)){
							args = cnl_cons(gc,cnl_cons(gc,names,values),args);
						}
						bind = args;
						/* ENV PUSH BEGIN */
						bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,1),cnl_cons(gc,cnl_make_syntax(gc,CNL_SYNTAX_BEGIN),CNL_NIL)),bind);
						targ = cnl_cons(gc,cnl_make_symbol(gc,"begin"),CNL_CDDDR(proc));
						env = cnl_cons(gc,cnl_cons(gc,bind,targ),CNL_CDR(env));
						/* ENV PUSH END */
					}else{
						if(CNL_FUNC_P(tmp)){
							ret = cnl_func_call(gc,CNL_CDAR(bind));
						}else{
							ret = CNL_CDAR(bind);
						}
						/* ENV POP BEGIN */
						env = CNL_CDR(env);
						if(CNL_PAIR_P(env)){
							bind = CNL_CAAR(env);
							args = CNL_CAR(bind);
							CNL_NUMBER(CNL_CAR(args))++;
							while(CNL_PAIR_P(CNL_CDR(args))){
								args = CNL_CDR(args);
							}
							CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
							ret = CNL_NIL;
						}
						/* ENV POP END */
					}
				}else{
					/* ENV PUSH BEGIN */
					bind = cnl_cons(gc,cnl_cons(gc,cnl_make_number(gc,0),CNL_NIL),CNL_CDR(bind));
					env = cnl_cons(gc,cnl_cons(gc,bind,targ),env);
					/* ENV PUSH END */
				}
			}
		}else if(CNL_SYMBOL_P(targ)){
			int find = 0;
			tmp = CNL_CDR(bind);
			while(CNL_PAIR_P(tmp)){
				if(cnl_symbol_equal_p(targ,CNL_CAAR(tmp))){
					ret = CNL_CDAR(tmp);
					find = 1;
					break;
				}
				tmp = CNL_CDR(tmp);
			}
			if(! find){
				ret = cnl_make_undef(gc);
			}
			/* ENV POP BEGIN */
			env = CNL_CDR(env);
			if(CNL_PAIR_P(env)){
				bind = CNL_CAAR(env);
				args = CNL_CAR(bind);
				CNL_NUMBER(CNL_CAR(args))++;
				while(CNL_PAIR_P(CNL_CDR(args))){
					args = CNL_CDR(args);
				}
				CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
				ret = CNL_NIL;
			}
			/* ENV POP END */
		}else{
			ret = targ;
			/* ENV POP BEGIN */
			env = CNL_CDR(env);
			if(CNL_PAIR_P(env)){
				bind = CNL_CAAR(env);
				args = CNL_CAR(bind);
				CNL_NUMBER(CNL_CAR(args))++;
				while(CNL_PAIR_P(CNL_CDR(args))){
					args = CNL_CDR(args);
				}
				CNL_SET_CDR(args,cnl_cons(gc,ret,CNL_CDR(args)));
				ret = CNL_NIL;
			}
			/* ENV POP END */
		}
	}

	bind = CNL_CDR(bind);
	return ret;
}

int main(int argc,char** argv){
	cnl_gc = cnl_make_gc();
	cnl_bind = cnl_make_default_binds(cnl_gc);
	yyparse();
	cnl_free_gc(cnl_gc);
	return 0;
}
