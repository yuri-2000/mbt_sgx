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

#include "SpatialIndexImpl.h"

#include "../../trusted/btree/BTree.h"
#include "../../trusted/btree/Statistics.h"

//using namespace SpatialIndex;

//const std::string VERSION = "0.82b";
//const std::string DATE = "March 16th 2005";

std::ostream& SpatialIndex::operator<<(std::ostream& os, const ISpatialIndex& i)
{

	const SpatialIndex::BTree::BTree* pBTree = dynamic_cast<const SpatialIndex::BTree::BTree*>(&i);
	if (pBTree != 0)
	{
		os << *pBTree;
		return os;
	}

	std::cerr << "ISpatialIndex operator<<: Not implemented yet for this index type." << std::endl;
	return os;
}

std::ostream& SpatialIndex::operator<<(std::ostream& os, const IStatistics& s)
{
	using std::endl;


	const SpatialIndex::BTree::Statistics* pBTreeStats = dynamic_cast<const SpatialIndex::BTree::Statistics*>(&s);
	if (pBTreeStats != 0)
	{
		os << * pBTreeStats;
		return os;
	}

	std::cerr << "IStatistics operator<<: Not implemented yet for this index type." << std::endl;
	return os;
}
