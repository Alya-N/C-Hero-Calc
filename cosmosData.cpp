#include "cosmosData.h"
#include "inputProcessing.h"

// Private constructor that is called by all public ones. Fully initializes all attributes
Monster::Monster(int someHp, int someDamage, FollowerCount aCost, std::string aName, Element anElement, HeroRarity aRarity, HeroSkill aSkill, int promoOne, int promoTwo, int promoFour, double promoFive, int aLevel, int aPromo) :
    hp(someHp),
    damage(someDamage),
    cost(aCost),
    baseName(aName),
    element(anElement),
    rarity(aRarity),
    skill(aSkill),
    level(aLevel),
    promo(aPromo),
    name(aName),
    promoOne(promoOne),
    promoTwo(promoTwo),
    promoFour(promoFour),
    promoFive(promoFive)
{
    if (this->rarity != NO_HERO) {
        if (this->rarity != WORLDBOSS) {
            int hpBonus = 0;
            int atkBonus = 0;
            int skillBonus = 0;
            int pointBonus = 0;
            double points = this->level-1;
            switch (promo){
                case 5:
                    this->skill.amount += promoFive;
                case 4:
                    hpBonus = promoFour;
                    atkBonus = promoFour;
                case 3:
                    switch(rarity){
                        case COMMON:
                            pointBonus = 1;
                            break;
                        case RARE:
                            pointBonus = 2;
                            break;
                        case LEGENDARY:
                            pointBonus = 3;
                            break;
                        case ASCENDED:
                            pointBonus = 4;
                            break;
                        default:
                            break;
                    }
                case 2:
                    atkBonus += promoTwo;
                case 1:
                    hpBonus += promoOne;
                    break;
                default:
                    this->promo = 0;
                    break;
            }
            double mult = (aSkill.skillType == GROW) ? skill.amount : 1;
            points *= this->rarity * mult + pointBonus;
            points = floor(points);
            if (aSkill.skillType == EASTER && this->level > 1)
                this->skill.amount = floor((this->skill.amount - 1) * this->rarity * (this->level-1));
            int value = this->hp + this->damage;

            this->hp = this->hp + (int) round((double) points * (double) this->hp / (double) value) + hpBonus;
            this->damage = this->damage + (int) round((double) points * (double) this->damage / (double) value) + atkBonus;
        }
        else
            this->level = 1; //So despite the level of the boss it always shows the replay for level 1, as bosses don't actually scale.
        if (this->promo)
            this->name = this->baseName + HEROLEVEL_SEPARATOR + std::to_string(this->level) + HEROPROMO_SEPARATOR + std::to_string(this->promo);
        else
            this->name = this->baseName + HEROLEVEL_SEPARATOR + std::to_string(this->level);
    }
}

// Constructor for normal Monsters
Monster::Monster(int someHp, int someDamage, FollowerCount aCost, std::string aName, Element anElement) :
    Monster(someHp, someDamage, aCost, aName, anElement, NO_HERO, NO_SKILL, 0, 0, 0, 0, 0, 0) {}

// Constructor for Heroes
Monster::Monster(int someHp, int someDamage, std::string aName, Element anElement, HeroRarity aRarity, HeroSkill aSkill, int promoOne, int promoTwo, int promoFour, double promoFive) :
    Monster(someHp, someDamage, 0, aName, anElement, aRarity, aSkill, promoOne, promoTwo, promoFour, promoFive, 1, 0) {}

// Constructor for leveled heroes
Monster::Monster(const Monster & baseHero, int aLevel, int aPromo) :
    Monster(baseHero.hp, baseHero.damage, baseHero.cost, baseHero.baseName, baseHero.element, baseHero.rarity, baseHero.skill, baseHero.promoOne, baseHero.promoTwo, baseHero.promoFour, baseHero.promoFive, aLevel, aPromo)
{
    this->index = baseHero.index;
//Put aSeethe skill back where it belongs once it's capped to 99
    if (this->skill.skillType == POSBONUS_L) {
        this->skill.skillType = POSBONUS;
        this->skill.amount = this->skill.amount * (double) aLevel;
    }
    //Abilities no longer scale past level 99
    if (aLevel > 99)
        aLevel = 99;
    switch(this->skill.skillType){
        default:            break;
        case BUFF_L:        this->skill.skillType = BUFF;
                            this->skill.amount = (double) floor((double) aLevel * this->skill.amount);
                            break;
        case PROTECT_L:     this->skill.skillType = PROTECT;
                            this->skill.amount = (double) floor((double) aLevel * this->skill.amount);
                            break;
        case CHAMPION_L:    this->skill.skillType = CHAMPION;
                            this->skill.amount = (double) floor((double) aLevel * this->skill.amount);
                            break;
        case HEAL_L:        this->skill.skillType = HEAL;
                            this->skill.amount = (double) floor((double) aLevel * this->skill.amount);
                            break;
        case AOE_L:         this->skill.skillType = AOE;
                            this->skill.amount = (double) floor((double) aLevel * this->skill.amount);
                            break;
        case LIFESTEAL_L:   this->skill.skillType = LIFESTEAL;
                            this->skill.amount = (double) floor((double) aLevel * this->skill.amount);
                            break;
        case WBIDEAL_L:     this->skill.skillType = WBIDEAL;
                            this->skill.amount = (double) floor((double) aLevel * this->skill.amount);
                            break;
        case DAMPEN_L:      this->skill.skillType = DAMPEN;
                            this->skill.amount = 1.0f - (double) aLevel * this->skill.amount;
                            break;
        case AOEZERO_L:     this->skill.skillType = AOEZERO;
                            this->skill.amount = (double) floor((double) aLevel * this->skill.amount);
                            break;
        case EXPLODE_L:     this->skill.skillType = EXPLODE;
                            this->skill.amount = this->skill.amount * floor(aLevel / 3) ;
                            break;
        case RESISTANCE_L:  this->skill.skillType = RESISTANCE;
                            this->skill.amount = this->skill.amount * (double) aLevel / 9;
                            break;
        case AOEREFLECT_L:  this->skill.skillType = AOEREFLECT;
                            this->skill.amount = this->skill.amount * (double) aLevel / 3;
                            break;
        case HPPIERCE_L:    this->skill.skillType = HPPIERCE;
                            this->skill.amount = this->skill.amount * (double) aLevel / 9;
                            break;
        case SACRIFICE_L:   this->skill.skillType = SACRIFICE;
                            this->skill.amount = this->skill.amount * (double) aLevel / 9;
                            break;
        case EXECUTE_CUBE:  this->skill.skillType = FLATEXEC;
                            this->skill.amount = pow(this->skill.amount, 3);
                            break;
        case AOEFIRST_CUBE: this->skill.skillType = AOEFIRST;
                            this->skill.amount = pow(this->skill.amount, 3);
                            break;
        case SELFARMOR_CUBE:this->skill.skillType = SELFARMOR;
                            this->skill.amount = pow(this->skill.amount, 3);
                            break;
    }
}

HeroSkill::HeroSkill(SkillType aType, Element aTarget, Element aSource, double anAmount) :
    skillType(aType),
    target(aTarget),
    sourceElement(aSource),
    amount(anAmount)
{
    this->hasAsymmetricAoe = (aType == VALKYRIE || aType == TRAMPLE ||
                              aType == COUNTER || aType == DEATHSTRIKE ||
                              aType == LEECH || aType == COUNTER_MAX_HP ||
                              aType == AOELAST || aType == FLATREF ||
                              aType == SELFHEAL || aType == EASTER ||
                              aType == AOEFIRST || aType == AOEFIRST_CUBE ||
                              aType == BULLSHIT || aType == CONVERT || 
                              aType == TRIPLE || aType == HPAMPLIFY);
    this->hasHeal = (aType == HEAL || aType == HEAL_L ||
                     aType == LIFESTEAL || aType == LIFESTEAL_L ||
                     aType == WBIDEAL   || aType == WBIDEAL_L   ||
                     aType == SACRIFICE || aType == SACRIFICE_L ||
                     aType == DEATHBUFF || aType == HEALFIRST   ||
                     aType == FLATHEAL  || aType == BLOODLUST);
    // hasAoe should include all things affected by dampen
    this->hasAoe = (aType == AOE || aType == AOE_L ||
                    aType == REVENGE ||
                    aType == EXPLODE ||
                    aType == SADISM ||
                    aType == ATTACKAOE ||
                    this->hasHeal || this->hasAsymmetricAoe);
    // For expanding armies, if new hero added to the back might have changed the fight, old result is not valid
    this->violatesFightResults = (aType == BUFF || aType == BUFF_L ||
                                  aType == PROTECT || aType == PROTECT_L ||
                                  aType == CHAMPION || aType == CHAMPION_L ||
                                  aType == AOE || aType == AOE_L ||
                                  aType == HEAL || aType == HEAL_L ||
                                  aType == LIFESTEAL || aType == LIFESTEAL_L ||
                                  aType == WBIDEAL   || aType == WBIDEAL_L   ||
                                  aType == BEER || aType == AOEZERO_L ||
                                  aType == AOEZERO || aType == ABSORB ||
                                  aType == SACRIFICE || aType == SACRIFICE_L ||
                                  aType == AOELAST || aType == HEALFIRST ||
                                  aType == PERCBUFF || aType == CONVERT ||
                                  aType == EASTER || aType == AOEFIRST ||
                                  aType == AOEFIRST_CUBE || aType == FURY);
}

// JSON Functions to provide results in an easily readable output format. Used my Latas for example
std::string Monster::toJSON() {
    std::stringstream s;
    s << "{";
        s << "\"id\""  << ":" << getRealIndex(*this);
        if (this->rarity != NO_HERO) {
            s << ",";
            s << "\"level\""  << ":" << this->level;
        }
    s << "}";
    return s.str();
}

std::string Army::toString(int tier) {
    std::stringstream s;
    s << "[";
    int index = isQuest(*this);

    if (index != -1) {
        s << "quest" << index << "-" << tier << " | ";
    }
    s << "Followers: " << std::setw(7) << numberWithSeparators(this->followerCost) << " | ";
    for (int i = this->monsterAmount-1; i >= 0; i--) {
        s << monsterReference[this->monsters[i]].name << " "; // Print in reversed Order
    } s << "<==]";
    return s.str();
}

std::string Army::toJSON() {
    if (this->isEmpty()) {return "null";}

    std::stringstream s;
    s << "{";
        s << "\"followers\"" << ":" << this->followerCost << ",";
        s << "\"monsters\"" << ":";
        s << "[";
        for (int i = this->monsterAmount-1; i >= 0; i--) {
            s << monsterReference[this->monsters[i]].toJSON();
            if (i > 0) {
                s << ",";
            }
        }
        s << "]";
    s << "}";
    return s.str();
}

void Instance::setTarget(Army aTarget) {
    this->target = aTarget;
    this->targetSize = aTarget.monsterAmount;
    this->lowestBossHealth = 0;

    HeroSkill currentSkill;
    this->hasAoe = false;
    this->hasHeal = false;
    this->hasAsymmetricAoe = false;
    this->hasBeer = false;
    this->hasGambler = false;
    this->hasWorldBoss = false;
    for (size_t i = 0; i < this->targetSize; i++) {
        currentSkill = monsterReference[this->target.monsters[i]].skill;
        this->hasAoe |= currentSkill.hasAoe;
        this->hasHeal |= currentSkill.hasHeal;
        this->hasAsymmetricAoe |= currentSkill.hasAsymmetricAoe;
        this->hasBeer |= currentSkill.skillType == BEER;
        this->hasGambler |= currentSkill.skillType == DICE || currentSkill.skillType == LUX || currentSkill.skillType == CRIT;
        this->hasWorldBoss |= monsterReference[this->target.monsters[i]].rarity == WORLDBOSS;
    }

    // Check which monsters can survive a hit from the final monster on the target. This helps reduce the amount of potential solutions in the last expand
    // Heroes with global Abilities also get accepted.
    // This produces only false positives not false negatives -> no correct solutions lost
    Monster lastMonster = monsterReference[this->target.monsters[this->targetSize - 1]];
    for (size_t i = 0; i < monsterReference.size(); i++) {
        Monster currentMonster = monsterReference[i];
        int attack = lastMonster.damage;
        if (counter[currentMonster.element] == lastMonster.element) {
            // attack = castCeil((double) attack * elementalBoost);
            attack = round((double) attack * elementalBoost);
        }
        this->monsterUsefulLast.push_back(currentMonster.hp > attack || currentMonster.skill.violatesFightResults);
    }
}

// Returns the index of a quest if the lineup is the same. Returns -1 if not a quest
int isQuest(Army & army) {
    bool match;
    for (size_t i = 0; i < quests.size(); i++) {
        match = false;
        if ((int) quests[i].size() == army.monsterAmount) {
            match = true;
            for (int j = 0; j < army.monsterAmount; j++) {
                if (quests[i][j] != monsterReference[army.monsters[j]].name) {
                    match = false;
                }
            }
        }
        if (match) {
            return (int) i;
        }
    }
    return -1;
}

// Access tools for monsters
std::map<std::string, MonsterIndex> monsterMap; // Maps monster Names to their indices in monsterReference
std::vector<Monster> monsterReference; // Global lookup for monster stats indices of monsters here can be used instead of the objects
std::vector<MonsterIndex> availableMonsters; // Contains indices of all monsters the user allows. Is affected by filters
std::vector<MonsterIndex> availableHeroes; // Contains all user heroes' indices

// Storage for Game Data
std::vector<Monster> monsterBaseList; // Raw Monster Data, holds the actual Objects
std::vector<Monster> baseHeroes; // Raw, unleveled Hero Data, holds actual Objects
std::map<std::string, std::string> heroAliases; //Alternate or shorthand names for heroes
std::vector<std::vector<std::string>> quests; // Quest Lineup from the game

// Converts string to int, int still needs to be cast to enum
std::map<std::string, int> stringToEnum = {
    {"NOTHING", NOTHING},
    {"BUFF", BUFF},
    {"PROTECT", PROTECT},
    {"CHAMPION", CHAMPION},
    {"AOE", AOE},
    {"HEAL", HEAL},
    {"LIFESTEAL", LIFESTEAL},
    {"DAMPEN", DAMPEN},
    {"AOEZERO", AOEZERO},
    {"AOEZERO_L", AOEZERO_L},
    {"BERSERK", BERSERK},
    {"FRIENDS", FRIENDS},
    {"ADAPT", ADAPT},
    {"RAINBOW", RAINBOW},
    {"TRAINING", TRAINING},
    {"WITHER", WITHER},
    {"REVENGE", REVENGE},
    {"VALKYRIE", VALKYRIE},
    {"TRAMPLE", TRAMPLE},
    {"BUFF_L", BUFF_L},
    {"PROTECT_L", PROTECT_L},
    {"CHAMPION_L", CHAMPION_L},
    {"AOE_L", AOE_L},
    {"HEAL_L", HEAL_L},
    {"LIFESTEAL_L", LIFESTEAL_L},
    {"DAMPEN_L", DAMPEN_L},
    {"BEER", BEER},
    {"GROW", GROW},
    {"COUNTER", COUNTER},
    {"DICE", DICE},
    {"LUX", LUX},
    {"CRIT", CRIT},
    {"EXPLODE", EXPLODE},
    {"ABSORB", ABSORB},
    {"HATE", HATE},
    {"EXPLODE_L", EXPLODE_L},
    {"DODGE", DODGE},
    {"DEATHSTRIKE", DEATHSTRIKE},
    {"LEECH", LEECH},
    {"EVOLVE", EVOLVE},
    {"COUNTER_MAX_HP", COUNTER_MAX_HP},
    {"EXECUTE", EXECUTE},
    {"RESISTANCE", RESISTANCE},
    {"AOEREFLECT", AOEREFLECT},
    {"HPPIERCE", HPPIERCE},
    {"SACRIFICE", SACRIFICE},
    {"RESISTANCE_L", RESISTANCE_L},
    {"AOEREFLECT_L", AOEREFLECT_L},
    {"HPPIERCE_L", HPPIERCE_L},
    {"SACRIFICE_L", SACRIFICE_L},
    {"POSBONUS", POSBONUS},
    {"POSBONUS_L", POSBONUS_L},
    {"DEATHBUFF", DEATHBUFF},
    {"VOID", VOID},
    {"AOELAST", AOELAST},
    {"SADISM", SADISM},
    {"SELFHEAL", SELFHEAL},
    {"COURAGE", COURAGE},
    {"SKILLDAMPEN", SKILLDAMPEN},
    {"SHIELDME", SHIELDME},
    {"FLATREF", FLATREF},
    {"EASTER", EASTER},
    {"HEALFIRST", HEALFIRST},
    {"PERCBUFF", PERCBUFF},
    {"EXECUTE_CUBE", EXECUTE_CUBE},
    {"SELFARMOR_CUBE", SELFARMOR_CUBE},
    {"AOEFIRST_CUBE", AOEFIRST_CUBE},
    {"FLATEXEC", FLATEXEC},
    {"SELFARMOR", SELFARMOR},
    {"AOEFIRST", AOEFIRST},
    {"CONVERT", CONVERT},
    {"TRIPLE", TRIPLE},
    {"ATTACKAOE", ATTACKAOE},
    {"FLATHEAL", FLATHEAL},
    {"HPAMPLIFY", HPAMPLIFY},
    {"FURY", FURY},
    {"BLOODLUST", BLOODLUST},
    {"WBIDEAL", WBIDEAL},
    {"WBIDEAL_L", WBIDEAL_L},
    {"BULLSHIT", BULLSHIT},

    {"EARTH", EARTH},
    {"AIR", AIR},
    {"WATER", WATER},
    {"FIRE", FIRE},
    {"ALL", ALL},
    {"SELF", SELF},

    {"NO_HERO", NO_HERO},
    {"COMMON", COMMON},
    {"RARE", RARE},
    {"LEGENDARY", LEGENDARY},
    {"ASCENDED", ASCENDED},
    {"WORLDBOSS", WORLDBOSS}
};

// Fill MonsterBaseList With Monsters Order is important for ReplayStrings and gambler abilities
void initMonsterData() {
    monsterBaseList.push_back(Monster(20,8,500,"a1",AIR));
    monsterBaseList.push_back(Monster(44,4,650,"e1",EARTH));
    monsterBaseList.push_back(Monster(16,10,500,"f1",FIRE));
    monsterBaseList.push_back(Monster(30,6,700,"w1",WATER));
    monsterBaseList.push_back(Monster(48,6,1950,"a2",AIR));
    monsterBaseList.push_back(Monster(30,8,1350,"e2",EARTH));
    monsterBaseList.push_back(Monster(18,16,1950,"f2",FIRE));
    monsterBaseList.push_back(Monster(24,12,1950,"w2",WATER));
    monsterBaseList.push_back(Monster(36,12,4000,"a3",AIR));
    monsterBaseList.push_back(Monster(26,16,3750,"e3",EARTH));
    monsterBaseList.push_back(Monster(54,8,4000,"f3",FIRE));
    monsterBaseList.push_back(Monster(18,24,4000,"w3",WATER));
    monsterBaseList.push_back(Monster(24,26,7500,"a4",AIR));
    monsterBaseList.push_back(Monster(72,10,9000,"e4",EARTH));
    monsterBaseList.push_back(Monster(52,16,11500,"f4",FIRE));
    monsterBaseList.push_back(Monster(36,20,9000,"w4",WATER));
    monsterBaseList.push_back(Monster(60,20,20500,"a5",AIR));
    monsterBaseList.push_back(Monster(36,40,27000,"e5",EARTH));
    monsterBaseList.push_back(Monster(42,24,15500,"f5",FIRE));
    monsterBaseList.push_back(Monster(78,18,26000,"w5",WATER));
    monsterBaseList.push_back(Monster(62,34,48000,"a6",AIR));
    monsterBaseList.push_back(Monster(72,24,35500,"e6",EARTH));
    monsterBaseList.push_back(Monster(104,20,47000,"f6",FIRE));
    monsterBaseList.push_back(Monster(44,44,42000,"w6",WATER));
    monsterBaseList.push_back(Monster(106,26,72000,"a7",AIR));
    monsterBaseList.push_back(Monster(66,36,57500,"e7",EARTH));
    monsterBaseList.push_back(Monster(54,44,57500,"f7",FIRE));
    monsterBaseList.push_back(Monster(92,32,79500,"w7",WATER));
    monsterBaseList.push_back(Monster(78,52,128500,"a8",AIR));
    monsterBaseList.push_back(Monster(60,60,107500,"e8",EARTH));
    monsterBaseList.push_back(Monster(94,50,160500,"f8",FIRE));
    monsterBaseList.push_back(Monster(108,36,120500,"w8",WATER));
    monsterBaseList.push_back(Monster(116,54,247500,"a9",AIR));
    monsterBaseList.push_back(Monster(120,48,218000,"e9",EARTH));
    monsterBaseList.push_back(Monster(102,58,227000,"f9",FIRE));
    monsterBaseList.push_back(Monster(80,70,209000,"w9",WATER));
    monsterBaseList.push_back(Monster(142,60,392500,"a10",AIR));
    monsterBaseList.push_back(Monster(122,64,344500,"e10",EARTH));
    monsterBaseList.push_back(Monster(104,82,393500,"f10",FIRE));
    monsterBaseList.push_back(Monster(110,70,337500,"w10",WATER));
    monsterBaseList.push_back(Monster(114,110,701500,"a11",AIR));
    monsterBaseList.push_back(Monster(134,81,565000,"e11",EARTH));
    monsterBaseList.push_back(Monster(164,70,614500,"f11",FIRE));
    monsterBaseList.push_back(Monster(152,79,657500,"w11",WATER));
    monsterBaseList.push_back(Monster(164,88,866500,"a12",AIR));
    monsterBaseList.push_back(Monster(128,120,951500,"e12",EARTH));
    monsterBaseList.push_back(Monster(156,92,859000,"f12",FIRE));
    monsterBaseList.push_back(Monster(188,78,887500,"w12",WATER));
    monsterBaseList.push_back(Monster(210,94,1386000,"a13",AIR));
    monsterBaseList.push_back(Monster(190,132,1985500,"e13",EARTH));
    monsterBaseList.push_back(Monster(166,130,1584500,"f13",FIRE));
    monsterBaseList.push_back(Monster(140,128,1199000,"w13",WATER));
    monsterBaseList.push_back(Monster(200,142,2392500,"a14",AIR));
    monsterBaseList.push_back(Monster(244,136,3022000,"e14",EARTH));
    monsterBaseList.push_back(Monster(168,168,2370500,"f14",FIRE));
    monsterBaseList.push_back(Monster(212,122,2079500,"w14",WATER));
    monsterBaseList.push_back(Monster(226,190,4448500,"a15",AIR));
    monsterBaseList.push_back(Monster(200,186,3586500,"e15",EARTH));
    monsterBaseList.push_back(Monster(234,136,2838000,"f15",FIRE));
    monsterBaseList.push_back(Monster(276,142,3879000,"w15",WATER));
    monsterBaseList.push_back(Monster(280,196,6427500,"a16",AIR));
    monsterBaseList.push_back(Monster(284,190,6267000,"e16",EARTH));
    monsterBaseList.push_back(Monster(288,192,6500500,"f16",FIRE));
    monsterBaseList.push_back(Monster(286,198,6737500,"w16",WATER));
    monsterBaseList.push_back(Monster(318,206,8382500,"a17",AIR));
    monsterBaseList.push_back(Monster(338,192,8265500,"e17",EARTH));
    monsterBaseList.push_back(Monster(236,292,9045000,"f17",FIRE));
    monsterBaseList.push_back(Monster(262,258,8786500,"w17",WATER));
    monsterBaseList.push_back(Monster(280,280,10975500,"a18",AIR));
    monsterBaseList.push_back(Monster(330,242,11283500,"e18",EARTH));
    monsterBaseList.push_back(Monster(392,200,10975500,"f18",FIRE));
    monsterBaseList.push_back(Monster(330,230,10454500,"w18",WATER));
    monsterBaseList.push_back(Monster(440,206,13644000,"a19",AIR));
    monsterBaseList.push_back(Monster(320,282,13553500,"e19",EARTH));
    monsterBaseList.push_back(Monster(352,244,12585000,"f19",FIRE));
    monsterBaseList.push_back(Monster(360,238,12539500,"w19",WATER));
    monsterBaseList.push_back(Monster(378,268,16121000,"a20",AIR));
    monsterBaseList.push_back(Monster(382,264,16012500,"e20",EARTH));
    monsterBaseList.push_back(Monster(388,266,16577800,"f20",FIRE));
    monsterBaseList.push_back(Monster(454,232,17091000,"w20",WATER));
    monsterBaseList.push_back(Monster(428,286,21413000,"a21",AIR));
    monsterBaseList.push_back(Monster(446,272,21126000,"e21",EARTH));
    monsterBaseList.push_back(Monster(362,338,21399000,"f21",FIRE));
    monsterBaseList.push_back(Monster(416,290,20950500,"w21",WATER));
    monsterBaseList.push_back(Monster(454,320,27686500,"a22",AIR));
    monsterBaseList.push_back(Monster(450,324,27835500,"e22",EARTH));
    monsterBaseList.push_back(Monster(458,318,27791000,"f22",FIRE));
    monsterBaseList.push_back(Monster(440,340,27938500,"w22",WATER));
    monsterBaseList.push_back(Monster(500,348,36290000,"a23",AIR));
    monsterBaseList.push_back(Monster(516,340,36741500,"e23",EARTH));
    monsterBaseList.push_back(Monster(424,410,36240000,"f23",FIRE));
    monsterBaseList.push_back(Monster(490,354,36121500,"w23",WATER));
    monsterBaseList.push_back(Monster(554,374,47156000,"a24",AIR));
    monsterBaseList.push_back(Monster(458,458,48035500,"e24",EARTH));
    monsterBaseList.push_back(Monster(534,392,47886000,"f24",FIRE));
    monsterBaseList.push_back(Monster(540,388,47951500,"w24",WATER));
    monsterBaseList.push_back(Monster(580,430,62274500,"a25",AIR));
    monsterBaseList.push_back(Monster(592,418,61548000,"e25",EARTH));
    monsterBaseList.push_back(Monster(764,328,62721500,"f25",FIRE));
    monsterBaseList.push_back(Monster(500,506,63628000,"w25",WATER));
    monsterBaseList.push_back(Monster(496,582,77548500,"a26",AIR));
    monsterBaseList.push_back(Monster(622,468,78527500,"e26",EARTH));
    monsterBaseList.push_back(Monster(638,462,80013000,"f26",FIRE));
    monsterBaseList.push_back(Monster(700,416,78570000,"w26",WATER));
    monsterBaseList.push_back(Monster(712,484,101147500,"a27",AIR));
    monsterBaseList.push_back(Monster(580,602,103158500,"e27",EARTH));
    monsterBaseList.push_back(Monster(690,498,100713000,"f27",FIRE));
    monsterBaseList.push_back(Monster(682,500,99672000,"w27",WATER));
    monsterBaseList.push_back(Monster(644,642,132923000,"a28",AIR));
    monsterBaseList.push_back(Monster(770,540,134058500,"e28",EARTH));
    monsterBaseList.push_back(Monster(746,552,132125000,"f28",FIRE));
    monsterBaseList.push_back(Monster(762,536,130511500,"w28",WATER));
    monsterBaseList.push_back(Monster(834,616,184115000,"a29",AIR));
    monsterBaseList.push_back(Monster(830,614,181902500,"e29",EARTH));
    monsterBaseList.push_back(Monster(746,676,179059500,"f29",FIRE));
    monsterBaseList.push_back(Monster(1008,512,185380500,"w29",WATER));
    monsterBaseList.push_back(Monster(700,906,252527500,"a30",AIR));
    monsterBaseList.push_back(Monster(1022,614,248541000,"e30",EARTH));
    monsterBaseList.push_back(Monster(930,690,257020000,"f30",FIRE));
    monsterBaseList.push_back(Monster(802,802,257924500,"w30",WATER));
    monsterBaseList.push_back(Monster(973,950,444349500,"a31",AIR));
    monsterBaseList.push_back(Monster(1187,739,410783500,"e31",EARTH));
    monsterBaseList.push_back(Monster(815,1139,447190000,"f31",FIRE));
    monsterBaseList.push_back(Monster(995,995,492537500,"w31",WATER));
    monsterBaseList.push_back(Monster(1089,1138,689803000,"a32",AIR));
    monsterBaseList.push_back(Monster(1110,1116,689367500,"e32",EARTH));
    monsterBaseList.push_back(Monster(1230,1028,710914500,"f32",FIRE));
    monsterBaseList.push_back(Monster(754,1512,608632000,"w32",WATER));
    monsterBaseList.push_back(Monster(1105,1457,1021416000,"a33",AIR));
    monsterBaseList.push_back(Monster(1545,1029,1002272500,"e33",EARTH));
    monsterBaseList.push_back(Monster(1257,1257,993061000,"f33",FIRE));
    monsterBaseList.push_back(Monster(1620,939,938082000,"w33",WATER));
    monsterBaseList.push_back(Monster(1715,1183,1444918500,"a34",AIR));
    monsterBaseList.push_back(Monster(1152,1772,1458291500,"e34",EARTH));
    monsterBaseList.push_back(Monster(1224,1664,1453356500,"f34",FIRE));
    monsterBaseList.push_back(Monster(1438,1434,1480580500,"w34",WATER));
    monsterBaseList.push_back(Monster(1609,1594,2053698000,"a35",AIR));
    monsterBaseList.push_back(Monster(1639,1639,2201440000,"e35",EARTH));
    monsterBaseList.push_back(Monster(2119,1121,1830521500,"f35",FIRE));
    monsterBaseList.push_back(Monster(1419,1929,2264341500,"w35",WATER));
    monsterBaseList.push_back(Monster(1994,1709,6290727000,"a36",AIR));
    monsterBaseList.push_back(Monster(1842,1902,6557680000,"e36",EARTH));
    monsterBaseList.push_back(Monster(1634,2028,6032259000,"f36",FIRE));
    monsterBaseList.push_back(Monster(2018,1808,6969143000,"w36",WATER));
    monsterBaseList.push_back(Monster(2919,1566,9773245000,"a37",AIR));
    monsterBaseList.push_back(Monster(2012,2311,10026327000,"e37",EARTH));
    monsterBaseList.push_back(Monster(1989,2111,8603685000,"f37",FIRE));
    monsterBaseList.push_back(Monster(2069,2529,11969168000,"w37",WATER));
    monsterBaseList.push_back(Monster(2082,3174,16987578000,"a38",AIR));
    monsterBaseList.push_back(Monster(2279,2842,16483620000,"e38",EARTH));
    monsterBaseList.push_back(Monster(2578,2388,15274811000,"f38",FIRE));
    monsterBaseList.push_back(Monster(2525,2525,16098453000,"w38",WATER));
    monsterBaseList.push_back(Monster(2540,3487,26358974000,"a39",AIR));
    monsterBaseList.push_back(Monster(2888,2888,24087491000,"e39",EARTH));
    monsterBaseList.push_back(Monster(3089,2657,23513333000,"f39",FIRE));
    monsterBaseList.push_back(Monster(2973,2837,24495221000,"w39",WATER));
    monsterBaseList.push_back(Monster(3363,3430,39177027000,"a40",AIR));
    monsterBaseList.push_back(Monster(3496,3038,34612964000,"e40",EARTH));
    monsterBaseList.push_back(Monster(2989,3621,35606695000,"f40",FIRE));
    monsterBaseList.push_back(Monster(3053,3517,35184287000,"w40",WATER));
    monsterBaseList.push_back(Monster(4484,3273,56223440000,"a41",AIR));
    monsterBaseList.push_back(Monster(3413,4111,52556405000,"e41",EARTH));
    monsterBaseList.push_back(Monster(3814,3814,55480717000,"f41",FIRE));
    monsterBaseList.push_back(Monster(3791,3791,54483043000,"w41",WATER));
    monsterBaseList.push_back(Monster(3797,4931,81014620000,"a42",AIR));
    monsterBaseList.push_back(Monster(4264,4264,77526752000,"e42",EARTH));
    monsterBaseList.push_back(Monster(4527,4112,80314766000,"f42",FIRE));
    monsterBaseList.push_back(Monster(4012,4571,78534090000,"w42",WATER));
    monsterBaseList.push_back(Monster(4834,4834,112958766000,"a43",AIR));
    monsterBaseList.push_back(Monster(4209,5318,105898763000,"e43",EARTH));
    monsterBaseList.push_back(Monster(4831,4831,112748588000,"f43",FIRE));
    monsterBaseList.push_back(Monster(5014,4574,109829937000,"w43",WATER));
    monsterBaseList.push_back(Monster(5789,4917,151864280000,"a44",AIR));
    monsterBaseList.push_back(Monster(5569,4948,144647311000,"e44",EARTH));
    monsterBaseList.push_back(Monster(4559,6112,147088733000,"f44",FIRE));
    monsterBaseList.push_back(Monster(5299,5299,148792746000,"w44",WATER));
    monsterBaseList.push_back(Monster(5277,6419,197143156000,"a45",AIR));
    monsterBaseList.push_back(Monster(5079,6421,186239297000,"e45",EARTH));
    monsterBaseList.push_back(Monster(5871,5871,202365391000,"f45",FIRE));
    monsterBaseList.push_back(Monster(6357,5231,191758841000,"w45",WATER));
}

// Fill BaseHeroes with Heroes. Order is important
void initBaseHeroes() {
    baseHeroes.push_back(Monster( 45, 20, "ladyoftwilight",     AIR,   COMMON,    {CHAMPION,      ALL, AIR, 3}, 28, 25, 15, 3));
    baseHeroes.push_back(Monster( 70, 30, "tiny",               EARTH, RARE,      {LIFESTEAL_L,   ALL, EARTH, 0.04167f}, 40, 28, 17, 0.0404));
    baseHeroes.push_back(Monster(110, 40, "nebra",              FIRE,  LEGENDARY, {BUFF,          ALL, FIRE, 20}, 140, 100, 83, 20));
    baseHeroes.push_back(Monster( 20, 10, "valor",              AIR,   COMMON,    {PROTECT,       AIR, AIR, 2}, 12, 7, 19, 2));
    baseHeroes.push_back(Monster( 30,  8, "rokka",              EARTH, COMMON,    {PROTECT,       EARTH, EARTH, 2}, 40, 12, 5, 2));
    baseHeroes.push_back(Monster( 24, 12, "pyromancer",         FIRE,  COMMON,    {PROTECT,       FIRE, FIRE, 2}, 16, 8, 8, 2));
    baseHeroes.push_back(Monster( 50,  6, "bewat",              WATER, COMMON,    {PROTECT,       WATER, WATER, 2}, 31, 10, 16, 2));
    baseHeroes.push_back(Monster( 22, 14, "hunter",             AIR,   COMMON,    {BUFF,          AIR, AIR, 2}, 14, 12, 11, 2));
    baseHeroes.push_back(Monster( 40, 20, "shaman",             EARTH, RARE,      {PROTECT,       EARTH, EARTH , 3}, 36, 16, 25, 3));
    baseHeroes.push_back(Monster( 82, 22, "alpha",              FIRE,  LEGENDARY, {AOE,           ALL, FIRE, 2}, 115, 37, 75, 5));
    baseHeroes.push_back(Monster( 28, 12, "carl",               WATER, COMMON,    {BUFF,          WATER, WATER , 2}, 18, 5, 12, 2));
    baseHeroes.push_back(Monster( 38, 22, "nimue",              AIR,   RARE,      {PROTECT,       AIR, AIR, 3}, 49, 19, 12, 3));
    baseHeroes.push_back(Monster( 70, 26, "athos",              EARTH, LEGENDARY, {PROTECT,       ALL, EARTH, 4}, 70, 32, 96, 6));
    baseHeroes.push_back(Monster( 24, 16, "jet",                FIRE,  COMMON,    {BUFF,          FIRE, FIRE, 2}, 11, 9, 12, 2));
    baseHeroes.push_back(Monster( 36, 24, "geron",              WATER, RARE,      {PROTECT,       WATER, WATER, 3}, 27, 11, 28, 3));
    baseHeroes.push_back(Monster( 46, 40, "rei",                AIR,   LEGENDARY, {BUFF,          ALL, AIR, 5}, 79, 67, 89, 10));
    baseHeroes.push_back(Monster( 19, 22, "ailen",              EARTH, COMMON,    {BUFF,          EARTH, EARTH, 2}, 21, 17, 12, 2));
    baseHeroes.push_back(Monster( 50, 18, "faefyr",             FIRE,  RARE,      {PROTECT,       FIRE, FIRE, 3}, 53, 17, 19, 3));
    baseHeroes.push_back(Monster( 60, 32, "auri",               WATER, LEGENDARY, {HEAL,          ALL, WATER, 4}, 70, 74, 23, 4));
    baseHeroes.push_back(Monster( 22, 32, "nicte",              AIR,   RARE,      {BUFF,          AIR, AIR, 4}, 21, 43, 24, 4));
    baseHeroes.push_back(Monster( 50, 12, "james",              EARTH, LEGENDARY, {VALKYRIE,      ALL, EARTH, 0.75f}, 103, 111, 41, 0.1));
    baseHeroes.push_back(Monster( 28, 16, "k41ry",              AIR,   COMMON,    {BUFF,          AIR, AIR, 3}, 19, 14, 19, 3));
    baseHeroes.push_back(Monster( 46, 20, "t4urus",             EARTH, RARE,      {BUFF,          ALL, EARTH, 2}, 28, 20, 30, 2));
    baseHeroes.push_back(Monster(100, 20, "tr0n1x",             FIRE,  LEGENDARY, {AOE,           ALL, FIRE, 3}, 107, 44, 35, 6));
    baseHeroes.push_back(Monster( 58,  8, "aquortis",           WATER, COMMON,    {BUFF,          WATER, WATER, 3}, 28, 18, 18, 3));
    baseHeroes.push_back(Monster( 30, 32, "aeris",              AIR,   RARE,      {HEAL,          ALL, AIR, 2}, 20, 36, 28, 2));
    baseHeroes.push_back(Monster( 75,  2, "geum",               EARTH, LEGENDARY, {BERSERK,       SELF, EARTH, 2}, 213, 8, 22, 0.2));
    baseHeroes.push_back(Monster( 46, 16, "forestdruid",        EARTH, RARE,      {BUFF,          EARTH, EARTH, 4}, 38, 16, 19, 4));
    baseHeroes.push_back(Monster( 32, 24, "ignitor",            FIRE,  RARE,      {BUFF,          FIRE, FIRE, 4}, 24, 22, 23, 4));
    baseHeroes.push_back(Monster( 58, 14, "undine",             WATER, RARE,      {BUFF,          WATER, WATER, 4}, 25, 7, 15, 4));
    baseHeroes.push_back(Monster( 38, 12, "rudean",             FIRE,  COMMON,    {BUFF,          FIRE, FIRE, 3}, 19, 14, 10, 3));
    baseHeroes.push_back(Monster( 18, 50, "aural",              WATER, RARE,      {BERSERK,       SELF, WATER, 1.2f}, 33, 31, 28, 0.12));
    baseHeroes.push_back(Monster( 46, 46, "geror",              AIR,   LEGENDARY, {FRIENDS,       SELF, AIR, 1.2f}, 95, 125, 36, 0.12));
    baseHeroes.push_back(Monster( 66, 44, "veildur",            EARTH, LEGENDARY, {CHAMPION,      ALL, EARTH, 3}, 98, 42, 51, 3));
    baseHeroes.push_back(Monster( 72, 48, "brynhildr",          AIR,   LEGENDARY, {CHAMPION,      ALL, AIR, 4}, 84, 56, 59, 4));
    baseHeroes.push_back(Monster( 78, 52, "groth",              FIRE,  LEGENDARY, {CHAMPION,      ALL, FIRE, 5}, 114, 70, 62, 5));
    baseHeroes.push_back(Monster( 30, 16, "ourea",              EARTH, COMMON,    {BUFF,          EARTH, EARTH, 3}, 17, 8, 8, 3));
    baseHeroes.push_back(Monster( 48, 20, "erebus",             FIRE,  RARE,      {CHAMPION,      FIRE, FIRE, 2}, 55, 18, 20, 2));
    baseHeroes.push_back(Monster( 62, 36, "pontus",             WATER, LEGENDARY, {ADAPT,         WATER, WATER, 2}, 121, 43, 79, 0.2));
    baseHeroes.push_back(Monster( 52, 20, "chroma",             AIR,   RARE,      {PROTECT,       AIR, AIR, 4}, 23, 15, 15, 4));
    baseHeroes.push_back(Monster( 26, 44, "petry",              EARTH, RARE,      {PROTECT,       EARTH, EARTH, 4}, 18, 16, 28, 4));
    baseHeroes.push_back(Monster( 58, 22, "zaytus",             FIRE,  RARE,      {PROTECT,       FIRE, FIRE, 4}, 57, 12, 16, 4));
    baseHeroes.push_back(Monster( 75, 45, "spyke",              AIR,   LEGENDARY, {TRAINING,      SELF, AIR, 10}, 112, 43, 73, 10));
    baseHeroes.push_back(Monster( 70, 55, "aoyuki",             WATER, LEGENDARY, {RAINBOW,       SELF, WATER, 100}, 75, 121, 66, 100));
    baseHeroes.push_back(Monster( 75,150, "gaiabyte",           EARTH, LEGENDARY, {WITHER,        SELF, EARTH, 2}, 151, 84, 52, 2));
    baseHeroes.push_back(Monster( 36, 14, "oymos",              AIR,   COMMON,    {BUFF,          AIR, AIR, 4}, 24, 15, 21, 4));
    baseHeroes.push_back(Monster( 32, 32, "xarth",              EARTH, RARE,      {CHAMPION,      EARTH, EARTH, 2}, 23, 25, 19, 2));
    baseHeroes.push_back(Monster( 76, 32, "atzar",              FIRE,  LEGENDARY, {ADAPT,         FIRE, FIRE, 2}, 85, 28, 48, 0.2));
    baseHeroes.push_back(Monster( 70, 42, "zeth",               WATER, LEGENDARY, {REVENGE,       ALL, WATER, 0.1f}, 127, 76, 24, 0.1));
    baseHeroes.push_back(Monster( 76, 46, "koth",               EARTH, LEGENDARY, {REVENGE,       ALL, EARTH, 0.15f}, 99, 39, 70, 0.1));
    baseHeroes.push_back(Monster( 82, 50, "gurth",              AIR,   LEGENDARY, {REVENGE,       ALL, AIR, 0.2f}, 108, 43, 68, 0.1));
    baseHeroes.push_back(Monster( 35, 25, "werewolf",           EARTH, COMMON,    {PROTECT_L,     ALL, EARTH, 0.1112f}, 23, 28, 19, 0.0202));
    baseHeroes.push_back(Monster( 55, 35, "jackoknight",        AIR,   RARE,      {BUFF_L,        ALL, AIR, 0.1112f}, 50, 13, 38, 0.0202));
    baseHeroes.push_back(Monster( 75, 45, "dullahan",           FIRE,  LEGENDARY, {CHAMPION_L,    ALL, FIRE, 0.1112f}, 114, 45, 65, 0.0404));
    baseHeroes.push_back(Monster( 36, 36, "ladyodelith",        WATER, RARE,      {PROTECT,       WATER, WATER, 4}, 19, 17, 29, 4));
    baseHeroes.push_back(Monster( 34, 54, "shygu",              AIR,   LEGENDARY, {PROTECT_L,     AIR, AIR, 0.1112f}, 62, 68, 71, 0.0202));
    baseHeroes.push_back(Monster( 72, 28, "thert",              EARTH, LEGENDARY, {PROTECT_L,     EARTH, EARTH, 0.1112f}, 61, 44, 69, 0.0202));
    baseHeroes.push_back(Monster( 32, 64, "lordkirk",           FIRE,  LEGENDARY, {PROTECT_L,     FIRE, FIRE, 0.1112f}, 77, 99, 83, 0.0202));
    baseHeroes.push_back(Monster( 30, 70, "neptunius",          WATER, LEGENDARY, {PROTECT_L,     WATER, WATER, 0.1112f}, 92, 73, 83, 0.0202));
    baseHeroes.push_back(Monster( 65, 12, "sigrun",             FIRE,  LEGENDARY, {VALKYRIE,      ALL, FIRE, 0.5f}, 132, 29, 61, 0.15));
    baseHeroes.push_back(Monster( 70, 14, "koldis",             WATER, LEGENDARY, {VALKYRIE,      ALL, WATER, 0.5f}, 173, 32, 46, 0.15));
    baseHeroes.push_back(Monster( 75, 16, "alvitr",             EARTH, LEGENDARY, {VALKYRIE,      ALL, EARTH, 0.5f}, 152, 50, 56, 0.15));
    baseHeroes.push_back(Monster( 30, 18, "hama",               WATER, COMMON,    {BUFF,          WATER, WATER, 4}, 21, 10, 7, 4));
    baseHeroes.push_back(Monster( 34, 34, "hallinskidi",        AIR,   RARE,      {CHAMPION,      AIR, AIR, 2}, 17, 54, 20, 2));
    baseHeroes.push_back(Monster( 60, 42, "rigr",               EARTH, LEGENDARY, {ADAPT,         EARTH, EARTH, 2}, 68, 21, 93, 0.2));
    baseHeroes.push_back(Monster(174, 46, "aalpha",             FIRE,  ASCENDED,  {AOE_L,         ALL, FIRE, 0.304f}, 251, 98, 92, 0.0304));
    baseHeroes.push_back(Monster(162, 60, "aathos",             EARTH, ASCENDED,  {PROTECT_L,     ALL, EARTH, 0.304f}, 281, 57, 76, 0.0304));
    baseHeroes.push_back(Monster(120,104, "arei",               AIR,   ASCENDED,  {BUFF_L,        ALL, AIR, 0.304f}, 167, 86, 176, 0.1414));
    baseHeroes.push_back(Monster(148, 78, "aauri",              WATER, ASCENDED,  {HEAL_L,        ALL, WATER, 0.152f}, 151, 221, 59, 0.0152));
    baseHeroes.push_back(Monster(190, 38, "atr0n1x",            FIRE,  ASCENDED,  {VALKYRIE,      ALL, FIRE, 0.75f}, 383, 56, 25, 0.05));
    baseHeroes.push_back(Monster(222,  8, "ageum",              EARTH, ASCENDED,  {BERSERK,       SELF, EARTH, 2}, 348, 35, 16, 0.2));
    baseHeroes.push_back(Monster(116,116, "ageror",             AIR,   ASCENDED,  {FRIENDS,       SELF, AIR, 1.3f}, 173, 22, 153, 0.13));
    baseHeroes.push_back(Monster(WORLDBOSS_HEALTH, 113, "lordofchaos", FIRE, WORLDBOSS, {AOE,      ALL, FIRE, 50}, 30, 20, 0.1, 10));
    baseHeroes.push_back(Monster( 38, 24, "christmaself",       WATER, COMMON,    {HEAL_L,        ALL, WATER, 0.1112f}, 28, 34, 22, 0.0202));
    baseHeroes.push_back(Monster( 54, 36, "reindeer",           AIR,   RARE,      {AOE_L,         ALL, AIR, 0.1112f}, 53, 18, 33, 0.0202));
    baseHeroes.push_back(Monster( 72, 48, "santaclaus",         FIRE,  LEGENDARY, {LIFESTEAL_L,   ALL, FIRE, 0.1112f}, 107, 96, 78, 0.0404));
    baseHeroes.push_back(Monster( 44, 44, "sexysanta",          EARTH, RARE,      {VALKYRIE,      ALL, EARTH, 0.66f}, 27, 41, 40, 0.05));
    baseHeroes.push_back(Monster( 24, 24, "toth",               FIRE,  COMMON,    {BUFF,          FIRE, FIRE, 4}, 18, 32, 16, 4));
    baseHeroes.push_back(Monster( 40, 30, "ganah",              WATER, RARE,      {CHAMPION,      WATER, WATER, 2}, 29, 26, 43, 2));
    baseHeroes.push_back(Monster( 58, 46, "dagda",              AIR,   LEGENDARY, {ADAPT,         AIR, AIR, 2}, 79, 78, 123, 0.2));
    baseHeroes.push_back(Monster(300,110, "bubbles",            WATER, ASCENDED,  {DAMPEN_L,      ALL, WATER, 0.0050f}, 291, 120, 58, 0.0005));
    baseHeroes.push_back(Monster(150, 86, "apontus",            WATER, ASCENDED,  {ADAPT,         WATER, WATER, 3}, 124, 211, 190, 0.3));
    baseHeroes.push_back(Monster(162, 81, "aatzar",             FIRE,  ASCENDED,  {ADAPT,         FIRE, FIRE, 3}, 333, 76, 86, 0.3));
    baseHeroes.push_back(Monster( 74, 36, "arshen",             AIR,   LEGENDARY, {TRAMPLE,       ALL, AIR, 1}, 83, 44, 28, 0.05));
    baseHeroes.push_back(Monster( 78, 40, "rua",                FIRE,  LEGENDARY, {TRAMPLE,       ALL, FIRE, 1}, 88, 44, 37, 0.05));
    baseHeroes.push_back(Monster( 82, 44, "dorth",              WATER, LEGENDARY, {TRAMPLE,       ALL, WATER, 1}, 90, 38, 40, 0.05));
    baseHeroes.push_back(Monster(141, 99, "arigr",              EARTH, ASCENDED,  {ADAPT,         EARTH, EARTH, 3}, 138, 121, 163, 0.3));
    baseHeroes.push_back(Monster(WORLDBOSS_HEALTH, 191, "motherofallkodamas", EARTH, WORLDBOSS, {DAMPEN,        ALL, EARTH, 0.5}, 30, 20, 0.1, 10));
    baseHeroes.push_back(Monster( 42, 50, "hosokawa",           AIR,   LEGENDARY, {BUFF_L,        AIR, AIR, 0.1112f}, 108, 115, 74, 0.0202));
    baseHeroes.push_back(Monster( 32, 66, "takeda",             EARTH, LEGENDARY, {BUFF_L,        EARTH, EARTH, 0.1112f}, 83, 82, 45, 0.0202));
    baseHeroes.push_back(Monster( 38, 56, "hirate",             FIRE,  LEGENDARY, {BUFF_L,        FIRE, FIRE, 0.1112f}, 53, 133, 44, 0.0202));
    baseHeroes.push_back(Monster( 44, 48, "hattori",            WATER, LEGENDARY, {BUFF_L,        WATER, WATER, 0.1112f}, 65, 74, 64, 0.0202));
    baseHeroes.push_back(Monster(135, 107,"adagda",             AIR,   ASCENDED,  {ADAPT,         AIR, AIR, 3}, 208, 78, 322, 0.3));
    baseHeroes.push_back(Monster( 30, 20, "bylar",              EARTH, COMMON,    {BUFF,          EARTH, EARTH, 4}, 18, 6, 15, 4));
    baseHeroes.push_back(Monster( 36, 36, "boor",               FIRE,  RARE,      {TRAINING,      SELF, FIRE, 3}, 19, 49, 20, 3));
    baseHeroes.push_back(Monster( 52, 52, "bavah",              WATER, LEGENDARY, {CHAMPION,      ALL, WATER, 4}, 84, 92, 43, 5));
    baseHeroes.push_back(Monster( 75, 25, "leprechaun",         EARTH, LEGENDARY, {BEER,          ALL, EARTH, 0}, 59, 13, 13, 1));
    baseHeroes.push_back(Monster( 30, 30, "sparks",             FIRE,  COMMON,    {GROW,          ALL, FIRE, 2}, 29, 29, 15, 0.2));
    baseHeroes.push_back(Monster( 48, 42, "leaf",               EARTH, RARE,      {GROW,          ALL, EARTH, 2}, 35, 57, 26, 0.2));
    baseHeroes.push_back(Monster( 70, 48, "flynn",              AIR,   LEGENDARY, {GROW,          ALL, AIR, 2}, 46, 93, 69, 0.2));
    baseHeroes.push_back(Monster(122,122, "abavah",             WATER, ASCENDED,  {CHAMPION_L,    ALL, ALL, 0.152f}, 217, 152, 145, 0.0152));
    baseHeroes.push_back(Monster( 66, 60, "drhawking",          AIR,   LEGENDARY, {AOEZERO_L,     ALL, AIR, 1}, 94, 136, 70, 0.3));
    baseHeroes.push_back(Monster(150, 90, "masterlee",          AIR,   ASCENDED,  {COUNTER,       AIR, AIR, 0.5f}, 314, 290, 225, 0.05));
    baseHeroes.push_back(Monster( 70, 38, "kumusan",            FIRE,  LEGENDARY, {COUNTER,       FIRE, FIRE, 0.2f}, 134, 132, 135, 0.05));
    baseHeroes.push_back(Monster( 78, 42, "liucheng",           WATER, LEGENDARY, {COUNTER,       WATER, WATER, 0.25f}, 149, 111, 80, 0.05));
    baseHeroes.push_back(Monster( 86, 44, "hidoka",             EARTH, LEGENDARY, {COUNTER,       EARTH, EARTH, 0.3f}, 144, 172, 61, 0.05));
    baseHeroes.push_back(Monster(WORLDBOSS_HEALTH, 11, "kryton", AIR,  WORLDBOSS, {TRAINING,      SELF, AIR, 10}, 20, 20, 30, 0.1));
    baseHeroes.push_back(Monster( 25, 26, "dicemaster",         WATER, COMMON,    {DICE,          SELF, SELF, 20}, 19, 36, 12, 20));
    baseHeroes.push_back(Monster( 28, 60, "luxuriusmaximus",    FIRE,  RARE,      {LUX,           SELF, EARTH, 1}, 44, 47, 29, 1));
    baseHeroes.push_back(Monster( 70, 70, "pokerface",          EARTH, LEGENDARY, {CRIT,          EARTH, EARTH, 3}, 119, 211, 111, 0.3));
    baseHeroes.push_back(Monster( 25, 25, "taint",              AIR,   COMMON,    {VALKYRIE,      ALL, AIR, 0.5f}, 13, 4, 21, 0.05));
    baseHeroes.push_back(Monster( 48, 50, "putrid",             EARTH, RARE,      {TRAINING,      SELF, EARTH, -3}, 51, 93, 50, 1));
    baseHeroes.push_back(Monster( 52, 48, "defile",             FIRE,  LEGENDARY, {EXPLODE,       ALL, FIRE, 50}, 60, 138, 72, 50));
    baseHeroes.push_back(Monster(150, 15, "neil",               WATER, LEGENDARY, {ABSORB,        SELF, WATER, 0.3}, 244, 20, 23, 0.05));
    baseHeroes.push_back(Monster( 78, 26, "mahatma",            AIR,   LEGENDARY, {HATE,          WATER, AIR, 0.75}, 56, 19, 85, 0.5));
    baseHeroes.push_back(Monster( 76, 30, "jade",               EARTH, LEGENDARY, {HATE,          AIR, EARTH, 0.75}, 45, 37, 91, 0.5));
    baseHeroes.push_back(Monster( 72, 36, "edana",              FIRE,  LEGENDARY, {HATE,          EARTH, FIRE, 0.75}, 91, 46, 66, 0.5));
    baseHeroes.push_back(Monster( 80, 30, "dybbuk",             WATER, LEGENDARY, {HATE,          FIRE, WATER, 0.75}, 65, 43, 63, 0.5));
    baseHeroes.push_back(Monster( 85, 135, "ashygu",            AIR,   ASCENDED,  {PROTECT_L,     AIR, AIR, 0.1819f}, 148, 113, 217, 0.01819));
    baseHeroes.push_back(Monster( 180, 70, "athert",            EARTH, ASCENDED,  {PROTECT_L,     EARTH, EARTH, 0.1819f}, 346, 67, 76, 0.01819));
    baseHeroes.push_back(Monster( 80, 160, "alordkirk",         FIRE,  ASCENDED,  {PROTECT_L,     FIRE, FIRE, 0.1819f}, 215, 255, 87, 0.01819));
    baseHeroes.push_back(Monster( 75, 175, "aneptunius",        WATER, ASCENDED,  {PROTECT_L,     WATER, WATER, 0.1819f}, 335, 201, 241, 0.01819));
    baseHeroes.push_back(Monster( 106,124, "ahosokawa",         AIR,   ASCENDED,  {BUFF_L,        AIR, AIR, 0.1819f}, 229, 423, 207, 0.01819));
    baseHeroes.push_back(Monster( 82, 164, "atakeda",           EARTH, ASCENDED,  {BUFF_L,        EARTH, EARTH, 0.1819f}, 252, 425, 66, 0.01819));
    baseHeroes.push_back(Monster( 96, 144, "ahirate",           FIRE,  ASCENDED,  {BUFF_L,        FIRE, FIRE, 0.1819f}, 199, 193, 119, 0.01819));
    baseHeroes.push_back(Monster( 114,126, "ahattori",          WATER, ASCENDED,  {BUFF_L,        WATER, WATER, 0.1819f}, 218, 167, 138, 0.01819));
    baseHeroes.push_back(Monster(WORLDBOSS_HEALTH, 175, "doyenne", WATER, WORLDBOSS, {DODGE,      ALL, ALL, 15000}, 30, 20, 0.1, 10));
    baseHeroes.push_back(Monster( 30, 40, "billy",              EARTH, COMMON,    {DEATHSTRIKE,   ALL, EARTH, 100}, 43, 30, 24, 25));
    baseHeroes.push_back(Monster( 88, 22, "sanqueen",           WATER, RARE,      {LEECH,         SELF, WATER, 0.8}, 71, 8, 16, 0.08));
    baseHeroes.push_back(Monster(150, 60, "cliodhna",           AIR,   LEGENDARY, {EVOLVE,        SELF, AIR, 1}, 193, 240, 78, 0.1));
    baseHeroes.push_back(Monster( 340, 64, "guy",               FIRE,  ASCENDED,  {COUNTER_MAX_HP, FIRE, FIRE, 1}, 160, 121, 115, 0.1));
    baseHeroes.push_back(Monster( 126,114, "adefile",           FIRE,  ASCENDED,  {EXPLODE_L,       ALL, FIRE, 7}, 126, 122, 114, 2));
    baseHeroes.push_back(Monster(186, 62, "raiderrose",         EARTH, ASCENDED,  {EXECUTE,       EARTH, EARTH, 0.6}, 353, 32, 476, 0.05));
    baseHeroes.push_back(Monster( 96, 30, "buccaneerbeatrice",  WATER, LEGENDARY, {EXECUTE,       WATER, WATER, 0.3}, 66, 32, 66, 0.05));
    baseHeroes.push_back(Monster(100, 32, "corsaircharles",     AIR,   LEGENDARY, {EXECUTE,       AIR, AIR, 0.35}, 87, 21, 64, 0.05));
    baseHeroes.push_back(Monster(105, 34, "maraudermagnus",     FIRE,  LEGENDARY, {EXECUTE,       FIRE, FIRE, 0.4}, 175, 67, 71, 0.05));
    baseHeroes.push_back(Monster( 46, 52, "frosty",             WATER, RARE,      {RESISTANCE_L,   ALL, WATER, 0.03f}, 51, 69, 16, 0.003));
    baseHeroes.push_back(Monster( 50, 18, "fir",                EARTH, COMMON,    {AOEREFLECT_L,   ALL, EARTH, 0.004f}, 33, 5, 11, 0.0004));
    baseHeroes.push_back(Monster( 78, 34, "5-12-6",             AIR,   RARE,      {HPPIERCE_L,     ALL, AIR, 0.03f}, 42, 37, 39, 0.003));
    baseHeroes.push_back(Monster(170, 18, "kedari",             FIRE,  LEGENDARY, {SACRIFICE_L,    ALL, FIRE, 2}, 133, 19, 25, 0.2));
    baseHeroes.push_back(Monster( 18, 26, "raze",               WATER, COMMON,    {TRAMPLE,       ALL, WATER, 0.7}, 8, 9, 14, 0.05));
    baseHeroes.push_back(Monster( 44, 48, "ruin",               AIR,   RARE,      {REVENGE,       ALL, AIR, 0.1}, 23, 61, 23, 0.05));
    baseHeroes.push_back(Monster( 48, 54, "seethe",             EARTH, LEGENDARY, {POSBONUS,      SELF, EARTH, 15}, 57, 178, 53, 15));
    baseHeroes.push_back(Monster(117,131, "aseethe",            EARTH, ASCENDED,  {POSBONUS_L,      SELF, EARTH, 0.45}, 90, 117, 124, 0.11));
    baseHeroes.push_back(Monster( 54, 54, "blossom",            EARTH, LEGENDARY, {DEATHBUFF,       ALL, EARTH, 0.10}, 99, 99, 144, 0.01));
    baseHeroes.push_back(Monster( 56, 56, "flint",              FIRE,  LEGENDARY, {DEATHBUFF,       ALL, FIRE,  0.11}, 100, 100, 150, 0.01));
    baseHeroes.push_back(Monster( 58, 58, "orin",               AIR,   LEGENDARY, {DEATHBUFF,       ALL, AIR,   0.12}, 101, 101, 156, 0.01));
    baseHeroes.push_back(Monster(130,130, "aurora",             WATER, ASCENDED,  {DEATHBUFF,       ALL, WATER, 0.15}, 221, 221, 344, 0.01));
    baseHeroes.push_back(Monster(220, 20, "cupid",              AIR,   LEGENDARY, {TRAMPLE,         ALL, AIR, 3}, 150, 50, 110, 1));
    baseHeroes.push_back(Monster( 22, 22, "transient",          ALL,   COMMON,    {VOID,       ALL, ALL, 0.5}, 11, 11, 20, 0.1));
    baseHeroes.push_back(Monster( 34, 34, "maunder",            ALL,   RARE,      {VOID,       ALL, ALL, 0.5}, 22, 22, 32, 0.1));
    baseHeroes.push_back(Monster( 50, 50, "thewanderer",        ALL,   LEGENDARY, {VOID,       ALL, ALL, 0.5}, 80, 80, 120, 0.1));
    baseHeroes.push_back(Monster(100, 40, "b-day",              AIR,   LEGENDARY, {AOELAST,    ALL, AIR, 0.1}, 61, 51, 49, 0.05));
    baseHeroes.push_back(Monster( 44, 22, "cloud",              AIR,   COMMON,    {SADISM,     ALL, AIR,   0.95}, 36, 32, 11, 0.05));
    baseHeroes.push_back(Monster( 64, 32, "ember",              FIRE,  RARE,      {SADISM,     ALL, FIRE,  0.95}, 34, 14, 19, 0.05));
    baseHeroes.push_back(Monster( 84, 42, "riptide",            WATER, LEGENDARY, {SADISM,     ALL, WATER, 0.95}, 89, 34, 22, 0.05));
    baseHeroes.push_back(Monster(180, 90, "spike",              EARTH, ASCENDED,  {SADISM,     ALL, EARTH, 0.95}, 234, 103, 136, 0.05));
    baseHeroes.push_back(Monster(180, 60, "amahatma",           AIR,   ASCENDED,  {HATE,       WATER, AIR,   1.7}, 241, 50, 195, 0.3));
    baseHeroes.push_back(Monster(172, 68, "ajade",              EARTH, ASCENDED,  {HATE,       AIR,   EARTH, 1.7}, 288, 41, 144, 0.3));
    baseHeroes.push_back(Monster(160, 80, "aedana",             FIRE,  ASCENDED,  {HATE,       EARTH, FIRE,  1.7}, 191, 180, 99, 0.3));
    baseHeroes.push_back(Monster(176, 66, "adybbuk",            WATER, ASCENDED,  {HATE,       FIRE,  WATER, 1.7}, 272, 55, 169, 0.3));
    baseHeroes.push_back(Monster( 30, 38, "willow",             AIR,   COMMON,    {SELFHEAL,    SELF,  AIR,   0.2}, 22, 19, 24, 0.2));
    baseHeroes.push_back(Monster( 70, 40, "gizmo",              FIRE,  RARE,      {COURAGE,     SELF,  FIRE,  3}, 43, 26, 52, 1));
    baseHeroes.push_back(Monster( 84, 50, "daisy",              WATER, LEGENDARY, {EASTER,     ALL,   WATER, 2.5}, 50, 84, 70, 0.5));
    baseHeroes.push_back(Monster(120,200, "thumper",            EARTH, ASCENDED,  {SKILLDAMPEN, SELF,  EARTH, 0.6}, 230, 320, 360, 0.1));
    baseHeroes.push_back(Monster( 40, 24, "bortles",            AIR,   COMMON,    {SHIELDME,    SELF,  AIR,   3}, 10, 13, 14, 3));
    baseHeroes.push_back(Monster( 40, 28, "murphy",             EARTH, RARE,      {FLATREF,     EARTH, EARTH, 60}, 24, 18, 28, 30));
    baseHeroes.push_back(Monster( 24, 82, "nerissa",            WATER, LEGENDARY, {RESISTANCE,  SELF,  WATER, 0.45}, 22, 45, 52, 0.05));
    baseHeroes.push_back(Monster(112, 55, "mother",             WATER, LEGENDARY, {HEALFIRST,   ALL,  WATER, 25}, 200, 120, 140, 5));
    baseHeroes.push_back(Monster( 48,164, "anerissa",           WATER, ASCENDED,  {RESISTANCE_L,SELF, WATER, 0.06}, 70, 112, 100, 0.006));
    baseHeroes.push_back(Monster( 51, 59, "agatha",             AIR,   LEGENDARY, {PERCBUFF,    ALL,  AIR,   0.1}, 76, 92, 74, 0.05));
    baseHeroes.push_back(Monster( 52, 60, "ophelia",            EARTH, LEGENDARY, {PERCBUFF,    ALL,  EARTH, 0.15}, 78, 99, 75, 0.05));
    baseHeroes.push_back(Monster( 53, 61, "helga",              WATER, LEGENDARY, {PERCBUFF,    ALL,  WATER, 0.2}, 84, 101, 82, 0.05));
    baseHeroes.push_back(Monster(108,124, "minerva",            FIRE,  ASCENDED,  {PERCBUFF,    ALL,  FIRE,  0.25}, 204, 241, 256, 0.05));
    baseHeroes.push_back(Monster(126,126, "awanderer",          ALL,   ASCENDED,  {VOID,        ALL,  ALL,   0.75}, 110, 110, 110, 0.1));
    baseHeroes.push_back(Monster( 76, 50, "tetra",              FIRE,  LEGENDARY, {BULLSHIT,       ALL,  FIRE,  1}, 154, 68, 68, 0.2));
    baseHeroes.push_back(Monster( 16, 28, "cathos",             EARTH, COMMON,    {SELFARMOR_CUBE, SELF, EARTH, 2}, 8, 16, 14, 1));
    baseHeroes.push_back(Monster( 42, 28, "catzar",             FIRE,  RARE,      {EXECUTE_CUBE,   ALL,  FIRE,  4}, 28, 10, 36, 1));
    baseHeroes.push_back(Monster( 80,  8, "crei",               AIR,   LEGENDARY, {AOEFIRST_CUBE,  ALL,  AIR,   4}, 112, 28, 26, 1));
    baseHeroes.push_back(Monster(210, 21, "acrei",              AIR,   ASCENDED,  {AOEFIRST_CUBE,  ALL,  AIR,   5}, 192, 34, 78, 1));
    baseHeroes.push_back(Monster( 75, 45, "smith",              EARTH, LEGENDARY, {TRIPLE,         ALL,  EARTH, 0.9}, 84, 47, 68, 0.1));
    baseHeroes.push_back(Monster( 32, 14, "mrcotton",           FIRE,  COMMON,    {ATTACKAOE,      ALL,  FIRE,  10}, 28, 7, 12, 5));
    baseHeroes.push_back(Monster( 54, 20, "sharkjellyn",        WATER, RARE,      {FLATHEAL,       SELF, WATER, 30}, 44, 13, 15, 15));
    baseHeroes.push_back(Monster( 50, 50, "chocoknight",        EARTH, LEGENDARY, {HPAMPLIFY,      ALL,  EARTH, 0.4}, 72, 47, 82, 0.1));
    baseHeroes.push_back(Monster(124,124, "achocoknight",       EARTH, ASCENDED,  {HPAMPLIFY,      ALL,  EARTH, 0.55}, 98, 64, 104, 0.1));
    baseHeroes.push_back(Monster( 92,211, "lili",               FIRE,  ASCENDED,  {CONVERT,        ALL,  FIRE,  0.1}, 174, 535, 171, 0.01));
    baseHeroes.push_back(Monster(WORLDBOSS_HEALTH, 151, "bornag", ALL,  WORLDBOSS, {RESISTANCE,    SELF, ALL,   1}, 20, 20, 30, 0.1));
    baseHeroes.push_back(Monster( 53, 61, "thrace",             FIRE,  LEGENDARY, {BLOODLUST,      AIR,  AIR,   125}, 82, 131, 104, 25));
    baseHeroes.push_back(Monster( 55, 63, "scinda",             AIR,   LEGENDARY, {BLOODLUST,      AIR,  AIR,   150}, 78, 125, 116, 30));
    baseHeroes.push_back(Monster( 57, 65, "myrmillo",           EARTH, LEGENDARY, {BLOODLUST,      AIR,  AIR,   175}, 94, 141, 99, 35));
    baseHeroes.push_back(Monster(144,126, "retia",              WATER, ASCENDED,  {BLOODLUST,      AIR,  AIR,   350}, 246, 318, 338, 100));
    baseHeroes.push_back(Monster( 48,  4, "newt",               WATER, COMMON,    {FURY,           AIR,  AIR,   2}, 14, 1, 4, 0.5));
    baseHeroes.push_back(Monster( 58,  6, "electra",            AIR,   RARE,      {FURY,           AIR,  AIR,   3}, 30, 4, 8, 1));
    baseHeroes.push_back(Monster( 72,  3, "boson",              FIRE,  LEGENDARY, {FURY,           AIR,  AIR,   3}, 181, 19, 21, 1));
    baseHeroes.push_back(Monster(214,  7, "higgs",              FIRE,  ASCENDED,  {FURY,           AIR,  AIR,   4}, 301, 14, 30, 1));
}

void initIndices() {
    for (auto i = monsterBaseList.begin(); i != monsterBaseList.end(); i++) {
        i->index = getRealIndex(*i);
    }

    for (auto i = baseHeroes.begin(); i != baseHeroes.end(); i++) {
        i->index = getRealIndex(*i);
    }
}

void initHeroAliases() {
    heroAliases["lady"] = "ladyoftwilight";
    heroAliases["lot"] = "ladyoftwilight";
    heroAliases["pyro"] = "pyromancer";
    heroAliases["kairy"] = "k41ry";
    heroAliases["taurus"] = "t4urus";
    heroAliases["tronix"] = "tr0n1x";
    heroAliases["druid"] = "forestdruid";
    heroAliases["veil"] = "veildur";
    heroAliases["bryn"] = "brynhildr";
    heroAliases["gaia"] = "gaiabyte";
    heroAliases["ww"] = "werewolf";
    heroAliases["wolf"] = "werewolf";
    heroAliases["jack"] = "jackoknight";
    heroAliases["jacko"] = "jackoknight";
    heroAliases["dull"] = "dullahan";
    heroAliases["dulla"] = "dullahan";
    heroAliases["odelith"] = "ladyodelith";
    heroAliases["kirk"] = "lordkirk";
    heroAliases["nep"] = "neptunius";
    heroAliases["hoso"] = "hosokawa";
    heroAliases["tak"] = "takeda";
    heroAliases["hall"] = "hallinskidi";
    heroAliases["atronix"] = "atr0n1x";
    heroAliases["elf"] = "christmaself";
    heroAliases["deer"] = "reindeer";
    heroAliases["santa"] = "santaclaus";
    heroAliases["ss"] = "sexysanta";
    heroAliases["lep"] = "leprechaun";
    heroAliases["hawking"] = "drhawking";
    heroAliases["dice"] = "dicemaster";
    heroAliases["lux"] = "luxuriusmaximus";
    heroAliases["luxurious"] = "luxuriusmaximus";
    heroAliases["poker"] = "pokerface";
    heroAliases["akirk"] = "alordkirk";
    heroAliases["anep"] = "aneptunius";
    heroAliases["ahoso"] = "ahosokawa";
    heroAliases["atak"] = "atakeda";
    heroAliases["san"] = "sanqueen";
    heroAliases["squeen"] = "sanqueen";
    heroAliases["clio"] = "cliodhna";
    heroAliases["cloddy"] = "cliodhna";
    heroAliases["fawkes"] = "guy";
    heroAliases["guyfawkes"] = "guy";
    heroAliases["beatrice"] = "buccaneerbeatrice";
    heroAliases["buccanerbeatrice"] = "buccaneerbeatrice";//To work with macrocreator without updating it
    heroAliases["charles"] = "corsaircharles";
    heroAliases["magnus"] = "maraudermagnus";
    heroAliases["rose"] = "raiderrose";
    heroAliases["elftwo"] = "5-12-6";
    heroAliases["numbers"] = "5-12-6";
    heroAliases["cipher"] = "5-12-6";
    heroAliases["mechaelf"] = "5-12-6";
    heroAliases["elfmech"] = "5-12-6";
    heroAliases["roboelf"] = "5-12-6";
    heroAliases["melf"] = "5-12-6";
    heroAliases["mekelf"] = "5-12-6";
    heroAliases["kraide"] = "kedari";
    heroAliases["trans"] = "transient";
    heroAliases["maund"] = "maunder";
    heroAliases["wanderer"] = "thewanderer";
    heroAliases["wander"] = "thewanderer";
    heroAliases["clown"] = "b-day";
    heroAliases["bday"] = "b-day";
    heroAliases["lee"] = "masterlee";
    heroAliases["mom"] = "mother";
    heroAliases["awander"] = "awanderer";
    heroAliases["cotton"] = "mrcotton";
    heroAliases["shark"] = "sharkjellyn";
    heroAliases["jelly"] = "sharkjellyn";
    heroAliases["choco"] = "chocoknight";
    heroAliases["chocobo"] = "chocoknight";
    heroAliases["achoco"] = "achocoknight";
    heroAliases["dchoco"] = "achocoknight";
    heroAliases["achocobo"] = "achocoknight";

    heroAliases["loc"] = "lordofchaos";
    heroAliases["fboss"] = "lordofchaos";
    heroAliases["moak"] = "motherofallkodamas";
    heroAliases["eboss"] = "motherofallkodamas";
    heroAliases["kry"] = "kryton";
    heroAliases["aboss"] = "kryton";
    heroAliases["doy"] = "doyenne";
    heroAliases["wboss"] = "doyenne";
    heroAliases["bor"] = "bornag";
    heroAliases["vboss"] = "bornag";
}

void initQuests() {
    quests.push_back({""});
    quests.push_back({"w5"});
    quests.push_back({"f1", "a1", "f1", "a1", "f1", "a1"});
    quests.push_back({"f5", "a5"});
    quests.push_back({"f2", "a2", "e2", "w2", "f3", "a3"});
    quests.push_back({"w3", "e3", "w3", "e3", "w3", "e3"});       //5
    quests.push_back({"w4", "e1", "a4", "f4", "w1", "e4"});
    quests.push_back({"f5", "a5", "f4", "a3", "f2", "a1"});
    quests.push_back({"e4", "w4", "w5", "e5", "w4", "e4"});
    quests.push_back({"w5", "f5", "e5", "a5", "w4", "f4"});
    quests.push_back({"w5", "e5", "a5", "f5", "e5", "w5"});       //10
    quests.push_back({"f5", "f6", "e5", "e6", "a5", "a6"});
    quests.push_back({"e5", "w5", "f5", "e6", "f6", "w6"});
    quests.push_back({"a8", "a7", "a6", "a5", "a4", "a3"});
    quests.push_back({"f7", "f6", "f5", "e7", "e6", "e6"});
    quests.push_back({"w5", "e6", "w6", "e8", "w8"});             //15
    quests.push_back({"a9", "f8", "a8"});
    quests.push_back({"w5", "e6", "w7", "e8", "w8"});
    quests.push_back({"f7", "f6", "a6", "f5", "a7", "a8"});
    quests.push_back({"e7", "w9", "f9", "e9"});
    quests.push_back({"f2", "a4", "f5", "a7", "f8", "a10"});      //20
    quests.push_back({"w10", "a10", "w10"});
    quests.push_back({"w9", "e10", "f10"});
    quests.push_back({"e9", "a9", "w8", "f8", "e8"});
    quests.push_back({"f6", "a7", "f7", "a8", "f8", "a9"});
    quests.push_back({"w8", "w7", "w8", "w8", "w7", "w8"});       //25
    quests.push_back({"a9", "w7", "w8", "e7", "e8", "f10"});
    quests.push_back({"e9", "f9", "w9", "f7", "w7", "w7"});
    quests.push_back({"a10", "a8", "a9", "a10", "a9"});
    quests.push_back({"a10", "w7", "f7", "e8", "a9", "a9"});
    quests.push_back({"e10", "e10", "e10", "f10"});               //30
    quests.push_back({"e9", "f10", "f9", "f9", "a10", "a7"});
    quests.push_back({"w1", "a9", "f10", "e9", "a10", "w10"});
    quests.push_back({"e9", "a9", "a9", "f9", "a9", "f10"});
    quests.push_back({"f8", "e9", "w9", "a9", "a10", "a10"});
    quests.push_back({"w8", "w8", "w10", "a10", "a10", "f10"});   //35
    quests.push_back({"a8", "a10", "f10", "a10", "a10", "a10"});
    quests.push_back({"e8", "a10", "e10", "f10", "f10", "e10"});
    quests.push_back({"f10", "e10", "w10", "a10", "w10", "w10"});
    quests.push_back({"w9", "a10", "w10", "e10", "a10", "a10"});
    quests.push_back({"w10", "a10", "w10", "a10", "w10", "a10"}); //40
    quests.push_back({"e12", "e11", "a11", "f11", "a12"});
    quests.push_back({"a11", "a11", "e11", "a11", "e11", "a11"});
    quests.push_back({"a8", "a11", "a10", "w10", "a12", "e12"});
    quests.push_back({"a10", "f10", "a12", "f10", "a10", "f12"});
    quests.push_back({"w4", "e11", "a12", "a12", "w11", "a12"});  //45
    quests.push_back({"a11", "a12", "a11", "f11", "a11", "f10"});
    quests.push_back({"f12", "w11", "e12", "a12", "w12"});
    quests.push_back({"a11", "a11", "e12", "a11", "a11", "a13"});
    quests.push_back({"a13", "f13", "f13", "f13"});
    quests.push_back({"f12", "f12", "f12", "f12", "f12", "f12"}); //50
    quests.push_back({"a11", "e11", "a13", "a11", "e11", "a13"});
    quests.push_back({"f13", "w13", "a13", "f12", "f12"});
    quests.push_back({"a9", "f13", "f13", "f12", "a12", "a12"});
    quests.push_back({"a13", "a13", "a12", "a12", "f11", "f12"});
    quests.push_back({"a11", "f10", "a11", "e14", "f13", "a11"}); //55
    quests.push_back({"f13", "a13", "f13", "e13", "w12"});
    quests.push_back({"e10", "a13", "w12", "f13", "f13", "f13"});
    quests.push_back({"f7", "w11", "w13", "e14", "f13", "a14"});
    quests.push_back({"a8", "f15", "a14", "f14", "w14"});
    quests.push_back({"f12", "w13", "a14", "f13", "a13", "e10"}); //60
    quests.push_back({"f13", "e13", "a13", "w12", "f12", "a12"});
    quests.push_back({"w13", "e12", "w12", "a14", "a12", "f13"});
    quests.push_back({"e15", "f14", "w14", "a15"});
    quests.push_back({"e12", "a14", "e14", "w13", "e12", "f13"});
    quests.push_back({"e13", "f12", "w11", "w12", "a14", "e14"}); //65
    quests.push_back({"a14", "e13", "a11", "a14", "f13", "e13"});
    quests.push_back({"f13", "w13", "e14", "f13", "f14", "a14"});
    quests.push_back({"a15", "e15", "f15", "w15"});
    quests.push_back({"f13", "a14", "e14", "f13", "a14", "f13"});
    quests.push_back({"a11", "a14", "w13", "e14", "a14", "f14"}); //70
    quests.push_back({"e13", "a14", "f14", "w13", "f14", "e14"});
    quests.push_back({"w10", "a14", "a14", "a14", "a14", "w14"});
    quests.push_back({"w13", "w13", "f14", "a15", "a15", "e13"});
    quests.push_back({"a14", "e14", "e14", "e14", "e14", "e14"});
    quests.push_back({"w15", "w15", "e15", "w15", "f15"});        //75
    quests.push_back({"f14", "e15", "a15", "w14", "a14", "e15"});
    quests.push_back({"w14", "a15", "w14", "e15", "a15", "w14"});
    quests.push_back({"w15", "w15", "w15", "w15", "f15", "f15"});
    quests.push_back({"a15", "a15", "a15", "a15", "a15", "w14"});
    quests.push_back({"f15", "w15", "w15", "w15", "w15", "w15"}); // 80
    quests.push_back({"f14", "e16", "e16", "e16", "e16"});
    quests.push_back({"w14", "a15", "f15", "a16", "f16", "f15"});
    quests.push_back({"w15", "f15", "w15", "w15", "a16", "w16"});
    quests.push_back({"a16", "w15", "a16", "e16", "a17"});
    quests.push_back({"f15", "w15", "w15", "w15", "e17", "e16"}); // 85
    quests.push_back({"a13", "a16", "a16", "a16", "a16", "f16"});
    quests.push_back({"e16", "f16", "f16", "f17", "a17"});
    quests.push_back({"w15", "f16", "a16", "a16", "f16", "e17"});
    quests.push_back({"f16", "f17", "a17", "a15", "a16", "a16"});
    quests.push_back({"f16", "f16", "f16", "f16", "f16", "a18"});  //90
    quests.push_back({"e16", "e16", "a17", "f17", "a17", "w15"});
    quests.push_back({"f17", "a18", "a18", "w17", "a17", "e16"});
    quests.push_back({"e18", "f16", "f16", "f16", "w16", "f18"});
    quests.push_back({"a21", "a20", "f20", "a21"});
    quests.push_back({"e18", "e17", "a18", "e17", "e17", "e20"}); //95
    quests.push_back({"a19", "a19", "w18", "w18", "f15", "e16"});
    quests.push_back({"w18", "f19", "f19", "e18", "e18", "a19"});
    quests.push_back({"f18", "w19", "w19", "e19", "e19", "f18"});
    quests.push_back({"f19", "a19", "e19", "f20", "a20", "f19"});
    quests.push_back({"a20", "w18", "w18", "a19", "w20", "f20"}); // 100
    quests.push_back({"a22", "e21", "f20", "w20", "f22"});
    quests.push_back({"f23", "w21", "f20", "a20", "a21"});
    quests.push_back({"f22", "w21", "w21", "f21", "e21"});
    quests.push_back({"a20", "f20", "e21", "a21", "a20", "f20"});
    quests.push_back({"f20", "e21", "f20", "w20", "e21", "f20"}); // 105
    quests.push_back({"e21", "w22", "f23", "a23", "a22"});
    quests.push_back({"f21", "a20", "f21", "a21", "w21", "e21"});
    quests.push_back({"w22", "w22", "a22", "f22", "e21", "w21"});
    quests.push_back({"e22", "f22", "a22", "w21", "e22", "w21"});
    quests.push_back({"a22", "w22", "a22", "w21", "e22", "w22"}); // 110
    quests.push_back({"f23", "a22", "e23", "e23", "e22", "w22"});
    quests.push_back({"w22", "w23", "a23", "w22", "f21", "f21"});
    quests.push_back({"w24", "a24", "e24", "f24", "f23", "f23"});
    quests.push_back({"a24", "a25", "a24", "f25", "e23"});
    quests.push_back({"e23", "f23", "e23", "w25", "a24", "a23"}); // 115
    quests.push_back({"e24", "a24", "e24", "f23", "w24", "w23"});
    quests.push_back({"e24", "e24", "a24", "w24", "f24", "w24"});
    quests.push_back({"f24", "a23", "a24", "f24", "f24", "w24"});
    quests.push_back({"f25", "f25", "a26", "a26", "w25"});
    quests.push_back({"e27", "w27", "e27", "w27"}); // 120
    quests.push_back({"w27", "f27", "f27", "w27"});
    quests.push_back({"a27", "e27", "f27", "w27"});
    quests.push_back({"e27", "e27", "w27", "f27"});
    quests.push_back({"a28", "f27", "w27", "f27"});
    quests.push_back({"a28", "a27", "w27", "w28"}); // 125
    quests.push_back({"f26", "w28", "w28", "w28"});
    quests.push_back({"e26", "w27", "w27", "w28"});
    quests.push_back({"f27", "w28", "w29", "w29"});
    quests.push_back({"a27", "w27", "w29", "w29"});
    quests.push_back({"e27", "w28", "w29", "w29"}); // 130
    quests.push_back({"a26", "e26", "w29", "e29", "f27"});
    quests.push_back({"w29", "f29", "a27", "a26", "a28"});
    quests.push_back({"w29", "e29", "a29", "a28"});
    quests.push_back({"a27", "w29", "a27", "f28", "a27"});
    quests.push_back({"w29", "w30", "a26", "a27", "a30"}); // 135
    quests.push_back({"f26", "a30", "f27", "e30", "w28"});
    quests.push_back({"a30", "a27", "a30", "a27", "e30"});
    quests.push_back({"a27", "w30", "f28", "w30", "a27", "a27"});
    quests.push_back({"w27", "w29", "a30", "a27", "e30", "a27"});
    quests.push_back({"f27", "e30", "f28", "a30", "e27", "a27"}); // 140
    quests.push_back({"w29", "a30", "a30", "nicte:1000"});
    quests.push_back({"a30", "w29", "e29", "ladyodelith:1000"});
    quests.push_back({"e30", "w29", "w30", "f25", "ignitor:1000"});
    quests.push_back({"f27", "w28", "a30", "a27", "petry:1000"});
    quests.push_back({"e26", "e30", "w29", "e29", "ignitor:1000"}); // 145
    quests.push_back({"f26", "e30", "f25", "a30", "w29", "undine:1000"});
    quests.push_back({"w29", "a30", "undine:1000", "petry:1000"});
    quests.push_back({"a28", "w30", "ignitor:1000", "chroma:1000"});
    quests.push_back({"a28", "a28", "w28", "undine:1000", "zaytus:1000"});
    quests.push_back({"w28", "w28", "w29", "undine:1000", "ladyodelith:1000"}); // 150
    quests.push_back({"e30", "e30", "w30", "w30", "undine:1000", "chroma:1000"});
    quests.push_back({"f30", "f30", "e30", "a30", "ignitor:1000", "zaytus:1000"});
    quests.push_back({"e30", "w30", "chroma:1000", "nicte:1000", "forestdruid:1000"});
    quests.push_back({"w30", "a30", "undine:1000", "zaytus:1000", "ignitor:1000"});
    quests.push_back({"f30", "zaytus:1000", "chroma:1000", "nicte:1000", "forestdruid:1000"}); // 155
    quests.push_back({"f30", "f30", "undine:1000", "ladyodelith:1000", "forestdruid:1000", "petry:1000"});
    quests.push_back({"e30", "e30", "ignitor:1000", "zaytus:1000", "nicte:1000", "undine:1000"});
    quests.push_back({"w30", "ladyodelith:1000", "ignitor:1000", "zaytus:1000", "chroma:1000", "forestdruid:1000"});
    quests.push_back({"ignitor:1000", "forestdruid:1000", "petry:1000", "chroma:1000", "ladyodelith:1000", "undine:1000"});
    quests.push_back({"f30", "ignitor:1000", "undine:1000", "neptunius:1000"}); // 160
    quests.push_back({"e30", "forestdruid:1000", "nicte:1000", "undine:1000", "shygu:1000"});
    quests.push_back({"e30", "f30", "petry:1000", "zaytus:1000", "ignitor:1000", "lordkirk:1000"});
    quests.push_back({"a30", "undine:1000", "ignitor:1000", "zaytus:1000", "nicte:1000", "shygu:1000"});
    quests.push_back({"f30", "ignitor:1000", "forestdruid:1000", "ladyodelith:1000", "undine:1000", "thert:1000"});
    quests.push_back({"zaytus:1000", "ignitor:1000", "undine:1000", "ladyodelith:1000", "chroma:1000", "hosokawa:1000"}); // 165
    quests.push_back({"forestdruid:1000", "undine:1000", "takeda:1000", "shygu:1000"});
    quests.push_back({"ladyodelith:1000", "petry:1000", "neptunius:1000", "hattori:1000"});
    quests.push_back({"zaytus:1000", "petry:1000", "chroma:1000", "mahatma:1000", "edana:1000"});
    quests.push_back({"nicte:1000", "ladyodelith:1000", "ignitor:1000", "lordkirk:1000", "dybbuk:1000"});
    quests.push_back({"petry:1000", "nicte:1000", "chroma:1000", "forestdruid:1000", "thert:1000", "jade:1000"}); // 170
    quests.push_back({"zaytus:1000", "chroma:1000", "hattori:1000", "hirate:1000"});
    quests.push_back({"undine:1000", "ladyodelith:1000", "nicte:1000", "hosokawa:1000", "edana:1000"});
    quests.push_back({"forestdruid:1000", "thert:1000", "neptunius:1000", "mahatma:1000"});
    quests.push_back({"zaytus:1000", "ignitor:1000", "lordkirk:1000", "hirate:1000", "takeda:1000"});
    quests.push_back({"petry:1000", "nicte:1000", "chroma:1000", "shygu:1000", "takeda:1000", "neptunius:1000"}); // 175
    quests.push_back({"thert:1000", "neptunius:1000", "mahatma:1000", "hosokawa:1000"});
    quests.push_back({"zaytus:1000", "lordkirk:1000", "jade:1000", "mahatma:1000", "dybbuk:1000"});
    quests.push_back({"forestdruid:1000", "jade:1000", "takeda:1000", "hattori:1000", "hirate:1000"});
    quests.push_back({"zaytus:1000", "chroma:1000", "shygu:1000", "hattori:1000", "mahatma:1000", "edana:1000"});
    quests.push_back({"forestdruid:1000", "neptunius:1000", "thert:1000", "mahatma:1000", "jade:1000", "dybbuk:1000"}); // 180
}

void readMonsterData(std::vector<std::string>::iterator it, std::vector<std::string>::iterator itEnd) {
    while (it != itEnd) {
                monsterBaseList.push_back(Monster(std::stoi(*(it)), std::stoi(*(it+1)), std::stoll(*(it+2)), *(it+3), (Element)stringToEnum[*(it+4)]));
        it += 5;
    }
}

void readBaseHeroes(std::vector<std::string>::iterator it, std::vector<std::string>::iterator itEnd) {
    while (it != itEnd) {
        baseHeroes.push_back(Monster(std::stoi(*(it)), std::stoi(*(it+1)), *(it+2),
                                     (Element)stringToEnum[*(it+3)], (HeroRarity)stringToEnum[*(it+4)],
                                     {(SkillType)stringToEnum[*(it+5)], (Element)stringToEnum[*(it+6)],
                                     (Element)stringToEnum[*(it+7)], std::stod(*(it+8))},
                                     std::stoi(*(it+9)), std::stoi(*(it+10)), std::stoi(*(it+11)), std::stod(*(it+12))));
        it += 13;
    }
}

void readHeroAliases(std::vector<std::string>::iterator it, std::vector<std::string>::iterator itEnd) {
    while (it != itEnd) {
        heroAliases[*(it)] = *(it + 1);
        it += 2;
    }
}

void readQuests(std::vector<std::string>::iterator it, std::vector<std::string>::iterator itEnd) {
    quests.push_back({""}); // quest 0 empty

    while (it != itEnd) {
        std::vector<std::string> newQuest;
        while (*it != "+") {
            newQuest.push_back(*it);
            it++;
        }
        quests.push_back(newQuest);
        it++;
    }
}

// Fills all references and storages with real data.
// Must be called before any other operation on monsters or input
void initGameData() {
    // Initialize Monster Data
    std::ifstream file;
    file.open("cqdata.txt");
    // If data file is present use it, else use hardcoded data
    // Reading done with minimal validity checking for now
    if (file) {
        std::string token;
        std::vector<std::string> tokens;
        while (file >> token) {
            tokens.push_back(token);
        }
        file.close();

        std::vector<std::string>::iterator it = std::find(tokens.begin(), tokens.end(), "*") + 2;
        std::vector<std::string>::iterator itEnd = std::find(it, tokens.end(), "*");
        readMonsterData(it, itEnd);

        it = itEnd + 2;
        itEnd = std::find(it, tokens.end(), "*");
        readBaseHeroes(it, itEnd);

        it = itEnd + 2;
        itEnd = std::find(it, tokens.end(), "*");
        readHeroAliases(it, itEnd);

        it = itEnd + 2;
        itEnd = std::find(it, tokens.end(), "*");
        readQuests(it, itEnd);
    }
    else {
        initMonsterData();
        initBaseHeroes();
        initHeroAliases();
        initQuests();
    }
    initIndices();

    for (size_t i = 0; i < monsterBaseList.size(); i++) {
        monsterReference.push_back(monsterBaseList[i]);
        monsterMap.insert(std::pair<std::string, MonsterIndex>(monsterBaseList[i].name, i));
    }
}

// Filter monsters according to user input. Fills the available-references
// Must be called before any instance can be solved
void filterMonsterData(FollowerCount minimumMonsterCost, FollowerCount maximumArmyCost) {
    std::vector<Monster> tempMonsterList = monsterBaseList; // Get a temporary list to sort
    sort(tempMonsterList.begin(), tempMonsterList.end(), isCheaper);
    availableMonsters.clear();

    for (size_t i = 0; i < tempMonsterList.size(); i++) {
        if (minimumMonsterCost <= tempMonsterList[i].cost && maximumArmyCost >= tempMonsterList[i].cost) {
            availableMonsters.push_back(monsterMap[tempMonsterList[i].name]);
        }
    }
}

// Remove monsters from available monsters higher than the maximum cost
void pruneAvailableMonsters(const FollowerCount maximumArmyCost, std::vector<MonsterIndex> & aMonsters) {
    int extra = 0;
    for (auto i = aMonsters.rbegin(); i != aMonsters.rend(); i++) {
        if (monsterReference[*i].cost > maximumArmyCost) {
            extra++;
        }
        else {
            break;
        }
    }
    aMonsters.resize(aMonsters.size()-extra);
}

// Add a leveled hero to the database and return its corresponding index
MonsterIndex addLeveledHero(Monster & hero, int level, int promo) {
    Monster m(hero, level, promo);
    monsterReference.emplace_back(m);

    return (MonsterIndex) (monsterReference.size() - 1);
}

// Get Index corresponding to the id used ingame. monsters >= 0, heroes <= -2, empty spot = -1
int getRealIndex(Monster & monster) {
    int index = INDEX_NO_MONSTER;
    size_t i;
    if (monster.rarity != NO_HERO) {
        for (i = 0; i < baseHeroes.size(); i++) {
            if (baseHeroes[i].baseName == monster.baseName) {
                index = (-int(i) - 2);
            }
        }
    } else {
        for (i = 0; i < monsterBaseList.size(); i++) {
            if (monster.name == monsterBaseList[i].name) {
                index = (int) i;
            }
        }
    }
    return index;
}
