/*
 file closure_impl.h
 Written by Pavel Kazmerchuk
 Original code by _Winnie http://www.gamedev.ru/forum/?group=0&topic=11026&page=4
 Space Concept 2005 http://www.sconcept.ru
 */
//---------------------------------------------------------------------------
#ifdef _MSC_VER
#    pragma once
#endif
//---------------------------------------------------------------------------
#ifndef _CLOSURE_H
#    define _CLOSURE_H
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS 
#define TEMPLATE_ARGS void
#define TEMPLATE_ARGS_COMMA 
#define PARAMS 
#define PARAMS_COMMA 
#define OPERATOR_ARGS_COMMA
#define ARGS 

#define DETAIL_NUM detail_0

#include "closure_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef TEMPLATE_ARGS_COMMA
#undef PARAMS
#undef PARAMS_COMMA
#undef ARGS
#undef DETAIL_NUM
#undef OPERATOR_ARGS_COMMA
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS ,class TP1
#define TEMPLATE_ARGS TP1
#define TEMPLATE_ARGS_COMMA ,TP1
#define PARAMS TP1 p1
#define PARAMS_COMMA ,TP1 p1
#define ARGS p1
#define OPERATOR_ARGS_COMMA ,
#define DETAIL_NUM detail_1

#include "closure_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef TEMPLATE_ARGS_COMMA
#undef PARAMS
#undef PARAMS_COMMA
#undef ARGS
#undef DETAIL_NUM
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS ,class TP1, class TP2
#define TEMPLATE_ARGS TP1, TP2
#define TEMPLATE_ARGS_COMMA ,TP1, TP2
#define PARAMS TP1 p1, TP2 p2
#define PARAMS_COMMA ,TP1 p1, TP2 p2
#define ARGS p1, p2
#define DETAIL_NUM detail_2

#include "closure_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef TEMPLATE_ARGS_COMMA
#undef PARAMS
#undef PARAMS_COMMA
#undef ARGS
#undef DETAIL_NUM
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS ,class TP1, class TP2, class TP3
#define TEMPLATE_ARGS TP1, TP2, TP3
#define TEMPLATE_ARGS_COMMA ,TP1, TP2, TP3
#define PARAMS TP1 p1, TP2 p2, TP3 p3
#define PARAMS_COMMA ,TP1 p1, TP2 p2, TP3 p3
#define ARGS p1, p2, p3
#define DETAIL_NUM detail_3

#include "closure_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef TEMPLATE_ARGS_COMMA
#undef PARAMS
#undef PARAMS_COMMA
#undef ARGS
#undef DETAIL_NUM
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS ,class TP1, class TP2, class TP3, class TP4
#define TEMPLATE_ARGS TP1, TP2, TP3, TP4
#define TEMPLATE_ARGS_COMMA ,TP1, TP2, TP3, TP4
#define PARAMS TP1 p1, TP2 p2, TP3 p3, TP4 p4
#define PARAMS_COMMA ,TP1 p1, TP2 p2, TP3 p3, TP4 p4
#define ARGS p1, p2, p3, p4
#define DETAIL_NUM detail_4

#include "closure_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef TEMPLATE_ARGS_COMMA
#undef PARAMS
#undef PARAMS_COMMA
#undef ARGS
#undef DETAIL_NUM
//---------------------------------------------------------------------------
#define CLOSURE(pobj, pfunc) (detail::closure(pobj, pfunc).create<pfunc>(pobj))
#define SCLOSURE(pfunc) (detail::closure(0, pfunc).create<pfunc>(0))
//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
