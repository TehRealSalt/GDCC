//-----------------------------------------------------------------------------
//
// Copyright (C) 2015 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// ACS expression parsing.
//
//-----------------------------------------------------------------------------

#include "ACC/Parse.hpp"

#include "AST/Exp.hpp"

#include "CC/Exp.hpp"

#include "Core/Array.hpp"
#include "Core/Exception.hpp"
#include "Core/TokenStream.hpp"


//----------------------------------------------------------------------------|
// Macros                                                                     |
//

#define DeclExpCreate(getter1, getter2) \
   auto exp = getter1(ctx, scope); \
   \
   auto expCreate = [&](AST::Exp::CRef (*creator)(AST::Exp const *, \
      AST::Exp const *, Core::Origin)) \
   { \
      auto pos = ctx.in.get().pos; \
      return creator(exp, getter2(ctx, scope), pos); \
   }


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace ACC
   {
      //
      // GetExp_Mult
      //
      AST::Exp::CRef GetExp_Mult(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_Cast, GetExp_Cast);

         for(;;) switch(ctx.in.peek().tok)
         {
         case Core::TOK_Mul: exp = expCreate(CC::ExpCreate_Mul); break;
         case Core::TOK_Div: exp = expCreate(CC::ExpCreate_Div); break;
         case Core::TOK_Mod: exp = expCreate(CC::ExpCreate_Mod); break;

         default:
            return exp;
         }

      }

      //
      // GetExp_Addi
      //
      AST::Exp::CRef GetExp_Addi(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_Mult, GetExp_Mult);

         for(;;) switch(ctx.in.peek().tok)
         {
         case Core::TOK_Add: exp = expCreate(CC::ExpCreate_Add); break;
         case Core::TOK_Sub: exp = expCreate(CC::ExpCreate_Sub); break;

         default:
            return exp;
         }

      }

      //
      // GetExp_Shft
      //
      AST::Exp::CRef GetExp_Shft(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_Addi, GetExp_Addi);

         for(;;) switch(ctx.in.peek().tok)
         {
         case Core::TOK_ShL: exp = expCreate(CC::ExpCreate_ShL); break;
         case Core::TOK_ShR: exp = expCreate(CC::ExpCreate_ShR); break;

         default:
            return exp;
         }

      }

      //
      // GetExp_Rela
      //
      AST::Exp::CRef GetExp_Rela(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_Shft, GetExp_Shft);

         for(;;) switch(ctx.in.peek().tok)
         {
         case Core::TOK_CmpLT: exp = expCreate(CC::ExpCreate_CmpLT); break;
         case Core::TOK_CmpGT: exp = expCreate(CC::ExpCreate_CmpGT); break;
         case Core::TOK_CmpLE: exp = expCreate(CC::ExpCreate_CmpLE); break;
         case Core::TOK_CmpGE: exp = expCreate(CC::ExpCreate_CmpGE); break;

         default:
            return exp;
         }

      }

      //
      // GetExp_Equa
      //
      AST::Exp::CRef GetExp_Equa(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_Rela, GetExp_Rela);

         for(;;) switch(ctx.in.peek().tok)
         {
         case Core::TOK_CmpEQ: exp = expCreate(CC::ExpCreate_CmpEQ); break;
         case Core::TOK_CmpNE: exp = expCreate(CC::ExpCreate_CmpNE); break;

         default:
            return exp;
         }
      }

      //
      // GetExp_BAnd
      //
      AST::Exp::CRef GetExp_BAnd(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_Equa, GetExp_Equa);

         while(ctx.in.peek().tok == Core::TOK_And)
            exp = expCreate(CC::ExpCreate_BitAnd);

         return exp;
      }

      //
      // GetExp_BOrX
      //
      AST::Exp::CRef GetExp_BOrX(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_BAnd, GetExp_BAnd);

         while(ctx.in.peek().tok == Core::TOK_OrX)
            exp = expCreate(CC::ExpCreate_BitOrX);

         return exp;
      }

      //
      // GetExp_BOrI
      //
      AST::Exp::CRef GetExp_BOrI(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_BOrX, GetExp_BOrX);

         while(ctx.in.peek().tok == Core::TOK_OrI)
            exp = expCreate(CC::ExpCreate_BitOrI);

         return exp;
      }

      //
      // GetExp_LAnd
      //
      AST::Exp::CRef GetExp_LAnd(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_BOrI, GetExp_BOrI);

         while(ctx.in.peek().tok == Core::TOK_And2)
            exp = expCreate(CC::ExpCreate_LogAnd);

         return exp;
      }

      //
      // GetExp_LOrI
      //
      AST::Exp::CRef GetExp_LOrI(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_LAnd, GetExp_LAnd);

         while(ctx.in.peek().tok == Core::TOK_OrI2)
            exp = expCreate(CC::ExpCreate_LogOrI);

         return exp;
      }

      //
      // GetExp_Cond
      //
      AST::Exp::CRef GetExp_Cond(ParserCtx const &ctx, CC::Scope &scope)
      {
         auto exp = GetExp_LOrI(ctx, scope);

         if(ctx.in.peek().tok == Core::TOK_Query)
         {
            auto pos = ctx.in.get().pos;

            auto expT = GetExp(ctx, scope);

            if(!ctx.in.drop(Core::TOK_Colon))
               throw Core::ExceptStr(ctx.in.peek().pos, "expected ':'");

            return CC::ExpCreate_Cnd(exp, expT, GetExp_Cond(ctx, scope), pos);
         }

         return exp;
      }

      //
      // GetExp_Assi
      //
      AST::Exp::CRef GetExp_Assi(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_Cond, GetExp_Assi);

         switch(ctx.in.peek().tok)
         {
         case Core::TOK_Equal: return expCreate(CC::ExpCreate_Assign);
         case Core::TOK_MulEq: return expCreate(CC::ExpCreate_MulEq);
         case Core::TOK_DivEq: return expCreate(CC::ExpCreate_DivEq);
         case Core::TOK_ModEq: return expCreate(CC::ExpCreate_ModEq);
         case Core::TOK_AddEq: return expCreate(CC::ExpCreate_AddEq);
         case Core::TOK_SubEq: return expCreate(CC::ExpCreate_SubEq);
         case Core::TOK_ShLEq: return expCreate(CC::ExpCreate_ShLEq);
         case Core::TOK_ShREq: return expCreate(CC::ExpCreate_ShREq);
         case Core::TOK_AndEq: return expCreate(CC::ExpCreate_BitAndEq);
         case Core::TOK_OrXEq: return expCreate(CC::ExpCreate_BitOrXEq);
         case Core::TOK_OrIEq: return expCreate(CC::ExpCreate_BitOrIEq);

         default:
            return exp;
         }
      }

      //
      // GetExp
      //
      AST::Exp::CRef GetExp(ParserCtx const &ctx, CC::Scope &scope)
      {
         DeclExpCreate(GetExp_Assi, GetExp_Assi);

         while(ctx.in.peek().tok == Core::TOK_Comma)
            exp = expCreate(CC::ExpCreate_Comma);

         return exp;
      }

      //
      // GetExpList
      //
      Core::Array<AST::Exp::CRef> GetExpList(ParserCtx const &ctx, CC::Scope &scope)
      {
         std::vector<AST::Exp::CRef> args;

         do
            args.emplace_back(GetExp_Assi(ctx, scope));
         while(ctx.in.drop(Core::TOK_Comma));

         return {Core::Move, args.begin(), args.end()};
      }
   }
}

// EOF

