// Copyright (c) 2013 Primecoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef PRIMECOIN_PRIME_H
#define PRIMECOIN_PRIME_H

#if defined(__i386__) || defined(_M_IX86) || defined(_X86_) || defined(__x86_64__) || defined(_M_X64)
#define USE_ROTATE
#endif

#ifdef _M_X64
typedef uint64 sieve_word_t;
#else
typedef unsigned long sieve_word_t;
#endif

//#include "main.h"
#include "mpirxx.h"

extern std::vector<unsigned int> vPrimes;
static const unsigned int nMaxSieveExtensions = 20;
static const unsigned int nMinSieveExtensions = 0;
static const unsigned int nDefaultSieveExtensions = 10;
static const unsigned int nDefaultSieveExtensionsTestnet = 4;
extern unsigned int nSieveExtensions;

extern unsigned int nMaxSieveSize;
extern unsigned int vPrimesSize;
extern unsigned int nMaxPrimes;
extern bool nPrintDebugMessages;
extern unsigned long nOverrideTargetValue;
extern unsigned int nOverrideBTTargetValue;
static const uint256 hashBlockHeaderLimit = (uint256(1) << 255);
static const CBigNum bnOne = 1;
static const CBigNum bnTwo = 2;
static const CBigNum bnConst8 = 8;
static const CBigNum bnPrimeMax = (bnOne << 2000) - 1;
static const CBigNum bnPrimeMin = (bnOne << 255);
static const mpz_class mpzOne = 1;
static const mpz_class mpzTwo = 2;
static const mpz_class mpzConst8 = 8;
static const mpz_class mpzPrimeMax = (mpzOne << 2000) - 1;
static const mpz_class mpzPrimeMin = (mpzOne << 255);


extern unsigned int nTargetInitialLength;
extern unsigned int nTargetMinLength;
extern DWORD * threadHearthBeat;

// Generate small prime table
void GeneratePrimeTable(unsigned int nSieveSize);
// Get next prime number of p
//bool PrimeTableGetNextPrime(unsigned int* p);
bool PrimeTableGetNextPrime(unsigned int& p);
// Get previous prime number of p
bool PrimeTableGetPreviousPrime(unsigned int& p);

// Compute primorial number p#
void BNPrimorial(unsigned int p, CBigNum& bnPrimorial);
void Primorial(unsigned int p, mpz_class& mpzPrimorial);
unsigned int PrimorialFast(unsigned int p);
// Compute the first primorial number greater than or equal to bn
//void PrimorialAt(CBigNum& bn, CBigNum& bnPrimorial);
void PrimorialAt(mpz_class& bn, mpz_class& mpzPrimorial);

// Test probable prime chain for: bnPrimeChainOrigin
// fFermatTest
//   true - Use only Fermat tests
//   false - Use Fermat-Euler-Lagrange-Lifchitz tests
// Return value:
//   true - Probable prime chain found (one of nChainLength meeting target)
//   false - prime chain too short (none of nChainLength meeting target)
//bool ProbablePrimeChainTest(const CBigNum& bnPrimeChainOrigin, unsigned int nBits, bool fFermatTest, unsigned int& nChainLengthCunningham1, unsigned int& nChainLengthCunningham2, unsigned int& nChainLengthBiTwin);
bool ProbablePrimeChainTest(const mpz_class& bnPrimeChainOrigin, unsigned int nBits, bool fFermatTest, unsigned int& nChainLengthCunningham1, unsigned int& nChainLengthCunningham2, unsigned int& nChainLengthBiTwin, bool fullTest =false);
bool ProbablePrimeChainTestOrig(const mpz_class& bnPrimeChainOrigin, unsigned int nBits, bool fFermatTest, unsigned int& nChainLengthCunningham1, unsigned int& nChainLengthCunningham2, unsigned int& nChainLengthBiTwin, bool fullTest =false);


static const unsigned int nFractionalBits = 24;
static const unsigned int TARGET_FRACTIONAL_MASK = (1u<<nFractionalBits) - 1;
static const unsigned int TARGET_LENGTH_MASK = ~TARGET_FRACTIONAL_MASK;
static const uint64 nFractionalDifficultyMax = (1ull << (nFractionalBits + 32));
static const uint64 nFractionalDifficultyMin = (1ull << 32);
static const uint64 nFractionalDifficultyThreshold = (1ull << (8 + 32));
static const unsigned int nWorkTransitionRatio = 32;
unsigned int TargetGetLimit();
unsigned int TargetGetInitial();
unsigned int TargetGetLength(unsigned int nBits);
bool TargetSetLength(unsigned int nLength, unsigned int& nBits);
unsigned int TargetGetFractional(unsigned int nBits);
uint64 TargetGetFractionalDifficulty(unsigned int nBits);
bool TargetSetFractionalDifficulty(uint64 nFractionalDifficulty, unsigned int& nBits);
std::string TargetToString(unsigned int nBits);
unsigned int TargetFromInt(unsigned int nLength);
bool TargetGetMint(unsigned int nBits, uint64& nMint);
bool TargetGetNext(unsigned int nBits, int64 nInterval, int64 nTargetSpacing, int64 nActualSpacing, unsigned int& nBitsNext);

// Mine probable prime chain of form: n = h * p# +/- 1
//bool MineProbablePrimeChain(CBlock& block, CBigNum& bnFixedMultiplier, bool& fNewBlock, unsigned int& nTriedMultiplier, unsigned int& nProbableChainLength, unsigned int& nTests, unsigned int& nPrimesHit);

// Check prime proof-of-work
enum // prime chain type
{
   PRIME_CHAIN_CUNNINGHAM1 = 1u,
   PRIME_CHAIN_CUNNINGHAM2 = 2u,
   PRIME_CHAIN_BI_TWIN     = 3u,
};
// bool CheckPrimeProofOfWork(uint256 hashBlockHeader, unsigned int nBits, const CBigNum& bnPrimeChainMultiplier, unsigned int& nChainType, unsigned int& nChainLength);

// prime target difficulty value for visualization
double GetPrimeDifficulty(unsigned int nBits);
// Estimate work transition target to longer prime chain
unsigned int EstimateWorkTransition(unsigned int nPrevWorkTransition, unsigned int nBits, unsigned int nChainLength);
// prime chain type and length value
std::string GetPrimeChainName(unsigned int nChainType, unsigned int nChainLength);


// Sieve of Eratosthenes for proof-of-work mining
//
// Includes the sieve extension feature from jhPrimeminer by jh000
//
// A layer of the sieve determines whether the CC1 or CC2 chain members near the
// origin fixed_multiplier * candidate_multiplier * 2^k are known to be
// composites.
//
// The default sieve is composed of layers 1 .. nChainLength.
//
// An extension i is composed of layers i .. i + nChainLength. The candidates
// indexes from the extensions are multiplied by 2^i. The first half of the
// candidates are covered by the default sieve and previous extensions.
//
// The larger numbers in the extensions have a slightly smaller probability of
// being primes and take slightly longer to test but they can be calculated very
// efficiently because the layers overlap.
class CSieveOfEratosthenes
{
   static const int nMinPrimeSeq = 4; // this is Prime number 11, the first prime with unknown factor status.
   unsigned int nSieveSize; // size of the sieve
   unsigned int nMaxPrimes; //weave up to max primes
   unsigned int nSieveExtensions; // extend the sieve a given number of times
   unsigned int nAllocatedSieveSize;
   mpz_class mpzHash; // hash of the block header
   mpz_class mpzFixedMultiplier; // fixed round multiplier

   // final set of candidates for probable primality checking
   sieve_word_t *vfCandidates;
   sieve_word_t *vfCompositeBiTwin;
   sieve_word_t *vfCompositeCunningham1;
   sieve_word_t *vfCompositeCunningham2;

   // bitsets that can be combined to obtain the final bitset of candidates
   sieve_word_t *vfCompositeLayerCC1;
   sieve_word_t *vfCompositeLayerCC2;

   // extended sets
   sieve_word_t *vfExtendedCandidates;
   sieve_word_t *vfExtendedCompositeBiTwin;
   sieve_word_t *vfExtendedCompositeCunningham1;
   sieve_word_t *vfExtendedCompositeCunningham2;
   static const unsigned int nWordBits = 8 * sizeof(sieve_word_t);	

   unsigned int nCandidatesWords;
   unsigned int nCandidatesBytes;

   unsigned int nMultiplierBytes;
   unsigned int nAllocatedMultiplierBytes;
   unsigned int *vCunningham1Multipliers;
   unsigned int *vCunningham2Multipliers;

   unsigned int nPrimeSeq; // prime sequence number currently being processed
   unsigned int nCandidateCount; // cached total count of candidates
   unsigned int nCandidateMultiplier; // current candidate for power test
   unsigned int nCandidateIndex; // internal candidate index
   bool fCandidateIsExtended; // is the current candidate in the extended part
   unsigned int nCandidateActiveExtension; // which extension is active

   unsigned int nChainLength; // target chain length
   unsigned int nBTChainLength; // target chain length
   unsigned int nSieveLayers; // sieve layers
   unsigned int nPrimes; // number of times to weave the sieve
   unsigned int nTotalPrimes;

   //CBlockIndex* pindexPrev;

   __inline unsigned int GetWordNum(unsigned int nBitNum) {
      return nBitNum / nWordBits;
   }

   __inline sieve_word_t  GetBitMask(unsigned int nBitNum) {
      return (sieve_word_t)1UL << (nBitNum % nWordBits);
   }

   void ProcessMultiplier(sieve_word_t *vfComposites, const unsigned int nMinMultiplier, const unsigned int nMaxMultiplier, const std::vector<unsigned int>& vPrimes, unsigned int *vMultipliers, unsigned int nLayerSeq);

public:
   CSieveOfEratosthenes(unsigned int nSieveSize, unsigned int nMaxPrimes, unsigned int nSieveExtensions, unsigned int nTargetChainLength, unsigned int nTargetBTLength, mpz_class& mpzHash, mpz_class& mpzFixedMultiplier)
   {
      this->nSieveSize = nSieveSize;
      this->nAllocatedSieveSize = nSieveSize;
      this->nSieveExtensions = nSieveExtensions;
      this->mpzHash = mpzHash;
      this->mpzFixedMultiplier = mpzFixedMultiplier;
      this->nChainLength = nTargetChainLength;
      this->nBTChainLength = nTargetBTLength;
      this->nSieveLayers = nChainLength + nSieveExtensions;
      this->nTotalPrimes = vPrimes.size();
      this->nPrimes = (uint64)nMaxPrimes;
      this->nPrimeSeq = nMinPrimeSeq;
      this->nCandidateCount = 0;
      this->nCandidateMultiplier = 0;
      this->nCandidateIndex = 0;
      this->fCandidateIsExtended = false;
      this->nCandidateActiveExtension = 0;
      this->nCandidatesWords = (nSieveSize + nWordBits - 1) / nWordBits;
      this->nCandidatesBytes = nCandidatesWords * sizeof(sieve_word_t);

      vfCandidates = (sieve_word_t *)malloc(nCandidatesBytes);
      vfCompositeBiTwin = (sieve_word_t *)malloc(nCandidatesBytes);
      vfCompositeCunningham1 = (sieve_word_t *)malloc(nCandidatesBytes);
      vfCompositeCunningham2 = (sieve_word_t *)malloc(nCandidatesBytes);
      memset(vfCandidates, 0, nCandidatesBytes);
      memset(vfCompositeBiTwin, 0, nCandidatesBytes);
      memset(vfCompositeCunningham1, 0, nCandidatesBytes);
      memset(vfCompositeCunningham2, 0, nCandidatesBytes);

      // bitsets that can be combined to obtain the final bitset of candidates
      vfCompositeLayerCC1 = (sieve_word_t *)malloc(nCandidatesBytes);
      vfCompositeLayerCC2 = (sieve_word_t *)malloc(nCandidatesBytes);

      vfExtendedCandidates = (sieve_word_t *)malloc(nSieveExtensions * nCandidatesBytes);
      vfExtendedCompositeBiTwin = (sieve_word_t *)malloc(nSieveExtensions * nCandidatesBytes);
      vfExtendedCompositeCunningham1 = (sieve_word_t *)malloc(nSieveExtensions * nCandidatesBytes);
      vfExtendedCompositeCunningham2 = (sieve_word_t *)malloc(nSieveExtensions * nCandidatesBytes);
      memset(vfExtendedCandidates, 0, nSieveExtensions * nCandidatesBytes);
      memset(vfExtendedCompositeBiTwin, 0, nSieveExtensions * nCandidatesBytes);
      memset(vfExtendedCompositeCunningham1, 0, nSieveExtensions * nCandidatesBytes);
      memset(vfExtendedCompositeCunningham2, 0, nSieveExtensions * nCandidatesBytes);

      this->nMultiplierBytes = nPrimes * nSieveLayers * sizeof(unsigned int);
      nAllocatedMultiplierBytes = nMultiplierBytes;
      vCunningham1Multipliers = (unsigned int *)malloc(nMultiplierBytes);
      vCunningham2Multipliers = (unsigned int *)malloc(nMultiplierBytes);
      memset(vCunningham1Multipliers, 0xFF, nMultiplierBytes);
      memset(vCunningham2Multipliers, 0xFF, nMultiplierBytes);
   }

   ~CSieveOfEratosthenes()
   {
      free(vfCandidates);
      free(vfCompositeBiTwin);
      free(vfCompositeCunningham1);
      free(vfCompositeCunningham2);

      free(vfCompositeLayerCC1);
      free(vfCompositeLayerCC2);

      free(vfExtendedCandidates);
      free(vfExtendedCompositeBiTwin);
      free(vfExtendedCompositeCunningham1);
      free(vfExtendedCompositeCunningham2);

      free(vCunningham1Multipliers);
      free(vCunningham2Multipliers);
   }


   void Init(unsigned int nSieveSize, unsigned int nMaxPrimes, unsigned int nSieveExtensions, unsigned int nTargetChainLength, unsigned int nTargetBTLength, mpz_class& mpzHash, mpz_class& mpzFixedMultiplier)
   {
      this->nSieveSize = nSieveSize;
      this->nSieveExtensions = nSieveExtensions;
      this->mpzHash = mpzHash;
      this->mpzFixedMultiplier = mpzFixedMultiplier;
      this->nChainLength = nTargetChainLength;
      this->nBTChainLength = nTargetBTLength;
      this->nSieveLayers = nChainLength + nSieveExtensions;
      this->nTotalPrimes = vPrimes.size();
      this->nPrimes = (uint64)nMaxPrimes;
      this->nPrimeSeq = nMinPrimeSeq;
      this->nCandidateCount = 0;
      this->nCandidateMultiplier = 0;
      this->nCandidateIndex = 0;
      this->fCandidateIsExtended = false;
      this->nCandidateActiveExtension = 0;
      this->nCandidatesWords = (nSieveSize + nWordBits - 1) / nWordBits;
      this->nCandidatesBytes = nCandidatesWords * sizeof(sieve_word_t);
      if (nSieveSize > nAllocatedSieveSize)
      {
         free(vfCandidates);
         free(vfCompositeBiTwin);
         free(vfCompositeCunningham1);
         free(vfCompositeCunningham2);
         free(vfCompositeLayerCC1);
         free(vfCompositeLayerCC2);
         free(vfExtendedCandidates);
         free(vfExtendedCompositeBiTwin);
         free(vfExtendedCompositeCunningham1);
         free(vfExtendedCompositeCunningham2);
         vfCandidates = (sieve_word_t *)malloc(nCandidatesBytes);
         vfCompositeBiTwin = (sieve_word_t *)malloc(nCandidatesBytes);
         vfCompositeCunningham1 = (sieve_word_t *)malloc(nCandidatesBytes);
         vfCompositeCunningham2 = (sieve_word_t *)malloc(nCandidatesBytes);
         vfCompositeLayerCC1 = (sieve_word_t *)malloc(nCandidatesBytes);
         vfCompositeLayerCC2 = (sieve_word_t *)malloc(nCandidatesBytes);
         vfExtendedCandidates = (sieve_word_t *)malloc(nSieveExtensions * nCandidatesBytes);
         vfExtendedCompositeBiTwin = (sieve_word_t *)malloc(nSieveExtensions * nCandidatesBytes);
         vfExtendedCompositeCunningham1 = (sieve_word_t *)malloc(nSieveExtensions * nCandidatesBytes);
         vfExtendedCompositeCunningham2 = (sieve_word_t *)malloc(nSieveExtensions * nCandidatesBytes);
         nAllocatedSieveSize = nSieveSize;
      }
      memset(vfCandidates, 0, nCandidatesBytes);
      memset(vfCompositeBiTwin, 0, nCandidatesBytes);
      memset(vfCompositeCunningham1, 0, nCandidatesBytes);
      memset(vfCompositeCunningham2, 0, nCandidatesBytes);
      memset(vfExtendedCandidates, 0, nSieveExtensions * nCandidatesBytes);
      memset(vfExtendedCompositeBiTwin, 0, nSieveExtensions * nCandidatesBytes);
      memset(vfExtendedCompositeCunningham1, 0, nSieveExtensions * nCandidatesBytes);
      memset(vfExtendedCompositeCunningham2, 0, nSieveExtensions * nCandidatesBytes);


      this->nMultiplierBytes = nPrimes * nSieveLayers * sizeof(unsigned int);
      if (nMultiplierBytes > nAllocatedMultiplierBytes)
      {
         free(vCunningham1Multipliers);
         free(vCunningham2Multipliers);
         vCunningham1Multipliers = (unsigned int *)malloc(nMultiplierBytes);
         vCunningham2Multipliers = (unsigned int *)malloc(nMultiplierBytes);
         nAllocatedMultiplierBytes = nMultiplierBytes;
      }
      memset(vCunningham1Multipliers, 0xFF, nMultiplierBytes);
      memset(vCunningham2Multipliers, 0xFF, nMultiplierBytes);
   }


   // Get total number of candidates for power test
   unsigned int GetCandidateCount()
   {
      //if (nCandidateCount)
      //   return nCandidateCount;

      unsigned int nCandidates = 0;
#ifdef __GNUC__
      for (unsigned int i = 0; i < nCandidatesWords; i++)
         nCandidates += __builtin_popcountl(vfCandidates[i]);
      for (unsigned int j = 0; j < nSieveExtensions; j++)
         for (unsigned int i = nCandidatesWords / 2; i < nCandidatesWords; i++)
            nCandidates += __builtin_popcountl(vfExtendedCandidates[j * nCandidatesWords + i]);
#else
      for (unsigned int i = 0; i < nCandidatesWords; i++)
      {
         sieve_word_t lBits = vfCandidates[i];
         for (unsigned int j = 0; j < nWordBits; j++)
         {
            nCandidates += (lBits & 1);
            lBits >>= 1;
         }
      }
      for (unsigned int j = 0; j < nSieveExtensions; j++)
      {
         for (unsigned int i = nCandidatesWords / 2; i < nCandidatesWords; i++)
         {
            sieve_word_t lBits = vfExtendedCandidates[j * nCandidatesWords + i];
            for (unsigned int j = 0; j < nWordBits; j++)
            {
               nCandidates += (lBits & 1);
               lBits >>= 1;
            }
         }
      }
#endif
      //nCandidateCount = nCandidates;
      return nCandidates;
   }

   // Scan for the next candidate multiplier (variable part)
   // Return values:
   //   True - found next candidate; nVariableMultiplier has the candidate
   //   False - scan complete, no more candidate and reset scan
   bool GetNextCandidateMultiplier(unsigned int& nVariableMultiplier, unsigned int& nCandidateType)
   {
      sieve_word_t *vfActiveCandidates;
      sieve_word_t *vfActiveCompositeTWN;
      sieve_word_t *vfActiveCompositeCC1;

      if (fCandidateIsExtended)
      {
         vfActiveCandidates = vfExtendedCandidates + nCandidateActiveExtension * nCandidatesWords;
         vfActiveCompositeTWN = vfExtendedCompositeBiTwin + nCandidateActiveExtension * nCandidatesWords;
         vfActiveCompositeCC1 = vfExtendedCompositeCunningham1 + nCandidateActiveExtension * nCandidatesWords;
      }
      else
      {
         vfActiveCandidates = vfCandidates;
         vfActiveCompositeTWN = vfCompositeBiTwin;
         vfActiveCompositeCC1 = vfCompositeCunningham1;
      }

      // Acquire the current word from the bitmap
      sieve_word_t lBits = vfActiveCandidates[GetWordNum(nCandidateIndex)];

      for(;;)
      {
         nCandidateIndex++;
         if (nCandidateIndex >= nSieveSize)
         {
            // Check if extensions are available
            if (!fCandidateIsExtended && nSieveExtensions > 0)
            {
               fCandidateIsExtended = true;
               nCandidateActiveExtension = 0;
               nCandidateIndex = nSieveSize / 2;
            }
            else if (fCandidateIsExtended && nCandidateActiveExtension + 1 < nSieveExtensions)
            {
               nCandidateActiveExtension++;
               nCandidateIndex = nSieveSize / 2;
            }
            else
            {
               // Out of candidates
               fCandidateIsExtended = false;
               nCandidateActiveExtension = 0;
               nCandidateIndex = 0;
               nCandidateMultiplier = 0;
               return false;
            }

            // Fix the pointers
            if (fCandidateIsExtended)
            {
               vfActiveCandidates = vfExtendedCandidates + nCandidateActiveExtension * nCandidatesWords;
               vfActiveCompositeTWN = vfExtendedCompositeBiTwin + nCandidateActiveExtension * nCandidatesWords;
               vfActiveCompositeCC1 = vfExtendedCompositeCunningham1 + nCandidateActiveExtension * nCandidatesWords;
            }
            else
            {
               vfActiveCandidates = vfCandidates;
               vfActiveCompositeTWN = vfCompositeBiTwin;
               vfActiveCompositeCC1 = vfCompositeCunningham1;
            }
         }

         if (nCandidateIndex % nWordBits == 0)
         {
            // Update the current word
            lBits = vfActiveCandidates[GetWordNum(nCandidateIndex)];

            // Check if any bits are set
            if (lBits == 0)
            {
               // Skip an entire word
               nCandidateIndex += nWordBits - 1;
               continue;
            }
         }

         if (lBits & GetBitMask(nCandidateIndex))
         {
            if (fCandidateIsExtended)
               nCandidateMultiplier = nCandidateIndex * (2 << nCandidateActiveExtension);
            else
               nCandidateMultiplier = nCandidateIndex;
            nVariableMultiplier = nCandidateMultiplier;
            if (~vfActiveCompositeTWN[GetWordNum(nCandidateIndex)] & GetBitMask(nCandidateIndex))
               nCandidateType = PRIME_CHAIN_BI_TWIN;
            else if (~vfActiveCompositeCC1[GetWordNum(nCandidateIndex)] & GetBitMask(nCandidateIndex))
               nCandidateType = PRIME_CHAIN_CUNNINGHAM1;
            else
               nCandidateType = PRIME_CHAIN_CUNNINGHAM2;
            return true;
         }
      }
   }

   // Weave the sieve for the next prime in table
   // Return values:
   //   True  - weaved another prime; nComposite - number of composites removed
   //   False - sieve already completed
   bool Weave();
};

inline void mpz_set_uint256(mpz_t r, uint256& u)
{
   mpz_import(r, 32 / sizeof(unsigned long), -1, sizeof(unsigned long), -1, 0, &u);
}


#endif
