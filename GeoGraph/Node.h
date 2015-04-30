/**
 * \file Node.h
 *
 * \ingroup GeoTree
 * 
 * \brief Class def header for a class geotree::Node
 *
 * @author david caratelli
 */

/** \addtogroup GeoTree
    
    @{*/
#ifndef NODE_H
#define NODE_H

#include "GeoAlgo/GeoVector.h"
#include <map>
#include <string>

typedef size_t NodeID_t;



namespace geotree{


  enum RelationType_t {
    kParent,
    kChild,
    kSibling
  };

  /**
     \class geotree::Node
     User defined class geograph::Node
     A Node is any object in the geometry:
     Cone, LineSegment, etc.
     Each is meant to represent a particle
     Each can be associated with a vertex
     (to be considered a point of origin)
  */

  class Node{
    
  public:
    
    /// Default constructor
    Node() {_node_id = 0; }

    /// Constructor
    Node(size_t n) { _node_id = n; _debug = false; }
    
    /// Default destructor
    virtual ~Node(){}

    /// getter for ID
    NodeID_t ID() const { return _node_id; }

    /// getter for parent ID
    NodeID_t parentID() const { return _parent_id; }

    /// getter for children
    const std::vector<NodeID_t>& childrenID() const { return _child_id_v; }

    /// Add child
    void addChild(NodeID_t id) { _child_id_v.push_back(id); }

    /// Set Parent
    void setParent(NodeID_t id) { _parent_id = id; }

    /// Add a correlated node and the associated score & vtx info
    void addCorrelation(const NodeID_t id, const double score,
    			const ::geoalgo::Point_t& vtx,
			const geotree::RelationType_t type);

    /// edit a correlated node's information (score, vtx, type)
    void editCorrelation(const NodeID_t id, const double score,
			 const ::geoalgo::Point_t& vtx,
			 const geotree::RelationType_t type);

    /// edit a correlated node's information (score)
    void editCorrelation(const NodeID_t id, const double score);

    /// edit a correlated node's information (vtx)
    void editCorrelation(const NodeID_t id,
			 const ::geoalgo::Point_t& vtx);

    /// edit a correlated node's information (type)
    void editCorrelation(const NodeID_t id,
			 const geotree::RelationType_t type);

    //*********************************
    // 3 maps for correlation is stupid
    // Change this to a struct!!!!!!!!!
    //*********************************
    
    /// getter for correlations
    const std::map<NodeID_t, double>& getCorrScores() { return _corrScores; }
    const std::map<NodeID_t, ::geoalgo::Vector_t>& getCorrVtx() { return _corrVtx; }
    const std::map<NodeID_t, geotree::RelationType_t>& getCorrType() { return _corrType; }

    /// getters for individual correlation objects
    const double getCorrScore(NodeID_t n) { return _corrScores[n]; }
    const ::geoalgo::Vector_t getCorrVtx(NodeID_t n) { return _corrVtx[n]; }
    const geotree::RelationType_t getCorrType(NodeID_t n) { return _corrType[n]; }

    /// get score (if exists)
    double getScore(NodeID_t node);
    
    /// erase elements for correlation maps
    void eraseCorrelation(const NodeID_t node);

    /// Node has parent
    bool hasParent() const;

    /// get parent's ID
    NodeID_t getParent() const;

    /// node has sibling?
    bool hasSiblings() const;

    std::vector<NodeID_t> getSiblings() const;

    /// Check if node is primary (has no parent or sibling)
     bool isPrimary() const;

    /// Check if node has a conflict (parent & sibling)
    bool hasConflict() const;

    /// debug setter
    void setDebug(bool on) { _debug = on; }

    /// Check if this node is correlated with another. Boolean return
    bool isCorrelated(NodeID_t id);

  private:

    // debug flag
    bool _debug;
    
    // unique ID that identifies this node
    NodeID_t _node_id;
    // ID linking to parent node
    NodeID_t _parent_id;
    // vector listing IDs of children nodes
    std::vector<NodeID_t> _child_id_v;
    // vertex
    geoalgo::Point_t _vtx;
    // each node can have a list of "correlated" nodes
    // each correlated node comes with a score
    std::map<NodeID_t, double> _corrScores;
    std::map<NodeID_t, ::geoalgo::Vector_t> _corrVtx;
    std::map<NodeID_t, geotree::RelationType_t> _corrType;
    //std::map<NodeID_t, std::pair<double,::geoalgo::Vector_t> > _correlated;



  };
}

#endif
/** @} */ // end of doxygen group 
