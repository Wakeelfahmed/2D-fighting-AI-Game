#include "Enemy.h"
#include "CollisionManager.h"
#include "EventManager.h"
#include "RenderManager.h"
#include "TextureManager.h"
#include "Engine.h"
// New LOS Lab.
#include "StateManager.h"
#include "DebugManager.h"
#include "EventManager.h"
#include "PathManager.h"
// Decision tree includes.
#include "DecisionTree.h"
#include "IdleCondition.h"
#include "IdleAction.h"
#include "PatrolAction.h"
//#include "MoveToPlayerAction.h"
#include "MoveToRangeAction.h"
#include "AttackAction.h"
#include "FleeAction.h"
#define SPEED 2.0
#include<iostream>
Enemy::Enemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath)
	:AnimatedSprite(s, d, STATE_IDLING), m_dir(0), m_isIdling(true), m_isDebuging(false), m_level(level), m_velMax(1.0), m_rotMax(10.0), m_accel(0.05),
	m_patPointsCounter(startingpath), m_hasLOS(false), m_isDetected(false), m_detectionRad(75.0), m_health(30)
{
	m_healthDst = { m_dst.x,m_dst.y - 10,32,7 };
	m_curHealthDst = { m_dst.x + 1,m_dst.y - 9,static_cast<float>(m_health),5 };
	// Instantiate tree.
	//m_tree = new DecisionTree(this);
	// Build tree. Move this call into the constructor of the proper sub-class if Enemy.
	//BuildTree();
	// Set initial idling state in tree.
	//GetTree()->GetIdleNode()->SetIsIdle(m_isIdling);
	SetAnimation(AnimState::STATE_IDLING, 1, 0, 1, 0);
}
void Enemy::Update(bool withinrange)
{
	m_healthDst = { m_dst.x,m_dst.y - 10,32,7 };
	m_curHealthDst = { m_dst.x + 1,m_dst.y - 9,static_cast<float>(m_health),5 };
	if (EVMA::KeyPressed(SDL_SCANCODE_T))
	{
		m_isIdling = !m_isIdling;
		GetTree()->GetIdleNode()->SetIsIdle(m_isIdling);
	}

	SDL_FRect* target = STMA::GetStates().front()->GetChild("player")->GetDst();
	SDL_FPoint targetCenter = STMA::GetStates().front()->GetChild("player")->GetCenter();
	std::vector<Tile*>& obsOrig = STMA::GetStates().front()->GetChild<TiledLevel*>("level")->GetObstacles();
	std::vector<Tile*> obstacles;
	// Add only obstacles that are between player and target.
	double targetDist = MAMA::Distance(GetCenter(), targetCenter);
	for (auto obstacle : obsOrig)
	{
		double obsDist = MAMA::Distance(GetCenter(), obstacle->GetCenter());
		if (obsDist < targetDist)
			obstacles.push_back(obstacle);
	}
	double angle = MAMA::AngleBetweenPoints(targetCenter.y - GetCenter().y,
		targetCenter.x - GetCenter().x);
	m_losRay = { static_cast<float>(GetCenter().x + MAMA::SetDeltaX(angle, 300.0)),
	static_cast<float>(GetCenter().y + MAMA::SetDeltaY(angle, 300.0)) };
	if (COMA::LOSCheck(GetCenter(), m_losRay, target, obstacles)) {
		m_hasLOS = true;
		GetTree()->GetLOSNode()->SetHasLOS(m_hasLOS);
	}
	else {
		m_hasLOS = false;
		GetTree()->GetLOSNode()->SetHasLOS(m_hasLOS);
	}

	GetTree()->GetRangeNode()->SetWithinRange(withinrange);

	if (MAMA::Distance(m_dst.x, target->x, m_dst.y, target->y) <= m_detectionRad) {
		m_isDetected = true;
		GetTree()->GetDetectNode()->SetHasDetected(m_isDetected);
	}
	else {
		m_isDetected = false;
		GetTree()->GetDetectNode()->SetHasDetected(m_isDetected);
	}

	// Make decision.
	GetTree()->MakeDecision();
	Animate();
}
void Enemy::Render()
{
	SDL_RenderCopyExF(REMA::GetRenderer(), TEMA::GetTexture("enemy"), GetSrc(), GetDst(), 0, 0, static_cast<SDL_RendererFlip>(m_dir));

	if (m_isDebuging)
	{
		if (m_hasLOS)
			DEMA::DrawLine(GetCenter(), m_losRay, { 0, 255, 0, 255 });
		else
			DEMA::DrawLine(GetCenter(), m_losRay, { 255, 0, 0, 255 });

		if (m_isDetected)
			DEMA::DrawCircle(GetCenter(), static_cast<float>(m_detectionRad), { 0, 255, 0, 255 });
		else
			DEMA::DrawCircle(GetCenter(), static_cast<float>(m_detectionRad), { 255, 0, 0, 255 });

		std::vector<std::vector<Tile*>>& level = STMA::GetStates().front()->GetChild<TiledLevel*>("level")->GetLevel();
		PAMA::GetShortestPath(level[(int)(STMA::GetStates().front()->GetChild("player")->GetCenter().y / 32)][(int)(STMA::GetStates().front()->GetChild("player")->GetCenter().x / 32)]->Node(),
			level[(int)(GetCenter().y / 32)][(int)(GetCenter().x / 32)]->Node());
		PAMA::DrawPath();
	}

	SDL_SetRenderDrawColor(REMA::GetRenderer(), 255, 255, 255, 255);
	SDL_RenderDrawRectF(REMA::GetRenderer(), &m_healthDst);
	if (m_health <= 10)
		SDL_SetRenderDrawColor(REMA::GetRenderer(), 255, 0, 0, 255);
	else
		SDL_SetRenderDrawColor(REMA::GetRenderer(), 0, 0, 255, 255);
	SDL_RenderFillRectF(REMA::GetRenderer(), &m_curHealthDst);
	for (int i = 0; i < mProjectiles.size(); i++) {
		mProjectiles[i]->render();
	}
}
void Enemy::Idle()
{
	if (GetActionState() != ActionState::IDLE_STATE) // If current action is not idle
	{
		SetActionState(ActionState::IDLE_STATE);
		SetAnimation(AnimState::STATE_IDLING, 1, 0, 1, 0);
	}
}
void Enemy::Attack() {
	if (GetActionState() != ActionState::ATTACK_STATE) // If current action is not idle
	{
		SetActionState(ActionState::ATTACK_STATE);
		SetAnimation(AnimState::STATE_ATTACK, 4, 1, 4, 128);
	}
}
void Enemy::Flee()
{
	if (GetActionState() != ActionState::PATROL_STATE) // If current action is not idle
	{
		SetActionState(ActionState::PATROL_STATE);
		SetAnimation(AnimState::STATE_RUNNING, 8, 1, 8, 0);
	}
	SDL_FPoint fleeDirection = { GetCenter().x - kWidth / 2, GetCenter().y - kHeight / 2 };

	// Normalize the direction vector
	float length = sqrt(fleeDirection.x * fleeDirection.x + fleeDirection.y * fleeDirection.y);
	if (length != 0)
	{
		fleeDirection.x /= length;
		fleeDirection.y /= length;
	}

	// Move the enemy away from the center of the screen
	m_dst.x += fleeDirection.x * m_vel;
	m_dst.y += fleeDirection.y * m_vel;

	// Check if the enemy is beyond the screen bounds
	if (m_dst.x < -m_dst.w || m_dst.x > kWidth || m_dst.y < -m_dst.h || m_dst.y > kHeight)
	{
		SetEnabled(false);	// Disable the enemy
	}
}
void Enemy::MoveToRange() {
	if (GetActionState() != ActionState::PATROL_STATE) // If current action is not idle
	{
		SetActionState(ActionState::PATROL_STATE);
		SetAnimation(AnimState::STATE_RUNNING, 8, 1, 8, 0);
	}
}
void Enemy::Patrol()
{
	if (GetActionState() != ActionState::PATROL_STATE) // If current action is not patrol
	{
		SetActionState(ActionState::PATROL_STATE);
		SetAnimation(AnimState::STATE_RUNNING, 8, 1, 8, 0);
	}

	double distToPatrolPoints = MAMA::Distance((static_cast<double>(m_patPoints[m_patPointsCounter].x) * 32 + (32 / 2.0)), GetCenter().x,
		(static_cast<double>(m_patPoints[m_patPointsCounter].y) * 32 + (32 / 2.0)), GetCenter().y);

	if (distToPatrolPoints <= 5.0)
	{
		m_patPointsCounter++;
		if (m_patPointsCounter == 4)
		{
			m_patPointsCounter = 0;
		}
	}

	switch (m_patPointsCounter)
	{
	case 0:
	{
		m_dir = 1;
		break;
	}
	case 1:
	{
		m_dir = 0;
		break;
	}
	case 2:
	{
		m_dir = 0;
		break;
	}
	case 3:
	{
		m_dir = 1;
		break;
	}
	}

	double destAngle = 0.0, perc = 1.0;
	destAngle = MAMA::Rad2Deg(MAMA::AngleBetweenPoints((static_cast<double>(m_patPoints[m_patPointsCounter].y) * 32 + (32 / 2.0)) - (m_dst.y + m_dst.h / 2),
		(static_cast<double>(m_patPoints[m_patPointsCounter].x) * 32 + (32 / 2.0)) - (m_dst.x + m_dst.w / 2))) + 90;
	m_angle += MAMA::Clamp(MAMA::Angle180(destAngle - m_angle), -m_rotMax, m_rotMax);
	m_vel += m_accel;
	m_vel = std::min(m_vel, m_velMax); // Clamp velocity to maximum.
	MAMA::SetDeltas(MAMA::Deg2Rad(m_angle - 90), m_dx, m_dy, m_vel * perc, m_vel * perc);
	if (CheckCollision(m_dx, m_dy))
		m_dx = 0, m_dy = 0;
	// Update various positions.
	m_dst.x += (float)m_dx;
	m_dst.y += (float)m_dy;

}
bool Enemy::CheckCollision(const double dX, const double dY)
{
	int enemyX = GetDst()->x / 32;
	int enemyY = GetDst()->y / 32;
	SDL_Rect p = { GetDst()->x + dX + 8 , GetDst()->y + dY + 6, GetDst()->w - 16, GetDst()->h - 6 }; // Adjusted bounding box.
	Tile* tiles[4] = { m_level->GetLevel()[enemyY][enemyX],																				// Enemy's tile.
					   m_level->GetLevel()[enemyY][(enemyX + 1 == kCols ? kCols - 1 : enemyX + 1)],										// Right tile.
					   m_level->GetLevel()[(enemyY + 1 == kRows ? kRows - 1 : enemyY + 1)][(enemyX + 1 == kCols ? kCols - 1 : enemyX + 1)],	// Bottom-Right tile.
					   m_level->GetLevel()[(enemyY + 1 == kRows ? kRows - 1 : enemyY + 1)][enemyX] };										// Bottom tile.
	for (int i = 0; i < 4; i++)
	{
		SDL_Rect t = MAMA::ConvertFRect2Rect(*(tiles[i]->GetDst()));
		if (tiles[i]->IsObstacle() && SDL_HasIntersection(&p, &t))
		{ // Collision!
			return true;
		}
	}
	return false;
}
void Enemy::TakeDamage() { m_health -= 5; }
DecisionTree* Enemy::GetTree()
{
	return m_tree;
}
ActionState Enemy::GetActionState() { return m_action; }
void Enemy::SetActionState(ActionState action)
{
	m_action = action;
}

void Enemy::set_playerReference(Player* Player) { playerReference = Player; }

CloseCombatEnemy::CloseCombatEnemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath)
	: Enemy(s, d, level, startingpath)
{
	m_tree = new DecisionTree(this);
	BuildTree();
	SetAnimation(AnimState::STATE_IDLING, 1, 0, 1, 0);
}
void CloseCombatEnemy::BuildTree()
{
	IdleCondition* idleCondition = new IdleCondition();
	m_tree->SetIdleNode(idleCondition);
	m_tree->GetNodes().push_back(idleCondition);

	// Add left action node (IdleAction)
	TreeNode* idleAction = m_tree->AddNode(idleCondition, new IdleAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(idleAction);

	// Create and add root node (Player Detected?)		//NOW RIGHT OF IDLE
	DetectCondition* playerDetectedCondition = new DetectCondition();
	m_tree->SetDetectNode(playerDetectedCondition);
	m_tree->AddNode(m_tree->GetIdleNode(), m_tree->GetDetectNode(), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(playerDetectedCondition);

	// Add right action node (Patrol)
	TreeNode* patrolAction1 = m_tree->AddNode(playerDetectedCondition, new PatrolAction(this), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(patrolAction1);

	// Add left condition node (Within Range?)
	RangeCondition* withinRangeCondition = new RangeCondition();
	m_tree->SetRangeNode(withinRangeCondition);
	m_tree->AddNode(m_tree->GetDetectNode(), m_tree->GetRangeNode(), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(withinRangeCondition);

	// Add right action node (Move to Player)
	TreeNode* moveToPlayerAction = m_tree->AddNode(withinRangeCondition, new MoveToRangeAction(this), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(moveToPlayerAction);

	// Add left condition node (Has LOS?)
	LOSCondition* hasLOSCondition = new LOSCondition();
	m_tree->SetLOSNode(hasLOSCondition);
	m_tree->AddNode(m_tree->GetRangeNode(), m_tree->GetLOSNode(), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(hasLOSCondition);

	// Add right action node (Move to Player)
	TreeNode* moveToPlayerAction2 = m_tree->AddNode(hasLOSCondition, new MoveToRangeAction(this), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(moveToPlayerAction2);

	// Add left action node (Attack)
	TreeNode* attackAction = m_tree->AddNode(hasLOSCondition, new AttackAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(attackAction);
}

RangedCombatEnemy::RangedCombatEnemy(SDL_Rect s, SDL_FRect d, TiledLevel* level, int startingpath)
	: Enemy(s, d, level, startingpath)
{
	m_tree = new DecisionTree(this);
	BuildTree();
	SetAnimation(AnimState::STATE_IDLING, 1, 0, 1, 0);
}
//void RangedCombatEnemy::Update(bool withinrange) {
//	std::cout << playerReference->GetDst()->x << std::endl;
//
//	m_healthDst = { m_dst.x,m_dst.y - 10,32,7 };
//	m_curHealthDst = { m_dst.x + 1,m_dst.y - 9,static_cast<float>(m_health),5 };
//	if (EVMA::KeyPressed(SDL_SCANCODE_T))
//	{
//		m_isIdling = !m_isIdling;
//		GetTree()->GetIdleNode()->SetIsIdle(m_isIdling);
//	}
//
//	SDL_FRect* target = STMA::GetStates().front()->GetChild("player")->GetDst();
//	SDL_FPoint targetCenter = STMA::GetStates().front()->GetChild("player")->GetCenter();
//	std::vector<Tile*>& obsOrig = STMA::GetStates().front()->GetChild<TiledLevel*>("level")->GetObstacles();
//	std::vector<Tile*> obstacles;
//	// Add only obstacles that are between player and target.
//	double targetDist = MAMA::Distance(GetCenter(), targetCenter);
//	for (auto obstacle : obsOrig)
//	{
//		double obsDist = MAMA::Distance(GetCenter(), obstacle->GetCenter());
//		if (obsDist < targetDist)
//			obstacles.push_back(obstacle);
//	}
//	double angle = MAMA::AngleBetweenPoints(targetCenter.y - GetCenter().y,
//		targetCenter.x - GetCenter().x);
//	m_losRay = { static_cast<float>(GetCenter().x + MAMA::SetDeltaX(angle, 300.0)),
//	static_cast<float>(GetCenter().y + MAMA::SetDeltaY(angle, 300.0)) };
//	if (COMA::LOSCheck(GetCenter(), m_losRay, target, obstacles)) {
//		m_hasLOS = true;
//		GetTree()->GetLOSNode()->SetHasLOS(m_hasLOS);
//	}
//	else {
//		m_hasLOS = false;
//		GetTree()->GetLOSNode()->SetHasLOS(m_hasLOS);
//	}
//
//	if (m_health > 7)
//		GetTree()->GetHealthNode()->SetOkHealth(1);
//	else
//		GetTree()->GetHealthNode()->SetOkHealth(0);
//	if (m_health < 30)
//		GetTree()->GetHurtNode()->SetIsHurt(1);
//	else
//		GetTree()->GetHurtNode()->SetIsHurt(0);
//
//	if (MAMA::Distance(m_dst.x, target->x, m_dst.y, target->y) <= m_detectionRad) {
//		m_isDetected = true;
//		GetTree()->GetDetectNode()->SetHasDetected(m_isDetected);
//		GetTree()->GetRangeNode()->SetWithinRange(1);
//	}
//	else {
//		m_isDetected = false;
//		GetTree()->GetRangeNode()->SetWithinRange(0);
//		GetTree()->GetDetectNode()->SetHasDetected(m_isDetected);
//	}
//
//	for (int i = 0; i < mProjectiles.size(); i++) {
//		if (mProjectiles[i]->get_shouldBeDestroyed()) {
//			Projectile* pr = mProjectiles[i];
//			mProjectiles.erase(mProjectiles.begin() + i);
//			delete pr;
//			i--;
//		}
//		else {
//			mProjectiles[i]->update(m_frame);
//		}
//		mProjectiles[i]->checkCollision(*playerReference->GetDst());
//		//mProjectiles[i]->CheckCollision(playerReference->GetDst());
//	}
//	// Make decision.
//	GetTree()->MakeDecision();
//	Animate();
//}
void RangedCombatEnemy::Update()
{
	m_healthDst = { m_dst.x,m_dst.y - 10,32,7 };
	m_curHealthDst = { m_dst.x + 1,m_dst.y - 9,static_cast<float>(m_health),5 };
	if (EVMA::KeyPressed(SDL_SCANCODE_T))
	{
		m_isIdling = !m_isIdling;
		GetTree()->GetIdleNode()->SetIsIdle(m_isIdling);
	}

	SDL_FRect* target = STMA::GetStates().front()->GetChild("player")->GetDst();
	SDL_FPoint targetCenter = STMA::GetStates().front()->GetChild("player")->GetCenter();
	std::vector<Tile*>& obsOrig = STMA::GetStates().front()->GetChild<TiledLevel*>("level")->GetObstacles();
	std::vector<Tile*> obstacles;
	// Add only obstacles that are between player and target.
	double targetDist = MAMA::Distance(GetCenter(), targetCenter);
	for (auto obstacle : obsOrig)
	{
		double obsDist = MAMA::Distance(GetCenter(), obstacle->GetCenter());
		if (obsDist < targetDist)
			obstacles.push_back(obstacle);
	}
	double angle = MAMA::AngleBetweenPoints(targetCenter.y - GetCenter().y,
		targetCenter.x - GetCenter().x);
	m_losRay = { static_cast<float>(GetCenter().x + MAMA::SetDeltaX(angle, 300.0)),
	static_cast<float>(GetCenter().y + MAMA::SetDeltaY(angle, 300.0)) };
	if (COMA::LOSCheck(GetCenter(), m_losRay, target, obstacles)) {
		m_hasLOS = true;
		GetTree()->GetLOSNode()->SetHasLOS(m_hasLOS);
	}
	else {
		m_hasLOS = false;
		GetTree()->GetLOSNode()->SetHasLOS(m_hasLOS);
	}

	if (m_health > 7)
		GetTree()->GetHealthNode()->SetOkHealth(1);
	else
		GetTree()->GetHealthNode()->SetOkHealth(0);
	if (m_health < 30)
		GetTree()->GetHurtNode()->SetIsHurt(1);
	else
		GetTree()->GetHurtNode()->SetIsHurt(0);

	if (MAMA::Distance(m_dst.x, target->x, m_dst.y, target->y) <= m_detectionRad) {
		m_isDetected = true;
		GetTree()->GetDetectNode()->SetHasDetected(m_isDetected);
		GetTree()->GetRangeNode()->SetWithinRange(1);
	}
	else {
		m_isDetected = false;
		GetTree()->GetDetectNode()->SetHasDetected(m_isDetected);
		GetTree()->GetRangeNode()->SetWithinRange(0);//now
	}

	for (int i = 0; i < mProjectiles.size(); i++) {
		if (mProjectiles[i]->get_shouldBeDestroyed()) {
			Projectile* pr = mProjectiles[i];
			mProjectiles.erase(mProjectiles.begin() + i);
			delete pr;
			i--;
		}
		else {
			mProjectiles[i]->update(m_frame);
		}
	}
	for (int i = 0; i < mProjectiles.size(); i++) {
		if (mProjectiles[i]->checkCollision(*playerReference->GetDst())) {
			mProjectiles[i]->set_shouldBeDestroyed(1);
			playerReference->Take_Damage();
			playerReference->Take_Damage();
		}
	}
	GetTree()->MakeDecision();	// Make decision
	Animate();
}

void RangedCombatEnemy::Attack()
{
	if (GetActionState() != ActionState::ATTACK_STATE) // If current action is not attack
	{
		SetActionState(ActionState::ATTACK_STATE);
		SetAnimation(AnimState::STATE_ATTACK, 4, 1, 4, 128);
	}
	if (mCurrentFrame < 50) {
		mCurrentFrame++;  // Increment the frame count
		return;  // Wait until the attack delay frames have passed
	}

	mCurrentFrame = 0;  // Reset the frame count

	Projectile* newProjectile;
	newProjectile = new Projectile(REMA::GetRenderer(), m_projectileImagePath, m_dst.x + 128 / 2 - 70, m_dst.y + 5, 20, 20, 0, 200, true);
	newProjectile->setTargetPosition(playerReference->GetDst()->x, playerReference->GetDst()->y);
	mProjectiles.push_back(newProjectile);
}
void RangedCombatEnemy::BuildTree()
{
	IdleCondition* idleCondition = new IdleCondition();
	m_tree->SetIdleNode(idleCondition);
	m_tree->GetNodes().push_back(idleCondition);

	TreeNode* idleAction = m_tree->AddNode(idleCondition, new IdleAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(idleAction);

	HurtCondition* hurtCondition = new HurtCondition();
	m_tree->SetHurtNode(hurtCondition);
	m_tree->AddNode(m_tree->GetIdleNode(), m_tree->GetHurtNode(), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(hurtCondition);

	HealthCondition* healthCondition = new HealthCondition();
	m_tree->SetHealthNode(healthCondition);
	m_tree->AddNode(m_tree->GetHurtNode(), m_tree->GetHealthNode(), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(healthCondition);

	// Add right condition node (Player Detected?)
	DetectCondition* playerDetectedCondition = new DetectCondition();
	m_tree->SetDetectNode(playerDetectedCondition);
	m_tree->AddNode(m_tree->GetHurtNode(), m_tree->GetDetectNode(), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(playerDetectedCondition);

	//reusing health for detect left
	m_tree->AddNode(playerDetectedCondition, healthCondition, TreeNodeType::LEFT_TREE_NODE);

	// Add left action node (Patrol)
	TreeNode* patrolAction = m_tree->AddNode(playerDetectedCondition, new PatrolAction(this), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(patrolAction);


	// Add left condition node (Health over 25%)
	/*HealthCondition* healthOverThresholdCondition = new HealthCondition(25);
	m_tree->AddNode(m_tree->GetDetectNode(), healthOverThresholdCondition, TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(healthOverThresholdCondition);*/

	// Add left action node (Flee)
	TreeNode* fleeAction = m_tree->AddNode(healthCondition, new FleeAction(this), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(fleeAction);

	//Add left condition node (Has LOS?)
	LOSCondition* hasLOSCondition = new LOSCondition();
	m_tree->SetLOSNode(hasLOSCondition);
	m_tree->AddNode(healthCondition, m_tree->GetLOSNode(), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(hasLOSCondition);

	// Add left condition node (Within Range?)
	RangeCondition* withinRangeCondition = new RangeCondition();
	m_tree->SetRangeNode(withinRangeCondition);
	m_tree->AddNode(m_tree->GetLOSNode(), m_tree->GetRangeNode(), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(withinRangeCondition);

	// Add left action node (Attack)
	TreeNode* attackAction = m_tree->AddNode(withinRangeCondition, new AttackAction(this), TreeNodeType::LEFT_TREE_NODE);
	m_tree->GetNodes().push_back(attackAction);

	// Add right action node (Move to range)
	TreeNode* moveToRangeAction = m_tree->AddNode(withinRangeCondition, new MoveToRangeAction(this), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(moveToRangeAction);

	// Add right action node (Move to range)
	TreeNode* moveToRangeAction2 = m_tree->AddNode(hasLOSCondition, new MoveToRangeAction(this), TreeNodeType::RIGHT_TREE_NODE);
	m_tree->GetNodes().push_back(moveToRangeAction2);
}

//void Enemy::shoot() {
//	Projectile* newProjectile;
//	if (m_enemyType == ENEMY_TYPE_BOSS) {
//		// Launch 3 projectiles at the same time
//		newProjectile = new Projectile(m_renderer, m_projectileImagePath, mPosX + mWidth / 2 - 70, mPosY + 5, 40, 40, 0, 200, true);
//		mProjectiles.push_back(newProjectile);
//
//		newProjectile = new Projectile(m_renderer, m_projectileImagePath, mPosX + mWidth / 2 - 35, mPosY + 10, 40, 40, 0, 200, true);
//		mProjectiles.push_back(newProjectile);
//
//		newProjectile = new Projectile(m_renderer, m_projectileImagePath, mPosX + mWidth / 2, mPosY + 10, 40, 40, 0, 200, true);
//		mProjectiles.push_back(newProjectile);
//
//		newProjectile = new Projectile(m_renderer, m_projectileImagePath, mPosX + mWidth / 2 + 35, mPosY + 5, 40, 40, 0, 200, true);
//		mProjectiles.push_back(newProjectile);
//
//		m_audioManager->PlayBossLaserAudio();
//	}
//	else {
//		newProjectile = new Projectile(m_renderer, m_projectileImagePath, mPosX + mWidth / 2 - 6, mPosY + 5, 14, 14, 0, 300, true);
//		mProjectiles.push_back(newProjectile);
//
//		m_audioManager->PlayLaserSound();
//	}
//}
std::vector<Projectile*>* Enemy::getProjectiles() {
	return &mProjectiles;
}
//bool Enemy::isCollidingWithPlayerProjectiles() {
//	SDL_Rect enemyRect = { mPosX, mPosY, mWidth, mHeight };
//
//	std::vector<Projectile*>* projectiles = m_player->getProjectiles();
//
//	for (int i = 0; i < projectiles->size(); i++) {
//		if (projectiles->at(i) != nullptr && projectiles->at(i)->checkCollision(enemyRect)) {
//			m_player->destroyProjectileAtIndex(i);
//			i--;
//			return true;
//		}
//	}
//
//	return false;
//}

void Enemy::destroyProjectileAtIndex(int index) {
	Projectile* pr = mProjectiles[index];
	mProjectiles.erase(mProjectiles.begin() + index);
	delete pr;
}