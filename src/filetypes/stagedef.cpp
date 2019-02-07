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
    num_collision = util::next_uint(input);
    collision_offset = util::next_uint(input);
    start_pos_offset = util::next_uint(input);
    fallout_plane_offset = util::next_uint(input);
    num_goals = util::next_uint(input);
    goals_offset = util::next_uint(input);
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
    model_offset = util::next_uint(input);
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
    starts = new StartPos[1];
    StartPos start {};
    start.x_pos = util::next_float(input);
    start.y_pos = util::next_float(input);
    start.z_pos = util::next_float(input);
    start.x_rot = util::next_ushort(input);
    start.y_rot = util::next_ushort(input);
    start.z_rot = util::next_ushort(input);
    starts[0] = start;
    
    logger->trace("Read fallout plane");
    input.seekg(fallout_plane_offset);
    FalloutPlane plane {};
    plane.y_pos = util::next_float(input);
    fallout_plane = plane;
    
    logger->trace("Read goal list");
    input.seekg(goals_offset);
    goals = new Goal[num_goals];
    for (uint i = 0; i < num_goals; ++i) {
        Goal goal {};
        goal.x_pos = util::next_float(input);
        goal.y_pos = util::next_float(input);
        goal.z_pos = util::next_float(input);
        goal.x_rot = util::next_ushort(input);
        goal.y_rot = util::next_ushort(input);
        goal.z_rot = util::next_ushort(input);
        goal.type = util::next_ushort(input);
        goals[i] = goal;
    }
    
    logger->trace("Read bumper list");
    input.seekg(bumper_offset);
    bumpers = new Bumper[num_bumpers];
    for (uint i = 0; i < num_bumpers; ++i) {
        Bumper bumper {};
        bumper.x_pos = util::next_float(input);
        bumper.y_pos = util::next_float(input);
        bumper.z_pos = util::next_float(input);
        bumper.x_rot = util::next_ushort(input);
        bumper.y_rot = util::next_ushort(input);
        bumper.z_rot = util::next_ushort(input);
        util::next_ushort(input);
        bumper.x_scale = util::next_float(input);
        bumper.y_scale = util::next_float(input);
        bumper.z_scale = util::next_float(input);
        bumpers[i] = bumper;
    }
    
    logger->trace("Read jamabar list");
    input.seekg(jamabar_offset);
    jamabars = new Jamabar[num_jamabars];
    for (uint i = 0; i < num_jamabars; ++i) {
        Jamabar jamabar {};
        jamabar.x_pos = util::next_float(input);
        jamabar.y_pos = util::next_float(input);
        jamabar.z_pos = util::next_float(input);
        jamabar.x_rot = util::next_ushort(input);
        jamabar.y_rot = util::next_ushort(input);
        jamabar.z_rot = util::next_ushort(input);
        util::next_ushort(input);
        jamabar.x_scale = util::next_float(input);
        jamabar.y_scale = util::next_float(input);
        jamabar.z_scale = util::next_float(input);
        jamabars[i] = jamabar;
    }
    
    logger->trace("Read banana list");
    input.seekg(banana_offset);
    bananas = new Banana[num_bananas];
    for (uint i = 0; i < num_bananas; ++i) {
        Banana banana {};
        banana.x_pos = util::next_float(input);
        banana.y_pos = util::next_float(input);
        banana.z_pos = util::next_float(input);
        banana.type = util::next_uint(input);
        bananas[i] = banana;
    }
    
    logger->trace("Read cone collision objects");
    input.seekg(cone_offset);
    cones = new ConeCollision[num_cones];
    for (uint i = 0; i < num_cones; ++i) {
        ConeCollision cone {};
        cone.x_pos = util::next_float(input);
        cone.y_pos = util::next_float(input);
        cone.z_pos = util::next_float(input);
        cone.x_rot = util::next_ushort(input);
        cone.y_rot = util::next_ushort(input);
        cone.z_rot = util::next_ushort(input);
        util::next_ushort(input);
        cone.radius = util::next_float(input);
        cone.height = util::next_float(input);
        util::next_float(input); // Radius again?
        cones[i] = cone;
    }
    
    logger->trace("Read sphere collision objects");
    input.seekg(sphere_offset);
    spheres = new SphereCollision[num_spheres];
    for (uint i = 0; i < num_cones; ++i) {
        SphereCollision sphere {};
        sphere.x_pos = util::next_float(input);
        sphere.y_pos = util::next_float(input);
        sphere.z_pos = util::next_float(input);
        sphere.radius = util::next_float(input);
        util::next_uint(input); // Unknown/Null
        spheres[i] = sphere;
    }
    
    logger->trace("Read cylinder collision objects");
    input.seekg(cylinder_offset);
    cylinders = new CylinderCollision[num_cylinders];
    for (uint i = 0; i < num_cylinders; ++i) {
        CylinderCollision cylinder {};
        cylinder.x_pos = util::next_float(input);
        cylinder.y_pos = util::next_float(input);
        cylinder.z_pos = util::next_float(input);
        cylinder.radius = util::next_float(input);
        cylinder.height = util::next_float(input);
        cylinder.x_rot = util::next_ushort(input);
        cylinder.y_rot = util::next_ushort(input);
        cylinder.z_rot = util::next_ushort(input);
        util::next_ushort(input);
        cylinders[i] = cylinder;
    }
    
    logger->trace("Read fallout volume list");
    input.seekg(fallouts_offset);
    fallout_volumes = new FalloutVolume[num_fallouts];
    for (uint i = 0; i < num_fallouts; ++i) {
        FalloutVolume fallout {};
        fallout.center_x = util::next_float(input);
        fallout.center_y = util::next_float(input);
        fallout.center_z = util::next_float(input);
        fallout.x_size = util::next_float(input);
        fallout.y_size = util::next_float(input);
        fallout.z_size = util::next_float(input);
        fallout.x_rot = util::next_ushort(input);
        fallout.y_rot = util::next_ushort(input);
        fallout.z_rot = util::next_ushort(input);
        util::next_ushort(input);
        fallout_volumes[i] = fallout;
    }
    
    logger->trace("Read background models list");
    // TODO
    
    logger->trace("Read unknown8s list");
    // TODO
    
    logger->trace("Read reflective models list");
    // TODO
    
    logger->trace("Read level model instances list");
    // TODO
    
    logger->trace("Read level model pointers A list");
    // TODO
    
    logger->trace("Read level model pointers B list");
    // TODO
    
    logger->trace("Read switches list");
    input.seekg(switch_offset);
    switches = new Switch[num_switches];
    for (uint i = 0; i < num_switches; ++i) {
        Switch s {};
        s.x_pos = util::next_float(input);
        s.y_pos = util::next_float(input);
        s.z_pos = util::next_float(input);
        s.x_rot = util::next_ushort(input);
        s.y_rot = util::next_ushort(input);
        s.z_rot = util::next_ushort(input);
        s.type = util::next_ushort(input);
        s.anim_ids = util::next_ushort(input);
        util::next_ushort(input);
        switches[i] = s;
    }
    
    logger->trace("Read fog animation header");
    
    logger->trace("Read wormhole list");
    input.seekg(wormhole_offset);
    wormholes = new Wormhole[num_wormholes];
    for (uint i = 0; i < num_wormholes; ++i) {
        Wormhole wormhole {};
        if (util::next_uint(input) != 1)
            logger->warn("Malformed wormhole entry, file may be invalid");
        wormhole.offset = (uint)input.tellg();
        wormhole.x_pos = util::next_float(input);
        wormhole.y_pos = util::next_float(input);
        wormhole.z_pos = util::next_float(input);
        wormhole.x_rot = util::next_ushort(input);
        wormhole.y_rot = util::next_ushort(input);
        wormhole.z_rot = util::next_ushort(input);
        util::next_ushort(input);
        wormhole.destination = (Wormhole*)(ulong)util::next_uint(input);
        wormholes[i] = wormhole;
    }
    for (uint i = 0; i < num_wormholes; ++i) {
        ulong offset = (ulong)wormholes[i].destination;
        for (uint j = 0; j < num_wormholes; ++j) {
            if (wormholes[j].offset == offset) {
                wormholes[i].destination = &wormholes[j];
            }
        }
    }
    
    logger->trace("Read fog");
    // TODO
    
    logger->trace("Read unknown3");
    // TODO
    
    logger->debug("Finished parsing STAGEDEF");
}

}
