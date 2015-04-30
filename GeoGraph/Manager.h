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

#include "Node.h"
#include <deque>
#include "GeoAlgo/GeoAlgo.h" //-> for bounding sphere
namespace geotree{

  /**
     \class geotree::Manager
     Class where information for all nodes in event is stored
     and organized
  */

  class Manager{

  public:

    /// Default constructor
    Manager(){ _verbose = false; _loose = true; }

    /// Default destructor
    virtual ~Manager(){}

    /// Reset function
    void Reset() { _node_v.clear(); _head_node_v.clear(); _node_map.clear(); }

    /// Set Objects (TEMP)
    void setObjects(size_t n);

    /// Search the index map
    NodeID_t FindID(size_t idx);

    /// Function to call when to make tree
    void MakeTree();

    /// Function to move a node
    void MoveNode(const NodeID_t tomove,const NodeID_t endpos);

    /// Function to get a node (const)
    const Node GetNode(NodeID_t id);

    /// Function to get a node (writeable)
    Node& GetWriteableNode(NodeID_t id);

    /// Function to print out a node's diagram
    void Diagram(NodeID_t id, int gen=0);

    /// Function to print out full diagram for nodes in manager
    void Diagram();

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
    void setVerbose(bool on) { _verbose = on; }
    
    /// setter for looseness
    void setLoose(bool on) { _loose = on; }

    //****testing**** move these functions private later
    /// Function to resolve sibling conflicts arising form multiple siblings
    void SortSiblings();
    void SortSiblings(NodeID_t ID);
    /// function to resolve case in which node has both sibling and parent -> right now do nothing
    void SortSiblingsAndParent();
    void SortSiblingsAndParent(NodeID_t ID);
    void ResolveSiblingsWithDifferentParent(NodeID_t ID);
    void ParentAndSiblingAreNotLogicallyConsistent();
    void ParentAndSiblingAreNotLogicallyConsistent(NodeID_t ID);
    void SiblingDoesNotHaveSameParent();
    void SiblingDoesNotHaveSameParent(NodeID_t ID);
    void IfConflictRemoveSibling();
    void IfConflictRemoveSibling(NodeID_t ID);
  private:

    /// verbosity flag
    bool _verbose;

    /// looseness flag: if true then merge two vertices (when possible)
    /// rather than picking the best one (e.g. multiple siblings)
    bool _loose;

    /// a vector to hold all nodes in the envet
    std::vector< ::geotree::Node> _node_v;

    /// keep track of the indices of primary nodes
    std::deque<NodeID_t> _head_node_v;

    /// map to link node ID to position in node vector
    std::map<NodeID_t, size_t> _node_map;

    /// map to link position in node vector with ID
    std::map<size_t, NodeID_t> _index_map;

    /// function to find best parent for all nodes
    void FindBestParent();

    /// function to find best parent of a node
    void FindBestParent(size_t n);

    /// is this node a subnode of another
    bool IsSubNode(NodeID_t search, NodeID_t top);

    /// geoalgo instance to find "average" vertex
    ::geoalgo::GeoAlgo _geoAlgo;

  };
}

#endif
