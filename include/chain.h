/////////////////////////////////////////////////////////////////////////////
///
/// @file chain.h
///
/// This file contains classes and functions related to Z2-matrix.
///
/// Most of this file are inherited from the CHomP library :written by Pawel Pilarczyk.
/// http://chomp.rutgers.edu/Projects/Computational_Homology/OriginalCHomP/software/
/// Classe outputstream is are from 'textfile.h'.The class chain is from 'chain.h'.
/// Some functions (especially concerned with whitney class) are original or modified ones.
///
/////////////////////////////////////////////////////////////////////////////

// Copyright (C) 1997-2013 by Pawel Pilarczyk.
// Copyright (C) 2015-2016 by Kota Ishibashi
//
// This file is part of the posetHom programs.  This is free software;
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


#ifndef CHIAN_H
#define CHIAN_H

#include <iostream>
#include <ostream>
#include <fstream>
#include <cstdlib>
#include <iomanip>
#include <algorithm>
#include <array>
#include "poset_convert.h"
#define CHAINFIXED 1
class outputstream;
class chain;

// --------------------------------------------------
// ----------------- various tools ------------------
// --------------------------------------------------

/// A simple template for swapping two elements with the use of a temporary
/// variable of the same type and the assignment operator.
template <class type>
inline void swapelements (type &x, type &y)
{
	type z (x);
	x = y;
	y = z;
	return;
} /* swapelements */


// --------------------------------------------------
// ----------------- OUTPUT STREAM ------------------
// --------------------------------------------------

/// This class defines an output stream for replacing the standard 'cout'.
/// It has the additional features of flushing the output after every
/// operation, suppressing the output, or logging the output to a file.
class outputstream
{
public:
	/// The default constructor.
	outputstream (std::ostream &_out = std::cout,
		bool _show = true, bool _flush = false);

	/// The destructor.
	~outputstream ();

	/// Turns on logging to a file.
	/// If the file exists, its contents is ereased.
	void logfile (const char *filename);

	/// Turns on logging to the same file as some other stream.
	void logfile (const outputstream &other);

	/// If this variable is set to true then everything that is
	/// written to this stream is also written to 'cout'.
	/// If this variable is set to false then the screen output
	/// is suppressed, while logging can still be turned on.
	bool show;

	/// If this variable is set to true then everything that is
	/// written to this stream is also written to the log file.
	bool log;

	/// If this variable is set to true then both the output stream
	/// 'cout' and the log file are flushed after every write operation.
	/// Note that this may slow down the output significantly if the
	/// log file is located on a network drive.
	bool flush;

	/// Returns a pointer to the stream which is working as a log file.
	/// Returns 0 if no such stream is bound with this output stream.
	std::ofstream *getlogstream (void);

	/// A reference to the main output stream bound with this stream.
	std::ostream &out;

	/// Makes this stream keep the allocated file open for ever, and not
	/// close it even in the destructor. It is assumed then that the
	/// file will be closed automatically by the operating system.
	void keepforever (void);

	/// Returns the precision of the main output stream.
	int precision () const;

	/// Local mute of the stream.
	/// This class defines an object which makes the stream mute
	/// by suppressing output to both the screen and the log file
	/// and restores its setting when the object is destroyed.
	struct mute
	{
		mute (outputstream &_stream): stream (_stream),
			show (_stream. show), log (_stream. log)
			{stream. show = false; stream. log = false; return;}
		~mute () {stream. show = show;
			stream. log = log; return;}
		outputstream &stream;
		bool show;
		bool log;
	};

protected:
	/// A pointer to the log file stream.
	std::ofstream *logstream;

	/// This variable is set to true if this log file pointer was copied
	/// from another output stream, and therefore it should not be
	/// deleted by this stream.
	bool copied;

}; /* class outputstream */

// --------------------------------------------------

inline outputstream::outputstream (std::ostream &_out, bool _show,
	bool _flush): out (_out)
{
	show = _show;
	flush = _flush;
	log = false;
	logstream = NULL;
	copied = false;
	return;
} /* outputstream::outputstream */

inline void outputstream::logfile (const char *filename)
{
	// delete the previous log stream
	if (logstream && !copied)
		delete logstream;

	// make a note that this output stream is the master copy
	copied = false;

	// create a file stream to log the output to;
	// note: if "keepforever" is applied to the output stream
	// then this data structure is not released and produces
	// a small memory leak; however, the file is automatically
	// closed by the system; this may be necessary to avoid
	// a crash of the program if any other output streams
	// still want to write to a copy of this stream
	// upon program's shutdown
	logstream = new std::ofstream (filename);

	// warn if the log file could not be created
	if (!logstream || !*logstream)
	{
		out << "WARNING: Can't create '" << filename <<
			"'. Logging to a file has been turned off." <<
			std::endl;
		if (logstream)
			delete logstream;
		logstream = NULL;
	}

	// otherwise set a flag indicating that logging to a file is on
	else
		log = true;

	return;
} /* outputstream::logfile */

inline void outputstream::logfile (const outputstream &other)
{
	if (!copied && logstream)
		delete logstream;
	logstream = other. logstream;
	if (logstream)
	{
		copied = true;
		log = true;
	}
	return;
} /* outputstream::logfile */

inline outputstream::~outputstream ()
{
	if (!copied && logstream)
		delete logstream;
	return;
} /* outputstream::~outputstream */

inline std::ofstream *outputstream::getlogstream (void)
{
	return logstream;
} /* outputstream::getlogstream */

inline void outputstream::keepforever (void)
{
	copied = true;
	return;
} /* outputstream::getlogstream */

inline int outputstream::precision () const
{
	return out. precision ();
} /* outputstream::precision */

/// The operator << for writing any kind of object to the output stream.
/// This object is written using the operator << of the standard stream.
template<typename type>
inline outputstream &operator << (outputstream &out, const type &object)
{
	if (out. flush)
	{
		if (out. show)
			out. out << object << std::flush;
		if (out. log && out. getlogstream ())
			(*(out. getlogstream ())) << object << std::flush;
	}
	else
	{
		if (out. show)
			out. out << object;
		if (out. log && out. getlogstream ())
			(*(out. getlogstream ())) << object;
	}
	return out;
} /* operator << */

/// A specialization of the operator << for writing a C-style string
/// to the output stream.
inline outputstream &operator << (outputstream &out, const char *object)
{
	if (out. flush)
	{
		if (out. show)
			out. out << object << std::flush;
		if (out. log && out. getlogstream ())
			(*(out. getlogstream ())) << object << std::flush;
	}
	else
	{
		if (out. show)
			out. out << object;
		if (out. log && out. getlogstream ())
			(*(out. getlogstream ())) << object;
	}
	return out;
} /* operator << */

/// A specialization of the operator << for putting manipulators
/// (like std::endl, std::flush) to the output stream.
inline outputstream &operator << (outputstream &out,
	std::ostream& (*object)(std::ostream&))
{
	if (out. flush)
	{
		if (out. show)
			out. out << object << std::flush;
		if (out. log && out. getlogstream ())
			(*(out. getlogstream ())) << object << std::flush;
	}
	else
	{
		if (out. show)
			out. out << object;
		if (out. log && out. getlogstream ())
			(*(out. getlogstream ())) << object;
	}
	return out;
} /* operator << */

// --------------------------------------------------

#ifndef OUTPUTSTREAM
/// This symbol is defined if the "outputstream" class is available,
/// and the basic streams like "sout" are defined.
#define OUTPUTSTREAM
#endif




class chain
{
public:
	/// The default constructor.
	chain ();

	/// The copy constructor.
	chain (const chain &c);

	/// The assignment operator.
	chain &operator = (const chain &c);

	/// The destructor.
	~chain ();

	/// Returns the size of the chain, that is, the number of
	/// elements with non-zero coefficients.
	int size () const;

	/// Returns true if the chain is empty (zero), false otherwise.
	bool empty () const;
	/// Finds and returns the coefficient in front of the given element.
	/// If the identifier is negative, then returns the first nonzero
	/// coefficient or 0 if none.
	Z2integer getcoefficient (int n = -1) const;

	/// Find the position of an element with the given identifier.
	/// Returns -1 if not found.
	int findnumber (int n) const;

        bool isContainNumber(chain chain,int n);
	/// Returns the coefficient in front of the i-th element
	/// in the chain.
	Z2integer coef (int i) const;

	/// Returns the number (identifier) of the i-th element in the chain.
	int num (int i) const;

        /// Determines if the chain contains a non-invertible coefficient.
        /// Returns true if yes, false if not.
        bool contains_non_invertible () const;

	/// Finds the best element in the chain for reduction, that is,
	/// the element with minimal value of delta.
	/// IF the given table is given, then additionally an element
	/// with the shortest chain length in the table is searched for.
	/// Returns the actual number of this element in the chain
	/// (not its identifier) or -1 if the chain is empty (zero).
	int findbest (chain *table = NULL) const;

	/// Adds an element algebraically to the chain.
	chain &add (int n, Z2integer e);

	/// Removes an element with the given identifier from the chain.
	chain &remove (int n);

	/// Adds one chain to another with a given coefficient.
	/// If the chain is a row of a matrix, then its number and the
	/// table of columns must be given for proper modification.
	/// If this is a column, its number and columns must be given.
	chain &add (const chain &other,
                    Z2integer e = (Z2integer) 1, int number = -1,
		chain *table = NULL);

	/// Swaps one chain with another. If the chain is a row of a
	/// matrix, then its number, the number of the other row and the
	/// table of columns must be given for proper modification;
	/// if this is a column, its number and columns must be given

        chain &swap (chain &other,
                               int number = -1, int othernumber = -1,
                               chain *table = NULL);

	/// Takes data from another chain. Destroys the other chain.
	chain &take (chain &c);

	/// Multiplies one or all the coefficients in the chain
	/// by the given number.
	chain &multiply (Z2integer e, int number = -1);

        void showChainAsVertex(std::vector<std::vector<int>> simpleces,std::vector<chain> chainList,bool homFlag, int homDim);

        /* for making the chain which represents whitney classes without subivision. */
        bool isExistSmaller(int g,std::vector<int> sigma,std::vector<int> vertexSet,std::vector<std::array<int,2>> arrowList /* int arrowList[][2] */,int numberOfArrows);
        bool isExistBigger(int l,std::vector<int> sigma,std::vector<int> vertexSet,std::vector<std::array<int,2>> arrowList /* int arrowList[][2] */,int numberOfArrows);
        bool isExistBitween(int l,int g,std::vector<int> sigma,std::vector<int> vertexSet,std::vector<std::array<int,2>> arrowList /* int arrowList[][2] */,int numberOfArrows);
        bool isRegular(std::vector<int> sigma,std::vector<int> tau);
        chain makeWhitneyChain(int p,std::vector<std::vector<int>> simpleces);

	/// Shows the chain to the output stream. Uses a given label
	/// for indicating identifiers of elements in the chain.
	outputstream &show (outputstream &out,
		const char *label = NULL) const;

	/// Shows the chain to the standard output stream. Uses a given label
	/// for indicating identifiers of elements in the chain.
	std::ostream &show (std::ostream &out, const char *label = NULL) const;

private:
	/// The length of the list and the length of the table.
	int len;

	/// Elements of the list sorted according to the identifier.
	/// If there are very few of them, they are kept in the space
	/// normally reserved for the addresses. Otherwise, an array
	/// is allocated in the memory.
	union
	{
		struct
		{
			int *n;
			Z2integer *e;
		} t;
		struct
		{
			#if CHAINFIXED
			int n [CHAINFIXED];
			Z2integer e [CHAINFIXED];
			#else
			int *n;
			Z2integer *e;
			#endif
		} x;
	};

	/// Inserts one chain element at the given position.
	chain &insertpair (int i, int n, Z2integer e);

	/// Removes one chain element at the given position.
	chain &removepair (int i);

        /// Swaps two numbers (identifiers) in the chain.
	chain &swapnumbers (int number1, int number2);

	/// Checks if the tables have been allocated depending
	/// on the value of their length. Only tables longer than
	/// some limit are allocated.
	bool allocated () const;

}; /* class chain */


// --------------------------------------------------
/// Ignores the input characters until the end of a line, including this
/// end of the line.
inline void ignoreline (std::istream &in)
{
	in. ignore (0x7FFFFFFFl, '\n');
	return;
} /* ignoreline */

/// Ignores white characters (spaces, tabulators, CRs and LFs),
/// as well as comments from the input text file. The comment
/// begins with a semicolon and ends with the end of the line.
inline void ignorecomments (std::istream &in)
{
	int ch = in. peek ();
	while (true)
	{
		switch (ch)
		{
		case ';':
			ignoreline (in);
			ch = in. peek ();
			break;
		case ' ':
		case '\t':
		case '\r':
		case '\n':
			in. get ();
			ch = in. peek ();
			break;
		default:
			return;
		}
	}
} /* ignorecomments */

/// Returns the matching closing parenthesis for the given opening one
/// or EOF if none.
inline int closingparenthesis (int ch)
{
	switch (ch)
	{
		case '(':
			return ')';
		case '{':
			return '}';
		case '[':
			return ']';
		case '<':
			return '>';
		case '/':
			return '/';
		default:
			return EOF;
	}
} /* closingparenthesis */

/// Reads an opening parenthesis from the input file.
/// Return a corresponding closing parenthesis or EOF if none was found.
inline int readparenthesis (std::istream &in)
{
	int closing = closingparenthesis (in. peek ());
	if (closing != EOF)
		in. get ();
	return closing;
} /* readparenthesis */



inline bool chain::allocated () const
{
	if (len <= static_cast<int> (CHAINFIXED))
		return false;
//	return (sizeof (int *) < ((sizeof (int) < sizeof (Z2integer)) ?
//		sizeof (Z2integer) : sizeof (int)) * len);
	else
		return true;
} /* chain::allocated */


inline chain::chain ()
{
	len = 0;
	return;
} /* chain::chain */


inline chain::chain (const chain &c)
{
	// copy the length of the chain
	len = c. len;

	// allocate new tables if necessary and copy the data
	if (allocated ())
	{
		t. n = new int [len];
		t. e = new Z2integer [len];
		if (!t. n || !t. e)
			throw "Not enough memory to create a chain copy.";
		for (int i = 0; i < len; ++ i)
		{
			t. n [i] = c. t. n [i];
			t. e [i] = c. t. e [i];
		}
	}
	else
	{
		for (int i = 0; i < len; ++ i)
		{
			x. n [i] = c. x. n [i];
			x. e [i] = c. x. e [i];
		}
	}
	return;
} /* chain::chain */


inline chain &chain::operator =
	(const chain &c)
{
	// protect against self-assignment
	if (&c == this)
		return *this;

	// first release allocated tables if any
	if (allocated ())
	{
		delete [] t. n;
		delete [] t. e;
	}

	// copy the length of the chain
	len = c. len;

	// allocate new tables if necessary and copy the data
	if (allocated ())
	{
		t. n = new int [len];
		t. e = new Z2integer [len];
		if (!t. n || !t. e)
			throw "Not enough memory to create a chain copy =.";
		for (int i = 0; i < len; ++ i)
		{
			t. n [i] = c. t. n [i];
			t. e [i] = c. t. e [i];
		}
	}
	else
	{
		for (int i = 0; i < len; ++ i)
		{
			x. n [i] = c. x. n [i];
			x. e [i] = c. x. e [i];
		}
	}
	return *this;
} /* chain::operator = */


inline chain::~chain ()
{
	if (allocated ())
	{
		delete [] t. n;
		delete [] t. e;
	}
	return;
} /* chain::~chain */


inline int chain::size () const
{
	return len;
} /* chain::size */


inline bool chain::empty () const
{
	return !len;
} /* chain::empty */


/*inline*/ Z2integer chain::getcoefficient (int n) const
{
	bool a = allocated ();
	const Z2integer *tetab = a ? t. e : x. e;
	if (n < 0)
	{
		if (len > 0)
			return tetab [0];
		else
		{
			Z2integer zero;
			zero = 0;
			return zero;
		}
	}

	const int *tntab = a ? t. n : x. n;
	int i = 0;
	while ((i < len) && (tntab [i] < n))
		++ i;
	if ((i >= len) || (tntab [i] != n))
	{
		Z2integer zero;
		zero = 0;
		return zero;
	}
	return tetab [i];
} /* chain::getcoefficient */

inline int chain::findnumber (int n) const
{
	bool a = allocated ();
	const int *tntab = a ? t. n : x. n;
	for (int i = 0; i < len; ++ i)
	{
		if (tntab [i] == n)
			return i;
		else if (tntab [i] > n)
			return -1;
	}
	return -1;
} /* chain::findnumber */

inline bool isContainNumber(chain chain,int n){
  for (int i = 0; i < chain.size(); ++i)
    {
      if (chain.num(i)==n-1) return true;
    }
  return false;
}

inline Z2integer chain::coef (int i) const
{
	if (i >= len)
		throw "Wrong coefficient requested from a chain.";
	return (allocated () ? t. e : x. e) [i];
} /* chain::coef */


inline int chain::num (int i) const
{
	if (i >= len)
		throw "Wrong number requested from a chain.";
	return (allocated () ? t. n : x. n) [i];
} /* chain::num */


inline bool chain::contains_non_invertible () const
{
	if (allocated ())
	{
		for (int i = 0; i < len; ++ i)
		{
			if (t. e [i]. delta () > 1)
				return true;
		}
	}
	else
	{
		for (int i = 0; i < len; ++ i)
		{
			if (x. e [i]. delta () > 1)
				return true;
		}
	}
	return false;
} /* chain::contains_non_invertible */


inline int chain::findbest (chain *table) const
{
	// if the chain is so short that the answer is obvious, return it
	if (len <= 1)
		return (len - 1);

	// find the number which has the smallest delta function value
	int this_delta, best_delta = -1;
	int best_i = 0;

	// go through the whole table
	bool a = allocated ();
	const int *tntab = a ? t. n : x. n;
	const Z2integer *tetab = a ? t. e : x. e;
	int i;
	for (i = 0; i < len; ++ i)
	{
		// compute the value of the function delta
		this_delta = tetab [i]. delta ();

		// if the value is the smallest possible
		// and no further analysis was required, finish here
		if (!table && (this_delta == 1))
			return i;

		// if this delta is better, remember it
		if (!i || (this_delta < best_delta))
		{
			best_delta = this_delta;
			best_i = i;
		}
	}

	// if no further analysis is required, return the result just now
	if (!table)
		return best_i;

	// analyse which element has the shortest corresponding chain
	int this_length, best_length =
		table [tntab [best_i]]. size ();
	for (i = best_i + 1; i < len; ++ i)
	{
		if (tetab [i]. delta () == best_delta)
		{
			this_length =
				table [tntab [i]]. size ();
			if (best_length > this_length)
			{
				best_length = this_length;
				best_i = i;
			}
		}
	}

	return best_i;
} /* chain::findbest */

// --------------------------------------------------


inline chain &chain::insertpair
	(int i, int n, Z2integer e)
{
	// remember if the table was previously allocated or not
	bool a = allocated ();

	// increase the length
	++ len;

	// determine if the new table should be allocated or not
	bool na = allocated ();

	// if a new table has to be allocated, do it
	if (na)
	{
		// allocate a new table
		int *newntab = new int [len];
		Z2integer *newetab = new Z2integer [len];
		if (!newntab || !newetab)
			throw "Cannot add an element to a chain.";

		// determine the addresses of the old tables
		int *oldntab = a ? t. n : x. n;
		Z2integer *oldetab = a ? t. e : x. e;

		// copy the old data and insert the new pair
		int j;
		for (j = 0; j < i; ++ j)
		{
			newntab [j] = oldntab [j];
			newetab [j] = oldetab [j];
		}
		newntab [i] = n;
		newetab [i] = e;
		for (j = i + 1; j < len; ++ j)
		{
			newntab [j] = oldntab [j - 1];
			newetab [j] = oldetab [j - 1];
		}

		// release the previous tables if they were allocated
		if (a)
		{
			delete [] t. n;
			delete [] t. e;
		}

		// take the new tables to the data structure
		t. n = newntab;
		t. e = newetab;
	}

	// otherwise just insert the new element at the appropriate position
	else // if (!na && !a)
	{
		for (int j = len - 1; j > i; -- j)
		{
			x. n [j] = x. n [j - 1];
			x. e [j] = x. e [j - 1];
		}
		x. n [i] = n;
		x. e [i] = e;
	}

	return *this;
} /* chain::insertpair */


inline chain &chain::removepair (int i)
{
	// remember if the table was previously allocated or not
	bool a = allocated ();

	// decrease the length
	if (len)
		-- len;

	// determine if the new table should be allocated or not
	bool na = allocated ();

	// allocate the new tables if necessary
	if (na)
	{
		int *newntab = new int [len];
		Z2integer *newetab = new Z2integer [len];
		if (!newntab || !newetab)
			throw "Cannot remove a pair from a chain.";

		// copy the data form the previous tables
		int j;
		for (j = 0; j < i; ++ j)
		{
			newntab [j] = t. n [j];
			newetab [j] = t. e [j];
		}
		for (j = i; j < len; ++ j)
		{
			newntab [j] = t. n [j + 1];
			newetab [j] = t. e [j + 1];
		}
		delete [] t. n;
		delete [] t. e;
		t. n = newntab;
		t. e = newetab;
	}

	// otherwise, copy the data from the previous tables
	else
	{
		int *oldntab = a ? t. n : x. n;
		Z2integer *oldetab = a ? t. e : x. e;

		// copy the data form the previous tables
		int j;
		for (j = 0; a && (j < i); ++ j)
		{
			x. n [j] = oldntab [j];
			x. e [j] = oldetab [j];
		}
		for (j = i; j < len; ++ j)
		{
			x. n [j] = oldntab [j + 1];
			x. e [j] = oldetab [j + 1];
		}

		// release the old tables if necessary
		if (a)
		{
			delete [] oldntab;
			delete [] oldetab;
		}
	}

	return *this;
} /* chain::removepair */


// --------------------------------------------------

inline chain &chain::swapnumbers (int number1,
	int number2)
{
	// if the numbers are the same, do nothing
	if (number1 == number2)
		return *this;

	// force the first number be less than the second number
	if (number1 > number2)
		std::swap (number1, number2);

	// determine the true tables to be processed
	bool a = allocated ();
	int *tntab = a ? t. n : x. n;
	Z2integer *tetab = a ? t. e : x. e;

	// find both numbers or the positions they should be at
	int i1 = 0, i2 = 0;
	while ((i1 < len) && (tntab [i1] < number1))
		++ i1;
	while ((i2 < len) && (tntab [i2] < number2))
		++ i2;

	// if the first number was found...
	if ((i1 < len) && (tntab [i1] == number1))
	{
		// if both numbers were found, exchange their coefficients
		if ((i2 < len) && (tntab [i2] == number2))
			swapelements (tetab [i1], tetab [i2]);
		// if only the first was found, move it to the new position
		else
		{
			Z2integer temp = tetab [i1];
			for (int i = i1 + 1; i < i2; ++ i)
			{
				tntab [i - 1] = tntab [i];
				tetab [i - 1] = tetab [i];
			}
			tntab [i2 - 1] = number2;
			tetab [i2 - 1] = temp;
		}
	}

	// otherwise if the second number only was found, move it to its pos.
	else if ((i2 < len) && (tntab [i2] == number2))
	{
		Z2integer temp = tetab [i2];
		for (int i = i2; i > i1; -- i)
		{
			tntab [i] = tntab [i - 1];
			tetab [i] = tetab [i - 1];
		}
		tntab [i1] = number1;
		tetab [i1] = temp;
	}

	return *this;
} /* chain::swapnumbers */




inline chain &chain::add (int n, Z2integer e = (Z2integer)1)
{
	// if the coefficient is zero, ignore the pair
	if (e == 0)
		return *this;
	bool a = allocated ();
	int *tntab = a ? t. n : x. n;
	Z2integer *tetab = a ? t. e : x. e;

	// find the position in the table for adding this pair
	int i = 0;
	while ((i < len) && (tntab [i] < n))
		++ i;

	// if an element with this identifier was found, add the coefficients
	if ((i < len) && (tntab [i] == n))
	{
		// add the coefficient
		tetab [i] += e;

		// if the coefficient became zero, remove this pair
		if (tetab [i] == 0)
			return removepair (i);

		// otherwise we are done
		else
			return *this;
	}

	// otherwise insert this pair into the chain
	return insertpair (i, n, e);

} /* chain::add */

inline chain &chain::swap (chain &other,
	int number, int othernumber, chain *table)
{
	// check which chains where allocated
	bool a = allocated ();
	bool oa = other. allocated ();

	// swap the data of the chains
	if (a && oa)
	{
		swapelements (t. n, other. t. n);
		swapelements (t. e, other. t. e);
	}
	else if (!a && !oa)
	{
		// common variable for interations (required by MSVC++)
		int i;

		// swap the data in the common area of both chains
		for (i = 0; (i < len) && (i < other. len); ++ i)
		{
			swapelements (x. n [i], other. x. n [i]);
			swapelements (x. e [i], other. x. e [i]);
		}

		// copy the remaining portion of the data
		for (i = len; i < other. len; ++ i)
		{
			x. n [i] = other. x. n [i];
			x. e [i] = other. x. e [i];
		}
		for (i = other. len; i < len; ++ i)
		{
			other. x. n [i] = x. n [i];
			other. x. e [i] = x. e [i];
		}
	}
	else if (a) // && !oa
	{
		int *tempn = t. n;
		Z2integer *tempe = t. e;
		for (int i = 0; i < other. len; ++ i)
		{
			x. n [i] = other. x. n [i];
			x. e [i] = other. x. e [i];
		}
		other. t. n = tempn;
		other. t. e = tempe;
	}
	else // if (oa) // && !a
	{
		int *tempn = other. t. n;
		Z2integer *tempe = other. t. e;
		for (int i = 0; i < len; ++ i)
		{
			other. x. n [i] = x. n [i];
			other. x. e [i] = x. e [i];
		}
		t. n = tempn;
		t. e = tempe;
	}

	// swap the lengths of the chains (do not swap 'a' with 'oa')
	swapelements (len, other. len);

	if (!table)
		return *this;

	// change the numbers in every relevant entry of the table
	int *tntab = oa ? t. n : x. n;
	int *ontab = a ? other. t. n : other. x. n;
	int i = 0, j = 0;
	while ((i < len) || (j < other. len))
	{
		// determine which table entry should be modified
		int n;
		if (i >= len)
			n = ontab [j ++];
		else if (j >= other. len)
			n = tntab [i ++];
		else if (tntab [i] < ontab [j])
			n = tntab [i ++];
		else if (ontab [j] < tntab [i])
			n = ontab [j ++];
		else
		{
			n = tntab [i ++];
			++ j;
		//	++ i;
		//	++ j;
		//	continue;
		}

		// swap numbers in that table entry
		table [n]. swapnumbers (othernumber, number);
	}

	return *this;
} /* chain::swap */

chain &chain::remove (int n)
{
	bool a = allocated ();
	int *tntab = a ? t. n : x. n;

	// find the element of the chain to be removed
	int i = 0;
	while ((i < len) && (tntab [i] < n))
		++ i;

	// if found, then remove it
	if ((i < len) && (tntab [i] == n))
		return removepair (i);

	return *this;
} /* chain::remove */


inline chain &chain::add (const chain &other,
	Z2integer e, int number, chain *table)
{
	// if the coefficient is zero or the other chain is zero,
	// then there is nothing to do
	if ((e == 0) || !other. len)
		return *this;

	// prepare big tables for the new chain
	int tablen = len + other. len;
	int *bigntab = new int [tablen];
	Z2integer *bigetab = new Z2integer [tablen];
	if (!bigntab || !bigetab)
		throw "Not enough memory to add chains.";

	// prepare the counters of elements of the two input chains
	// and of the output chain
	int i = 0, j = 0, k = 0;

	// determine the actual tables to be processed
	bool a = allocated ();
	bool oa = other. allocated ();
	const int *tntab = a ? t. n : x. n;
	const Z2integer *tetab = a ? t. e : x. e;
	const int *ontab = oa ? other. t. n : other. x. n;
	const Z2integer *oetab = oa ? other. t. e : other. x. e;

	// go through both input chains and compute the output chain
	while ((i < len) || (j < other. len))
	{
		if (i >= len)
		{
			bigntab [k] = ontab [j];
			bigetab [k] = e * oetab [j ++];
			if (table)
			{
				table [bigntab [k]]. add (number,
					bigetab [k]);
			}
			++ k;
		}
		else if ((j >= other. len) || (tntab [i] < ontab [j]))
		{
			bigntab [k] = tntab [i];
			bigetab [k ++] = tetab [i ++];
		}
		else if (tntab [i] > ontab [j])
		{
			bigntab [k] = ontab [j];
			bigetab [k] = e * oetab [j ++];
			if (table)
			{
				table [bigntab [k]]. add (number,
					bigetab [k]);
			}
			++ k;
		}
		else // if (tntab [i] == ontab [j])
		{
			bigntab [k] = tntab [i];
			Z2integer addelem = e * oetab [j ++];
			bigetab [k] = tetab [i ++] + addelem;
			Z2integer zero;
			zero = 0;
			if (bigetab [k] != zero)
			{
				if (table)
				{
					table [bigntab [k]]. add (number,
						addelem);
				}
				++ k;
			}
			else if (table)
			{
				table [bigntab [k]]. remove (number);
			}
		}
	}

	// release the old tables if they are useless now
	if (a && ((k != len) || (k == tablen)))
	{
		delete [] t. n;
		delete [] t. e;
	}

	// use the previous tables and release the big table if beneficial
	if (a && (k == len) && (k != tablen))
	{
		for (int i = 0; i < len; ++ i)
		{
			t. n [i] = bigntab [i];
			t. e [i] = bigetab [i];
		}
		delete [] bigntab;
		delete [] bigetab;
		return *this;
	}

	len = k;

	// if the new tables don't have to be allocated, only copy the data
	if (!allocated ())
	{
		for (int i = 0; i < len; ++ i)
		{
			x. n [i] = bigntab [i];
			x. e [i] = bigetab [i];
		}
		delete [] bigntab;
		delete [] bigetab;
		return *this;
	}

	// if the big tables cannot be used, allocate new tables
	if (len != tablen)
	{
		t. n = new int [len];
		t. e = new Z2integer [len];
		if (!t. n || !t. e)
			throw "Cannot shorten a sum of chains.";
		for (int i = 0; i < len; ++ i)
		{
			t. n [i] = bigntab [i];
			t. e [i] = bigetab [i];
		}
		delete [] bigntab;
		delete [] bigetab;
	}

	// otherwise, simply use the big tables
	else
	{
		t. n = bigntab;
		t. e = bigetab;
	}

	return *this;
} /* chain::add */




inline chain &chain::take (chain &c)
{
	// release the current tables if they were allocated
	if (allocated ())
	{
		delete [] t. n;
		delete [] t. e;
	}

	// if the other tables were allocated, take them
	if (c. allocated ())
	{
		t. n = c. t. n;
		t. e = c. t. e;
	}

	// otherwise copy the data from the internal other tables
	else
	{
		for (int i = 0; i < c. len; ++ i)
		{
			x. n [i] = c. x. n [i];
			x. e [i] = c. x. e [i];
		}
	}

	// copy the length and reset the other length
	len = c. len;
	c. len = 0;

	return *this;
} /* chain::take */


inline chain &chain::multiply (Z2integer e, int number)
{
	// check if the tables have been allocated or not
	bool a = allocated ();
	int *tntab = a ? t. n : x. n;
	Z2integer *tetab = a ? t. e : x. e;

	// if there is only one element to be multiplied, find it and do it
	if (number >= 0)
	{
		for (int i = 0; i < len; ++ i)
		{
			if (tntab [i] == number)
			{
				if (e == 0)
					removepair (i);
				else
				{
					tetab [i] *= e;
				//	if (tetab [i] == 0)
				//		removepair (i);
				}
				return *this;
			}
		}
	}

	// if the entire chain has to be multiplied by a non-zero number...
	else if (e != 0)
	{
		for (int i = 0; i < len; ++ i)
		{
			tetab [i] *= e;
			if (tetab [i] == 0)
				removepair (i);
		}
	}

	// otherwise, if the chain has to be made zero, clean it
	else
	{
		if (a)
		{
			delete [] t. n;
			delete [] t. e;
		}
		len = 0;
	}

	return *this;
} /* chain::multiply */


inline void showChainAsVertex(std::vector<std::vector<int>> simpleces,std::vector<chain> chainList, bool homFlag = false,int homDim = -1){
  if(chainList.empty()){
    std::cout <<"None"<< std::endl;
    std::cout << std::endl;
  } else {
    for (int k = 0 ; k < chainList.size(); ++k)
      {
        chain z = chainList[k];
        int size = z.size();
        if (homFlag && homDim != -1)
          {
            std::cout << "g" << homDim << "_" << k+1 << " = " << std::flush;
          }
        for (int i = 0; i < size -1; ++i)
          {
            if (homFlag && homDim != -1 && i != 0)
              {
                std::cout << std::setw(8 + (k+1) / 10 + 1 + (homDim/10) + 1 ) << std::setfill(' ') << std::right << std::flush;
              }
            std::cout <<"1*{"<< std::flush;
            for (int j = 0 ; j < simpleces[z.num(i)-1].size()-1; ++j)
              {
                std::cout << simpleces[z.num(i)-1][j] <<"," << std::flush;
              }
            std::cout <<  (simpleces[z.num(i)-1])[simpleces[z.num(i)-1].size()-1] << "}+"<< std::endl;
          }
        if (homFlag && homDim != -1 && size != 1)
          {
            std::cout << std::setw(8 + (k+1) / 10 + 1 + (homDim/10) + 1 ) << std::setfill(' ') << std::right << std::flush;
          }
        std::cout  <<"1*{"<< std::flush;
        for (int j = 0; j < simpleces[z.num(size - 1)-1].size()-1; ++j)
          {
            std::cout << simpleces[z.num(size - 1)-1][j] <<"," << std::flush;
          }
        std::cout << (simpleces[z.num(size - 1)-1])[simpleces[z.num(size - 1)-1].size()-1] << "}"<< std::endl;
        std::cout << std::endl;
      }
  }
  /* std::cout << std::endl; */
}

bool isExistSmaller(int g,std::vector<int> sigma,std::vector<int> vertexSet,std::vector<std::array<int,2>> arrowList /* int arrowList[][2] */,int numberOfArrows)
{
  for (auto x:sigma)
    {
      /* x < g */
      if (greater(x,g,vertexSet,arrowList,numberOfArrows))
        {
          return true;
        }
    }
  return false;
}

bool isExistBigger(int l,std::vector<int> sigma,std::vector<int> vertexSet,std::vector<std::array<int,2>> arrowList /* int arrowList[][2] */,int numberOfArrows)
{
  for (auto x:sigma)
    {
      /* x > l */
      if (greater(l,x,vertexSet,arrowList,numberOfArrows))
        {
          return true;
        }
    }
  return false;
}

bool isExistBitween(int l,int g,std::vector<int> sigma,std::vector<int> vertexSet, std::vector<std::array<int,2>> arrowList/* int arrowList[][2] */,int numberOfArrows)
{
  for (auto x:sigma)
    {
      /* x < g && l < x */
      if ((greater(x,g,vertexSet,arrowList,numberOfArrows) && greater(l,x,vertexSet,arrowList,numberOfArrows)))
        {
          return true;
        }
    }
  return false;
}

bool isRegular(std::vector<int> sigma,std::vector<int> tau,std::vector<int> vertexSet,std::vector<std::array<int,2>> arrowList /* int arrowList[][2] */,int numberOfArrows)
{
  if (!isSubset(tau,sigma)) return false;
  /*if number of tau's int which is less than sigma[0] is nonzero return false*/
  if (isExistSmaller(sigma[0],tau,vertexSet,arrowList,numberOfArrows))
    {
      return false;
    }
  for (int i = 1; i < sigma.size()-1; i += 2)
    {
      /*if number of tau's int which is between than sigma[i] and sigma[i+1] is nonzero return false*/
      if(isExistBitween(sigma[i],sigma[i+1],tau,vertexSet,arrowList,numberOfArrows)){
        return false;
      }
    }
  if (sigma.size() % 2 == 0)
    {
      if (isExistBigger(sigma[sigma.size()-1],tau,vertexSet,arrowList,numberOfArrows))
        {
          return false;
        }
    }
  return true;
}


inline chain makeWhitneyChain(int p,std::vector<std::vector<int>> simpleces,std::vector<int> vertexSet,std::vector<std::array<int,2>> arrowList /* int arrowList[][2] */,int numberOfArrows)
{
  chain res = chain();
  /*次元が一致する simplex (sigma)についてのループ*/
  for (int k = beginNum(p+1,simpleces) ; simpleces[k].size() == p+1; ++k)
    {
      /*上で固定した次元以上のsimplex (tau) にわたるループ*/
      for (int l = beginNum(p+1,simpleces); l < simpleces.size(); ++l)
        {
          /*sigma が tau で正則 なら 生成元なのでindexに追加（あとでmatrixで追加する都合+1）*/
          if (isRegular(simpleces[k],simpleces[l],vertexSet,arrowList,numberOfArrows))
            {
              res.add(k+1,(Z2integer) 1);
            }
        }
    }
  return res;
}



inline outputstream &chain::show (outputstream &out,
	const char *label) const
{
	if (len <= 0)
		out << "0";
	bool a = allocated ();
	const int *tntab = a ? t. n : x. n;
	const Z2integer *tetab = a ? t. e : x. e;
	for (int i = 0; i < len; ++ i)
	{
		Z2integer e = tetab [i];
		int n = tntab [i] + 1;

		if (e == 1)
			out << (i ? " + " : "") <<
				(label ? label : "") << n;
		else if (-e == 1)
			out << (i ? " - " : "-") <<
				(label ? label : "") << n;
		else
			out << (i ? " + " : "") << e << " * " <<
				(label ? label : "") << n;
	}
	return out;
} /* chain::show */

inline std::ostream &chain::show (std::ostream &out, const char *label) const
{
	outputstream tout (out);
	show (tout, label);
	return out;
} /* chain::show */

// --------------------------------------------------

/// Outputs the given chain to a standard output stream in the text mode.
/// Warning: The operators >> and << are not symmetric for chains.

inline std::ostream &operator << (std::ostream &out, const chain &c)
{
	c. show (out);
	return out;
} /* operator << */

/// Reads a chain from a standard input stream in the text mode.
/// Warning: The operators >> and << are not symmetric for chains.

inline std::istream &operator >> (std::istream &in, chain &c)
{
	ignorecomments (in);
	int closing = readparenthesis (in);

	ignorecomments (in);
	while (in. peek () != closing)
	{
		// read the coefficient
		Z2integer e (1);
		in >> e;

		// read the multiplication symbol
		ignorecomments (in);
		if (in. peek () != '*')
			throw "The multiplication sign '*' while reading a chain.";
		in. get ();

		// read the identifier
		ignorecomments (in);
		int n;
		in >> n;
		-- n;

		// if the coefficient is zero, then this is wrong
		if (e == 0)
			throw "A zero coefficient in a chain detected.";

		// add this element to the chain
		c. add (n, e);

		// prepare for the next pair to read
		ignorecomments (in);

		// read a comma or a plus between two elements of the chain
		if ((in. peek () == ',') || (in. peek () == '+'))
		{
			in. get ();
			ignorecomments (in);
		}
	}

	if (closing != EOF)
		in. get ();

	return in;
} /* operator >> */

#endif
