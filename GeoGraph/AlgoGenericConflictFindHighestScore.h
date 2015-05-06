/**
 * \file AlgoGenericConflictFindHighestScore.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::AlgoGenericConflictFindHighestScore
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef ALGOGENERICCONFLICTFINDHIGHESTSCORE_H
#define ALGOGENERICCONFLICTFINDHIGHESTSCORE_H

//#include "AlgoMultipleParentsBase.h"
#include "AlgoBase.h"

namespace geotree{

  class AlgoGenericConflictFindHighestScore : public AlgoBase {

  public:
    
    AlgoGenericConflictFindHighestScore() { _name="GenericConflictFindHighestScore"; }

    /// Constructor which syncs node collection for the algorithm
    AlgoGenericConflictFindHighestScore(NodeCollection* coll);// { _name="MultipleParents"; _coll = coll; _verbose = false; }

    void ResolveConflict(const NodeID_t& id, const NodeID_t& parent, const NodeID_t& sibling);

  };
}

#endif
/** @} */ // end of doxygen group 
