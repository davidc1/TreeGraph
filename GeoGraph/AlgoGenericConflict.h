/**
 * \file AlgoGenericConflict.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::AlgoGenericConflict
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef ALGOGENERICCONFLICT_H
#define ALGOGENERICCONFLICT_H

//#include "AlgoMultipleParentsBase.h"
#include "AlgoBase.h"

namespace geotree{

  class AlgoGenericConflict : public AlgoBase {

  public:
    
    AlgoGenericConflict() { _name="GenericConflict"; }

    /// Constructor which syncs node collection for the algorithm
    AlgoGenericConflict(NodeCollection* coll);// { _name="MultipleParents"; _coll = coll; _verbose = false; }

    void ResolveConflict(const NodeID_t& id, const NodeID_t& parent, const NodeID_t& sibling);

  };
}

#endif
/** @} */ // end of doxygen group 
