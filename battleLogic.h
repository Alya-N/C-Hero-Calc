#ifndef BATTLE_LOGIC_HEADER
#define BATTLE_LOGIC_HEADER

#include <vector>
#include <cmath>
#include <stdexcept>

#include "cosmosData.h"

const int VALID_RAINBOW_CONDITION = 15; // Binary 00001111 -> means all elements were added

// Struct keeping track of everything that is only valid for one turn
struct TurnData {
    int64_t baseDamage = 0;
    int64_t evolve = 0;
    double multiplier = 1;
    double witchMult = 1;

    int buffDamage = 0;
    int protection = 0;
    int armorArray[ARMY_MAX_SIZE];
    bool aliveAtTurnStart[ARMY_MAX_SIZE];
    int aoeDamage = 0;
    int aoeRevenge = 0;
    int aoeLast = 0;
    int aoeFirst = 0;
    int healing = 0;
    int sadism = 0;
    double dampFactor = 1;
    double resistance = 0;
    double skillDampen = 0;
    double aoeReflect = 0;
    int hpPierce = 0;
    int sacHeal = 0;
    int deathstrikeDamage = 0;
    int masochism = 0;
    int immunityValue = 0;
    int deathBuffHP = 0;
    int healFirst = 0;
    int tetrisMult = 0;
    int tetrisSeed = 0;
    int bloodlust = 0;

    Element opposingElement;

    double counter = 0;
    int flatRef = 0;
    double valkyrieMult = 0;
    double valkyrieDamage = 0;
    double tetrisDamage = 0;
    double absorbMult = 0;
    int absorbID = -1;
    double absorbDamage = 0;
    int explodeDamage = 0;
    bool trampleTriggered = false;
    double trampleMult = 0;
    double tripleMult = 0;
    bool guyActive = false;
    bool ricoActive = false;
    int direct_target = 0;
    int counter_target = 0;
    double hate = 0;
    double leech = 0;
    double selfHeal = 0;
    double execute = 0;
    bool immunity5K = false ;
    bool turnDamp = false ;
};

// Keep track of an army's condition during a fight and save some convenience data
class ArmyCondition {
    public:
        int armySize;
        Monster * lineup[ARMY_MAX_SIZE];
        int64_t remainingHealths[ARMY_MAX_SIZE];
        int64_t maxHealths[ARMY_MAX_SIZE];
        SkillType skillTypes[ARMY_MAX_SIZE];
        PassiveType passiveTypes[ARMY_MAX_SIZE];
        Element skillTargets[ARMY_MAX_SIZE];
        double skillAmounts[ARMY_MAX_SIZE];
        double passiveAmounts[ARMY_MAX_SIZE];

        bool rainbowConditions[ARMY_MAX_SIZE]; // for rainbow ability
        //int pureMonsters[ARMY_MAX_SIZE]; // for friends ability

        int dice;
        bool booze; // for leprechaun's ability
        int boozeValue;//Adds an illusory amount of enemies to lep's perception amplifying his aoe
        int aoeZero; // for hawking's ability
        int aoeLow; // for xmas common ability
        int flatLep; // for xmas rare ability
        double dampZero; // for bubbles's ability
        bool easterCheck; // for Daisy
        int easterID; // for Daisy

        int64_t seed;

        int64_t lastBerserk;
        double evolveTotal; //for evolve ability
        int deathBuffATK; //For S7 fairies
        int64_t furyArray[ARMY_MAX_SIZE];//for subatomic heroes

        int monstersLost;

        bool worldboss;

        TurnData turnData;

        inline void init(const Army & army, const int oldMonstersLost, const int aoeDamage);
        inline void afterDeath();
        inline void startNewTurn(const int turncounter);
        inline void getDamage(const int turncounter, const ArmyCondition & opposingCondition);
        inline void applyArmor(TurnData & opposing);
        inline void resolveDamage(TurnData & opposing);
        inline void resolveRevenge(TurnData & opposing);
        inline int findMaxHP();
        inline int getLuxTarget(const ArmyCondition & opposingCondition, int64_t seed);
};
//For gambler heroes (Dice,Lux,Pokerface,Tetra)
inline int64_t getTurnSeed(int64_t seed, int turncounter) {
    for (int i = 0; i < turncounter; ++i)
        seed = (16807 * seed) % 2147483647;
    return seed;
}
// extract and extrapolate all necessary data from an army
inline void ArmyCondition::init(const Army & army, const int oldMonstersLost, const int aoeDamage) {
    HeroSkill * skill;
    HeroPassive * passive;

    int tempRainbowCondition = 0;
    int tempPureMonsters = 0;

    seed = army.seed;
    armySize = army.monsterAmount;
    monstersLost = oldMonstersLost;
    lastBerserk = 0;
    evolveTotal = 0;
    deathBuffATK = 0;

    dice = -1;
    booze = false;
    boozeValue = 0;
    worldboss = false;
    aoeZero = 0;
    aoeLow = 0;
    flatLep = 0;
    dampZero = 1;
    easterCheck = false;
    easterID = -1;

    for (int i = armySize -1; i >= monstersLost; i--) {
        lineup[i] = &monsterReference[army.monsters[i]];

        skill = &(lineup[i]->skill);
        passive = &(lineup[i]->passive);
        skillTypes[i] = skill->skillType;
        skillTargets[i] = skill->target;
        skillAmounts[i] = skill->amount;
        passiveTypes[i] = passive->passiveType;
        passiveAmounts[i] = passive->amount;
        remainingHealths[i] = lineup[i]->hp - aoeDamage * ((skill->skillType == SKILLDAMPEN) ? (1 - skill->amount) : 1);
        furyArray[i] = lineup[i]->damage;

        worldboss |= lineup[i]->rarity == WORLDBOSS;

        maxHealths[i] = lineup[i]->hp;
        if (skill->skillType == DICE) dice = i; //assumes only 1 unit per side can have dice ability, have to change to bool and loop at turn zero if this changes
        if (skill->skillType == BEER){ booze = true; boozeValue = skill->amount;}
        if (skill->skillType == AOEZERO) aoeZero += skill->amount;
        if (skill->skillType == AOELOW) aoeLow += skill->amount;
        if (skill->skillType == FLATLEP) flatLep += skill->amount;
        if (skill->skillType == DAMPEN) dampZero *= skill->amount;
        if (skill->skillType == POSBONUS){ maxHealths[i] += round(skill->amount * (armySize - i - 1)); remainingHealths[i] = maxHealths[i] - aoeDamage; }
        // (7 - armySize + i) <- Get absolute position, not relative to first unit, for Lili
        if (skill->skillType == CONVERT){ maxHealths[i] += round(lineup[i]->damage * (7 - armySize + i) * skill->amount); remainingHealths[i] = maxHealths[i] - aoeDamage; }
        if (skill->skillType == EASTER){ easterID = i; }

        rainbowConditions[i] = tempRainbowCondition == VALID_RAINBOW_CONDITION;
        //pureMonsters[i] = tempPureMonsters;

        if (lineup[i]->element != ALL)
            tempRainbowCondition |= 1 << lineup[i]->element;
        if (skill->skillType == NOTHING) {
            tempPureMonsters++;
        }
    }
    if (easterID >= 0){
        for (int j = armySize - 1; j >= monstersLost; j--) {
            if ( lineup[j]->baseName == "sparks" || lineup[j]->baseName == "leaf" ||
                 lineup[j]->baseName == "flynn" || lineup[j]->baseName == "willow" ||
                 lineup[j]->baseName == "gizmo" || lineup[j]->baseName == "thumper" ) {
                    easterCheck = true;
                    maxHealths[easterID] += round(skillAmounts[easterID] * 84 / 134); 
                    remainingHealths[easterID] = maxHealths[easterID] - aoeDamage;
            }
        }
    }
}

// Reset turndata and fill it again with the hero abilities' values
inline void ArmyCondition::startNewTurn(const int turncounter) {
    int i;

    turnData.buffDamage = 0;
    turnData.protection = 0;
    turnData.aoeDamage = 0;
    turnData.aoeRevenge = 0;
    turnData.aoeLast = 0;
    turnData.aoeFirst = 0;
    turnData.healing = 0;
    turnData.dampFactor = 1;
    turnData.absorbMult = 0;
    turnData.absorbID = -1;
    turnData.absorbDamage = 0;
    turnData.resistance = 1;
    turnData.skillDampen = 1;
    turnData.sacHeal = 0;
    turnData.deathstrikeDamage = 0;
    turnData.masochism = 0;
    turnData.immunityValue = 0;
    turnData.deathBuffHP = 0;
    turnData.healFirst = 0;
    turnData.multiplier = 1;
    turnData.witchMult = 1;
    turnData.immunity5K = false ;
    turnData.turnDamp = turncounter ? false : true; //is it the first turn?

    switch (skillTypes[monstersLost]) {
        default:            break;
        case RESISTANCE:    turnData.resistance = 1 - skillAmounts[monstersLost];//Needs to be here so it happens before Neil's absorb.
                            break;
        case TURNDAMP:      if (turnData.turnDamp)
                            turnData.resistance = 1 - skillAmounts[monstersLost];
                            break;
        case SKILLDAMPEN:   turnData.skillDampen = 1 - skillAmounts[monstersLost];
                            break;
        case DODGE:         turnData.immunity5K = true ;
                            turnData.immunityValue = skillAmounts[monstersLost];
                            break;
        case SHIELDME:      turnData.protection += (int) skillAmounts[monstersLost] * (armySize - monstersLost - 1);
                            break;
        case SELFARMOR:     turnData.protection += skillAmounts[monstersLost];
                            break;
    }

    // Gather all skills that trigger globally
    for (i = monstersLost; i < armySize; i++) {
        turnData.aliveAtTurnStart[i] = remainingHealths[i] > 0;
        switch (skillTypes[i]) {
            default:        break;
            case PROTECT:   if (skillTargets[i] == ALL || skillTargets[i] == lineup[monstersLost]->element) {
                                turnData.protection += (int) skillAmounts[i];
                            } break;
            case BUFF:      if (skillTargets[i] == ALL || skillTargets[i] == lineup[monstersLost]->element) {
                                turnData.buffDamage += (int) skillAmounts[i];
                            } break;
            case BUFFUP:    if (turncounter % (lineup[i]->promo >= 5 ? 5 : 4) == 0 && turncounter != 0)//TODO: Change 5 to 3 when it's fixed
                                deathBuffATK += skillAmounts[i];
                            break;
            case CHAMPION:  if (skillTargets[i] == ALL || skillTargets[i] == lineup[monstersLost]->element) {
                                turnData.buffDamage += (int) skillAmounts[i];
                                turnData.protection += (int) skillAmounts[i];
                            } break;
            case HEAL:      turnData.healing += (int) skillAmounts[i];
                            break;
            case AOE:       turnData.aoeDamage += (int) skillAmounts[i];
                            break;
            case AOELIN:    turnData.aoeDamage += (int) (skillAmounts[i] + 2 * (turncounter + 1));
                            break;
            case AOEHP:     turnData.aoeDamage += (int) round(skillAmounts[i] * remainingHealths[i]);
                            break;
            case LIFESTEAL: turnData.aoeDamage += (int) skillAmounts[i];
                            turnData.healing += (int) skillAmounts[i];
                            break;
            case WBIDEAL:   turnData.aoeDamage  += (int) skillAmounts[i];
                            turnData.healing    += (int) skillAmounts[i];
                            turnData.buffDamage += (int) skillAmounts[i];
                            turnData.protection += (int) skillAmounts[i];
                            break;
            case DAMPEN:    turnData.dampFactor *= skillAmounts[i];
                            break;
            case ABSORB:    if (i != monstersLost) {
                                turnData.absorbMult = skillAmounts[i];
                                turnData.absorbID = i;
                            }
                            break;
            case SACRIFICE: turnData.sacHeal += (int) floor(skillAmounts[i] + 0.0001);
                            turnData.aoeDamage += (int) floor(skillAmounts[i] * 1.5 + 0.0001);//Add 0.0001 to fix discrepancy between js and c++ when c++ ends up with 1.9999...
                            turnData.masochism += (int) floor(skillAmounts[i] * 1.5 + 0.0001);
                            break;
            case AOELAST:   turnData.aoeLast += (int) round(skillAmounts[i] * (lineup[i]->damage + deathBuffATK));
                            break;
            case AOEFIRST:  turnData.aoeFirst += (int) skillAmounts[i];
                            break;
            case HEALFIRST: turnData.healFirst += (int) skillAmounts[i];
                            break;
            case PERCBUFF:  turnData.multiplier += skillAmounts[i];
                            turnData.witchMult += skillAmounts[i];
                            break;
            case FURY:      int cooldown;
                            switch (lineup[i]->rarity) {
                                case COMMON:
                                    cooldown = 5;
                                    break;
                                case RARE:
                                    cooldown = 6;
                                    break;
                                case LEGENDARY:
                                    cooldown = 7;
                                    break;
                                case ASCENDED:
                                    cooldown = 8;
                                    break;
                                default: throw std::logic_error("hero should not have FURY skill");
                            }
                            if ((turncounter + 1) % cooldown == 1 && turncounter != 0)
                                furyArray[i] = round((double)furyArray[i] * skillAmounts[i]);
                            break;
            case MORALE:    if (turncounter % 2 == 1){
                                int statBuff = 0;
                                for (int j = monstersLost; j < i ; j++){
                                    if (turnData.aliveAtTurnStart[j])
                                        statBuff++;
                                }
                                if (statBuff){
                                    statBuff *= skillAmounts[i];
                                    remainingHealths[i] += statBuff;
                                    maxHealths[i] += statBuff;
                                    furyArray[i] += statBuff;
                                }
                            }
                            break;
        }
    }
    if (passiveTypes[monstersLost] == ARMOR)
        turnData.protection = round(turnData.protection * (1 + passiveAmounts[monstersLost]));
}

// Handle all self-centered abilities and other multipliers on damage
// Protection needs to be calculated at this point.
inline void ArmyCondition::getDamage(const int turncounter, const ArmyCondition & opposingCondition) {

    turnData.baseDamage = lineup[monstersLost]->damage;

    turnData.opposingElement = opposingCondition.lineup[opposingCondition.monstersLost]->element;
    const int opposingProtection = opposingCondition.turnData.protection;
    const double opposingDampFactor = opposingCondition.turnData.dampFactor;
    const double opposingAbsorbMult = opposingCondition.turnData.absorbMult;
    const bool opposingImmunityDamage = opposingCondition.turnData.immunity5K;
    const double opposingDamage = opposingCondition.furyArray[opposingCondition.monstersLost];
    const double opposingResistance = opposingCondition.turnData.resistance;
    const double opposingSkillDampen = opposingCondition.turnData.skillDampen;
    const int opposingImmunityValue = opposingCondition.turnData.immunityValue;

    // Handle Monsters with skills that only activate on attack.
    turnData.trampleTriggered = false;
    turnData.explodeDamage = 0;
    turnData.valkyrieMult = 0;
    turnData.trampleMult = 0;
    turnData.tripleMult = 0;
    turnData.hate = 0; // same as above
    turnData.counter = 0;
    turnData.flatRef = 0;
    turnData.direct_target = 0;
    turnData.counter_target = 0;
    turnData.leech = 0;
    turnData.selfHeal = 0;
    turnData.execute = 0;
    turnData.guyActive = false;
    turnData.ricoActive = false;
    turnData.aoeReflect = 0;
    turnData.hpPierce = 0;
    turnData.sadism = 0;
    turnData.tetrisMult = 0;
    turnData.tetrisSeed = 0;
    turnData.bloodlust = 0;
    turnData.evolve = 0;

    // double friendsDamage = 0;

    switch (skillTypes[monstersLost]) {
        case FRIENDS:   for (int i = monstersLost + 1; i < armySize; i++) {
                            if (skillTypes[i] == NOTHING && remainingHealths[i] > 0)
                                turnData.baseDamage *= skillAmounts[monstersLost];
                        }
                        break;
        case TRAINING:  turnData.baseDamage += (int) (skillAmounts[monstersLost] * (double) turncounter);
                        break;
        case RAINBOW:   if (rainbowConditions[monstersLost]) {
                            turnData.baseDamage += (int) skillAmounts[monstersLost];
                        } break;
        case ADAPT:     if (turnData.opposingElement == skillTargets[monstersLost]) {
                            turnData.multiplier *= skillAmounts[monstersLost];
                        } break;
        case BERSERK:   if (lastBerserk)
                            turnData.valkyrieDamage = round((double)lastBerserk * skillAmounts[monstersLost]);
                        else
                            turnData.valkyrieDamage = turnData.baseDamage;
                        if (turnData.valkyrieDamage >= std::numeric_limits<int>::max())
                            turnData.baseDamage = static_cast<DamageType>(ceil(turnData.valkyrieDamage));
                        else
                            turnData.baseDamage = round(turnData.valkyrieDamage);
                        lastBerserk = turnData.baseDamage;
                        break;
        case VALKYRIE:  turnData.valkyrieMult = skillAmounts[monstersLost];
                        turnData.ricoActive = true;
                        break;
        case TRAMPLE:   turnData.trampleTriggered = true;
                        turnData.trampleMult = skillAmounts[monstersLost];
                        turnData.ricoActive = true;
                        break;
        case TRIPLE:    turnData.tripleMult = skillAmounts[monstersLost];
                        turnData.ricoActive = true;
                        break;
        case COUNTER:   turnData.counter = skillAmounts[monstersLost];
                        break;
        case FLATREF:   turnData.flatRef = skillAmounts[monstersLost];
                        break;
        case EXPLODE:   turnData.explodeDamage = skillAmounts[monstersLost]; // Explode damage gets added here, but still won't apply unless enemy frontliner dies
                        break;
        case DICE:      turnData.baseDamage += getTurnSeed(opposingCondition.seed, 100) % (int)(skillAmounts[monstersLost] + 1); // Only adds dice attack effect if dice is in front, max health is done before battle
                        break;
        // Pick a target, Bubbles currently dampens lux damage if not targeting first according to game code, interaction should be added if this doesn't change
        case LUX:       turnData.direct_target = getLuxTarget(opposingCondition, getTurnSeed(opposingCondition.seed, 99 -turncounter));
                        turnData.opposingElement = opposingCondition.lineup[turnData.direct_target]->element;
                        turnData.direct_target -= opposingCondition.monstersLost;
                        break;
        case CRIT:      turnData.multiplier *= getTurnSeed(opposingCondition.seed, 99 - turncounter) % 2 == 0 ? skillAmounts[monstersLost] : 1;
                        break;
        case HATE:      turnData.hate = skillAmounts[monstersLost];
                        break;
        case LEECH:     turnData.leech = skillAmounts[monstersLost];
                        break;
        case SELFHEAL:  turnData.selfHeal = skillAmounts[monstersLost];
                        break;
        case COUNTER_MAX_HP: turnData.counter = skillAmounts[monstersLost];
                        turnData.guyActive = true;
                        break;
        case EXECUTE:   turnData.execute = skillAmounts[monstersLost] * opposingSkillDampen * opposingCondition.maxHealths[opposingCondition.monstersLost];
                        break;
        case FLATEXEC:  turnData.execute = skillAmounts[monstersLost] * opposingSkillDampen;
                        break;
        case AOEREFLECT: turnData.aoeReflect = skillAmounts[monstersLost];
                        break;
        case HPPIERCE:  if (!opposingCondition.worldboss)
                        turnData.hpPierce = opposingCondition.maxHealths[opposingCondition.monstersLost] * skillAmounts[monstersLost];
                        break;
        case POSBONUS:  turnData.baseDamage += round(skillAmounts[monstersLost] * (armySize - monstersLost - 1));
                        break;
        case VOID:      if (lineup[monstersLost]->element != turnData.opposingElement)
                            turnData.multiplier *= skillAmounts[monstersLost] + 1;
                        break;
        case SADISM:    turnData.sadism += round(skillAmounts[monstersLost] * (turnData.baseDamage - evolveTotal));
                        turnData.aoeDamage += round(skillAmounts[monstersLost] * (turnData.baseDamage - evolveTotal));
                        break;
        case COURAGE:   turnData.buffDamage *= skillAmounts[monstersLost];
                        break;
        case EASTER:    if (easterCheck)
                            turnData.baseDamage += round(skillAmounts[monstersLost] * 50 / 134);
                        break;
        case BULLSHIT:  turnData.tetrisSeed = getTurnSeed(opposingCondition.seed, 99 - turncounter) % 6;
                        turnData.tetrisMult = skillAmounts[monstersLost];
                        turnData.ricoActive = true;
                        turnData.tetrisDamage = (turnData.baseDamage + turnData.buffDamage + deathBuffATK + evolveTotal) * turnData.tetrisMult * turnData.multiplier;
                        //Frontliner part goes here for more accurate damage.
                        switch(turnData.tetrisSeed){
                            case 0: turnData.multiplier *= 2 * turnData.tetrisMult;
                                    break;
                            case 1: turnData.multiplier *= 1.25 * turnData.tetrisMult;
                                    break;
                            case 2: turnData.multiplier *= 1.75 * turnData.tetrisMult;
                                    break;
                            case 3: turnData.multiplier *= 1.25 * turnData.tetrisMult;
                                    break;
                            case 4: turnData.multiplier *= 1.25 * turnData.tetrisMult;
                                    break;
                            case 5: turnData.multiplier *= 1.5 * turnData.tetrisMult;
                                    break;
                        }
                        break;
        case ATTACKAOE: turnData.aoeDamage += skillAmounts[monstersLost];
                        break;
        case HPAMPLIFY: turnData.aoeFirst += round(skillAmounts[monstersLost] * remainingHealths[monstersLost]);
                        break;
        case FLATHEAL:  turnData.healFirst += skillAmounts[monstersLost];
                        break;
        case CONVERT:   turnData.baseDamage -= round(lineup[monstersLost]->damage * (7 - armySize + monstersLost) * 0.1);
                        break;
        case FURY:      turnData.baseDamage = furyArray[monstersLost];
                        break;
        case BLOODLUST: turnData.bloodlust += skillAmounts[monstersLost];
                        break;
        case MORALE:    turnData.baseDamage = furyArray[monstersLost];
                        break;
        default:        break;

    }

    //Linear buffs
    turnData.valkyrieDamage = turnData.baseDamage + deathBuffATK + evolveTotal;// Get Base damage (deathBuff from Fairies, evolveTotal for Clio/Gladiator buff)

    if (turnData.buffDamage != 0) {
        turnData.valkyrieDamage += round(turnData.buffDamage * (passiveTypes[monstersLost] == DAMAGE ? (1 + passiveAmounts[monstersLost]) : 1));
    }

    //Multiplicative buffs
    if (passiveTypes[monstersLost] == DPS) {
        turnData.valkyrieDamage *= 1 + passiveAmounts[monstersLost];
    }

    if (opposingCondition.skillTypes[opposingCondition.monstersLost] == EVOLVE){
        turnData.evolve = round(turnData.valkyrieDamage * turnData.witchMult * opposingCondition.skillAmounts[opposingCondition.monstersLost]);//Has to be here for Clio.
    }

    if (turnData.multiplier > 1) {
        turnData.valkyrieDamage *= turnData.multiplier;
    }

    if (counter[turnData.opposingElement] == lineup[monstersLost]->element && lineup[monstersLost]->element != ALL) {
        turnData.valkyrieDamage *= elementalBoost + turnData.hate;
    }

    //Save full damage value so it is not affected by armor and absorb and other individual unit abilities.
    double ricoValue = turnData.valkyrieDamage;

    //Unique linear buff that happens independant of other buffs
    if (turnData.hpPierce)
        turnData.valkyrieDamage += turnData.hpPierce;

    //Multiplicative debuffs
    if (opposingResistance < 1)
        turnData.valkyrieDamage *= opposingResistance;

    if (opposingCondition.passiveTypes[opposingCondition.monstersLost] == AFFINITY && turnData.opposingElement == lineup[monstersLost]->element)
        turnData.valkyrieDamage *= 1 - opposingCondition.passiveAmounts[opposingCondition.monstersLost];

    //Linear debuffs
    if (turnData.valkyrieDamage > opposingProtection) { 
        turnData.valkyrieDamage -= (double) opposingProtection;
    } else {
        turnData.valkyrieDamage = 0;
    }

    //absorb damage, damage rounded up later
    if (opposingAbsorbMult != 0 && turnData.direct_target == 0) {
        turnData.absorbDamage = turnData.valkyrieDamage * opposingAbsorbMult;
        turnData.valkyrieDamage = turnData.valkyrieDamage - turnData.absorbDamage;
    }
    //leech healing based on damage dealt
    if (turnData.leech != 0)
        turnData.healFirst += round(turnData.leech * turnData.valkyrieDamage);
    //Check execute before resolve damage for reflect ability, neil absorbs damage before the execute, according to replays.
    if (turnData.execute && !opposingCondition.worldboss && ((double)(opposingCondition.remainingHealths[opposingCondition.monstersLost] - round(turnData.valkyrieDamage)) <= turnData.execute)) {
        if (turnData.valkyrieDamage < opposingCondition.remainingHealths[opposingCondition.monstersLost] + 1)
            turnData.valkyrieDamage = opposingCondition.remainingHealths[opposingCondition.monstersLost] + 1;
    }
    // for compiling heavyDamage version
    if (turnData.valkyrieDamage >= std::numeric_limits<int>::max())
        turnData.baseDamage = static_cast<DamageType>(ceil(turnData.valkyrieDamage));
    else
        // turnData.baseDamage = castCeil(turnData.valkyrieDamage);
        turnData.baseDamage = round(turnData.valkyrieDamage);

    turnData.valkyrieDamage = ricoValue;//Damage for ricochet skill.

    // Handle enemy dampen ability and reduce aoe effects
    if (opposingDampFactor < 1) {
        // turnData.explodeDamage = castCeil((double) turnData.explodeDamage * opposingDampFactor);
        // turnData.aoeDamage = castCeil((double) turnData.aoeDamage * opposingDampFactor);
        // turnData.healing = castCeil((double) turnData.healing * opposingDampFactor);
        turnData.explodeDamage = round((double) turnData.explodeDamage * opposingDampFactor);
        turnData.aoeDamage = round((double) turnData.aoeDamage * opposingDampFactor);
        turnData.healing = round((double) turnData.healing * opposingDampFactor);
        turnData.healFirst = round((double) turnData.healFirst * opposingDampFactor);
        turnData.sacHeal = round((double) turnData.sacHeal * opposingDampFactor);//Have to check if Bubbles affects it
        turnData.aoeLast = round((double) turnData.aoeLast * opposingDampFactor);
        turnData.aoeFirst = round((double) turnData.aoeFirst * opposingDampFactor);
        turnData.tripleMult *= opposingDampFactor;
    }

    //Damage negation (used by Doyenne WB)
    if( opposingImmunityDamage && (turnData.valkyrieDamage >= opposingImmunityValue || turnData.baseDamage >= opposingImmunityValue ) ) {
        turnData.valkyrieDamage = 0 ;
        turnData.baseDamage = 0 ;
    }

    //Check gladiators before resolve damage to make sure there is no left-right discrepancy. Buff takes place before delayed abilities but after AoE.
    if (!(turnData.bloodlust && !opposingCondition.worldboss && opposingCondition.passiveTypes[opposingCondition.monstersLost] != ANGEL && ((double)(opposingCondition.remainingHealths[opposingCondition.monstersLost] - round(turnData.valkyrieDamage) - turnData.aoeDamage - turnData.aoeFirst) <= 0)))
        turnData.bloodlust = 0;

}
//in case of Ricochet, apply armor to everyone.
inline void ArmyCondition::applyArmor(TurnData & opposing) {
    if (opposing.ricoActive) {
        for (int i = monstersLost + 1; i < armySize; i++) {
            turnData.armorArray[i] = 0;
            for (int j = monstersLost; j < armySize; j++) {
                if ((skillTypes[j] == PROTECT || skillTypes[j] == CHAMPION || skillTypes[j] == WBIDEAL) && (skillTargets[j] == ALL || skillTargets[j] == lineup[i]->element))
                    turnData.armorArray[i] += (int) skillAmounts[j];
            }
        }
    }
}
// Add damage to the opposing side and check for deaths
inline void ArmyCondition::resolveDamage(TurnData & opposing) {
    int frontliner = monstersLost; // save original frontliner
    int armoredRicochetValue; //So the ricochet doesn't heal due to armor
    double tempResistance; //Needed for frosty to dampen ricochet
    int aoeConst = 0; //AoE that is not affected by SKILLDAMPEN

    // Apply normal attack damage to the frontliner
    // If direct_target is non-zero that means Lux is hitting something not the front liner
    remainingHealths[frontliner + opposing.direct_target] -= opposing.baseDamage;

    // Lee and Fawkes can only counter if they are hit directly, so if they are opposing Lux and Lux
    // hits another units, they do not counter
    int counter_eligible = 1;
    if(skillTypes[monstersLost] == LUX && turnData.direct_target > 0) {
      counter_eligible = 0;
      // std::cout << "LUX DID NOT HIT FRONTLINER" << std::endl;
    }

    // Neil takes damage from absorb before Ricochet and fairy buffs take place, needs a check for being alive after that
    if (turnData.absorbID != -1 && counter_eligible) {
        remainingHealths[turnData.absorbID] -= round(opposing.absorbDamage);
        turnData.aliveAtTurnStart[turnData.absorbID] = remainingHealths[turnData.absorbID] > 0;
    }

//One unit behind by S4 units + Raze
    if (opposing.trampleTriggered) {
        for (int i = frontliner + 1; i < armySize; i++)
            if (remainingHealths[i] > 0){
                if (skillTypes[i] == RESISTANCE || (skillTypes[i] == TURNDAMP && turnData.turnDamp))
                    tempResistance = 1 - skillAmounts[i];
                else
                    tempResistance = 1;
                armoredRicochetValue = round(opposing.valkyrieDamage * opposing.trampleMult * tempResistance * turnData.dampFactor) - turnData.armorArray[i];
                if (armoredRicochetValue > 0)
                    remainingHealths[i] -= armoredRicochetValue;
                break;
            }
    }

//Three units behind hit by Smith
    if (opposing.tripleMult) {
        int times = 3;
        double baseMult = opposing.tripleMult;
        for (int i = frontliner + 1; i < armySize; i++)
            if (remainingHealths[i] > 0){
                if (skillTypes[i] == RESISTANCE || (skillTypes[i] == TURNDAMP && turnData.turnDamp))
                    tempResistance = 1 - skillAmounts[i];
                else
                    tempResistance = 1;
                armoredRicochetValue = round(opposing.valkyrieDamage * opposing.tripleMult * tempResistance) - turnData.armorArray[i];
                if (armoredRicochetValue > 0)
                    remainingHealths[i] -= armoredRicochetValue;
                times--;
                if (!times)
                    break;
                opposing.tripleMult *= baseMult;
            }
    }

//~~~~~~~~~~~~~~~~Tetris Skill Start ~~~~~~~~~~~~~~~~~~~~

//I know there must be a better way, but good grief, this skill is not good for the old battle system.
//Code copied from Trample (S4 ability)
    if (opposing.tetrisMult) {
        int times = 0;
        switch(opposing.tetrisSeed){
            //Nothing happens, all in first unit
            case 0: break;
            //3 units in the back affected
            case 1: times = 3;
                    for (int i = frontliner + 1; i < armySize; i++)
                        if (remainingHealths[i] > 0){
                            if (skillTypes[i] == RESISTANCE || (skillTypes[i] == TURNDAMP && turnData.turnDamp))
                                tempResistance = 1 - skillAmounts[i];
                            else
                                tempResistance = 1;
                            armoredRicochetValue = round(0.25 * opposing.tetrisDamage * tempResistance * turnData.dampFactor * (counter[lineup[i]->element] == turnData.opposingElement ? 1.5 : 1)) - turnData.armorArray[i];
                            if (armoredRicochetValue > 0)
                                remainingHealths[i] -= armoredRicochetValue;
                            times--;
                            opposing.tetrisDamage *= turnData.dampFactor;
                            if (!times)
                                break;
                        }
                    break;
            //Only one unit behind the frontliner
            case 2: for (int i = frontliner + 1; i < armySize; i++)
                        if (remainingHealths[i] > 0){
                            if (skillTypes[i] == RESISTANCE || (skillTypes[i] == TURNDAMP && turnData.turnDamp))
                                tempResistance = 1 - skillAmounts[i];
                            else
                                tempResistance = 1;
                            armoredRicochetValue = round(0.25 * opposing.tetrisDamage * tempResistance * turnData.dampFactor * (counter[lineup[i]->element] == turnData.opposingElement ? 1.5 : 1)) - turnData.armorArray[i];
                            if (armoredRicochetValue > 0)
                                remainingHealths[i] -= armoredRicochetValue;
                            break;
                        }
                    break;
            //Only one unit behind the frontliner
            case 3: for (int i = frontliner + 1; i < armySize; i++)
                        if (remainingHealths[i] > 0){
                            if (skillTypes[i] == RESISTANCE || (skillTypes[i] == TURNDAMP && turnData.turnDamp))
                                tempResistance = 1 - skillAmounts[i];
                            else
                                tempResistance = 1;
                            armoredRicochetValue = round(0.75 * opposing.tetrisDamage * tempResistance * turnData.dampFactor * (counter[lineup[i]->element] == turnData.opposingElement ? 1.5 : 1)) - turnData.armorArray[i];
                            if (armoredRicochetValue > 0)
                                remainingHealths[i] -= armoredRicochetValue;
                            break;
                        }
                    break;
            //Two units behind, at 50% and 25%
            case 4: times = 1;
                    for (int i = frontliner + 1; i < armySize; i++)
                        if (remainingHealths[i] > 0){
                            if (skillTypes[i] == RESISTANCE || (skillTypes[i] == TURNDAMP && turnData.turnDamp))
                                tempResistance = 1 - skillAmounts[i];
                            else
                                tempResistance = 1;
                            armoredRicochetValue = round(0.5 * opposing.tetrisDamage * tempResistance * turnData.dampFactor * (counter[lineup[i]->element] == turnData.opposingElement ? 1.5 : 1) / times) - turnData.armorArray[i];
                            if (armoredRicochetValue > 0)
                                remainingHealths[i] -= armoredRicochetValue;
                            times++;
                            opposing.tetrisDamage *= turnData.dampFactor;
                            if (times == 3)
                                break;
                        }
                    break;
            //Only one unit behind the frontliner
            case 5: for (int i = frontliner + 1; i < armySize; i++)
                        if (remainingHealths[i] > 0){
                            if (skillTypes[i] == RESISTANCE || (skillTypes[i] == TURNDAMP && turnData.turnDamp))
                                tempResistance = 1 - skillAmounts[i];
                            else
                                tempResistance = 1;
                            armoredRicochetValue = round(0.5 * opposing.tetrisDamage * tempResistance * turnData.dampFactor * (counter[lineup[i]->element] == turnData.opposingElement ? 1.5 : 1)) - turnData.armorArray[i];
                            if (armoredRicochetValue > 0)
                                remainingHealths[i] -= armoredRicochetValue;
                            break;
                        }
                    break;
        }
    }

//~~~~~~~~~~~~~~~~Tetris Skill End ~~~~~~~~~~~~~~~~~~~~

    if (opposing.explodeDamage != 0 && remainingHealths[frontliner] <= 0 && !worldboss) {
        // std::cout << "EXPLODE" << std::endl;
        opposing.aoeDamage += opposing.explodeDamage;
    }

    if (opposing.aoeReflect)
        aoeConst += std::max((int)round(turnData.baseDamage * opposing.aoeReflect),1);

    if (opposing.aoeLast) {
        for (int i = armySize - 1; i >= frontliner; i--) {
            if (remainingHealths[i] > 0 || worldboss){ //Check for last alive unit
                if (skillTypes[i] == SKILLDAMPEN)
                    remainingHealths[i] -= round(opposing.aoeLast * (1 - skillAmounts[i]));
                else
                    remainingHealths[i] -= opposing.aoeLast;
                break;
            }
        }
    }

    if (opposing.aoeFirst) {
        if (skillTypes[frontliner] == SKILLDAMPEN)
            remainingHealths[frontliner] -= round(opposing.aoeFirst * (1 - skillAmounts[frontliner]));
        else
            remainingHealths[frontliner] -= opposing.aoeFirst;
    }

    // Handle aoe Damage for all combatants
    for (int i = frontliner; i < armySize; i++) {
      int aliveAtBeginning = 0;
      if(remainingHealths[i] > 0 || i == frontliner) {
        aliveAtBeginning = 1;
      }
      remainingHealths[i] -= round(opposing.aoeDamage * (1 - (passiveTypes[i] == ANTIMAGIC ? passiveAmounts[i] : skillTypes[i] == SKILLDAMPEN ? skillAmounts[i] : 0)));
      remainingHealths[i] -= aoeConst;

      if (skillTypes[i] == SACRIFICE)
        remainingHealths[i] -= turnData.masochism;

      if (i > frontliner){
        if (opposing.valkyrieDamage) { // Aoe that doesnt affect the frontliner
          if (skillTypes[i] == RESISTANCE || (skillTypes[i] == TURNDAMP && turnData.turnDamp))
              tempResistance = 1 - skillAmounts[i];
          else
              tempResistance = 1;
          armoredRicochetValue = round(opposing.valkyrieDamage * tempResistance) - turnData.armorArray[i];
          if (armoredRicochetValue > 0)
              remainingHealths[i] -= armoredRicochetValue;
        }
        if (turnData.sadism)
          remainingHealths[i] -= turnData.sadism;
      }

//Check for death defying skills, death and death skills.
    if (passiveTypes[i] == ANGEL && remainingHealths[i] <= 0 && turnData.aliveAtTurnStart[i]){
        remainingHealths[i] = round((double)maxHealths[i] * passiveAmounts[i]);
        passiveTypes[i] = NONE;
    }

      if (remainingHealths[i] <= 0 && !worldboss) {
        if (i == monstersLost) {
          monstersLost++;
          lastBerserk = 0;
          evolveTotal = 0;
        }
        //Save revenge values
        switch (skillTypes[i]) {
            case REVENGE:
                turnData.aoeRevenge += (int) round((double) (lineup[i]->damage + deathBuffATK) * skillAmounts[i]);
                break;
            case DEATHSTRIKE:
                turnData.deathstrikeDamage += skillAmounts[i];
                break;
            case DEATHBUFF:
                turnData.deathBuffHP = (int) round(skillAmounts[i] * maxHealths[i]);
                for (int j = monstersLost; j < i; j++)
                    if (remainingHealths[j] > 0){
                        remainingHealths[j] += turnData.deathBuffHP;
                        maxHealths[j] += turnData.deathBuffHP;
                    }
                for (int j = i + 1; j < armySize; j++)
                    if (turnData.aliveAtTurnStart[j]){
                        remainingHealths[j] += turnData.deathBuffHP;
                        maxHealths[j] += turnData.deathBuffHP;
                    }
                deathBuffATK += (int) round(skillAmounts[i] * (lineup[i]->damage + deathBuffATK));
                break;
            default:
                break;
        }
        skillTypes[i] = NOTHING; // disable dead hero's ability
      } else {
            remainingHealths[i] += round((turnData.healing + //AoE heal
            (skillTypes[i] != SACRIFICE ? turnData.sacHeal : 0) + //Prevent sacrifice from working on the unit that sacrifices their health
            (i == frontliner ? (round(turnData.selfHeal * opposing.baseDamage) + turnData.healFirst) : 0)) * //Front healing
            (passiveTypes[i] == HEALPLUS ? (1 + passiveAmounts[i]) : 1));//have to put them together, because P6 requires all of them to be together to avoid rounding errors.
            if (remainingHealths[i] > maxHealths[i]) { // Avoid overhealing
                remainingHealths[i] = maxHealths[i];
        }
      }

      // Always apply the valkyrieMult if it is zero. Otherwise, given the way
      // that riochet is implemented it will cause melee attacks to turn into
      // ricochet
      if(opposing.valkyrieMult > 0) {
        // Only reduce the damage if it hit an alive unit
        if(aliveAtBeginning) {
          opposing.valkyrieDamage *= opposing.valkyrieMult * turnData.dampFactor;
        }
      }  else {
        opposing.valkyrieDamage *= opposing.valkyrieMult;
      }
    }
    // Handle wither ability
    if (skillTypes[monstersLost] == WITHER && monstersLost == frontliner) {
        // remainingHealths[monstersLost] = castCeil((double) remainingHealths[monstersLost] * skillAmounts[monstersLost]);
        remainingHealths[monstersLost] = round(remainingHealths[monstersLost] * ( 1 -((double) 1 / skillAmounts[monstersLost]) ));//Updated for promotion values.
    }

    // Delayed abilities start here. Happen after direct damage and AoE. Can carry over to the next unit
    if (monstersLost < armySize){
        //Reflect
        if (opposing.counter && counter_eligible){
            // Finding Guy's target
            if(opposing.guyActive)
                opposing.counter_target = findMaxHP();
            // Add opposing.counter_target to handle fawkes not targetting the frontliner
            remainingHealths[monstersLost + opposing.counter_target] -= static_cast<int64_t>(round(turnData.baseDamage * opposing.counter));
        }
        if (opposing.flatRef && counter_eligible){
            remainingHealths[monstersLost] -= opposing.flatRef;
        }
        //Clio buff
        if(opposing.evolve && remainingHealths[frontliner] > 0){
            evolveTotal += opposing.evolve;
        } //Gladiator buff
        else if(turnData.bloodlust && remainingHealths[monstersLost] > 0){
            evolveTotal += turnData.bloodlust;
            maxHealths[monstersLost] += turnData.bloodlust;
            remainingHealths[monstersLost] += turnData.bloodlust;
        }
        //If a delayed ability killed a frontliner, find next frontliner. Same procedure as when applying aoe.
        for (int i = monstersLost; i < armySize; i++){
            if (remainingHealths[i] <= 0 && !worldboss) {
                if (i == monstersLost) {
                    monstersLost++;
                    lastBerserk = 0;
                    evolveTotal = 0;
                }
                //Save revenge values
                switch (skillTypes[i]) {
                    case REVENGE:
                        turnData.aoeRevenge += (int) round((double) (lineup[i]->damage + deathBuffATK) * skillAmounts[i]);
                        break;
                    case DEATHSTRIKE:
                        turnData.deathstrikeDamage += skillAmounts[i];
                        break;
                    case DEATHBUFF:
                        turnData.deathBuffHP = (int) round(skillAmounts[i] * maxHealths[i]);
                        for (int j = monstersLost; j < i; j++)
                            if (remainingHealths[j] > 0){
                                remainingHealths[j] += turnData.deathBuffHP;
                                maxHealths[j] += turnData.deathBuffHP;
                            }
                        for (int j = i + 1; j < armySize; j++)
                            if (turnData.aliveAtTurnStart[j]){
                                remainingHealths[j] += turnData.deathBuffHP;
                                maxHealths[j] += turnData.deathBuffHP;
                            }
                        deathBuffATK += (int) round(skillAmounts[i] * (lineup[i]->damage + deathBuffATK));
                    default:
                        break;
                }
                skillTypes[i] = NOTHING; // disable dead hero's ability
            }
        }
    }

}
//Apply revenge abilities, check for dead units and check for more revenge procs
inline void ArmyCondition::resolveRevenge(TurnData & opposing) {
    if (opposing.aoeRevenge || opposing.deathstrikeDamage) {
        //Billy's single target Revenge
        remainingHealths[monstersLost] -= opposing.deathstrikeDamage;
        opposing.deathstrikeDamage = 0;
        //Revenge that affects all units
        for (int i = monstersLost; i < armySize; i++) {
            //Revenge AoE
            remainingHealths[i] -= opposing.aoeRevenge;
            //Check for dead units
            if (remainingHealths[i] <= 0 && !worldboss) {
                if (i == monstersLost) {
                    monstersLost++;
                    lastBerserk = 0;
                    evolveTotal = 0;
                }
                //Save Revenge values
                switch (skillTypes[i]) {
                    case REVENGE:
                        turnData.aoeRevenge += (int) round((double) (lineup[i]->damage + deathBuffATK) * skillAmounts[i]);
                        break;
                    case DEATHSTRIKE:
                        turnData.deathstrikeDamage += skillAmounts[i];
                        break;
                    case DEATHBUFF:
                        turnData.deathBuffHP = (int) round(skillAmounts[i] * maxHealths[i]);
                        for (int j = monstersLost; j < i; j++)
                            if (remainingHealths[j] > 0){
                                remainingHealths[j] += turnData.deathBuffHP;
                                maxHealths[j] += turnData.deathBuffHP;
                            }
                        for (int j = i + 1; j < armySize; j++)
                            if (turnData.aliveAtTurnStart[j]){
                                remainingHealths[j] += turnData.deathBuffHP;
                                maxHealths[j] += turnData.deathBuffHP;
                            }
                        deathBuffATK += (int) round(skillAmounts[i] * (lineup[i]->damage + deathBuffATK));
                    default:
                        break;
                }
                skillTypes[i] = NOTHING; // disable dead hero's ability
            }
        }
        opposing.aoeRevenge = 0;
    }
}
//Find highest HP unit for Guy's reflect.
inline int ArmyCondition::findMaxHP() {
    // go through alive monsters to determine most hp
    // If there is a tie...
    // Use the same loop as in ResolveDamage
    int max_hp = 0;
    int index_max_hp = 0;
    for (int i = monstersLost; i < armySize; i++) {
        if(remainingHealths[i] > max_hp) {
            max_hp = remainingHealths[i];
            index_max_hp = i;
        }
    }
    // This is an absolute index, the rest of the code expects a relative index
    // so return it relative to the starting point
    // std::cout << std::endl << "Choosing " << index_max_hp << " with " << remainingHealths[index_max_hp] << std::endl;
    return index_max_hp - monstersLost;
}

inline int ArmyCondition::getLuxTarget(const ArmyCondition & opposingCondition, int64_t seed) {
  /* Javascript code from 3.2.0.4
  function shuffleBySeed(arr, seed) {
    for (var size = arr.length, mapa = new Array(size), x = 0; x < size; x++) mapa[x] = (seed = (9301 * seed + 49297) % 233280) / 233280 * size | 0;
    for (var i = size - 1; i > 0; i--) arr[i] = arr.splice(mapa[size - 1 - i], 1, arr[i])[0]
}
  called like `else if ("rtrg" == skill.type) shuffleBySeed(turn.atk.damageFactor, seed);`
  damageFactor appears to be initialized to an array of one element consisting of [1]
  function getTurnData(AL, BL) {
    var df = [1];
    ...
    damageFactor: df,
    ...
  So looking at things that probably means that we need an array of size the of number alive monsters
  with the first element initialized to 1
  815500433
  */

  // Count the number of alive monsters;
  int alive_count = 0;
  for(int i = 0; i < opposingCondition.armySize; i++) {
    // New CQ code removes dead units, so simulate that here by checking for health
    // std::cout << i << ". Remaining health is " << opposingCondition.remainingHealths[i] << std::endl;
    if(opposingCondition.remainingHealths[i] > 0) {
      alive_count++;
    }
  }
  // std::cout << "Alive count is " << alive_count << std::endl;

  // Initialize the arr equivalent
  int arr[alive_count];
  arr[0] = 1;
  for(int i = 1; i < alive_count; i++) {
    arr[i] = 0;
  }

  // Initialize mapa
  int mapa[alive_count];
  for(int x = 0; x < alive_count; x++) {
    // mapa[x] = (seed = (9301 * seed + 49297) % 233280) / 233280 * alive_count | 0;
    seed = (9301 * seed + 49297) % 233280;
    // std::cout << "Seed is " << seed << std::endl;
    // From debugging the JS code the in between operation being a double is important
    double temp = seed;
    temp = temp/ 233280;
    //std::cout << "Temp is " << temp << std::endl;
    temp = temp * alive_count;
    //std::cout << "Temp is " << temp << std::endl;
    temp = int64_t(temp) | 0;
    //std::cout << "Temp is " << temp << std::endl;
    mapa[x] = temp;
    // std::cout << x << " => " << mapa[x] << std::endl << std::endl;
  }

  /* std::cout << "IN:";
  for(int j = 0; j < alive_count ; j++) {
    std::cout << arr[j] << ",";
  }
  std::cout << std::endl;
*/
  // Shuffle
  for (int i = alive_count - 1; i > 0; i--) {
    // arr[i] = arr.splice(mapa[size - 1 - i], 1, arr[i])[0]
    // https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/splice
    // array.splice(start[, deleteCount[, item1[, item2[, ...]]]])
    // so at the value contained in mapa[size -1 -i] in arr, remove one element, put the value
    // at arr[i] in its place and set arr[i] to the deleted value

    int mapa_index  = alive_count - 1 - i;
    int index_to_remove = mapa[mapa_index];
    int removed_value = arr[index_to_remove];
    // overwrite the removed value
    // std::cout << "Mapa index: " << mapa_index << std::endl;
    arr[index_to_remove] = arr[i];
    arr[i] = removed_value;

    /*
    for(int j = 0; j < alive_count ; j++) {
      std::cout << arr[j] << ",";
    }
    std::cout << std::endl;
    */
  }

  // now figure out which target to return
  int return_value = -1;
  for(int i = 0; i < alive_count; i++) {
    // std::cout << "Index " << i << " has value " << arr[i] << std::endl;
    if(arr[i] > 0) {
      // std::cout << "Setting return_value to " << i << std::endl;
      return_value = i;
    }
  }
int actual_target = opposingCondition.monstersLost;
//Moving the function that selects the alive enemy here, as it is needed for elemental damage and neil absorb check.
    if (return_value > 0) {
        int alive = 0;
        for (int i = opposingCondition.monstersLost + 1; i < opposingCondition.armySize; i++) {
            // std::cout << "I is " << i << std::endl;
            if(opposingCondition.remainingHealths[i] > 0) {
                alive++;
                // std::cout << "Alive incremented to " << alive << std::endl;
            }
            if (alive >= return_value) {
                actual_target = i;
                // std::cout << "Setting actual target to " << i << std::endl;
                break;
            }
        }
    }
  return actual_target;
}
// Simulates One fight between 2 Armies and writes results into left's LastFightData
inline bool simulateFight(Army & left, Army & right, bool verbose = false) {
    // left[0] and right[0] are the first monsters to fight
    ArmyCondition leftCondition;
    ArmyCondition rightCondition;

    int turncounter;
    // Ignore lastFightData if either army-affecting heroes were added or for debugging
    if (left.lastFightData.valid && !verbose) {
        // Set pre-computed values to pick up where we left off
        leftCondition.init(left, left.monsterAmount-1, left.lastFightData.leftAoeDamage);
        rightCondition.init(right, left.lastFightData.monstersLost, left.lastFightData.rightAoeDamage);
        // Check if the new addition died to Aoe
        if (leftCondition.remainingHealths[leftCondition.monstersLost] <= 0) {
            leftCondition.monstersLost++;
        }

        rightCondition.remainingHealths[rightCondition.monstersLost] = left.lastFightData.frontHealth;
        rightCondition.lastBerserk        = left.lastFightData.berserk;
        turncounter                        = left.lastFightData.turncounter;
    } else {
        // Load Army data into conditions
        leftCondition.init(left, 0, 0);
        rightCondition.init(right, 0, 0);

        //----- turn zero -----

        // Apply Dicemaster max health bonus here, attack bonus applied during battle
        if (leftCondition.dice > -1) {
            leftCondition.maxHealths[leftCondition.dice] += getTurnSeed(rightCondition.seed, 100) % ((int)leftCondition.skillAmounts[leftCondition.dice] + 1);
            leftCondition.remainingHealths[leftCondition.dice] = leftCondition.maxHealths[leftCondition.dice];
        }

        if (rightCondition.dice > -1) {
            rightCondition.maxHealths[rightCondition.dice] += getTurnSeed(leftCondition.seed, 100) % ((int)rightCondition.skillAmounts[rightCondition.dice] + 1);
            rightCondition.remainingHealths[rightCondition.dice] = rightCondition.maxHealths[rightCondition.dice];
        }

        // Apply Leprechaun's skill (Beer)
        if (leftCondition.booze && leftCondition.armySize < (rightCondition.armySize + leftCondition.boozeValue)){
            int aoeValue = 0;
            for (size_t i = 0; i < ARMY_MAX_SIZE; ++i) {
                rightCondition.remainingHealths[i] = round((double)(rightCondition.maxHealths[i] * leftCondition.armySize) / (rightCondition.armySize + leftCondition.boozeValue));
                if (rightCondition.dampZero < 1 || rightCondition.skillTypes[i] == SKILLDAMPEN){
                    aoeValue = (rightCondition.maxHealths[i] - rightCondition.remainingHealths[i]) * rightCondition.dampZero;
                    if(rightCondition.skillTypes[i] == SKILLDAMPEN)
                        aoeValue *=  1 - rightCondition.skillAmounts[i];
                    aoeValue = round(aoeValue);
                    rightCondition.remainingHealths[i] = rightCondition.maxHealths[i] - aoeValue;
                }
            }
        }

        if (rightCondition.booze && rightCondition.armySize < (leftCondition.armySize + rightCondition.boozeValue)){
            int aoeValue = 0;
            for (size_t i = 0; i < ARMY_MAX_SIZE; ++i) {
                leftCondition.remainingHealths[i] = round((double)(leftCondition.maxHealths[i] * rightCondition.armySize) / (leftCondition.armySize + rightCondition.boozeValue));
                if (leftCondition.dampZero < 1 || leftCondition.skillTypes[i] == SKILLDAMPEN){
                    aoeValue = (leftCondition.maxHealths[i] - leftCondition.remainingHealths[i]) * leftCondition.dampZero;
                    if(leftCondition.skillTypes[i] == SKILLDAMPEN)
                        aoeValue *= 1 - leftCondition.skillAmounts[i];
                    aoeValue = round(aoeValue);
                    leftCondition.remainingHealths[i] = leftCondition.maxHealths[i] - aoeValue;
                }
            }
        }

        // Reset Potential values in fightresults
        left.lastFightData.leftAoeDamage = 0;
        left.lastFightData.rightAoeDamage = 0;
        turncounter = 0;

        // Apply Xmas common AOE
        if (leftCondition.aoeLow || rightCondition.aoeLow) {
            int target;
            TurnData aoeLowHP;
            if (leftCondition.aoeLow){
                target = 0;
                for (int i = 1; i < rightCondition.armySize; ++i){
                    if(rightCondition.remainingHealths[i] < rightCondition.remainingHealths[target])
                        target = i;
                }
                rightCondition.remainingHealths[target] -= leftCondition.aoeLow;
                aoeLowHP.aoeDamage = 0;
                rightCondition.resolveDamage(aoeLowHP);
            }
            if (rightCondition.aoeLow){
                target = 0;
                for (int i = 1; i < leftCondition.armySize; ++i){
                    if(leftCondition.remainingHealths[i] < leftCondition.remainingHealths[target])
                        target = i;
                }
                leftCondition.remainingHealths[target] -= rightCondition.aoeLow;
                aoeLowHP.aoeDamage = 0;
                leftCondition.resolveDamage(aoeLowHP);
            }
        }

        // Apply Xmas rare AOE
        if (leftCondition.flatLep || rightCondition.flatLep) {
            TurnData turnFlatLep;
            if (leftCondition.flatLep && leftCondition.armySize < rightCondition.armySize) {
                leftCondition.flatLep = round(rightCondition.dampZero * leftCondition.flatLep * (rightCondition.armySize - leftCondition.armySize));
                left.lastFightData.rightAoeDamage += leftCondition.flatLep;
                turnFlatLep.aoeDamage = leftCondition.flatLep;
                rightCondition.resolveDamage(turnFlatLep);
            }
            if (rightCondition.flatLep && rightCondition.armySize < leftCondition.armySize) {
                rightCondition.flatLep = round(leftCondition.dampZero * rightCondition.flatLep * (leftCondition.armySize - rightCondition.armySize));
                left.lastFightData.leftAoeDamage += rightCondition.flatLep;
                turnFlatLep.aoeDamage = rightCondition.flatLep;
                leftCondition.resolveDamage(turnFlatLep);
            }
        }

        // Apply Hawking's AOE
        if (leftCondition.aoeZero || rightCondition.aoeZero) {
            TurnData turnZero;
            if (leftCondition.aoeZero) {
                leftCondition.aoeZero = round(rightCondition.dampZero * leftCondition.aoeZero);
                left.lastFightData.rightAoeDamage += leftCondition.aoeZero;
                turnZero.aoeDamage = leftCondition.aoeZero;
                rightCondition.resolveDamage(turnZero);
            }
            if (rightCondition.aoeZero) {
                rightCondition.aoeZero = round(leftCondition.dampZero * rightCondition.aoeZero);
                left.lastFightData.leftAoeDamage += rightCondition.aoeZero;
                turnZero.aoeDamage = rightCondition.aoeZero;
                leftCondition.resolveDamage(turnZero);
            }
        }

        //----- turn zero end -----
    }

    // Battle Loop. Continues until one side is out of monsters
    //TODO: handle 100 turn limit for non-wb, also handle it for wb better maybe
    while (leftCondition.monstersLost < leftCondition.armySize && rightCondition.monstersLost < rightCondition.armySize && turncounter < 99) {
        leftCondition.startNewTurn(turncounter);
        rightCondition.startNewTurn(turncounter);

        // Get damage with all relevant multipliers
        leftCondition.getDamage(turncounter, rightCondition);
        rightCondition.getDamage(turncounter, leftCondition);

        // Apply armor in case of ricochet
        leftCondition.applyArmor(rightCondition.turnData);
        rightCondition.applyArmor(leftCondition.turnData);

        // Check if anything died as a result
        leftCondition.resolveDamage(rightCondition.turnData);
        rightCondition.resolveDamage(leftCondition.turnData);

        //Save AOE for calc optimization when expanding armies
        left.lastFightData.leftAoeDamage += rightCondition.turnData.aoeDamage + leftCondition.turnData.sadism;
        left.lastFightData.rightAoeDamage += leftCondition.turnData.aoeDamage + rightCondition.turnData.sadism;

        //Resolve Revenge abilities
        while (rightCondition.turnData.aoeRevenge || rightCondition.turnData.deathstrikeDamage ||
               leftCondition.turnData.aoeRevenge  || leftCondition.turnData.deathstrikeDamage) {
            left.lastFightData.leftAoeDamage += rightCondition.turnData.aoeRevenge;
            leftCondition.resolveRevenge(rightCondition.turnData);
            left.lastFightData.rightAoeDamage += leftCondition.turnData.aoeRevenge;
            rightCondition.resolveRevenge(leftCondition.turnData);
        }

        turncounter++;

        if (verbose) {
            std::cout << std::endl << "After Turn " << turncounter << ":" << std::endl;

            std::cout << "Left:" << std::endl;
            std::cout << "  Damage: " << std::setw(4) << leftCondition.turnData.baseDamage << std::endl;
            std::cout << "  Health: ";
            for (int i = 0; i < leftCondition.armySize; i++) {
                std::cout << std::setw(4) << leftCondition.remainingHealths[i] << " ";
            } std::cout << std::endl;

            std::cout << "Right:" << std::endl;
            std::cout << "  Damage: " << std::setw(4) << rightCondition.turnData.baseDamage << std::endl;
            std::cout << "  Health: ";
            for (int i = 0; i < rightCondition.armySize; i++) {
                std::cout << std::setw(4) << rightCondition.remainingHealths[i] << " ";
            } std::cout << std::endl;
        }
    }

    // how 100 turn limit is handled for WB
    if (turncounter >= 99 && rightCondition.worldboss == true) {
        leftCondition.monstersLost = leftCondition.armySize;
    }

    // write all the results into a FightResult
    left.lastFightData.dominated = false;
    left.lastFightData.turncounter = turncounter;

    if (leftCondition.monstersLost >= leftCondition.armySize) { //draws count as right wins.
        left.lastFightData.monstersLost = rightCondition.monstersLost;
        left.lastFightData.berserk = rightCondition.lastBerserk;
        if (rightCondition.monstersLost < rightCondition.armySize) {
            left.lastFightData.frontHealth = (int64_t) (rightCondition.remainingHealths[rightCondition.monstersLost]);
        } else {
            left.lastFightData.frontHealth = 0;
        }
        return false;
    } else {
        left.lastFightData.monstersLost = leftCondition.monstersLost;
        left.lastFightData.frontHealth = (int64_t) (leftCondition.remainingHealths[leftCondition.monstersLost]);
        left.lastFightData.berserk = leftCondition.lastBerserk;
        return true;
    }
}

// Function determining if a monster is strictly better than another
bool isBetter(Monster * a, Monster * b, bool considerAbilities = false);

#endif
