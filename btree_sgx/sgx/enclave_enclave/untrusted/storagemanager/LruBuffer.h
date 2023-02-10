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

#ifndef __storagemanager_lrubuffer_h
#define __storagemanager_lrubuffer_h

#include "Buffer.h"
//#include <bits/stdc++.h>
#include <list>
#include <unordered_map>

using namespace std;

namespace SpatialIndex
{
	namespace StorageManager
	{
		class LruBuffer : public Buffer
		{
		public:
			LruBuffer(IStorageManager&, Tools::PropertySet& ps);
				// see Buffer.h for available properties.

			virtual ~LruBuffer();

			virtual void addEntry(long id, Buffer::Entry* pEntry);
			virtual void removeEntry();

			virtual void refer(long id);
			virtual void display();

			virtual void loadByteArray(const long id, unsigned long& len, byte** data);
			virtual void storeByteArray(long& id, const unsigned long len, const byte* const data);
			virtual void deleteByteArray(const long id);
			virtual void clear();
		protected:
			// store keys of cache
			list<long> idQueue;
			// store references of key in cache
			unordered_map<long, list<long>::iterator> idMap;  //id->idQueue's index
		}; // LruBuffer
	}
}

#endif /*__storagemanager_lrubuffer_h*/

