#pragma once
#ifndef __DECISIONTREE__
#define __DECISIONTREE__

#include "DetectCondition.h"
#include "HealthCondition.h"
#include "HurtCondition.h"
#include "IdleCondition.h"
#include "LOSCondition.h"
#include "RangeCondition.h"
#include <vector>

class Enemy; // Forward declaration required.

class DecisionTree
{
public:
	// Constructors and destructor
	DecisionTree();
	DecisionTree(Enemy* agent);
	~DecisionTree();

	// Getters and setters
	Enemy* GetAgent() const;
	std::vector<TreeNode*>& GetNodes();
	void SetAgent(Enemy* agent);
	// Node methods.
	DetectCondition* GetDetectNode();
	void SetDetectNode(DetectCondition* node);
	HealthCondition* GetHealthNode();
	void SetHealthNode(HealthCondition* node);
	HurtCondition* GetHurtNode();
	void SetHurtNode(HurtCondition* node);
	IdleCondition* GetIdleNode();
	void SetIdleNode(IdleCondition* node);
	LOSCondition* GetLOSNode();
	void SetLOSNode(LOSCondition* node);
	RangeCondition* GetRangeNode();
	void SetRangeNode(RangeCondition* node);
	// Other methods
	TreeNode* AddNode(TreeNode* parent, TreeNode* child, TreeNodeType type);
	void Display();
	void Clean();
	void MakeDecision(); // Traverse tree in order

	std::vector<TreeNode*> m_treeNodeList;
private:
	Enemy* m_agent;
	// Nodes.
	DetectCondition* m_detectNode;
	HealthCondition* m_healthNode;
	HurtCondition* m_hurtNode;
	IdleCondition* m_idleNode;
	LOSCondition* m_losNode;
	RangeCondition* m_rangeNode;
};

#endif