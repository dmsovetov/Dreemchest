//---------------------------------------------------------------------------
/*
 file delegate_impl.h
 Written by Pavel Kazmerchuk
 Space Concept 2005 http://www.sconcept.ru
 */
//---------------------------------------------------------------------------
#ifdef _MSC_VER
#	pragma once
#endif
//---------------------------------------------------------------------------
#ifndef _DELEGATE_H
#	define _DELEGATE_H
//---------------------------------------------------------------------------
#include <list>
//---------------------------------------------------------------------------
// Start of gui namespace
//---------------------------------------------------------------------------
namespace gui
{
//---------------------------------------------------------------------------
#include "closure.h"
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS 
#define TEMPLATE_ARGS 
#define PARAMS 
#define ARGS 

#include "delegate_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS ,class TP1
#define TEMPLATE_ARGS TP1
#define PARAMS TP1 p1
#define ARGS p1

#include "delegate_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS ,class TP1, class TP2
#define TEMPLATE_ARGS TP1, TP2
#define PARAMS TP1 p1, TP2 p2
#define ARGS p1, p2

#include "delegate_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS
//---------------------------------------------------------------------------
#define TEMPLATE_PARAMS ,class TP1, class TP2, class TP3
#define TEMPLATE_ARGS TP1, TP2, TP3
#define PARAMS TP1 p1, TP2 p2, TP3 p3
#define ARGS p1, p2, p3

#include "delegate_impl.h"

#undef TEMPLATE_PARAMS
#undef TEMPLATE_ARGS
#undef PARAMS
#undef ARGS
//---------------------------------------------------------------------------
}// End of gui namespace
//---------------------------------------------------------------------------
#endif