//-----------------------------------------------------------------------------
//
// Copyright (C) 2014-2018 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// C "operator -" pointer expressions.
//
//-----------------------------------------------------------------------------

#include "CC/Exp/Sub.hpp"

#include "CC/Type.hpp"

#include "IR/Block.hpp"

#include "Platform/Platform.hpp"

#include "SR/Temporary.hpp"
#include "SR/Type.hpp"


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace GDCC::CC
{
   //
   // Exp_SubPtrInt::v_genStmnt
   //
   void Exp_SubPtrInt::v_genStmnt(SR::GenStmntCtx const &ctx,
      SR::Arg const &dst) const
   {
      SR::GenStmnt_Point(this, IR::Code::SubU, ctx, dst);
   }

   //
   // Exp_SubPtrIntEq::v_genStmnt
   //
   void Exp_SubPtrIntEq::v_genStmnt(SR::GenStmntCtx const &ctx,
      SR::Arg const &dst) const
   {
      SR::GenStmnt_PointEq(this, IR::Code::SubU, ctx, dst, post);
   }

   //
   // Exp_SubPtrIntEq::v_getIRExp
   //
   IR::Exp::CRef Exp_SubPtrIntEq::v_getIRExp() const
   {
      return post ? expL->getIRExp() : Super::v_getIRExp();
   }

   //
   // Exp_SubPtrIntEq::v_isIRExp
   //
   bool Exp_SubPtrIntEq::v_isIRExp() const
   {
      return post ? expL->isIRExp() : Super::v_isIRExp();
   }

   //
   // Exp_SubPtrPtrW::v_genStmnt
   //
   void Exp_SubPtrPtrW::v_genStmnt(SR::GenStmntCtx const &ctx,
      SR::Arg const &dst) const
   {
      if(GenStmntNul(this, ctx, dst)) return;

      Core::FastU pointWords = expL->getType()->getSizeWords();

      // Evaluate both sub-expressions to stack.
      expL->genStmntStk(ctx);
      expR->genStmntStk(ctx);

      if(pointWords > 1)
      {
         auto diffBytes = (pointWords - 1) * Platform::GetWordBytes();

         SR::Temporary tmp{ctx, pos, 1};

         ctx.block.addStmnt(IR::Code::Move,
            tmp. getArg(), tmp.getArgStk());
         ctx.block.addStmnt(IR::Code::Move,
            IR::Arg_Nul(diffBytes), IR::Arg_Stk(diffBytes));

         ctx.block.addStmnt(IR::Code::SubI,
            tmp.getArg(), tmp.getArgStk(), tmp.getArg());
         ctx.block.addStmnt(IR::Code::Move,
            IR::Arg_Nul(diffBytes), IR::Arg_Stk(diffBytes));

         ctx.block.addStmnt(IR::Code::Move,
            tmp.getArgStk(), tmp.getArg());
      }
      else
      {
         // Subtract on stack.
         ctx.block.setArgSize().addStmnt(IR::Code::SubI,
            IR::Block::Stk(), IR::Block::Stk(), IR::Block::Stk());
      }

      // Adjust result, if needed.
      auto point = expL->getType()->getBaseType()->getSizePoint();
      if(point > 1)
      {
         // TODO: Use a shift where possible. That is, where it is either
         // known that the result is positive or if the target has
         // logical shift. Also that the rounding behavior of shifting
         // negative integers is acceptable. (That is, it will only break
         // if the pointers are already not properly aligned.)

         ctx.block.setArgSize().addStmnt(IR::Code::DivI,
            IR::Block::Stk(), IR::Block::Stk(), point);
      }

      // Move to destination.
      GenStmnt_MovePart(this, ctx, dst, false, true);
   }
}

// EOF

