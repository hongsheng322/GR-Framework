#include "WeaponInfo.h"
#include "../Projectile/Projectile.h"
#include "../Sound_Engine.h"
#include <iostream>
using namespace std;

CWeaponInfo::CWeaponInfo()
	: magRounds(10)
	, maxMagRounds(10)
	, totalRounds(8)
	, maxTotalRounds(8)
	, timeBetweenShots(0.5)
	, elapsedTime(0.0)
	, bFire(true)
	, weaponName("")
	, damage(10)
{
}


CWeaponInfo::~CWeaponInfo()
{
}

void CWeaponInfo::SetWeaponDamage(const int damage)
{
	this->damage = damage;
}

// Set the number of ammunition in the magazine for this player
void CWeaponInfo::SetMagRound(const int magRounds)
{
	this->magRounds = magRounds;
}

// Set the maximum number of ammunition in the magazine for this weapon
void CWeaponInfo::SetMaxMagRound(const int magRounds)
{
	this->magRounds = magRounds;
}

// The current total number of rounds currently carried by this player
void CWeaponInfo::SetTotalRound(const int totalRounds)
{
	this->totalRounds = totalRounds;
}

// The max total number of rounds currently carried by this player
void CWeaponInfo::SetMaxTotalRound(const int maxTotalRounds)
{
	this->maxTotalRounds = maxTotalRounds;
}


int CWeaponInfo::GetWeaponID(void) const
{
	return weaponID;
}

int CWeaponInfo::GetWeaponDamage(void) const
{
	return damage;
}

// Get the number of ammunition in the magazine for this player
int CWeaponInfo::GetMagRound(void) const
{
	return magRounds;
}

// Get the maximum number of ammunition in the magazine for this weapon
int CWeaponInfo::GetMaxMagRound(void) const
{
	return maxMagRounds;
}

// Get the current total number of rounds currently carried by this player
int CWeaponInfo::GetTotalRound(void) const
{
	return totalRounds;
}

// Get the max total number of rounds currently carried by this player
int CWeaponInfo::GetMaxTotalRound(void) const
{
	return maxTotalRounds;
}

std::string CWeaponInfo::GetName(void) const
{
	return weaponName;
}

// Set the time between shots
void CWeaponInfo::SetTimeBetweenShots(const double timeBetweenShots)
{
	this->timeBetweenShots = timeBetweenShots;
}

// Set the firing rate in rounds per min
void CWeaponInfo::SetFiringRate(const int firingRate)
{
	timeBetweenShots = 60.0 / (double)firingRate;	// 60 seconds divided by firing rate
}

// Set the firing flag
void CWeaponInfo::SetCanFire(const bool bFire)
{
	this->bFire = bFire;
}

// Get the time between shots
double CWeaponInfo::GetTimeBetweenShots(void) const
{
	return timeBetweenShots;
}

// Get the firing rate
int CWeaponInfo::GetFiringRate(void) const
{
	return (int)(60.0 / timeBetweenShots);	// 60 seconds divided by timeBetweenShots
}

// Get the firing flag
bool CWeaponInfo::GetCanFire(void) const
{
	return bFire;
}

// Initialise this instance to default values
void CWeaponInfo::Init(void)
{
	//// The number of ammunition in a magazine for this weapon
	//magRounds = 12;
	//// The maximum number of ammunition for this magazine for this weapon
	//maxMagRounds = 12;
	//// The current total number of rounds currently carried by this player
	//totalRounds = 12;
	//// The max total number of rounds currently carried by this player
	//maxTotalRounds = 999;

	// The time between shots
	timeBetweenShots = 0.2;
	// The elapsed time (between shots)
	elapsedTime = 0.0;
	// Boolean flag to indicate if weapon can fire now
	bFire = true;



}

// Update the elapsed time
void CWeaponInfo::Update(const double dt)
{
	elapsedTime += dt;
	if (elapsedTime > timeBetweenShots)
	{
		bFire = true;
		elapsedTime = 0.0;
	}

	if (bFired)
	{
		if (magRounds > 0)
		{
			magRounds--;
		}
		bFired = false;
	}
}

// Discharge this weapon
void CWeaponInfo::Discharge(Vector3 position, Vector3 target, Player* _source)
{
	if (bFire)
	{
		// If there is still ammo in the magazine, then fireddd
		if (magRounds > 0)
		{
			// Create a projectile with a cube mesh. Its position and direction is same as the player.
			// It will last for 3.0 seconds and travel at 500 units per second
			Projectile* aProjectile = Create::Bullet("sphere");
			aProjectile->SetPosition(Player::GetInstance()->GetPosition());
			aProjectile->SetCollider(true);
			aProjectile->SetLifetime(10);
			aProjectile->SetDamage(GetWeaponDamage());
			aProjectile->weap = this;
			aProjectile->pent = penetration;
			aProjectile->SetDirection(target);
			aProjectile->SetStatus(true);
			aProjectile->SetAABB(Vector3(0.5f, 0.5f, 0.5f), Vector3(-0.5f, -0.5f, -0.5f));
			aProjectile->source = Projectile::PLAYER_SOURCE;
			bullets.push_back(aProjectile);
			bFire = false;
			bFired = true;

			CSoundEngine::GetInstance()->playsinglesound("PewPew", 0.2f);
			//magRounds--;
		}
	}
}

// Reload this weapon
void CWeaponInfo::Reload(void)
{
	if (magRounds < maxMagRounds)
	{
		if (maxMagRounds - magRounds <= totalRounds)
		{
			totalRounds -= maxMagRounds - magRounds;
			magRounds = maxMagRounds;
		}
		else
		{
			magRounds += totalRounds;
			totalRounds = 0;
		}
		PrintSelf();
	}
}

// Add rounds
void CWeaponInfo::AddRounds(const int newRounds)
{
	if (totalRounds + newRounds > maxTotalRounds)
		totalRounds = maxTotalRounds;
	else
		totalRounds += newRounds;
}

// Print Self
void CWeaponInfo::PrintSelf(void)
{
	cout << "CWeaponInfo::PrintSelf()" << endl;
	cout << "========================" << endl;
	cout << "magRounds\t\t:\t" << magRounds << endl;
	cout << "maxMagRounds\t\t:\t" << maxMagRounds << endl;
	cout << "totalRounds\t\t:\t" << totalRounds << endl;
	cout << "maxTotalRounds\t\t:\t" << maxTotalRounds << endl;
	cout << "timeBetweenShots\t:\t" << timeBetweenShots << endl;
	cout << "elapsedTime\t\t:\t" << elapsedTime << endl;
	cout << "Damage\t\t:\t" << damage << endl;
	cout << "bFire\t\t:\t" << bFire << endl;
}

std::list<Projectile*> CWeaponInfo::GetProj()
{
	return bullets;
}
