//-----------------------------------------------------------------------------
//
// Copyright (C) 2013-2014 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Function parsing utilities.
//
//-----------------------------------------------------------------------------

#include "AS/Parse.hpp"

#include "Core/Exception.hpp"
#include "Core/TokenStream.hpp"

#include "IR/Function.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace AS
   {
      //
      // ParseFunction
      //
      void ParseFunction(ParserCtx const &ctx, IR::Function &func)
      {
         while(!ctx.in.drop(Core::TOK_LnEnd))
            switch(TokenPeekIdenti(ctx).in.get().str)
         {
         case Core::STR_alloc:    func.alloc    = GetFastU(TokenDropEq(ctx));      break;
         case Core::STR_ctype:    func.ctype    = GetCallType(TokenDropEq(ctx));   break;
         case Core::STR_defin:    func.defin    = GetFastU(TokenDropEq(ctx));      break;
         case Core::STR_label:    func.label    = GetString(TokenDropEq(ctx));     break;
         case Core::STR_linka:    func.linka    = GetLinkage(TokenDropEq(ctx));    break;
         case Core::STR_localArs: func.localArs = GetFastU(TokenDropEq(ctx));      break;
         case Core::STR_localReg: func.localReg = GetFastU(TokenDropEq(ctx));      break;
         case Core::STR_param:    func.param    = GetFastU(TokenDropEq(ctx));      break;
         case Core::STR_retrn:    func.retrn    = GetFastU(TokenDropEq(ctx));      break;
         case Core::STR_sflagClS: func.sflagClS = GetFastU(TokenDropEq(ctx));      break;
         case Core::STR_sflagNet: func.sflagNet = GetFastU(TokenDropEq(ctx));      break;
         case Core::STR_stype:    func.stype    = GetScriptType(TokenDropEq(ctx)); break;
         case Core::STR_valueInt: func.valueInt = GetFastI(TokenDropEq(ctx));      break;
         case Core::STR_valueStr: func.valueStr = GetString(TokenDropEq(ctx));     break;

         case Core::STR_block:
            while(ctx.in.drop(Core::TOK_LnEnd)) {}
            TokenDrop(ctx, Core::TOK_BraceO, "'{'");
            ParseBlock(ctx, func.block, Core::TOK_BraceC);
            break;

         default:
            throw Core::ParseExceptExpect(ctx.in.reget(), "Function argument", false);
         }
      }
   }
}

// EOF
