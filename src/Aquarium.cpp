#include "Aquarium.h"
#include <cstdlib>


string AquariumCreatureTypeToString(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return "BiggerFish";
        case AquariumCreatureType::NPCreature:
            return "BaseFish";
        case AquariumCreatureType::Crab:
            return "Crab";
        case AquariumCreatureType::Predator:
            return "Predator";
        default:
            return "UknownFish";
    }
}

// PlayerCreature Implementation
PlayerCreature::PlayerCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 10.0f, 1, sprite) {}


void PlayerCreature::setDirection(float dx, float dy) {
    m_dx = dx;
    m_dy = dy;
    normalize();
}

void PlayerCreature::move() {
    this->bounce();
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
}

void PlayerCreature::reduceDamageDebounce() {
    if (m_damage_debounce > 0) {
        --m_damage_debounce;
    }
}

void PlayerCreature::update() {
    this->reduceDamageDebounce();
    this->move();
}


void PlayerCreature::draw() const {
    
    ofLogVerbose() << "PlayerCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    if (this->m_damage_debounce > 0) {
        ofSetColor(ofColor::red); // Flash red if in damage debounce
    }
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
    ofSetColor(ofColor::white); // Reset color

}

void PlayerCreature::changeSpeed(int speed) {
    m_speed = speed;
}

void PlayerCreature::loseLife(int debounce) {
    if (m_damage_debounce <= 0) {
        if (m_lives > 0) this->m_lives -= 1;
        m_damage_debounce = debounce; // Set debounce frames
        ofLogNotice() << "Player lost a life! Lives remaining: " << m_lives << std::endl;
    }
    // If in debounce period, do nothing
    if (m_damage_debounce > 0) {
        ofLogVerbose() << "Player is in damage debounce period. Frames left: " << m_damage_debounce << std::endl;
    }
}

// NPCreature Implementation
NPCreature::NPCreature(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: Creature(x, y, speed, 30, 1, sprite) {
    m_dx = (rand() % 3 - 1); // -1, 0, or 1
    m_dy = (rand() % 3 - 1); // -1, 0, or 1
    normalize();

    m_creatureType = AquariumCreatureType::NPCreature;
}

void NPCreature::move() {
    // Simple AI movement logic (random direction)
    m_x += m_dx * m_speed;
    m_y += m_dy * m_speed;
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }
    bounce();
}

void NPCreature::draw() const {
    ofLogVerbose() << "NPCreature at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    ofSetColor(ofColor::white);
    if (m_sprite) {
        m_sprite->draw(m_x, m_y);
    }
}

int NPCreature::getPlayerX() {
    auto player = GetPlayer();
    if (!player) {
        ofLogError() << "Player not set yet!";
        return 0;
    }
    return player->getX();
}

int NPCreature::getPlayerY() {
    auto player = GetPlayer();
    if (!player) {
        ofLogError() << "Player not set yet!";
        return 0;
    }
    return player->getY();
}

int NPCreature::getPlayerDirection() {
    if (getPlayerX() > getX()) {
        return 1;
    }
    else if (getPlayerX() < getX()) {
        return -1;
    }
    else {
        return 0;
    }
}

Crab::Crab(float x, float aquariumHeight, int speed, std::shared_ptr<GameSprite> sprite)
: GroundCreature(x, aquariumHeight, speed, sprite) {
    m_dx = (rand() % 2 == 0) ? 1 : -1;

    setCollisionRadius(60);
    m_value = 10;
    m_creatureType = AquariumCreatureType::Crab;
}

void Crab::move() {

    m_x += m_dx * m_speed; // Moves at half speed
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void Crab::draw() const {
    ofLogVerbose() << "Crab at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}

BiggerFish::BiggerFish(float x, float y, int speed, std::shared_ptr<GameSprite> sprite)
: NPCreature(x, y, speed, sprite) {
    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();

    setCollisionRadius(60); // Bigger fish have a larger collision radius
    m_value = 5; // Bigger fish have a higher value
    m_creatureType = AquariumCreatureType::BiggerFish;
}

void BiggerFish::move() {
    // Bigger fish might move slower or have different logic
    m_x += m_dx * (m_speed * 0.5); // Moves at half speed
    m_y += m_dy * (m_speed * 0.5);
    if(m_dx < 0 ){
        this->m_sprite->setFlipped(true);
    }else {
        this->m_sprite->setFlipped(false);
    }

    bounce();
}

void BiggerFish::draw() const {
    ofLogVerbose() << "BiggerFish at (" << m_x << ", " << m_y << ") with speed " << m_speed << std::endl;
    this->m_sprite->draw(this->m_x, this->m_y);
}

Predator::Predator(float x, float y, int speed,
                   std::shared_ptr<GameSprite> headSprite,
                   std::shared_ptr<GameSprite> bodySprite,
                   std::shared_ptr<GameSprite> tailSprite,
                   int segmentCount)
: NPCreature(x, y, speed, headSprite),
  m_bodySprite(bodySprite),
  m_tailSprite(tailSprite)
{
    m_segments.resize(segmentCount + 2);
    for (int i = 0; i < m_segments.size(); ++i) {
        m_segments[i].position.set(x - i * m_segmentDistance, y);
    }

    m_dx = (rand() % 3 - 1);
    m_dy = (rand() % 3 - 1);
    normalize();

    setCollisionRadius(40);
    m_value = 10;
    m_creatureType = AquariumCreatureType::Predator;
}

void Predator::draw() const {
    if (!m_sprite || !m_bodySprite || !m_tailSprite) return;

    // HEAD rotation (facing the next segment)
    if (m_segments.size() >= 2) {
        float angle = atan2(
            m_segments[1].position.y - m_segments[0].position.y,
            m_segments[1].position.x - m_segments[0].position.x
        );
        m_sprite->drawRot(m_segments[0].position.x, m_segments[0].position.y, ofRadToDeg(angle) - 90);
    }

    // BODY segments
    for (size_t i = 1; i + 1 < m_segments.size(); ++i) {
        float angle = atan2(
            m_segments[i + 1].position.y - m_segments[i].position.y,
            m_segments[i + 1].position.x - m_segments[i].position.x
        );
        m_bodySprite->drawRot(m_segments[i].position.x, m_segments[i].position.y, ofRadToDeg(angle) - 90);
    }

    // TAIL rotation (facing previous segment)
    if (m_segments.size() >= 2) {
        size_t last = m_segments.size() - 1;
        float angle = atan2(
            m_segments[last - 1].position.y - m_segments[last].position.y,
            m_segments[last - 1].position.x - m_segments[last].position.x
        );
        m_tailSprite->drawRot(m_segments[last].position.x, m_segments[last].position.y, ofRadToDeg(angle) + 90);
    }
}

void Predator::move() {

    ofVec2f playerPos(getPlayerX(), getPlayerY());
    ofVec2f headPos = m_segments[0].position;

    // direction to player
    ofVec2f dir = playerPos - headPos;
    float len = dir.length();
    if (len > 0.0001f) dir.normalize();

    // add a subtle sine-wave wobble (so it "curls")
    float t = ofGetElapsedTimef();
    float angleOffset = sin(t * 4.0f) * 0.5f; // tune freq & magnitude
    float c = cos(angleOffset);
    float s = sin(angleOffset);
    ofVec2f rotatedDir(dir.x * c - dir.y * s,
                       dir.x * s + dir.y * c);

    // move head toward player
    float headSpeed = std::max(0.0f, static_cast<float>(m_speed) * 2); // tune multiplier
    m_segments[0].position += rotatedDir * headSpeed;

    // each segment follows the previous one, maintaining segment distance
    for (size_t i = 1; i < m_segments.size(); ++i) {
        ofVec2f delta = m_segments[i - 1].position - m_segments[i].position;
        float dist = delta.length();
        if (dist > 0.0001f) {
            delta.normalize();
            // move so the gap becomes segmentDistance (smooth trailing)
            m_segments[i].position += delta * (dist - m_segmentDistance);
        }
    }

    // sync base Creature position with head for collisions/logic
    this->m_x = m_segments[0].position.x;
    this->m_y = m_segments[0].position.y;

    // Optionally update sprite flip based on movement direction:
    if (rotatedDir.x < 0) this->m_sprite->setFlipped(true);
    else this->m_sprite->setFlipped(false);
}


// AquariumSpriteManager
AquariumSpriteManager::AquariumSpriteManager(){
    this->m_npc_fish = std::make_shared<GameSprite>("base-fish.png", 70,70);
    this->m_big_fish = std::make_shared<GameSprite>("bigger-fish.png", 120, 120);
    this->m_crab_fish = std::make_shared<GameSprite>("crab.png", 50, 50);
    this->m_predator_head = std::make_shared<GameSprite>("predator-head.png", 50, 50);
    this->m_predator_body = std::make_shared<GameSprite>("predator-body.png", 50, 50);
    this->m_predator_tail = std::make_shared<GameSprite>("predator-tail.png", 50, 50);
}

std::shared_ptr<GameSprite> AquariumSpriteManager::GetSprite(AquariumCreatureType t){
    switch(t){
        case AquariumCreatureType::BiggerFish:
            return std::make_shared<GameSprite>(*this->m_big_fish);
            
        case AquariumCreatureType::NPCreature:
            return std::make_shared<GameSprite>(*this->m_npc_fish);
        case AquariumCreatureType::Crab:
            return std::make_shared<GameSprite>(*this->m_crab_fish);
        case AquariumCreatureType::Predator:
            return std::make_shared<GameSprite>(*this->m_predator_head);
        case AquariumCreatureType::PredatorBody:
            return std::make_shared<GameSprite>(*this->m_predator_body);
        case AquariumCreatureType::PredatorTail:
            return std::make_shared<GameSprite>(*this->m_predator_tail);
        default:
            return nullptr;
    }
}


// Aquarium Implementation
Aquarium::Aquarium(int width, int height, std::shared_ptr<AquariumSpriteManager> spriteManager)
    : m_width(width), m_height(height) {
        m_sprite_manager =  spriteManager;
    }



void Aquarium::addCreature(std::shared_ptr<Creature> creature) {
    creature->setBounds(m_width - 20, m_height - 20);
    m_creatures.push_back(creature);
}

void Aquarium::addAquariumLevel(std::shared_ptr<AquariumLevel> level){
    if(level == nullptr){return;} // guard to not add noise
    this->m_aquariumlevels.push_back(level);
}

void Aquarium::update() {
    for (auto& creature : m_creatures) {
        creature->move();
    }
    this->Repopulate();
}

void Aquarium::draw() const {
    for (const auto& creature : m_creatures) {
        creature->draw();
    }
}


void Aquarium::removeCreature(std::shared_ptr<Creature> creature) {
    auto it = std::find(m_creatures.begin(), m_creatures.end(), creature);
    if (it != m_creatures.end()) {
        ofLogVerbose() << "removing creature " << endl;
        int selectLvl = this->currentLevel % this->m_aquariumlevels.size();
        auto npcCreature = std::static_pointer_cast<NPCreature>(creature);
        this->m_aquariumlevels.at(selectLvl)->ConsumePopulation(npcCreature->GetType(), npcCreature->getValue());
        m_creatures.erase(it);
    }
}

void Aquarium::clearCreatures() {
    m_creatures.clear();
}

std::shared_ptr<Creature> Aquarium::getCreatureAt(int index) {
    if (index < 0 || size_t(index) >= m_creatures.size()) {
        return nullptr;
    }
    return m_creatures[index];
}



void Aquarium::SpawnCreature(AquariumCreatureType type) {
    int x = rand() % this->getWidth();
    int y = rand() % this->getHeight();
    int speed = 1 + rand() % 25; // Speed between 1 and 25

    switch (type) {
        case AquariumCreatureType::NPCreature:
            this->addCreature(std::make_shared<NPCreature>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::NPCreature)));
            break;
        case AquariumCreatureType::BiggerFish:
            this->addCreature(std::make_shared<BiggerFish>(x, y, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::BiggerFish)));
            break;
        case AquariumCreatureType::Crab:
            this->addCreature(std::make_shared<Crab>(x, this->getHeight(), speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::Crab)));
        case AquariumCreatureType::Predator:
            this->addCreature(std::make_shared<Predator>(x, 0, speed, this->m_sprite_manager->GetSprite(AquariumCreatureType::Predator),
                                                        this->m_sprite_manager->GetSprite(AquariumCreatureType::PredatorBody),
                                                        this->m_sprite_manager->GetSprite(AquariumCreatureType::PredatorTail)));
        default:
            ofLogError() << "Unknown creature type to spawn!";
            break;
    }

}


// repopulation will be called from the levl class
// it will compose into aquarium so eating eats frm the pool of NPCs in the lvl class
// once lvl criteria met, we move to new lvl through inner signal asking for new lvl
// which will mean incrementing the buffer and pointing to a new lvl index
void Aquarium::Repopulate() {
    ofLogVerbose("entering phase repopulation");
    // lets make the levels circular
    int selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
    ofLogVerbose() << "the current index: " << selectedLevelIdx << endl;
    std::shared_ptr<AquariumLevel> level = this->m_aquariumlevels.at(selectedLevelIdx);


    if(level->isCompleted()){
        level->levelReset();
        this->currentLevel += 1;
        selectedLevelIdx = this->currentLevel % this->m_aquariumlevels.size();
        ofLogNotice()<<"new level reached : " << selectedLevelIdx << std::endl;
        level = this->m_aquariumlevels.at(selectedLevelIdx);
        this->clearCreatures();
    }

    
    // now lets find how many to respawn if needed 
    std::vector<AquariumCreatureType> toRespawn = level->Repopulate();
    ofLogVerbose() << "amount to repopulate : " << toRespawn.size() << endl;
    if(toRespawn.size() <= 0 ){return;} // there is nothing for me to do here
    for(AquariumCreatureType newCreatureType : toRespawn){
        this->SpawnCreature(newCreatureType);
    }
}


// Aquarium collision detection
std::shared_ptr<GameEvent> DetectAquariumCollisions(std::shared_ptr<Aquarium> aquarium, std::shared_ptr<PlayerCreature> player) {
    if (!aquarium || !player) return nullptr;
    
    for (int i = 0; i < aquarium->getCreatureCount(); ++i) {
        std::shared_ptr<Creature> npc = aquarium->getCreatureAt(i);
        if (auto predator = std::dynamic_pointer_cast<Predator>(npc)) {
            std::vector<Predator::Segment> segments = predator->getSegments();
            for (size_t s = 0; s < segments.size(); ++s) {
                float dx = segments[s].position.x - player->getX();
                float dy = segments[s].position.y - player->getY();
                float distanceSq = dx * dx + dy * dy;
                float collisionRadius = (s == 0) ? 20.0f : (s == segments.size() - 1) ? 15.0f : 12.0f;
                if (distanceSq < collisionRadius * collisionRadius) {
                    return std::make_shared<GameEvent>(GameEventType::COLLISION, player, predator);
                }
            }
        }
        else {
            if (npc && checkCollision(player, npc)) {
                return std::make_shared<GameEvent>(GameEventType::COLLISION, player, npc);
            }
        }
    }
    return nullptr;
};

//  Imlementation of the AquariumScene

void AquariumGameScene::Update(){
    std::shared_ptr<GameEvent> event;

    float dx = 0;
    float dy = 0;

    if(keysDown[OF_KEY_LEFT])  dx -= 1;
    if(keysDown[OF_KEY_RIGHT]) dx += 1;
    if(keysDown[OF_KEY_UP])    dy -= 1;
    if(keysDown[OF_KEY_DOWN])  dy += 1;

    m_player->setDirection(dx, dy);
    this->m_player->update();

    if (this->updateControl.tick()) {
        event = DetectAquariumCollisions(this->m_aquarium, this->m_player);
        if (event != nullptr && event->isCollisionEvent()) {
            ofLogVerbose() << "Collision detected between player and NPC!" << std::endl;
            if(event->creatureB != nullptr){
                event->print();
                if(this->m_player->getPower() < event->creatureB->getValue()){
                    ofLogNotice() << "Player is too weak to eat the creature!" << std::endl;
                    this->m_player->loseLife(3*60); // 3 frames debounce, 3 seconds at 60fps
                    if(this->m_player->getLives() <= 0){
                        this->m_lastEvent = std::make_shared<GameEvent>(GameEventType::GAME_OVER, this->m_player, nullptr);
                        return;
                    }
                }
                else{
                    this->m_aquarium->removeCreature(event->creatureB);
                    this->m_player->addToScore(1, event->creatureB->getValue());
                    if (this->m_player->getScore() % 25 == 0){
                        this->m_player->increasePower(1);
                        ofLogNotice() << "Player power increased to " << this->m_player->getPower() << "!" << std::endl;
                    }
                    
                }
                
                

            } else {
                ofLogError() << "Error: creatureB is null in collision event." << std::endl;
            }
        }
        this->m_aquarium->update();
    }

}

void AquariumGameScene::Draw() {
    this->m_player->draw();
    this->m_aquarium->draw();
    this->paintAquariumHUD();

}


void AquariumGameScene::paintAquariumHUD(){
    float panelWidth = ofGetWindowWidth() - 150;
    ofDrawBitmapString("Score: " + std::to_string(this->m_player->getScore()), panelWidth, 20);
    ofDrawBitmapString("Power: " + std::to_string(this->m_player->getPower()), panelWidth, 30);
    ofDrawBitmapString("Lives: " + std::to_string(this->m_player->getLives()), panelWidth, 40);
    for (int i = 0; i < this->m_player->getLives(); ++i) {
        ofSetColor(ofColor::red);
        ofDrawCircle(panelWidth + i * 20, 50, 5);
    }
    ofSetColor(ofColor::white); // Reset color to white for other drawings
}

void AquariumLevel::populationReset(){
    for(auto node: this->m_levelPopulation){
        node->currentPopulation = 0; // need to reset the population to ensure they are made a new in the next level
    }
}

void AquariumLevel::ConsumePopulation(AquariumCreatureType creatureType, int power){
    for(std::shared_ptr<AquariumLevelPopulationNode> node: this->m_levelPopulation){
        ofLogVerbose() << "consuming from this level creatures" << endl;
        if(node->creatureType == creatureType){
            ofLogVerbose() << "-cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            if(node->currentPopulation == 0){
                return;
            } 
            node->currentPopulation -= 1;
            ofLogVerbose() << "+cosuming from type: " << AquariumCreatureTypeToString(node->creatureType) <<" , currPop: " << node->currentPopulation << endl;
            this->m_level_score += power;
            return;
        }
    }
}

bool AquariumLevel::isCompleted(){
    return this->m_level_score >= this->m_targetScore;
}




std::vector<AquariumCreatureType> Level_0::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        ofLogVerbose() << "to Repopulate :  " << delta << endl;
        if(delta >0){
            for(int i = 0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;

}

std::vector<AquariumCreatureType> Level_1::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}

std::vector<AquariumCreatureType> Level_2::Repopulate() {
    std::vector<AquariumCreatureType> toRepopulate;
    for(std::shared_ptr<AquariumLevelPopulationNode> node : this->m_levelPopulation){
        int delta = node->population - node->currentPopulation;
        if(delta >0){
            for(int i=0; i<delta; i++){
                toRepopulate.push_back(node->creatureType);
            }
            node->currentPopulation += delta;
        }
    }
    return toRepopulate;
}
