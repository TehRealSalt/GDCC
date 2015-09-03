//-----------------------------------------------------------------------------
//
// Copyright (C) 2014 David Hill
//
// See COPYING for license information.
//
//-----------------------------------------------------------------------------
//
// Shift statement handling.
//
//-----------------------------------------------------------------------------

#include "Bytecode/ZDACS/Info.hpp"

#include "Bytecode/ZDACS/Code.hpp"

#include "Core/Exception.hpp"

#include "IR/Function.hpp"


//----------------------------------------------------------------------------|
// Global Functions                                                           |
//

namespace GDCC
{
   namespace Bytecode
   {
      namespace ZDACS
      {
         //
         // Info::genStmnt_ShLU_W
         //
         void Info::genStmnt_ShLU_W()
         {
            if(stmnt->op.size <= 1)
            {
               numChunkCODE += 4;
               return;
            }

            if(stmnt->args[2].a != IR::ArgBase::Lit)
            {
               genStmntCall(stmnt->op.size);
               return;
            }

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            // Shift of 0 is a no-op.
            if(!shift) return;

            Core::FastU shiftWords = shift / 32;
            Core::FastU keepWords  = stmnt->op.size - shiftWords;

            numChunkCODE += shiftWords * 12 + keepWords * 8;

            if(shift % 32)
               numChunkCODE += 20 + (keepWords - 1) * 56;
            else
               numChunkCODE += keepWords * 8;
         }

         //
         // Info::genStmnt_ShRI_W
         //
         void Info::genStmnt_ShRI_W()
         {
            if(stmnt->op.size <= 1)
            {
               numChunkCODE += 4;
               return;
            }

            if(stmnt->args[2].a != IR::ArgBase::Lit)
            {
               genStmntCall(stmnt->op.size);
               return;
            }

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            // Shift of 0 is a no-op.
            if(!shift) return;

            Core::FastU shiftWords = shift / 32;
            Core::FastU keepWords  = stmnt->op.size - shiftWords;

            numChunkCODE += shiftWords * 4 + keepWords * 8;

            if(shift % 32)
               numChunkCODE += (keepWords - 1) * 56 + 20;
            else
               numChunkCODE += keepWords * 8;

            if(shiftWords)
               numChunkCODE += 20 + (shiftWords - 1) * 4;
         }

         //
         // Info::genStmnt_ShRU_W
         //
         void Info::genStmnt_ShRU_W()
         {
            if(stmnt->op.size == 0)
            {
               numChunkCODE += 4;
               return;
            }

            if(stmnt->op.size == 1)
            {
               genStmnt_ShRU_W1();
               return;
            }

            if(stmnt->args[2].a != IR::ArgBase::Lit)
            {
               genStmntCall(stmnt->op.size);
               return;
            }

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            // Shift of 0 is a no-op.
            if(!shift) return;

            Core::FastU shiftWords = shift / 32;
            Core::FastU keepWords  = stmnt->op.size - shiftWords;

            numChunkCODE += shiftWords * 4 + keepWords * 8;

            if(shift % 32)
               numChunkCODE += (keepWords - 1) * 56 + 32;
            else
               numChunkCODE += keepWords * 8;

            numChunkCODE += shiftWords * 8;
         }

         //
         // Info::genStmnt_ShRU_W1
         //
         void Info::genStmnt_ShRU_W1()
         {
            if(stmnt->args[2].a == IR::ArgBase::Lit)
               numChunkCODE += 24;
            else if(stmnt->args[2].a == IR::ArgBase::Stk)
               numChunkCODE += 68;
            else
               numChunkCODE += 48 + lenPushArg(stmnt->args[2], 0) * 2;
         }

         //
         // Info::putStmnt_ShLU_W
         //
         void Info::putStmnt_ShLU_W()
         {
            if(stmnt->op.size == 0)
            {
               putCode(Code::Drop_Nul);
               return;
            }

            if(stmnt->op.size == 1)
            {
               putCode(Code::ShLU);
               return;
            }

            if(stmnt->args[2].a != IR::ArgBase::Lit)
            {
               putStmntCall(stmnt->op.size);
               return;
            }

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            // Shift of 0 is a no-op.
            if(!shift) return;

            Core::FastU shiftBits  = shift % 32;
            Core::FastU shiftWords = shift / 32;
            Core::FastU keepWords  = stmnt->op.size - shiftWords;

            for(Core::FastU n = shiftWords; n--;)
               putCode(Code::Drop_Nul);

            for(Core::FastU n = keepWords; n--;)
               putCode(Code::Drop_LocReg, func->localReg + n);

            for(Core::FastU n = shiftWords; n--;)
               putCode(Code::Push_Lit, 0);

            if(shiftBits)
            {
               // ret[0] = l[0] << r
               putCode(Code::Push_LocReg, func->localReg + 0);
               putCode(Code::Push_Lit,    shiftBits);
               putCode(Code::ShLU);

               for(Core::FastU n = 1; n != keepWords; ++n)
               {
                  // ret[n] = (l[n] << r) | (l[n-1] >> (32 - r))

                  putCode(Code::Push_LocReg, func->localReg + n);
                  putCode(Code::Push_Lit,    shiftBits);
                  putCode(Code::ShLU);

                  putCode(Code::Push_LocReg, func->localReg + n - 1);
                  putStmntShiftRU(32 - shiftBits);

                  putCode(Code::OrIU);
               }
            }
            else
            {
               // ret[n] = l[n]
               for(Core::FastU n = 0; n != keepWords; ++n)
                  putCode(Code::Push_LocReg, func->localReg + n);
            }
         }

         //
         // Info::putStmnt_ShRI_W
         //
         void Info::putStmnt_ShRI_W()
         {
            if(stmnt->op.size == 0)
            {
               putCode(Code::Drop_Nul);
               return;
            }

            if(stmnt->op.size == 1)
            {
               putCode(Code::ShRI);
               return;
            }

            if(stmnt->args[2].a != IR::ArgBase::Lit)
            {
               putStmntCall(stmnt->op.size);
               return;
            }

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            // Shift of 0 is a no-op.
            if(!shift) return;

            Core::FastU shiftBits  = shift % 32;
            Core::FastU shiftWords = shift / 32;
            Core::FastU keepWords  = stmnt->op.size - shiftWords;

            for(Core::FastU n = keepWords; n--;)
               putCode(Code::Drop_LocReg, func->localReg + n);

            for(Core::FastU n = shiftWords; n--;)
               putCode(Code::Drop_Nul);

            if(shiftBits)
            {
               for(Core::FastU n = 0; n != keepWords - 1; ++n)
               {
                  // ret[n] = (l[n] >> r) | (l[n+1] << (32 - r))

                  putCode(Code::Push_LocReg, func->localReg + n);
                  putStmntShiftRU(shiftBits);

                  putCode(Code::Push_LocReg, func->localReg + n + 1);
                  putCode(Code::Push_Lit,    32 - shiftBits);
                  putCode(Code::ShLU);

                  putCode(Code::OrIU);
               }

               // ret[N-1] = l[N-1] >> r
               putCode(Code::Push_LocReg, func->localReg + keepWords - 1);
               putCode(Code::Push_Lit,    shiftBits);
               putCode(Code::ShRI);
            }
            else
            {
               // ret[n] = l[n]
               for(Core::FastU n = 0; n != keepWords; ++n)
                  putCode(Code::Push_LocReg, func->localReg + n);
            }

            if(shiftWords)
            {
               putCode(Code::Push_LocReg, func->localReg + keepWords - 1);
               putCode(Code::Push_Lit,    31);
               putCode(Code::ShRI);

               for(Core::FastU n = shiftWords - 1; n--;)
                  putCode(Code::Copy);
            }
         }

         //
         // Info::putStmnt_ShRU_W
         //
         void Info::putStmnt_ShRU_W()
         {
            if(stmnt->op.size == 0)
            {
               putCode(Code::Drop_Nul);
               return;
            }

            if(stmnt->op.size == 1)
            {
               putStmnt_ShRU_W1();
               return;
            }

            if(stmnt->args[2].a != IR::ArgBase::Lit)
            {
               putStmntCall(stmnt->op.size);
               return;
            }

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            // Shift of 0 is a no-op.
            if(!shift) return;

            Core::FastU shiftBits  = shift % 32;
            Core::FastU shiftWords = shift / 32;
            Core::FastU keepWords  = stmnt->op.size - shiftWords;

            for(Core::FastU n = keepWords; n--;)
               putCode(Code::Drop_LocReg, func->localReg + n);

            for(Core::FastU n = shiftWords; n--;)
               putCode(Code::Drop_Nul);

            if(shiftBits)
            {
               for(Core::FastU n = 0; n != keepWords - 1; ++n)
               {
                  // ret[n] = (l[n] >> r) | (l[n+1] << (32 - r))

                  putCode(Code::Push_LocReg, func->localReg + n);
                  putStmntShiftRU(shiftBits);

                  putCode(Code::Push_LocReg, func->localReg + n + 1);
                  putCode(Code::Push_Lit,    32 - shiftBits);
                  putCode(Code::ShLU);

                  putCode(Code::OrIU);
               }

               // ret[N-1] = l[N-1] >> r
               putCode(Code::Push_LocReg, func->localReg + keepWords - 1);
               putStmntShiftRU(shiftBits);
            }
            else
            {
               // ret[n] = l[n]
               for(Core::FastU n = 0; n != keepWords; ++n)
                  putCode(Code::Push_LocReg, func->localReg + n);
            }

            for(Core::FastU n = shiftWords; n--;)
               putCode(Code::Push_Lit, 0);
         }

         //
         // Info::putStmnt_ShRU_W1
         //
         void Info::putStmnt_ShRU_W1()
         {
            if(stmnt->args[2].a == IR::ArgBase::Lit)
            {
               putStmntShiftRU(GetWord(stmnt->args[2].aLit));
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
               putCode(Code::InvU);
               putCode(Code::AndU);
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
               putCode(Code::InvU);
               putCode(Code::AndU);
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
            putCode(Code::AndU);
         }

         //
         // Info::trStmnt_ShLU_W
         //
         void Info::trStmnt_ShLU_W()
         {
            if(stmnt->op.size <= 1)
            {
               trStmntShift(true);
               return;
            }

            if(!trStmntShift())
               return;

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            if(shift)
               func->setLocalTmp(stmnt->op.size - shift / 32);
         }

         //
         // Info::trStmnt_ShRI_W
         //
         void Info::trStmnt_ShRI_W()
         {
            if(stmnt->op.size <= 1)
            {
               trStmntShift(true);
               return;
            }

            if(!trStmntShift())
               return;

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            if(shift)
               func->setLocalTmp(stmnt->op.size - shift / 32);
         }

         //
         // Info::trStmnt_ShRU_W
         //
         void Info::trStmnt_ShRU_W()
         {
            if(stmnt->op.size == 0)
            {
               trStmntShift(true);
               return;
            }

            if(stmnt->op.size == 1)
            {
               trStmnt_ShRU_W1();
               return;
            }

            if(!trStmntShift())
               return;

            Core::FastU shift = GetWord(stmnt->args[2].aLit) % (32 * stmnt->op.size);

            if(shift)
               func->setLocalTmp(stmnt->op.size - shift / 32);
         }

         //
         // Info::trStmnt_ShRU_W1
         //
         void Info::trStmnt_ShRU_W1()
         {
            CheckArgC(stmnt, 3);

            if(stmnt->args[1].a != IR::ArgBase::Stk &&
               stmnt->args[2].a == IR::ArgBase::Stk)
               throw Core::ExceptStr(stmnt->pos, "trStmnt_ShRU_W1 disorder");

            moveArgStk_dst(stmnt->args[0], stmnt->op.size);
            if(moveArgStk_src(stmnt->args[1], stmnt->op.size)) return;

            if(!isPushArg(stmnt->args[2]) || !isFastArg(stmnt->args[2]))
            {
               func->setLocalTmp(1);
               moveArgStk_src(stmnt->args[2], 1);
            }
         }

         //
         // Info::trStmntShift
         //
         bool Info::trStmntShift(bool moveLit)
         {
            CheckArgC(stmnt, 3);

            if(stmnt->args[1].a != IR::ArgBase::Stk &&
               stmnt->args[2].a == IR::ArgBase::Stk)
               throw Core::ExceptStr(stmnt->pos, "trStmntShift disorder");

            moveArgStk_dst(stmnt->args[0], stmnt->op.size);
            if(moveArgStk_src(stmnt->args[1], stmnt->op.size)) return false;

            if(!moveLit && stmnt->args[2].a == IR::ArgBase::Lit)
               return true;

            moveArgStk_src(stmnt->args[2], 1);
            return false;
         }
      }
   }
}

// EOF

