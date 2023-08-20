#pragma once
#ifndef __CONDITIONNODE_H__
#define __CONDITIONNODE_H__
#include "TreeNode.h"

enum class ConditionNodeType
{
	IDLE_NODE
};

class ConditionNode : public TreeNode // Abstract class.
{
public:
	ConditionNode() { m_isLeaf = false; }
	virtual ~ConditionNode() = default;
	virtual bool Condition() = 0;
};

#endif