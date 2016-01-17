/////////////////////////////////////////////////////////////////////////////
///
/// @file Z2integer.h
///
/// This file contains classes and functions related to Z2integer.
///
/// Most of this file is inherited from the CHomP library:written by Pawel Pilarczyk.
/// http://chomp.rutgers.edu/Projects/Computational_Homology/OriginalCHomP/software/
/// Class Z2integer is from 'integer.h'.
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 1997-2013 by Pawel Pilarczyk.
// Copyright (C) 2015-2016 by Kota Ishibashi
//
// This file is part of the Homology Library.  This library is free software;
// you can redistribute it and/or modify it under the terms of the GNU
// General Public License as published by the Free Software Foundation;
// either version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this software; see the file "license.txt".  If not, write to the
// Free Software Foundation, Inc., 59 Temple Place - Suite 330, Boston,
// MA 02111-1307, USA.
// Started in 2015. Last revision: January 29, 2016.



#include <iostream>

// classes defined in this module (in this order):
class Z2integer;

class Z2integer{
public:
  // assignment operator(s)
  Z2integer &operator = (int n);
  // initialize the integers:
  static int initialize ();
  explicit Z2integer (int n = 0);
  // the function "delta": equal to 0 on 0,
  // equal to 1 on invertibles, otherwise > 1
  int delta (void) const;

  // a normalized number (a better representant for homology)
  Z2integer normalized () const;
  // several operators
  Z2integer operator - () const;
  Z2integer &operator += (const Z2integer &n);
  Z2integer &operator *= (const Z2integer &n);
  Z2integer operator + (const Z2integer &n) const;
  Z2integer operator * (const Z2integer &n) const;
  Z2integer operator / (const Z2integer &n) const;
  Z2integer operator % (const Z2integer &n) const;
  int operator == (const Z2integer &n) const;
  friend std::ostream &operator << (std::ostream &out, const Z2integer &n);
  friend bool operator < (const Z2integer &x, const Z2integer &y);
  friend bool operator > (const Z2integer &x, const Z2integer &y);
 protected:
  // mod2
  static const int p = 2;
  // the integer number
  signed short num;
  // various additional procedures
  static int cut_down (int n);
  static unsigned invert (unsigned n, unsigned q);
}; /* class Z2integer */

// --------------------------------------------------

 inline Z2integer::Z2integer (int n)
{
	if (p)
		num = (signed short) cut_down (n);
	else
	{
		num = (signed short) n;
		if ((long) num != (long) n)
			throw "Number out of range at initialization.";
	}
	return;
}  /* integer::integer */


inline int Z2integer::cut_down (int n)
{
	if (n >= 0)
		if (n < p)
			return n;
		else
			return (n % p);
	else
	{
		int num = p - ((-n) % p);
		if (num == p)
			return 0;
		else
			return num;
	}
} /* cut_down */


inline Z2integer &Z2integer::operator = (int n)
{
	if (p)
		num = (signed short) cut_down (n);
	else
	{
		num = (unsigned short) n;
		if ((long) num != (long) n)
			throw "Number out of range at assignment.";
	}
	return *this;
} /* Z2integer::operator = */

inline Z2integer Z2integer::operator / (const Z2integer &n) const
{
	Z2integer result;
	if (p)
		result = num * (int) invert (n. num, p);
	else
		result. num = (unsigned short) (num / n. num);
	return result;
} /* Z2integer::operator / */

inline Z2integer Z2integer::operator % (const Z2integer &n) const
{
	Z2integer result;
	if (p)
		result. num = 0;
	else
		result = num % n. num;
	return result;
} /* operator % */

inline Z2integer Z2integer::operator - () const
{
	if (p)
	{
		Z2integer negative;
		negative. num = (unsigned short) (p - num);
		return negative;
	}
	else
	{
		unsigned short result = (unsigned short) -num;
		if ((long) result + (long) num != 0)
			throw "Number out of range (unary -).";
		Z2integer intresult;
		intresult = result;
		return intresult;
	}
} /* Z2integer::operator - (unary) */

inline Z2integer &Z2integer::operator += (const Z2integer &n)
{
	if (!p)
		if (((n. num >= 0) && (num + n. num < num)) ||
			((n. num < 0) && (num + n. num > num)))
			throw "Number out of range (+).";
	num += n. num;
	if (p)
		if (num >= p)
			num -= (unsigned short) p;
	return *this;
} /* Z2integer::operator += */

inline Z2integer &Z2integer::operator *= (const Z2integer &n)
{
	if (p)
	{
		long result = (long) num * (long) n. num;
		if (result >= 0)
			num = (unsigned short) (result % p);
		else
		{
			num = (unsigned short) (p - ((-result) % p));
			if (num == p)
				num = 0;
		}
	}
	else
	{
		long result = (long) num * (long) (n. num);
		num = (unsigned short) result;
		if ((long) num != result)
			throw "Number out of range (*).";
	}
	return *this;
} /* Z2integer::operator *= */

inline Z2integer Z2integer::operator + (const Z2integer &n) const
{
	Z2integer m (n);
	m += *this;
	return m;
} /* operator + */

inline Z2integer Z2integer::operator * (const Z2integer &n) const
{
	Z2integer m (n);
	m *= *this;
	return m;
} /* operator * */

inline int Z2integer::operator == (const Z2integer &n) const
{
	return (n. num == num);
} /* operator == */

inline std::ostream &operator << (std::ostream &out, const Z2integer &n)
{
	out << (long) n. num;
	return out;
} /* operator << */

inline std::istream &operator >> (std::istream &in, Z2integer &n)
{
	long number;
	in >> number;
	if (!in)
		return in;
	n = number;
	return in;
} /* operator >> */

inline int operator != (const Z2integer &n, const Z2integer &m)
{
	return (!(n == m));
} /* operator != */

inline int operator == (const Z2integer &n, int m)
{
	Z2integer intm;
	intm = m;
	return (n == intm);
} /* operator == */

inline int operator != (const Z2integer &n, int m)
{
	return !(n == m);
} /* operator != */

inline Z2integer operator - (const Z2integer &n, const Z2integer &m)
{
	return (n + -m);
} /* operator - */

inline int Z2integer::initialize ()
{	return 2;
} /* Z2integer::initialize */

inline int Z2integer::delta (void) const
{
	if (p)
		return (num ? 1 : 0);
	else
		return ((num >= 0) ? num : -num);
} /* Z2integer::delta */

inline Z2integer Z2integer::normalized (void) const
{
	Z2integer n;
	if (num < 0)
		n. num = (signed short) (-num);
	else
		n. num = num;
	return n;
} /* Z2integer::normalized */

inline bool operator < (const Z2integer &x, const Z2integer &y)
{
	return (x. num < y. num);
} /* operator < */

inline bool operator > (const Z2integer &x, const Z2integer &y)
{
	return (x. num > y. num);
} /* operator > */
