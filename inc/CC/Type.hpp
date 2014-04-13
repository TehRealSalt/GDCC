//-----------------------------------------------------------------------------
//
// Copyright (C) 2013-2014 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// C types.
//
//
// C/C++ type classification.
//
// Where semantic differences between languages exist, the line is marked by
// that language, indicating that the type has the properties of that
// relative position in the chart only for that language. For example, enum
// is an integer type in C, but merely a scalar type in C++.
//
// This chart includes extended types, except for alternative spellings like
// __fixed and __str. (__str is an alternative spelling for __str_ent*.)
//
//   function types
//   +-member function types (C++)
//   +-static function types
//   object types
//   +-accum types
//   | +-signed accum types
//   | | +-short _Accum
//   | | +-_Accum
//   | | +-long _Accum
//   | | +-_Sat short _Accum
//   | | +-_Sat _Accum
//   | | +-_Sat long _Accum
//   | +-unsigned accum types
//   |   +-unsigned short _Accum
//   |   +-unsigned _Accum
//   |   +-unsigned long _Accum
//   |   +-_Sat unsigned short _Accum
//   |   +-_Sat unsigned _Accum
//   |   +-_Sat unsigned long _Accum
//   +-aggregate types
//   | +-array types
//   | +-structure types
//   | +-tuple types
//   +-character types
//   | +-char
//   | +-signed char
//   | +-unsigned char
//   +-fract types
//   | +-signed fract types
//   | | +-short _Fract
//   | | +-_Fract
//   | | +-long _Fract
//   | | +-_Sat short _Fract
//   | | +-_Sat _Fract
//   | | +-_Sat long _Fract
//   | +-unsigned fract types
//   |   +-unsigned short _Fract
//   |   +-unsigned _Fract
//   |   +-unsigned long _Fract
//   |   +-_Sat unsigned short _Fract
//   |   +-_Sat unsigned _Fract
//   |   +-_Sat unsigned long _Fract
//   +-real types
//   | +-fixed-point types
//   | +-integer types
//   | +-real floating types
//   +-scalar types
//   | +-arithmetic types
//   | | +-fixed-point types
//   | | | +-primary fixed-point types
//   | | | | +-primary signed fixed-point types
//   | | | | | +-short _Accum
//   | | | | | +-_Accum
//   | | | | | +-long _Accum
//   | | | | | +-short _Fract
//   | | | | | +-_Fract
//   | | | | | +-long _Fract
//   | | | | +-primary unsigned fixed-point types
//   | | | |   +-unsigned short _Accum
//   | | | |   +-unsigned _Accum
//   | | | |   +-unsigned long _Accum
//   | | | |   +-unsigned short _Fract
//   | | | |   +-unsigned _Fract
//   | | | |   +-unsigned long _Fract
//   | | | +-saturating fixed-point types
//   | | |   +-saturating signed fixed-point types
//   | | |   | +-_Sat short _Accum
//   | | |   | +-_Sat _Accum
//   | | |   | +-_Sat long _Accum
//   | | |   | +-_Sat short _Fract
//   | | |   | +-_Sat _Fract
//   | | |   | +-_Sat long _Fract
//   | | |   +-saturating unsigned fixed-point types
//   | | |     +-_Sat unsigned short _Accum
//   | | |     +-_Sat unsigned _Accum
//   | | |     +-_Sat unsigned long _Accum
//   | | |     +-_Sat unsigned short _Fract
//   | | |     +-_Sat unsigned _Fract
//   | | |     +-_Sat unsigned long _Fract
//   | | +-floating types
//   | | | +-complex types
//   | | | | +-float _Complex
//   | | | | +-double _Complex
//   | | | | +-long double _Complex
//   | | | +-imaginary types
//   | | | | +-float _Imaginary
//   | | | | +-double _Imaginary
//   | | | | +-long double _Imaginary
//   | | | +-real floating types
//   | | |   +-float
//   | | |   +-double
//   | | |   +-long double
//   | | +-integer types
//   | |   +-bool (C++)
//   | |   +-char
//   | |   +-char16_t (C++)
//   | |   +-char32_t (C++)
//   | |   +-enumerated types (C)
//   | |   +-signed integer types
//   | |   | +-extended signed integer types
//   | |   | +-standard signed integer types
//   | |   |   +-signed char
//   | |   |   +-short int
//   | |   |   +-int
//   | |   |   +-long int
//   | |   |   +-long long int
//   | |   +-unsigned integer types
//   | |   | +-extended unsigned integer types
//   | |   | +-standard unsigned integer types
//   | |   |   +-unsigned char
//   | |   |   +-unsigned short int
//   | |   |   +-unsigned int
//   | |   |   +-unsigned long int
//   | |   |   +-unsigned long long int
//   | |   |   +-_Bool (C)
//   | |   +-wchar_t (C++)
//   | +-enumerated types (C++)
//   | | +-scoped enumeration types
//   | | +-unscoped enumeration types
//   | +-pointer types
//   | +-pointer to member types (C++)
//   | +-std::nullptr_t (C++)
//   | +-__fastbool
//   +-extended integer types
//   | +-extended signed integer types
//   | +-extended unsigned integer types
//   +-standard integer types
//   | +-standard signed integer types
//   | +-standard unsigned integer types
//   +-union types
//   +-void
//   reference types
//   +-lvalue reference types
//   +-rvalue reference types
//   __label
//   __str_ent
//
//-----------------------------------------------------------------------------

#ifndef GDCC__CC__Type_H__
#define GDCC__CC__Type_H__

#include "../Core/Counter.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define GDCC_CC_Type_ImplGet(type, class, ...) \
   AST::Type::CRef GetType##type() \
      {static AST::Type::CRef t{new Type_##class(__VA_ARGS__)}; return t;}


//----------------------------------------------------------------------------|
// Types                                                                      |
//

namespace GDCC
{
   namespace AST
   {
      class Type;
   }
}


//----------------------------------------------------------------------------|
// Global Variables                                                           |
//

namespace GDCC
{
   namespace CC
   {
      extern Core::CounterRef<AST::Type const> TypeBool;
      extern Core::CounterRef<AST::Type const> TypeBoolSoft;

      extern Core::CounterRef<AST::Type const> TypeChar;

      extern Core::CounterRef<AST::Type const> TypeFixedPrS;
      extern Core::CounterRef<AST::Type const> TypeFixedPrSH;
      extern Core::CounterRef<AST::Type const> TypeFixedPrSL;
      extern Core::CounterRef<AST::Type const> TypeFixedPrU;
      extern Core::CounterRef<AST::Type const> TypeFixedPrUH;
      extern Core::CounterRef<AST::Type const> TypeFixedPrUL;

      extern Core::CounterRef<AST::Type const> TypeFixedSaS;
      extern Core::CounterRef<AST::Type const> TypeFixedSaSH;
      extern Core::CounterRef<AST::Type const> TypeFixedSaSL;
      extern Core::CounterRef<AST::Type const> TypeFixedSaU;
      extern Core::CounterRef<AST::Type const> TypeFixedSaUH;
      extern Core::CounterRef<AST::Type const> TypeFixedSaUL;

      extern Core::CounterRef<AST::Type const> TypeFloatCS;
      extern Core::CounterRef<AST::Type const> TypeFloatCSL;
      extern Core::CounterRef<AST::Type const> TypeFloatCSLL;

      extern Core::CounterRef<AST::Type const> TypeFloatIS;
      extern Core::CounterRef<AST::Type const> TypeFloatISL;
      extern Core::CounterRef<AST::Type const> TypeFloatISLL;

      extern Core::CounterRef<AST::Type const> TypeFloatRS;
      extern Core::CounterRef<AST::Type const> TypeFloatRSL;
      extern Core::CounterRef<AST::Type const> TypeFloatRSLL;

      extern Core::CounterRef<AST::Type const> TypeFractPrS;
      extern Core::CounterRef<AST::Type const> TypeFractPrSH;
      extern Core::CounterRef<AST::Type const> TypeFractPrSL;
      extern Core::CounterRef<AST::Type const> TypeFractPrU;
      extern Core::CounterRef<AST::Type const> TypeFractPrUH;
      extern Core::CounterRef<AST::Type const> TypeFractPrUL;

      extern Core::CounterRef<AST::Type const> TypeFractSaS;
      extern Core::CounterRef<AST::Type const> TypeFractSaSH;
      extern Core::CounterRef<AST::Type const> TypeFractSaSL;
      extern Core::CounterRef<AST::Type const> TypeFractSaU;
      extern Core::CounterRef<AST::Type const> TypeFractSaUH;
      extern Core::CounterRef<AST::Type const> TypeFractSaUL;

      extern Core::CounterRef<AST::Type const> TypeIntegPrS;
      extern Core::CounterRef<AST::Type const> TypeIntegPrSH;
      extern Core::CounterRef<AST::Type const> TypeIntegPrSHH;
      extern Core::CounterRef<AST::Type const> TypeIntegPrSL;
      extern Core::CounterRef<AST::Type const> TypeIntegPrSLL;
      extern Core::CounterRef<AST::Type const> TypeIntegPrU;
      extern Core::CounterRef<AST::Type const> TypeIntegPrUH;
      extern Core::CounterRef<AST::Type const> TypeIntegPrUHH;
      extern Core::CounterRef<AST::Type const> TypeIntegPrUL;
      extern Core::CounterRef<AST::Type const> TypeIntegPrULL;
   }
}


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace CC
   {
      Core::CounterRef<AST::Type const> GetTypeBool();
      Core::CounterRef<AST::Type const> GetTypeBoolSoft();

      Core::CounterRef<AST::Type const> GetTypeChar();

      Core::CounterRef<AST::Type const> GetTypeFixedPrS();
      Core::CounterRef<AST::Type const> GetTypeFixedPrSH();
      Core::CounterRef<AST::Type const> GetTypeFixedPrSL();
      Core::CounterRef<AST::Type const> GetTypeFixedPrU();
      Core::CounterRef<AST::Type const> GetTypeFixedPrUH();
      Core::CounterRef<AST::Type const> GetTypeFixedPrUL();

      Core::CounterRef<AST::Type const> GetTypeFixedSaS();
      Core::CounterRef<AST::Type const> GetTypeFixedSaSH();
      Core::CounterRef<AST::Type const> GetTypeFixedSaSL();
      Core::CounterRef<AST::Type const> GetTypeFixedSaU();
      Core::CounterRef<AST::Type const> GetTypeFixedSaUH();
      Core::CounterRef<AST::Type const> GetTypeFixedSaUL();

      Core::CounterRef<AST::Type const> GetTypeFloatCS();
      Core::CounterRef<AST::Type const> GetTypeFloatCSL();
      Core::CounterRef<AST::Type const> GetTypeFloatCSLL();

      Core::CounterRef<AST::Type const> GetTypeFloatIS();
      Core::CounterRef<AST::Type const> GetTypeFloatISL();
      Core::CounterRef<AST::Type const> GetTypeFloatISLL();

      Core::CounterRef<AST::Type const> GetTypeFloatRS();
      Core::CounterRef<AST::Type const> GetTypeFloatRSL();
      Core::CounterRef<AST::Type const> GetTypeFloatRSLL();

      Core::CounterRef<AST::Type const> GetTypeFractPrS();
      Core::CounterRef<AST::Type const> GetTypeFractPrSH();
      Core::CounterRef<AST::Type const> GetTypeFractPrSL();
      Core::CounterRef<AST::Type const> GetTypeFractPrU();
      Core::CounterRef<AST::Type const> GetTypeFractPrUH();
      Core::CounterRef<AST::Type const> GetTypeFractPrUL();

      Core::CounterRef<AST::Type const> GetTypeFractSaS();
      Core::CounterRef<AST::Type const> GetTypeFractSaSH();
      Core::CounterRef<AST::Type const> GetTypeFractSaSL();
      Core::CounterRef<AST::Type const> GetTypeFractSaU();
      Core::CounterRef<AST::Type const> GetTypeFractSaUH();
      Core::CounterRef<AST::Type const> GetTypeFractSaUL();

      Core::CounterRef<AST::Type const> GetTypeIntegPrS();
      Core::CounterRef<AST::Type const> GetTypeIntegPrSH();
      Core::CounterRef<AST::Type const> GetTypeIntegPrSHH();
      Core::CounterRef<AST::Type const> GetTypeIntegPrSL();
      Core::CounterRef<AST::Type const> GetTypeIntegPrSLL();
      Core::CounterRef<AST::Type const> GetTypeIntegPrU();
      Core::CounterRef<AST::Type const> GetTypeIntegPrUH();
      Core::CounterRef<AST::Type const> GetTypeIntegPrUHH();
      Core::CounterRef<AST::Type const> GetTypeIntegPrUL();
      Core::CounterRef<AST::Type const> GetTypeIntegPrULL();
   }
}

#endif//GDCC__CC__Type_H__

