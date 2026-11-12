/* More subroutines needed by GCC output code on some machines.  */
/* Compile this one with gcc.  */
/* Copyright (C) 1989, 92-98, 1999 Free Software Foundation, Inc.

This file is part of GNU CC.

GNU CC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU CC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,

/* LSW1 decompilation adaptation of the SN Systems libgcc2.c (SN Systems'
   GPL source release of their GCC 2.95-based ProDG for Nintendo GameCube
   toolchain, NGC_GNU_SRC/NGC/gcc/libgcc2.c, itself GCC 2.95's libgcc2.c).
   Only the routines that are linked into the game are kept: the 64-bit
   shifts, divide/modulo (with __udivmoddi4 and __clz_tab), the 64-bit
   compares and the L__main constructor runner (with the upstream block that
   restores the normal C types before it). The SN revision-history banner,
   the host/target configuration headers and every other libgcc2 routine are
   removed; "libgcc_config.h" supplies the PowerPC EABI values. Each routine
   is built as its own object by defining one L_* macro (see
   src/sn/libgcc/_*.c), as the libgcc Makefile does.  */

/* As a special exception, if you link this library with other files,
   some of which are compiled with GCC, to produce an executable,
   this library does not by itself cause the resulting executable
   to be covered by the GNU General Public License.
   This exception does not however invalidate any other reasons why
   the executable file might be covered by the GNU General Public License.  */

#include "libgcc_config.h"

/* In the first part of this file, we are interfacing to calls generated
   by the compiler itself.  These calls pass values into these routines
   which have very specific modes (rather than very specific types), and
   these compiler-generated calls also expect any return values to have
   very specific modes (rather than very specific types).  Thus, we need
   to avoid using regular C language type names in this part of the file
   because the sizes for those types can be configured to be anything.
   Instead we use the following special type names.  */

typedef unsigned int UQItype	__attribute__ ((mode (QI)));
typedef 	 int SItype	__attribute__ ((mode (SI)));
typedef unsigned int USItype	__attribute__ ((mode (SI)));
typedef		 int DItype	__attribute__ ((mode (DI)));
typedef unsigned int UDItype	__attribute__ ((mode (DI)));

typedef 	float SFtype	__attribute__ ((mode (SF)));
typedef		float DFtype	__attribute__ ((mode (DF)));

#if LIBGCC2_LONG_DOUBLE_TYPE_SIZE == 96
typedef		float XFtype	__attribute__ ((mode (XF)));
#endif
#if LIBGCC2_LONG_DOUBLE_TYPE_SIZE == 128
typedef		float TFtype	__attribute__ ((mode (TF)));
#endif

typedef int word_type __attribute__ ((mode (__word__)));

/* Make sure that we don't accidentally use any normal C language built-in
   type names in the first part of this file.  Instead we want to use *only*
   the type names defined above.  The following macro definitions insure
   that if we *do* accidentally use some normal C language built-in type name,
   we will get a syntax error.  */

#define char bogus_type
#define short bogus_type
#define int bogus_type
#define long bogus_type
#define unsigned bogus_type
#define float bogus_type
#define double bogus_type

#define SI_TYPE_SIZE (sizeof (SItype) * BITS_PER_UNIT)

/* DIstructs are pairs of SItype values in the order determined by
   LIBGCC2_WORDS_BIG_ENDIAN.  */

#if LIBGCC2_WORDS_BIG_ENDIAN
  struct DIstruct {SItype high, low;};
#else
  struct DIstruct {SItype low, high;};
#endif

/* We need this union to unpack/pack DImode values, since we don't have
   any arithmetic yet.  Incoming DImode parameters are stored into the
   `ll' field, and the unpacked result is read from the struct `s'.  */

typedef union
{
  struct DIstruct s;
  DItype ll;
} DIunion;

#if (defined (L_udivmoddi4) || defined (L_muldi3) || defined (L_udiv_w_sdiv)\
     || defined (L_divdi3) || defined (L_udivdi3) \
     || defined (L_moddi3) || defined (L_umoddi3))

#include "longlong.h"

#endif /* udiv or mul */

extern DItype __fixunssfdi (SFtype a);
extern DItype __fixunsdfdi (DFtype a);
#if LIBGCC2_LONG_DOUBLE_TYPE_SIZE == 96
extern DItype __fixunsxfdi (XFtype a);
#endif
#if LIBGCC2_LONG_DOUBLE_TYPE_SIZE == 128
extern DItype __fixunstfdi (TFtype a);
#endif

#if defined (L_negdi2) || defined (L_divdi3) || defined (L_moddi3)
#if defined (L_divdi3) || defined (L_moddi3)
static inline
#endif
DItype
__negdi2 (DItype u)
{
  DIunion w;
  DIunion uu;

  uu.ll = u;

  w.s.low = -uu.s.low;
  w.s.high = -uu.s.high - ((USItype) w.s.low > 0);

  return w.ll;
}
#endif

/* Unless shift functions are defined whith full ANSI prototypes,
   parameter b will be promoted to int if word_type is smaller than an int.  */
#ifdef L_lshrdi3
DItype
__lshrdi3 (DItype u, word_type b)
{
  DIunion w;
  word_type bm;
  DIunion uu;

  if (b == 0)
    return u;

  uu.ll = u;

  bm = (sizeof (SItype) * BITS_PER_UNIT) - b;
  if (bm <= 0)
    {
      w.s.high = 0;
      w.s.low = (USItype)uu.s.high >> -bm;
    }
  else
    {
      USItype carries = (USItype)uu.s.high << bm;
      w.s.high = (USItype)uu.s.high >> b;
      w.s.low = ((USItype)uu.s.low >> b) | carries;
    }

  return w.ll;
}
#endif

#ifdef L_ashldi3
DItype
__ashldi3 (DItype u, word_type b)
{
  DIunion w;
  word_type bm;
  DIunion uu;

  if (b == 0)
    return u;

  uu.ll = u;

  bm = (sizeof (SItype) * BITS_PER_UNIT) - b;
  if (bm <= 0)
    {
      w.s.low = 0;
      w.s.high = (USItype)uu.s.low << -bm;
    }
  else
    {
      USItype carries = (USItype)uu.s.low >> bm;
      w.s.low = (USItype)uu.s.low << b;
      w.s.high = ((USItype)uu.s.high << b) | carries;
    }

  return w.ll;
}
#endif

#ifdef L_ashrdi3
DItype
__ashrdi3 (DItype u, word_type b)
{
  DIunion w;
  word_type bm;
  DIunion uu;

  if (b == 0)
    return u;

  uu.ll = u;

  bm = (sizeof (SItype) * BITS_PER_UNIT) - b;
  if (bm <= 0)
    {
      /* w.s.high = 1..1 or 0..0 */
      w.s.high = uu.s.high >> (sizeof (SItype) * BITS_PER_UNIT - 1);
      w.s.low = uu.s.high >> -bm;
    }
  else
    {
      USItype carries = (USItype)uu.s.high << bm;
      w.s.high = uu.s.high >> b;
      w.s.low = ((USItype)uu.s.low >> b) | carries;
    }

  return w.ll;
}
#endif

#if (defined (L_udivdi3) || defined (L_divdi3) || \
     defined (L_umoddi3) || defined (L_moddi3))
#define L_udivmoddi4
#endif

#ifdef L_udivmoddi4
static const UQItype __clz_tab[256] =
{
  0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,
  6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,6,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,7,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
  8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,8,
};

#if (defined (L_udivdi3) || defined (L_divdi3) || \
     defined (L_umoddi3) || defined (L_moddi3))
static inline
#endif
UDItype
__udivmoddi4 (UDItype n, UDItype d, UDItype *rp)
{
  DIunion ww;
  DIunion nn, dd;
  DIunion rr;
  USItype d0, d1, n0, n1, n2;
  USItype q0, q1;
  USItype b, bm;

  nn.ll = n;
  dd.ll = d;

  d0 = dd.s.low;
  d1 = dd.s.high;
  n0 = nn.s.low;
  n1 = nn.s.high;

#if !UDIV_NEEDS_NORMALIZATION
  if (d1 == 0)
    {
      if (d0 > n1)
	{
	  /* 0q = nn / 0D */

	  udiv_qrnnd (q0, n0, n1, n0, d0);
	  q1 = 0;

	  /* Remainder in n0.  */
	}
      else
	{
	  /* qq = NN / 0d */

	  if (d0 == 0)
	    d0 = 1 / d0;	/* Divide intentionally by zero.  */

	  udiv_qrnnd (q1, n1, 0, n1, d0);
	  udiv_qrnnd (q0, n0, n1, n0, d0);

	  /* Remainder in n0.  */
	}

      if (rp != 0)
	{
	  rr.s.low = n0;
	  rr.s.high = 0;
	  *rp = rr.ll;
	}
    }

#else /* UDIV_NEEDS_NORMALIZATION */

  if (d1 == 0)
    {
      if (d0 > n1)
	{
	  /* 0q = nn / 0D */

	  count_leading_zeros (bm, d0);

	  if (bm != 0)
	    {
	      /* Normalize, i.e. make the most significant bit of the
		 denominator set.  */

	      d0 = d0 << bm;
	      n1 = (n1 << bm) | (n0 >> (SI_TYPE_SIZE - bm));
	      n0 = n0 << bm;
	    }

	  udiv_qrnnd (q0, n0, n1, n0, d0);
	  q1 = 0;

	  /* Remainder in n0 >> bm.  */
	}
      else
	{
	  /* qq = NN / 0d */

	  if (d0 == 0)
	    d0 = 1 / d0;	/* Divide intentionally by zero.  */

	  count_leading_zeros (bm, d0);

	  if (bm == 0)
	    {
	      /* From (n1 >= d0) /\ (the most significant bit of d0 is set),
		 conclude (the most significant bit of n1 is set) /\ (the
		 leading quotient digit q1 = 1).

		 This special case is necessary, not an optimization.
		 (Shifts counts of SI_TYPE_SIZE are undefined.)  */

	      n1 -= d0;
	      q1 = 1;
	    }
	  else
	    {
	      /* Normalize.  */

	      b = SI_TYPE_SIZE - bm;

	      d0 = d0 << bm;
	      n2 = n1 >> b;
	      n1 = (n1 << bm) | (n0 >> b);
	      n0 = n0 << bm;

	      udiv_qrnnd (q1, n1, n2, n1, d0);
	    }

	  /* n1 != d0...  */

	  udiv_qrnnd (q0, n0, n1, n0, d0);

	  /* Remainder in n0 >> bm.  */
	}

      if (rp != 0)
	{
	  rr.s.low = n0 >> bm;
	  rr.s.high = 0;
	  *rp = rr.ll;
	}
    }
#endif /* UDIV_NEEDS_NORMALIZATION */

  else
    {
      if (d1 > n1)
	{
	  /* 00 = nn / DD */

	  q0 = 0;
	  q1 = 0;

	  /* Remainder in n1n0.  */
	  if (rp != 0)
	    {
	      rr.s.low = n0;
	      rr.s.high = n1;
	      *rp = rr.ll;
	    }
	}
      else
	{
	  /* 0q = NN / dd */

	  count_leading_zeros (bm, d1);
	  if (bm == 0)
	    {
	      /* From (n1 >= d1) /\ (the most significant bit of d1 is set),
		 conclude (the most significant bit of n1 is set) /\ (the
		 quotient digit q0 = 0 or 1).

		 This special case is necessary, not an optimization.  */

	      /* The condition on the next line takes advantage of that
		 n1 >= d1 (true due to program flow).  */
	      if (n1 > d1 || n0 >= d0)
		{
		  q0 = 1;
		  sub_ddmmss (n1, n0, n1, n0, d1, d0);
		}
	      else
		q0 = 0;

	      q1 = 0;

	      if (rp != 0)
		{
		  rr.s.low = n0;
		  rr.s.high = n1;
		  *rp = rr.ll;
		}
	    }
	  else
	    {
	      USItype m1, m0;
	      /* Normalize.  */

	      b = SI_TYPE_SIZE - bm;

	      d1 = (d1 << bm) | (d0 >> b);
	      d0 = d0 << bm;
	      n2 = n1 >> b;
	      n1 = (n1 << bm) | (n0 >> b);
	      n0 = n0 << bm;

	      udiv_qrnnd (q0, n1, n2, n1, d1);
	      umul_ppmm (m1, m0, q0, d0);

	      if (m1 > n1 || (m1 == n1 && m0 > n0))
		{
		  q0--;
		  sub_ddmmss (m1, m0, m1, m0, d1, d0);
		}

	      q1 = 0;

	      /* Remainder in (n1n0 - m1m0) >> bm.  */
	      if (rp != 0)
		{
		  sub_ddmmss (n1, n0, n1, n0, m1, m0);
		  rr.s.low = (n1 << b) | (n0 >> bm);
		  rr.s.high = n1 >> bm;
		  *rp = rr.ll;
		}
	    }
	}
    }

  ww.s.low = q0;
  ww.s.high = q1;
  return ww.ll;
}
#endif

#ifdef L_divdi3
UDItype __udivmoddi4 ();

DItype
__divdi3 (DItype u, DItype v)
{
  word_type c = 0;
  DIunion uu, vv;
  DItype w;

  uu.ll = u;
  vv.ll = v;

  if (uu.s.high < 0)
    c = ~c,
    uu.ll = __negdi2 (uu.ll);
  if (vv.s.high < 0)
    c = ~c,
    vv.ll = __negdi2 (vv.ll);

  w = __udivmoddi4 (uu.ll, vv.ll, (UDItype *) 0);
  if (c)
    w = __negdi2 (w);

  return w;
}
#endif

#ifdef L_moddi3
UDItype __udivmoddi4 ();
DItype
__moddi3 (DItype u, DItype v)
{
  word_type c = 0;
  DIunion uu, vv;
  DItype w;

  uu.ll = u;
  vv.ll = v;

  if (uu.s.high < 0)
    c = ~c,
    uu.ll = __negdi2 (uu.ll);
  if (vv.s.high < 0)
    vv.ll = __negdi2 (vv.ll);

  (void) __udivmoddi4 (uu.ll, vv.ll, &w);
  if (c)
    w = __negdi2 (w);

  return w;
}
#endif

#ifdef L_umoddi3
UDItype __udivmoddi4 ();
UDItype
__umoddi3 (UDItype u, UDItype v)
{
  UDItype w;

  (void) __udivmoddi4 (u, v, &w);

  return w;
}
#endif

#ifdef L_udivdi3
UDItype __udivmoddi4 ();
UDItype
__udivdi3 (UDItype n, UDItype d)
{
  return __udivmoddi4 (n, d, (UDItype *) 0);
}
#endif

#ifdef L_cmpdi2
word_type
__cmpdi2 (DItype a, DItype b)
{
  DIunion au, bu;

  au.ll = a, bu.ll = b;

  if (au.s.high < bu.s.high)
    return 0;
  else if (au.s.high > bu.s.high)
    return 2;
  if ((USItype) au.s.low < (USItype) bu.s.low)
    return 0;
  else if ((USItype) au.s.low > (USItype) bu.s.low)
    return 2;
  return 1;
}
#endif

#ifdef L_ucmpdi2
word_type
__ucmpdi2 (DItype a, DItype b)
{
  DIunion au, bu;

  au.ll = a, bu.ll = b;

  if ((USItype) au.s.high < (USItype) bu.s.high)
    return 0;
  else if ((USItype) au.s.high > (USItype) bu.s.high)
    return 2;
  if ((USItype) au.s.low < (USItype) bu.s.low)
    return 0;
  else if ((USItype) au.s.low > (USItype) bu.s.low)
    return 2;
  return 1;
}
#endif


/* From here on down, the routines use normal data types.  */

#define SItype bogus_type
#define USItype bogus_type
#define DItype bogus_type
#define UDItype bogus_type
#define SFtype bogus_type
#define DFtype bogus_type

#undef char
#undef short
#undef int
#undef long
#undef unsigned
#undef float
#undef double


#ifdef L__main

#include "gbl-ctors.h"
/* Some systems use __main in a way incompatible with its use in gcc, in these
   cases use the macros NAME__MAIN to give a quoted symbol and SYMBOL__MAIN to
   give the same symbol without quotes for an alternative entry point.  You
   must define both, or neither.  */
#ifndef NAME__MAIN
#define NAME__MAIN "__main"
#define SYMBOL__MAIN __main
#endif

#ifdef INIT_SECTION_ASM_OP
#undef HAS_INIT_SECTION
#define HAS_INIT_SECTION
#endif

#if !defined (HAS_INIT_SECTION) || !defined (OBJECT_FORMAT_ELF)
/* Run all the global destructors on exit from the program.  */

void
__do_global_dtors ()
{
#ifdef DO_GLOBAL_DTORS_BODY
  DO_GLOBAL_DTORS_BODY;
#else
  static func_ptr *p = __DTOR_LIST__ + 1;
  while (*p)
    {
      p++;
      (*(p-1)) ();
    }
#endif
}
#endif

#ifndef HAS_INIT_SECTION
/* Run all the global constructors on entry to the program.  */

#ifndef ON_EXIT
#define ON_EXIT(a, b)
#else
/* Make sure the exit routine is pulled in to define the globals as
   bss symbols, just in case the linker does not automatically pull
   bss definitions from the library.  */

extern int _exit_dummy_decl;
int *_exit_dummy_ref = &_exit_dummy_decl;
#endif /* ON_EXIT */

void
__do_global_ctors ()
{
  DO_GLOBAL_CTORS_BODY;
//  ON_EXIT (__do_global_dtors, 0);
}
#endif /* no HAS_INIT_SECTION */

#if !defined (HAS_INIT_SECTION) || defined (INVOKE__main)
/* Subroutine called automatically by `main'.
   Compiling a global function named `main'
   produces an automatic call to this function at the beginning.

   For many systems, this routine calls __do_global_ctors.
   For systems which support a .init section we use the .init section
   to run __do_global_ctors, so we need not do anything here.  */

void
SYMBOL__MAIN ()
{
  /* Support recursive calls to `main': run initializers just once.  */
  static int initialized;
  if (! initialized)
    {
      initialized = 1;
      __do_global_ctors ();
    }
}
#endif /* no HAS_INIT_SECTION or INVOKE__main */

#endif /* L__main */
