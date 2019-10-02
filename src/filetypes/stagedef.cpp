//
// Created by Rune Tynan on 8/11/2018.
//

#include <fstream>

#include "at_logging"
#include "at_utils"
#include "filetypes/stagedef.h"

namespace types {

static logging::Logger *logger = logging::get_logger("stagedef");

STAGEDEF::STAGEDEF(const std::string &filename) {
    logger->debug("Parsing STAGEDEF");
    
    std::fstream input = std::fstream(filename);
    
    logger->trace("Read STAGEDEF header");
    ulong magic = util::next_ulong(input);
    if (magic != 0x00000000447A0000) {
        logger->warn("SMB2 Stagedef magic doesn't match. Parsing will likely fail");
    }
    num_collision = util::next_uint(input);
    collision_offset = util::next_uint(input);
    start_pos_offset = util::next_uint(input);
    fallout_plane_offset = util::next_uint(input);
    uint num_goals = util::next_uint(input);
    uint goals_offset = util::next_uint(input);
    num_bumpers = util::next_uint(input);
    bumper_offset = util::next_uint(input);
    num_jamabars = util::next_uint(input);
    jamabar_offset = util::next_uint(input);
    num_bananas = util::next_uint(input);
    banana_offset = util::next_uint(input);
    num_cones = util::next_uint(input);
    cone_offset = util::next_uint(input);
    num_spheres = util::next_uint(input);
    sphere_offset = util::next_uint(input);
    num_cylinders = util::next_uint(input);
    cylinder_offset = util::next_uint(input);
    num_fallouts = util::next_uint(input);
    fallouts_offset = util::next_uint(input);
    num_backgrounds = util::next_uint(input);
    background_offset = util::next_uint(input);
    num_unkown8 = util::next_uint(input);
    unknown8_offset = util::next_uint(input);
    util::next_uint(input); // Unkown/Null
    if (util::next_uint(input) != 1)
        logger->warn("Header constant not 1, file may be invalid");
    num_reflective = util::next_uint(input);
    reflective_offset = util::next_uint(input);
    util::next_long(input, 12); // Unknown/Null
    num_model_instances = util::next_uint(input);
    model_instances_offset = util::next_uint(input);
    num_models_a = util::next_uint(input);
    models_a_offset = util::next_uint(input);
    num_models_b = util::next_uint(input);
    models_b_offset = util::next_uint(input);
    util::next_long(input, 12); // Unknown/Null
    num_switches = util::next_uint(input);
    switch_offset = util::next_uint(input);
    fog_anim_offset = util::next_uint(input);
    num_wormholes = util::next_uint(input);
    wormhole_offset = util::next_uint(input);
    fog_offset = util::next_uint(input);
    unknown3_offset = util::next_uint(input);
    // 1988 Unknown/Null
    
    logger->trace("Read collision group headers");
    // TODO
    
    logger->trace("Read start pos");
    input.seekg(start_pos_offset);
    start = StartPos();
    start.read(input);
    
    logger->trace("Read fallout plane");
    input.seekg(fallout_plane_offset);
    fallout_plane = FalloutPlane();
    fallout_plane.read(input);
    
    logger->trace("Read goal list");
    input.seekg(goals_offset);
    for (uint i = 0; i < num_goals; ++i) {
        this->_load_obj(input, this->goals);
    }
    
    logger->trace("Read bumper list");
    input.seekg(bumper_offset);
    for (uint i = 0; i < num_bumpers; ++i) {
        this->_load_obj(input, this->bumpers);
    }
    
    logger->trace("Read jamabar list");
    input.seekg(jamabar_offset);
    for (uint i = 0; i < num_jamabars; ++i) {
        this->_load_obj(input, this->jamabars);
    }
    
    logger->trace("Read banana list");
    input.seekg(banana_offset);
    for (uint i = 0; i < num_bananas; ++i) {
        this->_load_obj(input, this->bananas);
    }
    
    logger->trace("Read cone collision objects");
    input.seekg(cone_offset);
    for (uint i = 0; i < num_cones; ++i) {
        this->_load_obj(input, this->cones);
    }
    
    logger->trace("Read sphere collision objects");
    input.seekg(sphere_offset);
    for (uint i = 0; i < num_cones; ++i) {
        this->_load_obj(input, this->spheres);
    }
    
    logger->trace("Read cylinder collision objects");
    input.seekg(cylinder_offset);
    for (uint i = 0; i < num_cylinders; ++i) {
        this->_load_obj(input, this->cylinders);
    }
    
    logger->trace("Read fallout volume list");
    input.seekg(fallouts_offset);
    for (uint i = 0; i < num_fallouts; ++i) {
        this->_load_obj(input, this->fallout_volumes);
    }
    
    logger->trace("Read background models list");
    input.seekg(background_offset);
    for (uint i = 0; i < num_backgrounds; ++i) {
        this->_load_obj(input, this->backgrounds);
    }
    
    logger->trace("Read unknown8s list");
    // TODO
    
    logger->trace("Read reflective models list");
    input.seekg(reflective_offset);
    for (uint i = 0; i < num_reflective; ++i) {
        this->_load_obj(input, this->reflective_models);
    }
    
    logger->trace("Read level model instances list");
    input.seekg(model_instances_offset);
    for (uint i = 0; i < num_model_instances; ++i) {
        this->_load_obj(input, this->model_instances);
    }
    
    logger->trace("Read level model pointers A list");
    input.seekg(models_a_offset);
    for (uint i = 0; i < num_models_a; ++i) {
        this->_load_obj(input, this->level_pointer_As);
    }
    
    logger->trace("Read level model pointers B list");
    input.seekg(models_b_offset);
    for (uint i = 0; i < num_models_b; ++i) {
        this->_load_obj(input, this->level_pointer_Bs);
    }
    
    logger->trace("Read switches list");
    input.seekg(switch_offset);
    for (uint i = 0; i < num_switches; ++i) {
        this->_load_obj(input, this->switches);
    }
    
    logger->trace("Read fog animation header");
    if (fog_anim_offset != 0) {
        input.seekg(fog_anim_offset);
        fog_anim_header = new FogAnimationHeader {};
        fog_anim_header->read(input);
    }
    
    logger->trace("Read wormhole list");
    input.seekg(wormhole_offset);
    for (uint i = 0; i < num_wormholes; ++i) {
        this->_load_obj(input, this->wormholes);
    }
    
    logger->trace("Read fog");
    if (fog_offset != 0) {
        input.seekg(fog_offset);
        fog = new Fog {};
    }
    
    logger->trace("Read unknown3");
    // TODO
    
    logger->debug("Finished parsing STAGEDEF");
}

std::string* STAGEDEF::_load_model_name(std::iostream& input, uint offset) {
    static std::map<uint, std::string*> names;
    
    if (!names.count(offset)) {
        size_t pos = input.tellg();
        input.seekg(offset);
        model_names.emplace_back(util::next_string(input));
        names[offset] = &model_names.back();
        input.seekg(pos);
    }
    
    return names[offset];
}

}
