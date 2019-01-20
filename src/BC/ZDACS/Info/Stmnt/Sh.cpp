//-----------------------------------------------------------------------------
//
// Copyright (C) 2014-2019 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Shift statement handling.
//
//-----------------------------------------------------------------------------

#include "BC/ZDACS/Info.hpp"

#include "BC/ZDACS/Code.hpp"

#include "IR/Exception.hpp"
#include "IR/Function.hpp"
#include "IR/Glyph.hpp"

#include <sstream>


//----------------------------------------------------------------------------|
// Extern Functions                                                           |
//

namespace GDCC::BC::ZDACS
{
   //
   // Info::genStmnt_ShLF
   //
   void Info::genStmnt_ShLF()
   {
      auto n = getStmntSize();

      if(n == 0)
      {
         numChunkCODE += 4;
         return;
      }

      genStmntCall(n);
   }

   //
   // Info::genStmnt_ShLU
   //
   void Info::genStmnt_ShLU()
   {
      auto n = getStmntSize();

      if(n <= 1)
      {
         numChunkCODE += 4;
         return;
      }

      if(stmnt->args[2].a != IR::ArgBase::Lit)
      {
         genStmntCall(n);
         return;
      }

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      // Shift of 0 is a no-op.
      if(!shift) return;

      Core::FastU shiftWords = shift / 32;
      Core::FastU keepWords  = n - shiftWords;

      numChunkCODE += shiftWords * 12 + keepWords * 8;

      if(shift % 32)
         numChunkCODE += 20 + (keepWords - 1) * 56;
      else
         numChunkCODE += keepWords * 8;
   }

   //
   // Info::genStmnt_ShRI
   //
   void Info::genStmnt_ShRI()
   {
      auto n = getStmntSize();

      if(n <= 1)
      {
         numChunkCODE += 4;
         return;
      }

      if(stmnt->args[2].a != IR::ArgBase::Lit)
      {
         genStmntCall(n);
         return;
      }

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      // Shift of 0 is a no-op.
      if(!shift) return;

      Core::FastU shiftWords = shift / 32;
      Core::FastU keepWords  = n - shiftWords;

      numChunkCODE += shiftWords * 4 + keepWords * 8;

      if(shift % 32)
         numChunkCODE += (keepWords - 1) * 56 + 20;
      else
         numChunkCODE += keepWords * 8;

      if(shiftWords)
         numChunkCODE += 20 + (shiftWords - 1) * 4;
   }

   //
   // Info::genStmnt_ShRU
   //
   void Info::genStmnt_ShRU()
   {
      auto n = getStmntSize();

      if(n == 0)
      {
         numChunkCODE += 4;
         return;
      }

      if(n == 1)
      {
         genStmnt_ShRU1();
         return;
      }

      if(stmnt->args[2].a != IR::ArgBase::Lit)
      {
         genStmntCall(n);
         return;
      }

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      // Shift of 0 is a no-op.
      if(!shift) return;

      Core::FastU shiftWords = shift / 32;
      Core::FastU keepWords  = n - shiftWords;

      numChunkCODE += shiftWords * 4 + keepWords * 8;

      if(shift % 32)
         numChunkCODE += (keepWords - 1) * 56 + 32;
      else
         numChunkCODE += keepWords * 8;

      numChunkCODE += shiftWords * 8;
   }

   //
   // Info::genStmnt_ShRU1
   //
   void Info::genStmnt_ShRU1()
   {
      if(stmnt->args[2].a == IR::ArgBase::Lit)
         numChunkCODE += 24;
      else if(stmnt->args[2].a == IR::ArgBase::Stk)
         numChunkCODE += 68;
      else
         numChunkCODE += 48 + lenPushArg(stmnt->args[2], 0) * 2;
   }

   //
   // Info::preStmnt_ShLF
   //
   void Info::preStmnt_ShLF()
   {
      auto n = getStmntSize();

      if(n == 0)
         return;

      if(stmnt->code == IR::Code::ShLF)
         addFunc_ShLF_W(n);
      else
         addFunc_ShRF_W(n);
   }

   //
   // Info::preStmnt_ShLU
   //
   void Info::preStmnt_ShLU()
   {
      auto n = getStmntSize();

      if(n <= 1)
         return;

      if(stmnt->code == IR::Code::ShLU)
         addFunc_ShLU_W(n);
      else if(stmnt->code == IR::Code::ShRI)
         addFunc_ShRI_W(n);
      else
         addFunc_ShRU_W(n);
   }

   //
   // Info::putStmnt_ShLF
   //
   void Info::putStmnt_ShLF()
   {
      auto n = getStmntSize();

      if(n == 0)
         return putCode(Code::Drop_Nul);

      putStmntCall(getCallName(stmnt->code, n), n);
   }

   //
   // Info::putStmnt_ShLU
   //
   void Info::putStmnt_ShLU()
   {
      auto n = getStmntSize();

      if(n == 0)
      {
         putCode(Code::Drop_Nul);
         return;
      }

      if(n == 1)
      {
         putCode(Code::ShLU);
         return;
      }

      if(stmnt->args[2].a != IR::ArgBase::Lit)
         return putStmntCall(getCallName(stmnt->code, n), n);

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      // Shift of 0 is a no-op.
      if(!shift) return;

      Core::FastU shiftBits  = shift % 32;
      Core::FastU shiftWords = shift / 32;
      Core::FastU keepWords  = n - shiftWords;

      for(Core::FastU i = shiftWords; i--;)
         putCode(Code::Drop_Nul);

      for(Core::FastU i = keepWords; i--;)
         putCode(Code::Drop_LocReg, func->localReg + i);

      for(Core::FastU i = shiftWords; i--;)
         putCode(Code::Push_Lit, 0);

      if(shiftBits)
      {
         // ret[0] = l[0] << r
         putCode(Code::Push_LocReg, func->localReg + 0);
         putCode(Code::Push_Lit,    shiftBits);
         putCode(Code::ShLU);

         for(Core::FastU i = 1; i != keepWords; ++i)
         {
            // ret[i] = (l[i] << r) | (l[i-1] >> (32 - r))

            putCode(Code::Push_LocReg, func->localReg + i);
            putCode(Code::Push_Lit,    shiftBits);
            putCode(Code::ShLU);

            putCode(Code::Push_LocReg, func->localReg + i - 1);
            putStmntShiftRU(32 - shiftBits);

            putCode(Code::BOrI);
         }
      }
      else
      {
         // ret[i] = l[i]
         for(Core::FastU i = 0; i != keepWords; ++i)
            putCode(Code::Push_LocReg, func->localReg + i);
      }
   }

   //
   // Info::putStmnt_ShRI
   //
   void Info::putStmnt_ShRI()
   {
      auto n = getStmntSize();

      if(n == 0)
      {
         putCode(Code::Drop_Nul);
         return;
      }

      if(n == 1)
      {
         putCode(Code::ShRI);
         return;
      }

      if(stmnt->args[2].a != IR::ArgBase::Lit)
         return putStmntCall(getCallName(stmnt->code, n), n);

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      // Shift of 0 is a no-op.
      if(!shift) return;

      Core::FastU shiftBits  = shift % 32;
      Core::FastU shiftWords = shift / 32;
      Core::FastU keepWords  = n - shiftWords;

      for(Core::FastU i = keepWords; i--;)
         putCode(Code::Drop_LocReg, func->localReg + i);

      for(Core::FastU i = shiftWords; i--;)
         putCode(Code::Drop_Nul);

      if(shiftBits)
      {
         for(Core::FastU i = 0; i != keepWords - 1; ++i)
         {
            // ret[i] = (l[i] >> r) | (l[i+1] << (32 - r))

            putCode(Code::Push_LocReg, func->localReg + i);
            putStmntShiftRU(shiftBits);

            putCode(Code::Push_LocReg, func->localReg + i + 1);
            putCode(Code::Push_Lit,    32 - shiftBits);
            putCode(Code::ShLU);

            putCode(Code::BOrI);
         }

         // ret[N-1] = l[N-1] >> r
         putCode(Code::Push_LocReg, func->localReg + keepWords - 1);
         putCode(Code::Push_Lit,    shiftBits);
         putCode(Code::ShRI);
      }
      else
      {
         // ret[i] = l[i]
         for(Core::FastU i = 0; i != keepWords; ++i)
            putCode(Code::Push_LocReg, func->localReg + i);
      }

      if(shiftWords)
      {
         putCode(Code::Push_LocReg, func->localReg + keepWords - 1);
         putCode(Code::Push_Lit,    31);
         putCode(Code::ShRI);

         for(Core::FastU i = shiftWords - 1; i--;)
            putCode(Code::Copy);
      }
   }

   //
   // Info::putStmnt_ShRU
   //
   void Info::putStmnt_ShRU()
   {
      auto n = getStmntSize();

      if(n == 0)
      {
         putCode(Code::Drop_Nul);
         return;
      }

      if(n == 1)
      {
         putStmnt_ShRU1();
         return;
      }

      if(stmnt->args[2].a != IR::ArgBase::Lit)
         return putStmntCall(getCallName(stmnt->code, n), n);

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      // Shift of 0 is a no-op.
      if(!shift) return;

      Core::FastU shiftBits  = shift % 32;
      Core::FastU shiftWords = shift / 32;
      Core::FastU keepWords  = n - shiftWords;

      for(Core::FastU i = keepWords; i--;)
         putCode(Code::Drop_LocReg, func->localReg + i);

      for(Core::FastU i = shiftWords; i--;)
         putCode(Code::Drop_Nul);

      if(shiftBits)
      {
         for(Core::FastU i = 0; i != keepWords - 1; ++i)
         {
            // ret[i] = (l[i] >> r) | (l[i+1] << (32 - r))

            putCode(Code::Push_LocReg, func->localReg + i);
            putStmntShiftRU(shiftBits);

            putCode(Code::Push_LocReg, func->localReg + i + 1);
            putCode(Code::Push_Lit,    32 - shiftBits);
            putCode(Code::ShLU);

            putCode(Code::BOrI);
         }

         // ret[N-1] = l[N-1] >> r
         putCode(Code::Push_LocReg, func->localReg + keepWords - 1);
         putStmntShiftRU(shiftBits);
      }
      else
      {
         // ret[i] = l[i]
         for(Core::FastU i = 0; i != keepWords; ++i)
            putCode(Code::Push_LocReg, func->localReg + i);
      }

      for(Core::FastU i = shiftWords; i--;)
         putCode(Code::Push_Lit, 0);
   }

   //
   // Info::putStmnt_ShRU1
   //
   void Info::putStmnt_ShRU1()
   {
      if(stmnt->args[2].a == IR::ArgBase::Lit)
      {
         putStmntShiftRU(getWord(stmnt->args[2].aLit));
      }
      else if(stmnt->args[2].a == IR::ArgBase::Stk)
      {
         // If shift is 0, jump to end.
         putCode(Code::Jcnd_Lit);
         putWord(0);
         putWord(putPos + 60);

         putCode(Code::Drop_LocReg);
         putWord(func->localReg + 0);
         putCode(Code::Push_LocReg);
         putWord(func->localReg + 0);
         putCode(Code::ShRI);
         putCode(Code::Push_Lit);
         putWord(0x80000000);
         putCode(Code::DecU_LocReg);
         putWord(func->localReg + 0);
         putCode(Code::Push_LocReg);
         putWord(func->localReg + 0);
         putCode(Code::ShRI);
         putCode(Code::BNot);
         putCode(Code::BAnd);
      }
      else
      {
         putStmntPushArg(stmnt->args[2], 0);

         // If shift is 0, jump to end.
         putCode(Code::Jcnd_Lit);
         putWord(0);
         putWord(putPos + 40 + lenPushArg(stmnt->args[2], 0));

         putCode(Code::ShRI);
         putCode(Code::Push_Lit, 0x80000000);
         putStmntPushArg(stmnt->args[2], 0);
         putCode(Code::Push_Lit, 1);
         putCode(Code::SubU);
         putCode(Code::ShRI);
         putCode(Code::BNot);
         putCode(Code::BAnd);
      }
   }

   //
   // Info::putStmntShiftRU
   //
   void Info::putStmntShiftRU(Core::FastU shift)
   {
      putCode(Code::Push_Lit, shift % 32);
      putCode(Code::ShRI);
      putCode(Code::Push_Lit, 0xFFFFFFFF >> (shift % 32));
      putCode(Code::BAnd);
   }

   //
   // Info::trStmnt_ShLF
   //
   void Info::trStmnt_ShLF()
   {
      auto n = getStmntSize();

      if(!trStmntShift(true))
         return;

      if(n <= 2)
         return;

      func->setLocalTmp(1);
   }

   //
   // Info::trStmnt_ShLU
   //
   void Info::trStmnt_ShLU()
   {
      auto n = getStmntSize();

      if(n <= 1)
         return (void)trStmntShift(true);

      if(!trStmntShift(false))
         return;

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      if(shift)
         func->setLocalTmp(n - shift / 32);
   }

   //
   // Info::trStmnt_ShRI
   //
   void Info::trStmnt_ShRI()
   {
      auto n = getStmntSize();

      if(n <= 1)
         return (void)trStmntShift(true);

      if(!trStmntShift(false))
         return;

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      if(shift)
         func->setLocalTmp(n - shift / 32);
   }

   //
   // Info::trStmnt_ShRU
   //
   void Info::trStmnt_ShRU()
   {
      auto n = getStmntSize();

      if(n == 0)
         return (void)trStmntShift(true);

      if(n == 1)
         return trStmnt_ShRU1();

      if(!trStmntShift(false))
         return;

      Core::FastU shift = getWord(stmnt->args[2].aLit) % (32 * n);

      if(shift)
         func->setLocalTmp(n - shift / 32);
   }

   //
   // Info::trStmnt_ShRU1
   //
   void Info::trStmnt_ShRU1()
   {
      if(stmnt->args[1].a != IR::ArgBase::Stk &&
         stmnt->args[2].a == IR::ArgBase::Stk)
         IR::ErrorCode(stmnt, "disorder");

      moveArgStk_dst(stmnt->args[0]);
      moveArgStk_src(stmnt->args[1]);

      if(!isPushArg(stmnt->args[2]) || !isFastArg(stmnt->args[2]))
      {
         func->setLocalTmp(1);
         moveArgStk_src(stmnt->args[2]);
      }
   }
}

// EOF
