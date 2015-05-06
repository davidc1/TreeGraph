/**
 * \file AlgoParentIsSiblingsSibling.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::AlgoParentIsSiblingsSibling
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef ALGOPARENTISSIBLINGSSIBLING_H
#define ALGOPARENTISSIBLINGSSIBLING_H

//#include "AlgoMultipleParentsBase.h"
#include "AlgoBase.h"

namespace geotree{

  class AlgoParentIsSiblingsSibling : public AlgoBase {

  public:
    
    AlgoParentIsSiblingsSibling() { _name="ParentIsSiblingsSibling"; }

    /// Constructor which syncs node collection for the algorithm
    AlgoParentIsSiblingsSibling(NodeCollection* coll);// { _name="MultipleParents"; _coll = coll; _verbose = false; }

    void ResolveConflict(const NodeID_t& id, const NodeID_t& parent, const NodeID_t& sibling);

  };
}

#endif
/** @} */ // end of doxygen group 
