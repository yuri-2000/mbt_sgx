// Spatial Index Library
//
// Copyright (C) 2002 Navel Ltd.
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

#include <time.h>
#include <stdlib.h>

#include "../spatialindex/SpatialIndexImpl.h"

#include "LruBuffer.h"

using namespace SpatialIndex;
using namespace SpatialIndex::StorageManager;
using std::map;

IBuffer* SpatialIndex::StorageManager::returnLruBuffer(IStorageManager& sm, Tools::PropertySet& ps)
	throw (std::exception, Tools::Exception)
{
	try
	{
		IBuffer* b = new LruBuffer(sm, ps);
		return b;
	}
	catch (...)
	{
		throw;
	}
}

IBuffer* SpatialIndex::StorageManager::createNewLruBuffer(IStorageManager& sm, unsigned int capacity, bool bWriteThrough)
	throw (std::exception, Tools::Exception)
{
	Tools::Variant var;
	Tools::PropertySet ps;

	var.m_varType = Tools::VT_ULONG;
	var.m_val.ulVal = capacity;
	ps.setProperty("Capacity", var);

	var.m_varType = Tools::VT_BOOL;
	var.m_val.blVal = bWriteThrough;
	ps.setProperty("WriteThrough", var);

	return returnLruBuffer(sm, ps);
}

LruBuffer::LruBuffer(IStorageManager& sm, Tools::PropertySet& ps) : Buffer(sm, ps)
{
	////srand48(time(NULL));
}

LruBuffer::~LruBuffer()
{
}

void LruBuffer::addEntry(long id, Entry* e)  //加入cache
{
	assert(m_buffer.size() <= m_capacity);

	if (m_buffer.size() == m_capacity) removeEntry();
	assert(m_buffer.find(id) == m_buffer.end());  //该id不曾加入过cache
	m_buffer.insert(std::pair<long, Entry*>(id, e));
}

void LruBuffer::removeEntry()  //从满的cache中置换出最近较少使用的元素
{
	if (m_buffer.size() == 0) return;

//	unsigned int entry = (unsigned int) floor(((double) m_buffer.size()) * drand48());
//	map<long, Entry*>::iterator it = m_buffer.begin();
//	for (unsigned int cIndex = 0; cIndex < entry; cIndex++) it++;

	// delete least recently used element
	long id = idQueue.back();
	// Pops the last elmeent
	idQueue.pop_back();
	// Erase the last
	idMap.erase(id);

	map<long, Entry*>::iterator it = m_buffer.find(id);
	if ((*it).second->m_bDirty)
	{
		//long id = (*it).first;
		m_pStorageManager->storeByteArray(id, ((*it).second)->m_length, (const byte *) ((*it).second)->m_pData);
	}

	delete (*it).second;
	m_buffer.erase(it);
}

// Refers key x with in the LRU cache. Cache is not full!!!
void LruBuffer::refer(long x)
{
//	// not present in cache
//	if (idMap.find(x) == idMap.end()) {
//		// cache is full
//		if (idQueue.size() == m_capacity) {
//			// delete least recently used element
//			long last = idQueue.back();
//			// Pops the last elmeent
//			idQueue.pop_back();
//			// Erase the last
//			idMap.erase(last);
//		}
//	}
//	// present in cache
//	else
	if (idMap.find(x) != idMap.end())
	{
		idQueue.erase(idMap[x]);
	}

	// update reference
	idQueue.push_front(x);
	idMap[x] = idQueue.begin();
}

// Function to display contents of cache
void LruBuffer::display()
{
	// Iterate in the deque and print all the elements in it
	for (auto it = idQueue.begin(); it != idQueue.end(); it++)
		cout << (*it) << " ";
	cout << endl;
}

void LruBuffer::loadByteArray(const long id, unsigned long& len, byte** data)
{
    Buffer::loadByteArray(id, len, data);
    refer(id);
}

void LruBuffer::storeByteArray(long& id, const unsigned long len, const byte* const data)
{
    Buffer::storeByteArray(id, len, data);
    refer(id);
}

void LruBuffer::deleteByteArray(const long id)
{
    Buffer::deleteByteArray(id);
	if (idMap.find(id) != idMap.end())
	{
        idQueue.erase(idMap[id]);
        idMap.erase(id);
    }
}

void LruBuffer::clear()
{
	display();
	Buffer::clear();
	idMap.clear();
	idQueue.clear();
}
