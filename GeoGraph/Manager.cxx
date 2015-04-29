#ifndef MANAGER_CXX
#define MANAGER_CXX

#include "Manager.h"

namespace geotree{

  // A temporary function to create some crappy nodes
  // to do some testing...TEMP
  void Manager::setObjects(size_t n){

    if (_verbose) { std::cout << "Setting " << n << " objects to prepare tree" << std::endl; }
    for (size_t i=0; i < n; i++){
      // Assign an ID double the element number. Just because
      size_t nID = i*2;
      Node thisnode(nID);
      _node_v.push_back(thisnode);
      size_t idx = _node_v.size()-1; // index in vector for this node
      _node_map[i*2] = idx;
      _index_map[idx] = nID;
      if (_verbose) { std::cout << "Created Node Num. " << i << " with ID " << _node_v.back().ID() << std::endl; }
    }
    return;
  }

  // find node ID from position in node vector
  NodeID_t Manager::FindID(size_t idx){
    
    if (_index_map.find(idx) == _index_map.end())
      throw ::geoalgo::GeoAlgoException("Looking for an index that is out of bounds!");

    return _index_map[idx];
  }

  // Function to be called when Trees will be made
  void Manager::MakeTree(){

    if (_verbose) { std::cout << "Making tree" << std::endl; }

    // loop over nodes.
    // what to do:
    // 1) if no parent or sibling -> add as head node
    // 2) if parent exists, add as child to that parent
    // 3) if sibling exists, create new parent node (example: pi0)
    for (size_t n=0; n < _node_v.size(); n++){
      if (_verbose) { std::cout << "Examining node " << n << " with ID: " << _node_v[n].ID() << std::endl; }

      // check if this node has been already added
      if (NodeAdded(_node_v[n].ID())){
	if (_verbose) { std::cout << "\tthis node has already been added. Skip" << std::endl; }
	continue;
      }

      // check if node is primary
      if (_node_v[n].isPrimary()){
	if (_verbose) { std::cout << "\tnode is primary" << std::endl; }
	_head_node_v.emplace_back(_node_v[n].ID());
      }
      // if node has a parent add it
      if (_node_v[n].hasParent()){
	if (_verbose) { std::cout << "\tnode has parent" << std::endl; }
	NodeID_t parent = _node_v[n].getParent();
	_node_v[_node_map[parent]].addChild(_node_v[n].ID());
	_node_v[_node_map[n]].setParent(parent);
      }
      // if node has a parent && a sibling
      // find vertex consistent with all 3 objects
      if (_node_v[n].hasConflict()){
	if (_verbose) { std::cout << "\tnode has conflict" << std::endl; }
	// The philosophy right now:
	// Add particle as child of its parent.
	// Do not worry about sibling relationship.
	// they are siblings since they come from the
	// same parent, but they may not share a vertex
	// worry about that later
	// So bottom line now:
	// if siblings & parent -> just add as child to parent
	continue;
      }
      // if node has sibling: make a common head node for the two siblings
      if (_node_v[n].hasSiblings()){
	if (_verbose) { std::cout << "\tnode has sibling" << std::endl; }
	if (_verbose) { std::cout << "\tadding node " << _node_v[n].ID() << " now" << std::endl; }
	// get siblings
	auto siblings = _node_v[n].getSiblings();
	if (_verbose) { std::cout << "\tnode has " << siblings.size() << " siblings" << std::endl;
	}
	// if > 1 siblings for now skip
	// Make sure all siblings share the same vertex
	if (siblings.size() > 1){
	  auto const vtx = _node_v[n].getCorrVtx(siblings[0]);
	  for (auto &s : siblings){
	    auto const vtx2 = _node_v[n].getCorrVtx(s);
	    if (vtx != vtx2)
	      throw ::geoalgo::GeoAlgoException("Multiple siblings @ different Vertices. Should have been solved by SortSiblings!");
	  }//for all siblings
	}// if multiple siblings
	// create new node to host the new siblings
	NodeID_t id = _node_v[n].ID()*100+siblings[0]*1000; 
	// Make sure this ID does not exist
	if (_node_map.find(id) != _node_map.end())
	  throw ::geoalgo::GeoAlgoException("About to create a NodeID that already exists. Not acceptable!");
	Node tmpNode(id);
	_node_v.push_back(tmpNode);
	_node_map[id] = _node_v.size()-1;
	// add child nodes to newly created node
	_node_v[_node_map[id]].addChild(_node_v[n].ID());
	_node_v[n].setParent(id);
	// loop over siblings and add them
	for (auto& sib : siblings){ 
	  _node_v[_node_map[id]].addChild(sib);
	  _node_v[_node_map[sib]].setParent(id);
	}
	if (_verbose) { std::cout << "\tadding node " << _node_v.back().ID() << " to _head_node_v" << std::endl; }
	_head_node_v.emplace_back(_node_v.back().ID());
	if (_verbose){ 
	  std::cout << "\tadded node " << id 
		    << " as parent of: [" << _node_v[n].ID() << ", ";
	  for (auto &sib : siblings)
	    std::cout << sib << ", ";
	  std::cout << std::endl;
	}
      }// if node has a sbinling
    }// for all nodes
    
  return;
  }

  // Function to move node from one location to another
  void Manager::MoveNode(const NodeID_t child, const NodeID_t parent){

    // do these nodes exist?
    if (_node_map.find(child) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID of Node to be moved not found!");
    if (_node_map.find(parent) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID of Node destination not found!");

    // Add node as child
    _node_v[_node_map[parent]].addChild(child);
    // edit parent info for child node
    _node_v[_node_map[child]].setParent(parent);

    // if moved node was in "_head_node_v" remove it
    auto const nodepos = std::find(_head_node_v.begin(),_head_node_v.end(),child);
    if (nodepos != _head_node_v.end())
      _head_node_v.erase(nodepos);

    return;
  }

  // Get a node (const)
  const Node Manager::GetNode(NodeID_t id){

    // does this node exist?
    if (_node_map.find(id) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    return _node_v[_node_map[id]];
  }

  // Get a node (writeable)
  Node& Manager::GetWriteableNode(NodeID_t id){

    // does this node exist?
    if (_node_map.find(id) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    return _node_v[_node_map[id]];
  }

  // function to print out full diagram for trees in manager
  void Manager::Diagram(){
    
    for (size_t i=0; i < _head_node_v.size(); i++)
      Diagram(_head_node_v[i],0);

    return;
  }


  // Function to print tree diagram
  void Manager::Diagram(NodeID_t id, int gen){

    // does this node exist?
    if (_node_map.find(id) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    for (int g=0; g < gen; g++)
      std::cout << "..";
    std::cout << id << std::endl;
    // get list of children
    // this node:
    Node thisnode = _node_v[_node_map[id]];
    // vector of children ids
    auto const child_ids = thisnode.childrenID();
    for (size_t x=0; x < child_ids.size(); x++)
      Diagram(child_ids[x],gen+1);
    
    return;
  }


  // find if node added to _head_node_v
  bool Manager::NodeAdded(NodeID_t n){

    bool found = false;
    
    for (size_t i=0; i < _head_node_v.size(); i++){
      if (_head_node_v[i] == n)
	return true;
      // check if this node exists in daughters
      found = IsSubNode(n, _head_node_v[i]);
      if (found)
	return found;
    }
      
    return found; 
  }

  // find node as subnode of other node
  bool Manager::IsSubNode(NodeID_t search, NodeID_t top){

    bool found = false;

    for (size_t i=0; i < _node_v[_node_map[top]].childrenID().size(); i++){
      NodeID_t thisChildID = _node_v[_node_map[top]].childrenID()[i];
      if (thisChildID == search){
	found = true;
	return found;
      }
      found = IsSubNode(search,thisChildID);
      if (found)
	return found;
    }// loop over all children
    return found;
  }
      
    
  // Correlation provided indicates relationship between node id1 and node id2
  void Manager::AddCorrelation(const NodeID_t id1, const NodeID_t id2,
			       const double score,
			       const geoalgo::Point_t& vtx,
			       const geotree::RelationType_t type){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "Adding Correlation..." << std::endl; }
    _node_v[_node_map[id2]].addCorrelation(id1,score,vtx,type);

    //type returned is the relation of 1 w.r.t. 2
    // find "inverse" relation to assign to 2 w.r.t. 1
    geotree::RelationType_t otherRel;
    if (type == geotree::RelationType_t::kSibling)
      otherRel = geotree::RelationType_t::kSibling;
    else if (type == geotree::RelationType_t::kChild)
      otherRel = geotree::RelationType_t::kParent;
    else if (type == geotree::RelationType_t::kParent)
      otherRel = geotree::RelationType_t::kChild;
    
    if (_verbose) { std::cout << "Adding Correlation..." << std::endl; }
    _node_v[_node_map[id1]].addCorrelation(id2,score,vtx,otherRel);

    return;
  }


  void Manager::EditCorrelation(const NodeID_t id1, const NodeID_t id2,
				const double score,
				const geoalgo::Point_t& vtx,
				const geotree::RelationType_t type){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "Editing Correlation..." << std::endl; }
    _node_v[_node_map[id2]].editCorrelation(id1,score,vtx,type);

    //type returned is the relation of 1 w.r.t. 2
    // find "inverse" relation to assign to 2 w.r.t. 1
    geotree::RelationType_t otherRel;
    if (type == geotree::RelationType_t::kSibling)
      otherRel = geotree::RelationType_t::kSibling;
    else if (type == geotree::RelationType_t::kChild)
      otherRel = geotree::RelationType_t::kParent;
    else if (type == geotree::RelationType_t::kParent)
      otherRel = geotree::RelationType_t::kChild;
    
    _node_v[_node_map[id1]].editCorrelation(id2,score,vtx,otherRel);

    return;
  }


  void Manager::EditCorrelation(const NodeID_t id1, const NodeID_t id2,
				const double score){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "Editing Correlation Score..." << std::endl; }
    _node_v[_node_map[id2]].editCorrelation(id1,score);
    _node_v[_node_map[id1]].editCorrelation(id2,score);

    return;
  }


  void Manager::EditCorrelation(const NodeID_t id1, const NodeID_t id2,
				const geoalgo::Point_t& vtx){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "Editing Correlation Vtx..." << std::endl; }
    _node_v[_node_map[id2]].editCorrelation(id1,vtx);
    _node_v[_node_map[id1]].editCorrelation(id2,vtx);

    return;
  }


  void Manager::EditCorrelation(const NodeID_t id1, const NodeID_t id2,
				const geotree::RelationType_t type){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "Editing Correlation Relation..." << std::endl; }
    _node_v[_node_map[id2]].editCorrelation(id1,type);

    //type returned is the relation of 1 w.r.t. 2
    // find "inverse" relation to assign to 2 w.r.t. 1
    geotree::RelationType_t otherRel;
    if (type == geotree::RelationType_t::kSibling)
      otherRel = geotree::RelationType_t::kSibling;
    else if (type == geotree::RelationType_t::kChild)
      otherRel = geotree::RelationType_t::kParent;
    else if (type == geotree::RelationType_t::kParent)
      otherRel = geotree::RelationType_t::kChild;
    
    _node_v[_node_map[id1]].editCorrelation(id2,otherRel);

    return;
  }


  /// Erase a correlation completely
  void Manager::EraseCorrelation(const NodeID_t id1, const NodeID_t id2){

    // make sure nodes exist
    if (_node_map.find(id1) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");
    if (_node_map.find(id2) == _node_map.end())
      throw ::geoalgo::GeoAlgoException("Node ID not found!");

    if (_verbose) { std::cout << "Removing Correlation..." << std::endl; }
    _node_v[_node_map[id1]].eraseCorrelation(id2);
    _node_v[_node_map[id2]].eraseCorrelation(id1);

    return;
  }

  void Manager::ResolveConflicts(){

    // what is a conflict?
    // 1) two parents
    // 2) a sibling with different parent
    // 3) multiple siblings
    // 4) has both siblings & parent

    // solutions:
    // 1) choose parent with best score
    
    // NOTE: THIS IS NOT IMPLEMENTED
    // 2) compare score of possible scenarios:
    // example: A -> B and C -> D 
    // && B & D siblings
    // score 1: (A -> B) + (C -> D)
    // score 2: (A -> B) + (B && D)
    // score 3: (C -> D) + (B && D)
    // choose whichever is highest
    
    // first resolve conflict 1)
    // if multiple parents, choose the
    // one with the highest score
    if (_verbose) { std::cout << "Find Best parent of nodes if they have multiple ones..." << std::endl; }
    FindBestParent();

    // Conflict 3)
    // Resolve conflict of multiple siblings
    if (_verbose) { std::cout << "merge or find best sibling if there are multiple siblings..." << std::endl; }
    SortSiblings();

    // Conflict 4)
    // Resolve potential conflict between siblings & parent
    if (_verbose) { std::cout << "if parent and siblings resolve conflict..." << std::endl; }
    SortSiblingsAndParent();

    return;
  }


  /// Find best parent for each Node
  void Manager::FindBestParent(){

    std::map<NodeID_t,size_t>::iterator it;

    for (it = _node_map.begin(); it != _node_map.end(); it++)
      FindBestParent(it->first);

    return;
  }

  /// function to find best parent of a node (remove other parents)
  void Manager::FindBestParent(NodeID_t ID){

    if (_verbose) { std::cout << "look for best parent for node: " << ID << std::endl; }

    // iterator for correlation types
    std::map<NodeID_t,geotree::RelationType_t>::const_iterator it;
    // get correaltion types for this node
    auto const cTypes = _node_v[_node_map[ID]].getCorrType();   
    // get correlation scores for this node
    auto const cScores = _node_v[_node_map[ID]].getCorrScores();

    // vector where to hold parent IDs
    std::vector<size_t> parentIDs;
    // vector where to hold parent scores
    std::vector<double> parentScores;

    for (it = cTypes.begin(); it != cTypes.end(); it++){
      if (it->second == geotree::RelationType_t::kParent){
	//if (it->second == geotree::RelationType_t::kChild){
	size_t pID = it->first;
	parentIDs.push_back(pID);
	parentScores.push_back(_node_v[_node_map[ID]].getScore(pID));
      }
    }

    if (_verbose) { std::cout << "\tfound " << parentIDs.size() << " parents" << std::endl; }

    // if multiple parents erase all but the one with the highest score
    if (parentIDs.size() > 1){
      NodeID_t bestParent = 0;
      double bestScore = 0;
      for (size_t i=0; i < parentScores.size(); i++){
	if (_verbose) { std::cout << "\t\tParent ID: " << parentIDs[i] << "\tScore: " << parentScores[i] << std::endl; }
	if (parentScores[i] > bestScore){
	  bestScore = parentScores[i];
	  bestParent = parentIDs[i];
	}
      }
      // best parent is that at position i
      for (auto const& pID : parentIDs){
	if (pID != bestParent){
	  if (_verbose) { std::cout << "\t\terasing corr. between " << pID << " and " << ID << std::endl; }
	  // not the best -> eliminate!
	  _node_v[_node_map[ID]].eraseCorrelation(pID);
	  // do the same backwards! (eliminate reverse corr.)
	  _node_v[_node_map[pID]].eraseCorrelation(ID);
	}
      }

    }// if more than 1 parents
    
    return;
  }

  // Sort all siblings at once
  void Manager::SortSiblings(){
   
    std::map<NodeID_t,size_t>::iterator it;

    for (it = _node_map.begin(); it != _node_map.end(); it++)
      SortSiblings(it->first);
 
    return;
  }
  
  // Siblings sorting:
  // Either merge siblings (loose == true)
  // or only pick the best one (loose == false)
  // After decision, modify all other correlations
  // accordingly
  void Manager::SortSiblings(NodeID_t ID){

    if (_node_v[_node_map[ID]].hasSiblings() == false){
      if (_verbose) { std::cout << "no siblings. No issue..." << std::endl; }
      return;
    }

    auto siblings = _node_v[_node_map[ID]].getSiblings();
    if (siblings.size() == 1){
      if (_verbose) { std::cout << "Only 1 sibling. No issue..." << std::endl; }
      return;
    }

    // If there are multiple siblings but with the same vertex
    // -> then we are good to go. Everything is in agreement
    bool AllSame = true;
    auto const vtx1 = _node_v[_node_map[ID]].getCorrVtx(siblings[0]);
    for (size_t i=1; i < siblings.size(); i++){
      auto const vtx2 = _node_v[_node_map[ID]].getCorrVtx(siblings[i]);
      if (vtx1 != vtx2){
	AllSame = false;
	break;
      }
    }// for all remaining siblings
    // if all vertices are the same -> return
    if (AllSame)
      return;

    // if a particle has multiple siblings we can either:
    // - merge those siblings together into a single vtxx
    // - find the best sibling
    if (_loose){
      // find all vertices of siblings
      std::vector<::geoalgo::Vector_t> siblingVtxList;
      // siblings contains NodeID of all siblings. Use to get vtx
      for (auto& sID : siblings)
	siblingVtxList.push_back(_node_v[_node_map[ID]].getCorrVtx(sID));
      // find "average" vertex location
      if (_verbose) { 
	std::cout << "Find Bounding Sphere from points: " << std::endl;
	for (auto& v : siblingVtxList)
	  std::cout << "\t" << v << std::endl;
      }
      ::geoalgo::Point_t newVtx = _geoAlgo.boundingSphere(siblingVtxList).Center();
      if (_verbose) { std::cout << "average vtx from " << siblings.size() << " siblings is: " << newVtx << std::endl; }
      // edit all correlations so that vertices are updated.
      for (auto& sID : siblings)
	EditCorrelation(ID,sID,newVtx);
      // also, we need to add all sibling correlations from node ID to its sisters
      for (size_t s1 = 0; s1 < siblings.size()-1; s1++){
	for (size_t s2 = s1+1; s2 < siblings.size(); s2++)
	  AddCorrelation(siblings[s1],siblings[s2],0.,newVtx,::geotree::RelationType_t::kSibling);
      }
    }// if loose (accept more siblings and smear vertex)
    // if instead one should just select the best vertex
    else{
      double maxScore = 0.;
      NodeID_t bestSibling;
      for (auto& sID : siblings){
	double score = _node_v[_node_map[ID]].getCorrScore(sID);
	if (score > maxScore){
	  maxScore = score;
	  bestSibling = sID;
	}
      }// for all siblings
      if (_verbose) { std::cout << "Best Correlation with Node " << bestSibling << " (score = " << maxScore << ")" << std::endl; }
      // now erase correlation with all other siblings
      for (auto& sID : siblings){
	if (sID != bestSibling)
	  EraseCorrelation(ID,sID);
      }
    }// if we should just keep the best correlation
    
    return;
  }


  // Sort all SiblingsAndParent
  void Manager::SortSiblingsAndParent(){
   
    std::map<NodeID_t,size_t>::iterator it;

    for (it = _node_map.begin(); it != _node_map.end(); it++)
      SortSiblingsAndParent(it->first);
 
    return;
  }


  // Sort the fact that a node has siblings and a parent
  // either accept both parent and sibling (editing vertex) (loose == true)
  // or pick the one with the highest score (loose == false)
  // either way, modify all other correlations accordingly
  // Call this function if:
  // 1) already checked that there is only 1 parent
  // 2) already sorted siblings of this node
  void Manager::SortSiblingsAndParent(NodeID_t ID){

    // Establish that there is indeed a conflict (sibling & parent)
    // if none -> return
    if (_node_v[_node_map[ID]].hasConflict() == false){
      if (_verbose) { std::cout << "No conflict to solve! continue..." << std::endl; }
      return;
    }

    // do the siblings have the same parent?
    // get the parent NodeID
    auto const parentID = _node_v[_node_map[ID]].getParent();
    // Now loop through siblings. Do they have the same parent?
    bool sameParent = true;
    auto const siblings  = _node_v[_node_map[ID]].getSiblings();
    for (auto& s : siblings){
      if (_node_v[_node_map[s]].hasParent() == false){
	sameParent = false;
	break;
      }// if the node does not have a parent
      else if (_node_v[_node_map[s]].getParent() != parentID){
	sameParent = false;
	break;
      }// if the node has a different parent
    }// for all siblings
    if (sameParent){
      if (_verbose) { std::cout << "Siblings all have the same parent! yay!" << std::endl; }
      return;
    }
    else {
      if (_verbose) { std::cout << "Siblings don't all have the same parent! fix..." << std::endl; }
      // Need to find the best combination
      // Assume no sibling has another parent
      // Also, assume siblings have been sorted already (so all have the same communal siblings)
      // if LOOSE  : assign parent as parent of all others
      // otherwise : which has a larger score? parent or sibling? Choose the largest score
      if (_loose){
	// Assign parent as parent to all others (with score of parent)
	auto parentScore = _node_v[_node_map[ID]].getCorrScore(parentID);
	// vertex is average of vtx for parent and vertices for siblings
	auto parentVtx = _node_v[_node_map[ID]].getCorrVtx(parentID);
	std::vector<::geoalgo::Vector_t> vtxList;
	vtxList.push_back(parentVtx);
	for (auto& sID : siblings)
	  vtxList.push_back(_node_v[_node_map[ID]].getCorrVtx(sID));
	// find "average" vertex location
	if (_verbose) { 
	  std::cout << "Find Bounding Sphere from points: " << std::endl;
	  for (auto& v : vtxList)
	    std::cout << "\t" << v << std::endl;
	}
	::geoalgo::Point_t newVtx = _geoAlgo.boundingSphere(vtxList).Center();
	if (_verbose) { std::cout << "average vtx from " << siblings.size() << " siblings is: " << newVtx << std::endl; }
	// Edit all sibling & parent correlations to match the new vertex information
	if (_verbose) { std::cout << "Editing Corr Vtx between this ID and Parent" << std::endl; }
	EditCorrelation(ID,parentID,newVtx);
	for (size_t s1 = 0; s1 < siblings.size(); s1++){
	  if (_verbose) { std::cout << "Editing Corr Vtx between this ID and sibling" << std::endl; }
	  EditCorrelation(ID,siblings[s1],newVtx);
	  // Also, edit the correlations amongst the various siblings -> STUPID!!!
	  for (size_t s2 = s1+1; s2 < siblings.size(); s2++){
	    if (s2 != ID){
	      if (_verbose) { std::cout << "Editing Corr Vtx between siblings" << std::endl; }
	      EditCorrelation(siblings[s1],siblings[s2],newVtx);
	    }
	  }
	  // Finally, if a parent exists edit vtx
	  // if it does not exist, add parent
	  if (_node_v[_node_map[siblings[s1]]].hasParent() == true){
	    // ASSUME SAME PARENT!!!
	    if (_verbose) { std::cout << "Editing Corr Vtx between Sibling & Parent" << std::endl; }
	    EditCorrelation(s1,parentID,newVtx);
	  }
	  else{
	    if (_verbose) { std::cout << "Adding Corr Vtx between Sibling & Parent" << std::endl; }
	    AddCorrelation(siblings[s1],parentID,parentScore,newVtx,::geotree::RelationType_t::kChild);
	  }
	}// for all siblings
      }// if loose
      // if not loose -> find best relation for this node
      else{
	// get parent score
	auto parentScore  = _node_v[_node_map[ID]].getCorrScore(parentID);
	// get score for first sibling (assuming sibling sorting already happened
	auto siblingScore = _node_v[_node_map[ID]].getCorrScore(siblings[0]);
	if (_verbose) { std::cout << "Not loose. Parent Score: " << parentScore << " and sibling score: " << siblingScore << std::endl; }
	if (parentScore > siblingScore){
	  // remove sibling score
	  if (_verbose) { std::cout << "Erase sibling correlation" << std::endl; }
	  // for all siblings
	  for (auto& s : siblings)
	    EraseCorrelation(ID,s);
	}// if parent is better
	// if sibling is better
	else{
	  // remove parent score
	  if (_verbose) { std::cout << "Erase parent correlation" << std::endl; }
	  EraseCorrelation(ID,parentID);
	}
      }// if not loose
    }// if not everything is ok and we need to resolve conflicts
    return;
  }

}

#endif
