/**
 * \file AlgoMultipleParentsHighScore.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::AlgoMultipleParentsHighScore
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef ALGOMULTIPLEPARENTSHIGHSCORE_H
#define ALGOMULTIPLEPARENTSHIGHSCORE_H

//#include "AlgoMultipleParentsBase.h"
#include "AlgoBase.h"

namespace geotree{

  class AlgoMultipleParentsHighScore : public AlgoBase {

  public:
    
    AlgoMultipleParentsHighScore() { _name="MultipleParentsHighScore"; }

    /// Constructor which syncs node collection for the algorithm
    AlgoMultipleParentsHighScore(NodeCollection* coll);// { _name="MultipleParents"; _coll = coll; _verbose = false; }

    void FindBestParent(const NodeID_t& id, const std::vector<NodeID_t>& parents);

  };
}

#endif
/** @} */ // end of doxygen group 
