/**
 * @class Character
 * @brief This is a base class representing our entity.
 * Handels their attributes and interactions.
*/

#ifndef CHARACTER_H
#define CHARACTER_H

#include "JSON.h"
#include <string>
#include <fstream>
#include <map>
#include <iostream>

class Character
{
protected:
	const std::string name; ///< Name of the Character.
	float maxHP, HP, DMG, ACD; ///< Maximum health points, health points, damage points and attack cooldown time of the Character, respectively.
	Character(const std::string _name, float _maxHP, float _DMG, float _ACD); ///< Generic constructor of the class.
	/**
	* @brief Starts hitting procedure.
	* @param The victim Character.
	* Inflicts damage to the victim's health points.
	* Overload available for XP management.
	*/
	virtual void doHit(Character& victim);
public:
	const std::string getName() const; ///< Getter for name.
	const float getHealthPoints() const; ///< Getter for health points.
	const float getMaxHealthPoints() const; ///< Getter for maximum health points.
	const float getDamage() const; ///< Getter for damage.
	const float getAttackCoolDown() const; ///< Getter for attack cooldown.
	/**
	* @brief Decreases HP by attacker's damage.
	* @brief Calculates the potentialXP from the damage dealt.
	* @param The Character pointer of the attacker.
	* @return The poteintal XP.
	*/
    float gotHit(Character* attacker);
	/**
	* @brief Checks vitals of Character.
	* @return true if alive
	* @return false if dead
	*/
	bool isAlive();
	/**
	* @brief Fight simulation.
	* @param Enemy.
	* Characters hit each other.
	* With every hit an attack cooldown starts.
	* Exits if either dies.
	*/
	void fightTilDeath(Character &enemy);
};

float RONAF(const float& num); //Rule Out Negative Any Float

#endif
