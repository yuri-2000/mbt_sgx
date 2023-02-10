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

#include "Tools.h"
//#include "ExternalSort.h"
#include <cmath>

//const std::string VERSION = "0.3b";
//const std::string DATE = "06 June 2004";

Tools::IndexOutOfBoundsException::IndexOutOfBoundsException(int i)
{
//	std::ostringstream s;
//	s << "Invalid index " << i;
//	m_error = s.str();
	m_error = "Invalid index!";
}

std::string Tools::IndexOutOfBoundsException::what()
{
	return "IndexOutOfBoundsException: " + m_error;
}

Tools::IllegalArgumentException::IllegalArgumentException(std::string s) : m_error(s)
{
}

std::string Tools::IllegalArgumentException::what()
{
	return "IllegalArgumentException: " + m_error;
}

Tools::IllegalStateException::IllegalStateException(std::string s) : m_error(s)
{
}

std::string Tools::IllegalStateException::what()
{
	return "IllegalStateException: " + m_error + "\nPlease contact " + EMAIL;
}

Tools::EndOfStreamException::EndOfStreamException(std::string s) : m_error(s)
{
}

std::string Tools::EndOfStreamException::what()
{
	return "EndOfStreamException: " + m_error;
}

Tools::ResourceLockedException::ResourceLockedException(std::string s) : m_error(s)
{
}

std::string Tools::ResourceLockedException::what()
{
	return "ResourceLockedException: " + m_error;
}

Tools::InvalidPageException::InvalidPageException(long id)
{
//	std::ostringstream s;
//	s << "Unknown page id " << id;
//	m_error = s.str();
	m_error = "Unknown page id!";
}

std::string Tools::InvalidPageException::what()
{
	return "InvalidPageException: " + m_error;
}

Tools::NotSupportedException::NotSupportedException(std::string s) : m_error(s)
{
}

std::string Tools::NotSupportedException::what()
{
	return "NotSupportedException: " + m_error;
}

Tools::ParseErrorException::ParseErrorException(std::string s) : m_error(s)
{
}

std::string Tools::ParseErrorException::what()
{
	return "ParseErrorException: " + m_error;
}

Tools::Variant::Variant() : m_varType(VT_EMPTY)
{
}

unsigned long Tools::PropertySet::getByteArraySize()
{
	unsigned long size = sizeof(unsigned long);

	for (std::map<std::string, Variant>::iterator it = m_propertySet.begin(); it != m_propertySet.end(); it++)
	{
		switch ((*it).second.m_varType)
		{
		case VT_LONG:
			size += sizeof(long);
			break;
		case VT_BYTE:
			size += sizeof(byte);
			break;
		case VT_SHORT:
			size += sizeof(short);
			break;
		case VT_FLOAT:
			size += sizeof(float);
			break;
		case VT_DOUBLE:
			size += sizeof(double);
			break;
		case VT_CHAR:
			size += sizeof(char);
			break;
		case VT_USHORT:
			size += sizeof(unsigned short);
			break;
		case VT_ULONG:
			size += sizeof(unsigned long);
			break;
		case VT_INT:
			size += sizeof(int);
			break;
		case VT_UINT:
			size += sizeof(unsigned int);
			break;
		case VT_BOOL:
			size += sizeof(byte);
			break;
		default:
			throw NotSupportedException("Cannot serialize a variant of this type.");
		}
		size += (*it).first.size() + sizeof(unsigned long) + sizeof(VariantType);
	}

	return size;
}

unsigned long Tools::PropertySet::loadFromByteArray(byte* const data)
{
	byte* ptr = data;
	unsigned long numberOfProperties;
	memcpy(&numberOfProperties, ptr, sizeof(unsigned long));
	ptr += sizeof(unsigned long);

	unsigned long strSize;
	char* str;
	Variant v;

	for (unsigned long cIndex = 0; cIndex < numberOfProperties; cIndex++)
	{
		memcpy(&strSize, ptr, sizeof(unsigned long));
		ptr += sizeof(unsigned long);
		str = new char[strSize + 1]; str[strSize] = 0;
		memcpy(str, ptr, strSize);
		ptr += strSize;
		memcpy(&(v.m_varType), ptr, sizeof(VariantType));
		ptr += sizeof(VariantType);

		switch (v.m_varType)
		{
		case VT_LONG:
			long l;
			memcpy(&l, ptr, sizeof(long));
			ptr += sizeof(long);
			v.m_val.lVal = l;
			break;
		case VT_BYTE:
			byte b;
			memcpy(&b, ptr, sizeof(byte));
			ptr += sizeof(byte);
			v.m_val.bVal = b;
			break;
		case VT_SHORT:
			short s;
			memcpy(&s, ptr, sizeof(short));
			ptr += sizeof(short);
			v.m_val.iVal = s;
			break;
		case VT_FLOAT:
			float f;
			memcpy(&f, ptr, sizeof(float));
			ptr += sizeof(float);
			v.m_val.fltVal = f;
			break;
		case VT_DOUBLE:
			double d;
			memcpy(&d, ptr, sizeof(double));
			ptr += sizeof(double);
			v.m_val.dblVal = d;
			break;
		case VT_CHAR:
			char c;
			memcpy(&c, ptr, sizeof(char));
			ptr += sizeof(char);
			v.m_val.cVal = c;
			break;
		case VT_USHORT:
			unsigned short us;
			memcpy(&us, ptr, sizeof(unsigned short));
			ptr += sizeof(unsigned short);
			v.m_val.uiVal = us;
			break;
		case VT_ULONG:
			unsigned long ul;
			memcpy(&ul, ptr, sizeof(unsigned long));
			ptr += sizeof(unsigned long);
			v.m_val.ulVal = ul;
			break;
		case VT_INT:
			int i;
			memcpy(&i, ptr, sizeof(int));
			ptr += sizeof(int);
			v.m_val.intVal = i;
			break;
		case VT_UINT:
			unsigned int ui;
			memcpy(&ui, ptr, sizeof(unsigned int));
			ptr += sizeof(unsigned int);
			v.m_val.uintVal = ui;
			break;
		case VT_BOOL:
			byte bl;
			memcpy(&bl, ptr, sizeof(byte));
			ptr += sizeof(byte);
			v.m_val.blVal = static_cast<bool>(bl);
			break;
		default:
			throw NotSupportedException("Cannot serialize a variant of this type.");
		}
		m_propertySet.insert(std::pair<std::string, Variant>(std::string(str), v));
		delete[] str;
	}

	return getByteArraySize();
}

void Tools::PropertySet::storeToByteArray(unsigned long& len, byte** data)
{
	len = getByteArraySize();
	*data = new byte[len];

	byte* ptr = *data;

	unsigned long numberOfProperties = m_propertySet.size();
	memcpy(ptr, &numberOfProperties, sizeof(unsigned long));
	ptr += sizeof(unsigned long);

	for (std::map<std::string, Variant>::iterator it = m_propertySet.begin(); it != m_propertySet.end(); it++)
	{
		unsigned long strSize = (*it).first.size();
		memcpy(ptr, &strSize, sizeof(unsigned long));
		ptr += sizeof(unsigned long);
		memcpy(ptr, (*it).first.c_str(), strSize);
		ptr += strSize;
		memcpy(ptr, &((*it).second.m_varType), sizeof(VariantType));
		ptr += sizeof(VariantType);

		switch ((*it).second.m_varType)
		{
		case VT_LONG:
			memcpy(ptr, &((*it).second.m_val.lVal), sizeof(long));
			ptr += sizeof(long);
			break;
		case VT_BYTE:
			memcpy(ptr, &((*it).second.m_val.bVal), sizeof(byte));
			ptr += sizeof(byte);
			break;
		case VT_SHORT:
			memcpy(ptr, &((*it).second.m_val.iVal), sizeof(short));
			ptr += sizeof(short);
			break;
		case VT_FLOAT:
			memcpy(ptr, &((*it).second.m_val.fltVal), sizeof(float));
			ptr += sizeof(float);
			break;
		case VT_DOUBLE:
			memcpy(ptr, &((*it).second.m_val.dblVal), sizeof(double));
			ptr += sizeof(double);
			break;
		case VT_CHAR:
			memcpy(ptr, &((*it).second.m_val.cVal), sizeof(char));
			ptr += sizeof(char);
			break;
		case VT_USHORT:
			memcpy(ptr, &((*it).second.m_val.uiVal), sizeof(unsigned short));
			ptr += sizeof(unsigned short);
			break;
		case VT_ULONG:
			memcpy(ptr, &((*it).second.m_val.ulVal), sizeof(unsigned long));
			ptr += sizeof(unsigned long);
			break;
		case VT_INT:
			memcpy(ptr, &((*it).second.m_val.intVal), sizeof(int));
			ptr += sizeof(int);
			break;
		case VT_UINT:
			memcpy(ptr, &((*it).second.m_val.uintVal), sizeof(unsigned int));
			ptr += sizeof(unsigned int);
			break;
		case VT_BOOL:
			byte bl;
			bl = (*it).second.m_val.blVal;
			memcpy(ptr, &bl, sizeof(byte));
			ptr += sizeof(byte);
			break;
		default:
			throw NotSupportedException("Cannot serialize a variant of this type.");
		}
	}
}

Tools::Variant Tools::PropertySet::getProperty(std::string property)
{
	std::map<std::string, Variant>::iterator it = m_propertySet.find(property);
	if (it != m_propertySet.end()) return (*it).second;
	else return Variant();
}

void Tools::PropertySet::setProperty(std::string property, Variant& v)
{
	m_propertySet.insert(std::pair<std::string, Variant>(property, v));
}

void Tools::PropertySet::removeProperty(std::string property)
{
	std::map<std::string, Variant>::iterator it = m_propertySet.find(property);
	if (it != m_propertySet.end()) m_propertySet.erase(it);
}

/*
Tools::IObjectStream* Tools::externalSort(IObjectStream& source, unsigned long bufferSize)
{
	return new ExternalSort(source, bufferSize);

}

Tools::IObjectStream* Tools::externalSort(IObjectStream& source, IObjectComparator& comp, unsigned long bufferSize)
{
	return new ExternalSort(source, comp, bufferSize);
}

Tools::Interval::Interval() : m_type(IT_RIGHTOPEN), m_low(0.0), m_high(0.0)
{
}

Tools::Interval::Interval(IntervalType t, double l, double h) : m_type(t), m_low(l), m_high(h)
{
	assert(l < h);
}

Tools::Interval::Interval(double l, double h) : m_type(IT_RIGHTOPEN), m_low(l), m_high(h)
{
	assert(l < h);
}

Tools::Interval::Interval(const Interval& iv)
{
	m_low = iv.m_low;
	m_high = iv.m_high;
	m_type = iv.m_type;
}

Tools::IInterval& Tools::Interval::operator=(const IInterval& iv)
{
	if (this != &iv)
	{
		m_low = iv.getLowerBound();
		m_high = iv.getUpperBound();
		m_type = iv.getIntervalType();
	}

	return *this;
}

Tools::Interval& Tools::Interval::operator=(const Interval& iv)
{
	*this = *(static_cast<const IInterval*>(&iv));
	return *this;
}

double Tools::Interval::getLowerBound() const
{
	return m_low;
}

double Tools::Interval::getUpperBound() const
{
	return m_high;
}

void Tools::Interval::setBounds(double l, double h)
{
	assert(l <= h);

	m_low = l;
	m_high = h;
}

bool Tools::Interval::intersectsInterval(const IInterval& i) const
{
	return intersectsInterval(i.getIntervalType(), i.getLowerBound(), i.getUpperBound());
}

bool Tools::Interval::intersectsInterval(IntervalType type, const double low, const double high) const
{
	if (m_high < m_low) throw IllegalStateException("intersectsInterval: high boundary is smaller than low boundary.");

	if (m_low > high || m_high < low) return false;
	if ((m_low > low && m_low < high) || (m_high > low && m_high < high)) return true;

	switch (m_type)
	{
	case IT_CLOSED:
		if (m_low == high)
		{
			if (type == IT_CLOSED || type == IT_LEFTOPEN) return true;
			else return false;
		}
		else if (m_high == low)
		{
			if (type == IT_CLOSED || type == IT_RIGHTOPEN) return true;
			else return false;
		}
		break;
	case IT_OPEN:
		if (m_low == high || m_high == low) return false;
		break;
	case IT_RIGHTOPEN:
		if (m_low == high)
		{
			if (type == IT_CLOSED || type == IT_LEFTOPEN) return true;
			else return false;
		}
		else if (m_high == low)
		{
			return false;
		}
		break;
	case IT_LEFTOPEN:
		if (m_low == high)
		{
			return false;
		}
		else if (m_high == low)
		{
			if (type == IT_CLOSED || type == IT_RIGHTOPEN) return true;
			else return false;
		}
		break;
	}

	return true;
}

bool Tools::Interval::containsInterval(const IInterval& i) const
{
	if (m_high < m_low) throw IllegalStateException("intersectsInterval: high boundary is smaller than low boundary.");

	double low = i.getLowerBound();
	double high = i.getUpperBound();
	IntervalType type = i.getIntervalType();

	if (m_low < low && m_high > high) return true;
	if (m_low > low || m_high < high) return false;

	switch (m_type)
	{
	case IT_CLOSED:
		break;
	case IT_OPEN:
		if ((m_low == low && m_high == high && type != IT_OPEN) ||
			(m_low == low && (type == IT_CLOSED || type == IT_RIGHTOPEN)) ||
			(m_high == high && ( type == IT_CLOSED || type == IT_LEFTOPEN))) return false;
		break;
	case IT_RIGHTOPEN:
		if (m_high == high && (type == IT_CLOSED || type == IT_LEFTOPEN))return false;
		break;
	case IT_LEFTOPEN:
		if (m_low == low && (type == IT_CLOSED || type == IT_RIGHTOPEN))return false;
		break;
	}

	return true;
}

Tools::IntervalType Tools::Interval::getIntervalType() const
{
	return m_type;
}

Tools::Random::Random()
 : m_type(Tools::RGT_MERSENNE)
{
	initMersenne(time(NULL));
}

Tools::Random::Random(unsigned long seed)
 : m_type(Tools::RGT_MERSENNE)
{
	initMersenne(seed);
}

Tools::Random::Random(unsigned long seed, RandomGeneratorType t)
 : m_type(t)
{
	switch (m_type)
	{
	case RGT_MERSENNE:
		initMersenne(seed);
		break;
	case RGT_DRAND48:
		srand48(seed);
		break;
	default:
		throw Tools::IllegalArgumentException("Unkown random number generator type.");
	}
}

void Tools::Random::initMersenne(unsigned long seed)
{
	m_mt[0] = seed;

	for (m_mti = 1; m_mti < MERS_N; m_mti++)
	{
		m_mt[m_mti] = (1812433253UL * (m_mt[m_mti - 1] ^ (m_mt[m_mti - 1] >> 30)) + m_mti);
	}

	m_architecture = Tools::System::getArchitecture();
}

unsigned long Tools::Random::randomBits()
{
	// generate 32 random bits
	unsigned long y;

	if (m_mti >= MERS_N)
	{
		// generate MERS_N words at one time
		const unsigned long LOWER_MASK = (1LU << MERS_R) - 1; // lower MERS_R bits
		const unsigned long UPPER_MASK = -1L  << MERS_R;      // upper (32 - MERS_R) bits
		static const unsigned long mag01[2] = {0, MERS_A};
    
		unsigned long kk;
		for (kk=0; kk < MERS_N - MERS_M; kk++)
		{
			y = (m_mt[kk] & UPPER_MASK) | (m_mt[kk+1] & LOWER_MASK);
			m_mt[kk] = m_mt[kk+MERS_M] ^ (y >> 1) ^ mag01[y & 1];
		}

		for (; kk < MERS_N-1; kk++)
    	{
			y = (m_mt[kk] & UPPER_MASK) | (m_mt[kk+1] & LOWER_MASK);
			m_mt[kk] = m_mt[kk+(MERS_M-MERS_N)] ^ (y >> 1) ^ mag01[y & 1];
		}

		y = (m_mt[MERS_N-1] & UPPER_MASK) | (m_mt[0] & LOWER_MASK);
		m_mt[MERS_N-1] = m_mt[MERS_M-1] ^ (y >> 1) ^ mag01[y & 1];
		m_mti = 0;
	}

	y = m_mt[m_mti++];

	// Tempering (May be omitted):
	y ^=  y >> MERS_U;
	y ^= (y << MERS_S) & MERS_B;
	y ^= (y << MERS_T) & MERS_C;
	y ^=  y >> MERS_L;

	return y;
}
  
double Tools::Random::nextUniformDouble()
{
	if (m_type == RGT_DRAND48)
	{
		return drand48();
	}
	else if (m_type == RGT_MERSENNE)
	{
		union {double f; unsigned long i[2];} convert;
		unsigned long r = randomBits();

		switch (m_architecture)
		{
		case ARCH_LITTLEENDIAN:
			convert.i[0] =  r << 20;
			convert.i[1] = (r >> 12) | 0x3FF00000;
			return convert.f - 1.0;
		case ARCH_BIGENDIAN:
			convert.i[1] =  r << 20;
			convert.i[0] = (r >> 12) | 0x3FF00000;
			return convert.f - 1.0;
		case ARCH_NONIEEE:
		default:
			;
		}

		// This somewhat slower method works for all architectures, including 
		// non-IEEE floating point representation:
		return static_cast<double>(r) * (1.0 / (static_cast<double>(static_cast<unsigned long>(-1L) + 1.0)));
	}
	else
	{
		throw Tools::IllegalArgumentException("Unknown random number generator type.");
	}
}

long Tools::Random::nextUniformLong(long low, long high)
{
	return static_cast<long>((high - low) * nextUniformDouble()) + low;
}

double Tools::Random::nextUniformDouble(double low, double high)
{
	return (low + ((high - low) * nextUniformDouble()));
}

// mean 0.0, standard deviation 1.0
double Tools::Random::nextGaussianDouble()
{
	static bool haveNextNextGaussian = false;
	static double nextNextGaussian;

	if (haveNextNextGaussian)
	{
		haveNextNextGaussian = false;
		return nextNextGaussian;
	}
	else
	{
		double v1, v2, s;

		do
		{
			v1 = 2 * nextUniformDouble() - 1;   // between -1.0 and 1.0
			v2 = 2 * nextUniformDouble() - 1;   // between -1.0 and 1.0
			s = v1 * v1 + v2 * v2;
		}
		while (s >= 1 || s == 0);

		double multiplier = std::sqrt(-2 * std::log(s)/s);
		nextNextGaussian = v2 * multiplier;
		haveNextNextGaussian = true;
		return v1 * multiplier;
	}
}

double Tools::Random::nextGaussianDouble(double m, double std)
{
 	return m + (std * nextGaussianDouble());
}

long Tools::Random::nextZipfLong(long low, long high, Level p)
{
	return low + static_cast<long>(static_cast<double>(high - low) * nextZipfDouble(p));
}

double Tools::Random::nextZipfDouble(double low, double high, Level p)
{
	return (high - low) * nextZipfDouble(p) + low;
}

double Tools::Random::nextZipfDouble(Level p)
{
	long i;
	double r, HsubV, sum, l;
	long V = 1000000;

	//HsubV = 0.0;
	//for(i = 1; i <= V; i++) HsubV += 1.0 / pow((double) i, l);

	switch (p)
	{
	case LVL_VERY_LOW:
		HsubV = 1998.54;
		l = 0.5;
		break;
	case LVL_LOW:
		HsubV = 14.3927;
		l = 1.0;
		break;
	case LVL_MEDIUM:
		HsubV = 2.61038;
		l = 1.5;
		break;
	case LVL_HIGH:
		HsubV = 1.64493;
		l = 2.0;
		break;
	case LVL_VERY_HIGH:
		HsubV = 1.34149;
		l = 2.5;
		break;
	default:
		throw Tools::IllegalArgumentException("Unknown skedeness level.");
	}

	r = nextUniformDouble() * HsubV;
	sum = 1.0; i = 1;
	while(sum < r)
	{
		i++;
		sum += 1.0 / std::pow(static_cast<double>(i), l);
	}

	// i follows Zipf distribution and lies between 1 and V
	return (static_cast<double>(i) - 1.0) / (static_cast<double>(V) - 1.0);
}

bool Tools::Random::flipCoin()
{
	if (nextUniformDouble() >= 0.5) return true;
	return false;
}

const long Tools::Hash::M_31 = 2147483647;
Tools::Random Tools::Hash::m_rand = Tools::Random();

Tools::Hash::Hash()
{
	m_a = m_rand.nextUniformLong(0L, std::numeric_limits<long>::max()) & M_31;
	m_b = m_rand.nextUniformLong(0L, std::numeric_limits<long>::max()) & M_31;
}

Tools::Hash::Hash(Tools::Random& r)
{
	m_a = r.nextUniformLong(0L, std::numeric_limits<long>::max()) & M_31;
	m_b = r.nextUniformLong(0L, std::numeric_limits<long>::max()) & M_31;
}

Tools::Hash::Hash(unsigned long seed1, unsigned long seed2)
 : m_a(seed1 & M_31), m_b(seed2 & M_31)
{
}

unsigned long Tools::Hash::hash31(long x)
{
	long long r = (m_a * x) + m_b;
	r = ((r >> 31) + r) & M_31;

	return static_cast<unsigned long>(r);
}

void Tools::Hash::getSeeds(long& a, long& b)
{
	a = m_a;
	b = m_b;
}

Tools::ResourceUsage::ResourceUsage()
{
	reset();
}

void Tools::ResourceUsage::start()
{
	struct timezone dummy;

	if (getrusage(RUSAGE_SELF, &m_tmpRU) != 0) throw IllegalStateException("Tools::ResourceUsage::start: getrusage failed.");
	if (gettimeofday(&m_tmpTV, &dummy) != 0) throw IllegalStateException("Tools::ResourceUsage::start: gettimeofday failed.");

	// maximum resident set size
	m_peakMemory = std::max(m_peakMemory, m_tmpRU.ru_maxrss);

	// total memory
	m_totalMemory = std::max(m_totalMemory, m_tmpRU.ru_ixrss + m_tmpRU.ru_idrss + m_tmpRU.ru_isrss + m_tmpRU.ru_maxrss);
}

void Tools::ResourceUsage::stop()
{
	struct timezone dummy;
  	struct timeval dif;
	struct rusage ru;
  	struct timeval tv;

	if (getrusage(RUSAGE_SELF, &ru) != 0) throw IllegalStateException("Tools::ResourceUsage::stop: getrusage failed.");
	if (gettimeofday(&tv, &dummy) != 0) throw IllegalStateException("Tools::ResourceUsage::stop: gettimeofday failed.");

	// total_time
	subtractTimeval(dif, tv, m_tmpTV);
	addTimeval(m_totalTime, dif);

	// system_time
	subtractTimeval(dif, ru.ru_stime, m_tmpRU.ru_stime);
	addTimeval(m_systemTime, dif);

	// user_time
	subtractTimeval(dif, ru.ru_utime, m_tmpRU.ru_utime);
  	addTimeval(m_userTime, dif);

	// readIO, writeIOs
	m_readIO += ru.ru_inblock - m_tmpRU.ru_inblock;
	m_writeIO += ru.ru_oublock - m_tmpRU.ru_oublock;

	// maximum resident set size
	m_peakMemory = std::max(m_peakMemory, ru.ru_maxrss);

	// total memory
	m_totalMemory = std::max(m_totalMemory, ru.ru_ixrss + ru.ru_idrss + ru.ru_isrss + ru.ru_maxrss);

	// page faults
	m_pageFaults += ru.ru_majflt - m_tmpRU.ru_majflt;
}

void Tools::ResourceUsage::reset()
{
	m_pageFaults = 0;
	m_readIO = 0;
	m_writeIO = 0;
	m_peakMemory = 0;
	m_totalMemory = 0;
	m_totalTime.tv_sec = 0;
	m_totalTime.tv_usec = 0;
	m_userTime.tv_sec = 0;
	m_userTime.tv_usec = 0;
	m_systemTime.tv_sec = 0;
	m_systemTime.tv_usec = 0;
}

double Tools::ResourceUsage::combineTime(const struct timeval& t)
{
	return static_cast<double>(t.tv_sec) + static_cast<double>(t.tv_usec) / 1000000.0;
}

void Tools::ResourceUsage::addTimeval(struct timeval& result, const struct timeval& a)
{
	result.tv_sec += a.tv_sec;
	result.tv_usec += a.tv_usec;

	if (result.tv_usec > 1000000)
	{
		long div = result.tv_usec / 1000000;
		result.tv_sec += div;
		result.tv_usec -= div * 1000000;
	}
}

void Tools::ResourceUsage::subtractTimeval(struct timeval& result, const struct timeval& a, const struct timeval& b)
{
	result.tv_sec = a.tv_sec - b.tv_sec;
	result.tv_usec = a.tv_usec - b.tv_usec;

	if (result.tv_usec < 0)
	{
		result.tv_sec -= 1;
    	result.tv_usec += 1000000;
    }
}

double Tools::ResourceUsage::getTotalTime()
{
	return combineTime(m_totalTime);
}

double Tools::ResourceUsage::getUserTime()
{
	return combineTime(m_userTime);
}

double Tools::ResourceUsage::getSystemTime()
{
	return combineTime(m_systemTime);
}

long Tools::ResourceUsage::getPageFaults()
{
	return m_pageFaults;
}

long Tools::ResourceUsage::getReadIO()
{
	return m_readIO;
}

long Tools::ResourceUsage::getWriteIO()
{
	return m_writeIO;
}

long Tools::ResourceUsage::getPeakResidentMemoryUsage()
{
	return m_peakMemory;
}

long Tools::ResourceUsage::getTotalMemoryUsage()
{
	return m_totalMemory;
}

Tools::CycleCounter::CycleCounter() : m_totalCycles(0), m_bRunning(false)
{
}

double Tools::CycleCounter::getCPUMHz()
{
	unsigned long long v1 = rdtsc();
	sleep(10);
	unsigned long long v2 = rdtsc();
	return ((v2 - v1) / 1e7);
}

double Tools::CycleCounter::getCyclesPerSecond()
{
	unsigned long long v1 = rdtsc();
	sleep(10);
	unsigned long long v2 = rdtsc();
	return ((v2 - v1) / 10);
}

inline unsigned long long Tools::CycleCounter::rdtsc()
{
	unsigned long long ret;
	__asm__ __volatile__("rdtsc": "=A" (ret):);
	return ret;
}

void Tools::CycleCounter::start()
{
	if (! m_bRunning)
	{
		m_tmpCycles = rdtsc();
		m_bRunning = true;
	}
}

void Tools::CycleCounter::stop()
{
	if (m_bRunning)
	{
		unsigned long long t = rdtsc();
		m_totalCycles += t - m_tmpCycles;
		m_bRunning = false;
	}
}

void Tools::CycleCounter::reset()
{
	m_totalCycles = 0;
	m_bRunning = false;
}

double Tools::CycleCounter::getTotalCycles()
{
	return m_totalCycles;
}

#ifdef PTHREADS
Tools::SharedLock::SharedLock(pthread_rwlock_t* pLock)
	: m_pLock(pLock)
{
	pthread_rwlock_rdlock(m_pLock);
}

Tools::SharedLock::~SharedLock()
{
	pthread_rwlock_unlock(m_pLock);
}

Tools::ExclusiveLock::ExclusiveLock(pthread_rwlock_t* pLock)
	: m_pLock(pLock)
{
	pthread_rwlock_wrlock(m_pLock);
}

Tools::ExclusiveLock::~ExclusiveLock()
{
	pthread_rwlock_unlock(m_pLock);
}
#endif

Tools::StringTokenizer::StringTokenizer(const std::string& str, const std::string& delimiters)
	: m_index(0)
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    std::string::size_type pos = str.find_first_of(delimiters, lastPos);

    while (std::string::npos != pos || std::string::npos != lastPos)
    {
        // Found a token, add it to the vector.
        m_token.push_back(str.substr(lastPos, pos - lastPos));
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
}

std::string Tools::StringTokenizer::getNextToken()
{
	return m_token.at(m_index++);
}

bool Tools::StringTokenizer::hasMoreTokens()
{
	return (m_index < m_token.size());
}

void Tools::StringTokenizer::reset()
{
	m_index = 0;
}

std::string Tools::trimLeft(const std::string& source, const std::string& t)
{
    std::string str = source;
    return str.erase(0, source.find_first_not_of(t));
}

std::string Tools::trimRight(const std::string& source, const std::string& t)
{
    std::string str = source;
    return str.erase(str.find_last_not_of(t) + 1);
}

std::string Tools::trim(const std::string& source, const std::string& t)
{
    std::string str = source;
    return trimLeft(trimRight(str, t), t);
}

char Tools::toLower(char c)
{
    return std::tolower(c);
}

char Tools::toUpper(char c)
{
    return std::toupper(c);
}

std::string Tools::toUpperCase(const std::string& s)
{
    std::string t = s;
    transform(t.begin(), t.end(), t.begin(), Tools::toUpper);
    return t;
}

std::string Tools::toLowerCase(const std::string& s)
{
    std::string t = s;
    transform(t.begin(), t.end(), t.begin(), Tools::toLower);
    return t;
}

std::ostream& Tools::operator<<(std::ostream& os, const Tools::PropertySet& p)
{
	for (std::map<std::string, Variant>::const_iterator it = p.m_propertySet.begin(); it != p.m_propertySet.end(); it++)
	{
		switch ((*it).second.m_varType)
		{
		case VT_LONG:
			os << (*it).first << ": " << (*it).second.m_val.lVal << ", ";
			break;
		case VT_BYTE:
			os << (*it).first << ": " << (*it).second.m_val.bVal << ", ";
			break;
		case VT_SHORT:
			os << (*it).first << ": " << (*it).second.m_val.iVal << ", ";
			break;
		case VT_FLOAT:
			os << (*it).first << ": " << (*it).second.m_val.fltVal << ", ";
			break;
		case VT_DOUBLE:
			os << (*it).first << ": " << (*it).second.m_val.dblVal << ", ";
			break;
		case VT_CHAR:
			os << (*it).first << ": " << (*it).second.m_val.cVal << ", ";
			break;
		case VT_USHORT:
			os << (*it).first << ": " << (*it).second.m_val.uiVal << ", ";
			break;
		case VT_ULONG:
			os << (*it).first << ": " << (*it).second.m_val.ulVal << ", ";
			break;
		case VT_INT:
			os << (*it).first << ": " << (*it).second.m_val.intVal << ", ";
			break;
		case VT_UINT:
			os << (*it).first << ": " << (*it).second.m_val.uintVal << ", ";
			break;
		case VT_BOOL:
			os << (*it).first << ": " << (*it).second.m_val.blVal << ", ";
			break;
		case VT_PCHAR:
			os << (*it).first << ": " << (*it).second.m_val.pcVal << ", ";
			break;
		case VT_PVOID:
			os << (*it).first << ": ?" << ", ";
			break;
		case VT_EMPTY:
			os << (*it).first << ": empty" << ", ";
			break;
		default:
			os << (*it).first << ": unknown" << ", ";
		}
	}

	return os;
}

Tools::Architecture Tools::System::getArchitecture()
{
	union {double f; unsigned long i[2];} convert;
	convert.f = 1.0;

	// Note: Old versions of the Gnu g++ compiler may make an error here,
	// compile with the option  -fenum-int-equiv  to fix the problem
	if (convert.i[1] == 0x3FF00000) return ARCH_LITTLEENDIAN;
	else if (convert.i[0] == 0x3FF00000) return ARCH_BIGENDIAN;
	else return ARCH_NONIEEE;
}

std::ostream& Tools::operator<<(std::ostream& os, const Tools::Interval& iv)
{
	os << "Type: " << iv.m_type << ", Low: " << iv.m_low << ", High: " << iv.m_high;

	return os;
}

std::ostream& Tools::operator<<(std::ostream& os, const Tools::Hash& h)
{
	os << "Seed: " << h.m_a << " " << h.m_b;
	
	return os;
}
*/
