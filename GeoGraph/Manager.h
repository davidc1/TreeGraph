/**
 * \file Manager.h
 *
 * \ingroup GeoTree
 *
 * \brief Class def header for a class geotree::Manager
 * 
 * @author david caratelli
 */

/** \addtogroup GeoTree

    @{*/
#ifndef MANAGER_H
#define MANAGER_H

#include "GeoAlgo/GeoAlgo.h"         //-> for bounding sphere
#include "NodeCollection.h"          //-> where nodes are stored
//#include "AlgoMultipleParentsBase.h" //-> algorithm to resolve conflict due to multiple parents
#include "AlgoMultipleParentsHighScore.h"
#include "AlgoParentIsSiblingsSibling.h"

namespace geotree{

  /**
     \class geotree::Manager
     Class where information for all nodes in event is stored
     and organized
  */

  class Manager{

  public:

    /// Default constructor
    Manager();

    /// Default destructor
    virtual ~Manager(){}

    /// Reset function
    void Reset() { _coll.Reset(); }

    /// Set Objects (TEMP)
    void setObjects(size_t n);

    /// Search the index map
    NodeID_t FindID(size_t idx) { return _coll.FindID(idx); }

    /// Function to call when to make tree
    void MakeTree();

    /// Function to print out full diagram for nodes in manager
    void Diagram() { _coll.Diagram(); }

    /// Function to find node in _head_node_v. Return true if found
    bool NodeAdded(NodeID_t n);

    /// CompareNodes: act on result of correlation check
    /// NOTE: RelationType is the relatioship of id2 w.r.t. id1
    /// eg: type == Child => id2 is Child of id1
    void AddCorrelation(const NodeID_t id1, const NodeID_t id2,
			const double score,
			const geoalgo::Point_t& vtx,
			const geotree::RelationType_t type);

    /// CompareNodes: act on result of correlation check
    /// NOTE: RelationType is the relatioship of id2 w.r.t. id1
    /// eg: type == Child => id2 is Child of id1
    void EditCorrelation(const NodeID_t id1, const NodeID_t id2,
			 const double score,
			 const geoalgo::Point_t& vtx,
			 const geotree::RelationType_t type);

    void EditCorrelation(const NodeID_t id1, const NodeID_t id2,
			 const double score);

    void EditCorrelation(const NodeID_t id1, const NodeID_t id2,
			 const geoalgo::Point_t& vtx);

    void EditCorrelation(const NodeID_t id1, const NodeID_t id2,
			 const geotree::RelationType_t type);

    /// Erase a correlation completely
    void EraseCorrelation(const NodeID_t id1, const NodeID_t id2);

    /// Resolve conflicts: each node may have several correlations
    /// find the "best" one and take it as the one that determines
    /// that node's vertex
    void ResolveConflicts();

    /// setter for verbosity
    void setVerbose(bool on) { _verbose = on; _coll.SetVerbose(on); _algoMultipleParents->SetVerbose(on); }
    
    /// setter for looseness
    void setLoose(bool on) { _loose = on; }

    //****testing**** move these functions private later
    /// Function to resolve sibling conflicts arising form multiple siblings
    void SortSiblings();
    void SortSiblings(NodeID_t ID);
    /// function to resolve case in which node has both sibling and parent -> right now do nothing
    //void SortSiblingsAndParent();
    //void SortSiblingsAndParent(NodeID_t ID);
    void ResolveSiblingsWithDifferentParent(NodeID_t ID);
    void ParentIsSiblingsSibling();
    void ParentIsSiblingsSibling(NodeID_t ID);
    //void SiblingDoesNotHaveSameParent();
    //void SiblingDoesNotHaveSameParent(NodeID_t ID);
    void IfConflictRemoveSibling();
    void IfConflictRemoveSibling(NodeID_t ID);
  private:

    /// verbosity flag
    bool _verbose;

    /// looseness flag: if true then merge two vertices (when possible)
    /// rather than picking the best one (e.g. multiple siblings)
    bool _loose;

    /// collection that stores the nodes
    NodeCollection _coll;
    
    /// function to find best parent for all nodes
    void FindBestParent();

    /// function to find best parent of a node
    void FindBestParent(size_t n);

    /// is this node a subnode of another
    bool IsSubNode(NodeID_t search, NodeID_t top);

    /// geoalgo instance to find "average" vertex
    ::geoalgo::GeoAlgo _geoAlgo;

    /// multiple parents algorithm
    AlgoMultipleParentsHighScore* _algoMultipleParents;
    AlgoParentIsSiblingsSibling*  _algoParentIsSiblingsSibling;

  };
}

#endif
