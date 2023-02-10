// Tools Library
//
// Copyright (C) 2004  Navel Ltd.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
// Contact information:
//  Mailing address:
//    Marios Hadjieleftheriou
//    University of California, Riverside
//    Department of Computer Science
//    Surge Building, Room 310
//    Riverside, CA 92521
//
//  Email:
//    lifeifei@cs.bu.edu, mhadji@gmail.com

#ifndef __tools_h
#define __tools_h

#include <assert.h>
#include <cmath>
//#include <iostream>
#include <string>
//#include <sstream>
//#include <fstream>
#include <queue>
#include <vector>
#include <map>
//#include <set>
#include <stack>
//#include <list>
//#include <algorithm>

//#include <sys/time.h>
//#include <sys/resource.h>
#include <unistd.h>

//#include <cstring>
//#include <limits>

#include "PointerPool.h"
#include "PoolPointer.h"

//#include "Point.h"
//#include "Region.h"
//#include "LineSegment.h"

#define EMAIL "lifeifei@cs.bu.edu, mhadji@gmail.com"

typedef unsigned char byte;

#define interface class

namespace Tools
{
	//const std::string VERSION;
	//const std::string DATE;

	enum IntervalType
	{
		IT_RIGHTOPEN = 0x0,
		IT_LEFTOPEN,
		IT_OPEN,
		IT_CLOSED
	};

	enum Level
	{
		LVL_VERY_LOW = 0x0,
		LVL_LOW,
		LVL_MEDIUM,
		LVL_HIGH,
		LVL_VERY_HIGH
	};

	enum VariantType
	{
		VT_LONG = 0x0,
		VT_BYTE,
		VT_SHORT,
		VT_FLOAT,
		VT_DOUBLE,
		VT_CHAR,
		VT_USHORT,
		VT_ULONG,
		VT_INT,
		VT_UINT,
		VT_BOOL,
		VT_PCHAR,
		VT_PVOID,
		VT_EMPTY
	};

	enum Architecture
	{
		ARCH_LITTLEENDIAN = 0x0,
		ARCH_BIGENDIAN,
		ARCH_NONIEEE
	};

	enum RandomGeneratorType
	{
		RGT_DRAND48 = 0x0,
		RGT_MERSENNE
	};

	//
	// Exceptions
	//
	class Exception
	{
	public:
		virtual std::string what() = 0;
		virtual ~Exception() {}
	};

	class IndexOutOfBoundsException : public Exception
	{
	public:
		IndexOutOfBoundsException(int i);
		virtual ~IndexOutOfBoundsException() {};
		virtual std::string what();

	private:
		std::string m_error;
	}; // IndexOutOfBoundsException

	class IllegalArgumentException : public Exception
	{
	public:
		IllegalArgumentException(std::string s);
		virtual ~IllegalArgumentException() {};
		virtual std::string what();

	private:
		std::string m_error;
	}; // IllegalArgumentException

	class IllegalStateException : public Exception
	{
	public:
		IllegalStateException(std::string s);
		virtual ~IllegalStateException() {};
		virtual std::string what();

	private:
		std::string m_error;
	}; // IllegalStateException

	class EndOfStreamException : public Exception
	{
	public:
		EndOfStreamException(std::string s);
		virtual ~EndOfStreamException() {};
		virtual std::string what();

	private:
		std::string m_error;
	}; // EndOfStreamException

	class ResourceLockedException : public Exception
	{
	public:
		ResourceLockedException(std::string s);
		virtual ~ResourceLockedException() {};
		virtual std::string what();

	private:
		std::string m_error;
	}; // ResourceLockedException

	class InvalidPageException : public Exception
	{
	public:
		InvalidPageException(long id);
		virtual ~InvalidPageException() {};
		virtual std::string what();

	private:
		std::string m_error;
	}; // InvalidPageException

	class NotSupportedException : public Exception
	{
	public:
		NotSupportedException(std::string s);
		virtual ~NotSupportedException() {};
		virtual std::string what();

	private:
		std::string m_error;
	}; // NotSupportedException

	class ParseErrorException : public Exception
	{
	public:
		ParseErrorException(std::string s);
		virtual ~ParseErrorException() {};
		virtual std::string what();

	private:
		std::string m_error;
	}; // ParseErrorException

	//
	// Interfaces
	//
	interface IInterval
	{
	public:
		virtual IInterval& operator=(const IInterval&) = 0;
		virtual double getLowerBound() const = 0;
		virtual double getUpperBound() const = 0;
		virtual void setBounds(double, double) = 0;
		virtual bool intersectsInterval(const IInterval&) const = 0;
		virtual bool intersectsInterval(IntervalType type, double start, double end) const = 0;
		virtual bool containsInterval(const IInterval&) const = 0;
		virtual IntervalType getIntervalType() const = 0;
		virtual ~IInterval() {}
	}; // IInterval

	interface IObject
	{
	public:
		virtual IObject* clone() throw (NotSupportedException) = 0;
			// return a new object that is an exact copy of this one.
			// IMPORTANT: do not return the this pointer!

		virtual ~IObject() {}
	}; // IObject

	interface ISerializable //: public virtual IObject
	{
	public:
		virtual unsigned long getByteArraySize() = 0;
			// returns the size of the required byte array.
		virtual unsigned long loadFromByteArray(byte* const data) = 0;
			// load this object using the byte array.
		virtual void storeToByteArray(unsigned long& len, byte** data) = 0;
			// store this object in the byte array.
		virtual ~ISerializable() {}
	};

	interface IComparable //: public virtual IObject
	{
	public:
		virtual bool operator<(const IComparable& o) const = 0;
		virtual bool operator>(const IComparable& o) const = 0;
		virtual bool operator==(const IComparable& o) const = 0;
		virtual ~IComparable() {}
	}; //IComparable

	interface IObjectComparator
	{
	public:
		virtual int compare(IObject* o1, IObject* o2) = 0;
		virtual ~IObjectComparator() {}
	}; // IObjectComparator

	interface IObjectStream
	{
	public:
		virtual IObject* getNext() = 0;
			// returns the next entry in the stream.

		virtual bool hasNext() = 0;
			// returns true if there are more entries in the stream.

		virtual unsigned long size() throw (NotSupportedException) = 0;
			// returns the total number of entries available in the stream.

		virtual bool rewind() throw (NotSupportedException) = 0;
			// sets the stream pointer to the first entry, if possible.

		virtual ~IObjectStream() {}
	}; // IObjectStream

	namespace Geometry
	{
		class Region;
		class Point;

		enum Quadrant
		{
			UPPERRIGHT = 0x0,
			LOWERRIGHT,
			UPPERLEFT,
			LOWERLEFT
		};

		// since all base classes are interfaces (there is no state involved) all
		// inheritance can be virtual for efficiency.
		interface IShape : public virtual ISerializable
		{
		public:
			virtual bool intersectsShape(const IShape& in) const = 0;
			virtual bool containsShape(const IShape& in) const = 0;
			virtual bool touchesShape(const IShape& in) const = 0;
			virtual void getCenter(Point& out) const = 0;
			virtual unsigned long getDimension() const = 0;
			virtual void getMBR(Region& out) const = 0;
			virtual double getArea() const = 0;
			virtual double getMinimumDistance(const IShape& in) const = 0;
			virtual ~IShape() {}
		}; // IShape

		// since all base classes are interfaces (there is no state involved) all
		// inheritance can be virtual for efficiency.
		interface ITimeShape : public virtual IShape, public virtual IInterval
		{
		public:
			virtual bool intersectsShapeInTime(const ITimeShape& in) const = 0;
			virtual bool intersectsShapeInTime(const IInterval& ivI, const ITimeShape& in) const = 0;
			virtual bool containsShapeInTime(const ITimeShape& in) const = 0;
			virtual bool containsShapeInTime(const IInterval& ivI, const ITimeShape& in) const = 0;
			virtual bool touchesShapeInTime(const ITimeShape& in) const = 0;
			virtual bool touchesShapeInTime(const IInterval& ivI, const ITimeShape& in) const = 0;
			virtual double getAreaInTime() const = 0;
			virtual double getAreaInTime(const IInterval& ivI) const = 0;
			virtual double getIntersectingAreaInTime(const ITimeShape& r) const = 0;
			virtual double getIntersectingAreaInTime(const IInterval& ivI, const ITimeShape& r) const = 0;
			virtual ~ITimeShape() {}
		}; // ITimeShape

		// since all base classes are interfaces (there is no state involved) all
		// inheritance can be virtual for efficiency.
		interface IEvolvingShape : public virtual IShape
		{
		public:
			virtual void getVMBR(Region& out) const = 0;
			virtual void getMBRAtTime(double t, Region& out) const = 0;
			virtual ~IEvolvingShape() {}
		}; // IEvolvingShape

	}

//	IObjectStream* externalSort(IObjectStream& source, unsigned long bufferSize);
//	IObjectStream* externalSort(IObjectStream& source, IObjectComparator& pComp, unsigned long bufferSize);

	class Variant
	{
	public:
		Variant();

		VariantType m_varType;

		union
		{
			long lVal;            // VT_LONG
			byte bVal;            // VT_BYTE
			short iVal;           // VT_SHORT
			float fltVal;         // VT_FLOAT
			double dblVal;        // VT_DOUBLE
			char cVal;            // VT_CHAR
			unsigned short uiVal; // VT_USHORT
			unsigned long  ulVal; // VT_ULONG
			int intVal;           // VT_INT
			unsigned int uintVal; // VT_UINT
			bool blVal;           // VT_BOOL
			char* pcVal;          // VT_PCHAR
			void* pvVal;          // VT_PVOID
		} m_val;
	}; // Variant

	class PropertySet;

	//std::ostream& operator<<(std::ostream& os, const PropertySet& p);

	class PropertySet : public ISerializable
	{
	public:
		virtual ~PropertySet() {}

		virtual unsigned long getByteArraySize();
		virtual unsigned long loadFromByteArray(byte* const data);
		virtual void storeToByteArray(unsigned long& len, byte** data);

		Variant getProperty(std::string property);
		void setProperty(std::string property, Variant& v);
		void removeProperty(std::string property);

	private:
		std::map<std::string, Variant> m_propertySet;

		//friend std::ostream& operator<<(std::ostream& os, const PropertySet& p);
	}; // PropertySet

	/*
	// does not support degenerate intervals.
	class Interval : public IInterval
		{
		public:
			Interval();
			Interval(IntervalType, double, double);
			Interval(double, double);
			Interval(const Interval&);
			virtual IInterval& operator=(const IInterval&);
			virtual Interval& operator=(const Interval&);
			virtual double getLowerBound() const;
			virtual double getUpperBound() const;
			virtual void setBounds(double, double);
			virtual bool intersectsInterval(const IInterval&) const;
			virtual bool intersectsInterval(IntervalType type, double start, double end) const;
			virtual bool containsInterval(const IInterval&) const;
			virtual IntervalType getIntervalType() const;
			virtual ~Interval() {};

			IntervalType m_type;
			double m_low;
			double m_high;
		}; // Interval

		std::ostream& operator<<(std::ostream& os, const Interval& iv);

		// this is only for 32 bit architectures.
		class Random
		{
		public:
			Random();
			Random(unsigned long seed);
			Random(unsigned long seed, RandomGeneratorType t);

			long nextUniformLong(long, long);
			double nextUniformDouble();
			double nextUniformDouble(double, double);

			// these are iterative, thus extremely slow. Use with caution.
			double nextGaussianDouble();
			double nextGaussianDouble(double, double);

			// these are iterative, thus extremely slow. Use with caution.
			long nextZipfLong(long, long, Level);
			double nextZipfDouble(double, double, Level);
			double nextZipfDouble(Level);

			bool flipCoin();
			unsigned long randomBits();

		private:
			void initMersenne(unsigned long seed);

			enum
			{
				MERS_N = 624,
				MERS_M = 397,
				MERS_R = 31,
				MERS_U = 11,
				MERS_S = 7,
				MERS_T = 15,
				MERS_L = 18,
				MERS_A = 0x9908B0DF,
				MERS_B = 0x9D2C5680,
				MERS_C = 0xEFC60000
			};

			RandomGeneratorType m_type;
			unsigned long m_mt[Tools::Random::MERS_N];
			unsigned long m_mti;
			Architecture m_architecture;
		}; // Random
		class Hash;
		std::ostream& operator<<(std::ostream& os, const Hash& h);
		class Hash
		{
		public:
			Hash();
			Hash(Tools::Random& r);
			Hash(unsigned long seed1, unsigned long seed2);

			// from 0 to max long
			unsigned long hash31(long x);

			void getSeeds(long& a, long& b);

		private:
			static const long M_31;
			static Tools::Random m_rand;
			long m_a, m_b;

			friend std::ostream& Tools::operator<<(std::ostream& os, const Tools::Hash& h);
		};


		class ResourceUsage
		{
		public:
			ResourceUsage();

			void start();
			void stop();
			void reset();

			double getTotalTime();
			double getUserTime();
			double getSystemTime();
			long getPageFaults();
			long getReadIO();
			long getWriteIO();
			long getPeakResidentMemoryUsage();
			long getTotalMemoryUsage();

		private:
			double combineTime(const struct timeval&);
			void addTimeval(struct timeval&, const struct timeval&);
			void subtractTimeval(struct timeval&, const struct timeval&, const struct timeval&);

	  		struct rusage m_tmpRU;
	  		struct timeval m_tmpTV;
	  		struct timeval m_totalTime;
	  		struct timeval m_userTime;
	  		struct timeval m_systemTime;
	  		long m_pageFaults;
	  		long m_readIO;
	  		long m_writeIO;
	  		long m_peakMemory;
	  		long m_totalMemory;
	  	};

		class CycleCounter
		{
		public:
			CycleCounter();
			void start();
			void stop();
			void reset();
			double getTotalCycles();
			double getCPUMHz();
			double getCyclesPerSecond();

		private:
			unsigned long long m_tmpCycles;
			unsigned long long m_totalCycles;
			bool m_bRunning;

			static unsigned long long rdtsc();
		}; // CycleCounter

		class System
		{
		public:
			static Architecture getArchitecture();
		}; // System

		class SharedLock
		{
		public:
	#ifdef PTHREADS
			SharedLock(pthread_rwlock_t* pLock);
			~SharedLock();

		private:
			pthread_rwlock_t* m_pLock;
	#endif
		}; // SharedLock

		class ExclusiveLock
		{
		public:
	#ifdef PTHREADS
			ExclusiveLock(pthread_rwlock_t* pLock);
			~ExclusiveLock();

		private:
			pthread_rwlock_t* m_pLock;
	#endif
		}; // ExclusiveLock

		class StringTokenizer
		{
		public:
			StringTokenizer(const std::string& s, const std::string& delimiters = " \t");
			bool hasMoreTokens();
			std::string getNextToken();
			void reset();

		private:
			unsigned long m_index;
			std::vector<std::string> m_token;
		}; // StringTokenizer

		std::string trimLeft(const std::string& source, const std::string& t = " \t");
		std::string trimRight(const std::string& source, const std::string& t = " \t");
		std::string trim(const std::string& source, const std::string& t = " \t");
		char toLower(char c);
		char toUpper(char c);
		std::string toUpperCase(const std::string& s);
		std::string toLowerCase(const std::string& s);
    */
}

#endif /* __tools_h */
