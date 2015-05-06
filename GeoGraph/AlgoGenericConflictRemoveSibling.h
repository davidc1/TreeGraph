/**
 * \file AlgoGenericConflictRemoveSibling.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::AlgoGenericConflictRemoveSibling
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef ALGOGENERICCONFLICTREMOVESIBLING_H
#define ALGOGENERICCONFLICTREMOVESIBLING_H

//#include "AlgoMultipleParentsBase.h"
#include "AlgoBase.h"

namespace geotree{

  class AlgoGenericConflictRemoveSibling : public AlgoBase {

  public:
    
    AlgoGenericConflictRemoveSibling() { _name="GenericConflictRemoveSibling"; }

    /// Constructor which syncs node collection for the algorithm
    AlgoGenericConflictRemoveSibling(NodeCollection* coll);// { _name="MultipleParents"; _coll = coll; _verbose = false; }

    void ResolveConflict(const NodeID_t& id, const NodeID_t& parent, const NodeID_t& sibling);

  };
}

#endif
/** @} */ // end of doxygen group 
