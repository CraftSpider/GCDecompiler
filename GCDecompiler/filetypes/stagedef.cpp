//
// Created by Rune Tynan on 8/11/2018.
//

#include <fstream>
#include "a_logging"
#include "stagedef.h"
#include "utils.h"

namespace types {

static logging::Logger *logger = logging::get_logger("stagedef");

STAGEDEF::STAGEDEF(const std::string &filename) {
    logger->debug("Parsing STAGEDEF");
    
    std::fstream input = std::fstream(filename);
    
    logger->trace("Read STAGEDEF header");
    ulong magic = next_long(input);
    num_collision = next_int(input);
    collision_offset = next_int(input);
    start_pos_offset = next_int(input);
    fallout_plane_offset = next_int(input);
    num_goals = next_int(input);
    goals_offset = next_int(input);
    num_bumpers = next_int(input);
    bumper_offset = next_int(input);
    num_jamabars = next_int(input);
    jamabar_offset = next_int(input);
    num_bananas = next_int(input);
    banana_offset = next_int(input);
    num_cones = next_int(input);
    cone_offset = next_int(input);
    num_spheres = next_int(input);
    sphere_offset = next_int(input);
    num_cylinders = next_int(input);
    cylinder_offset = next_int(input);
    num_fallouts = next_int(input);
    fallouts_offset = next_int(input);
    num_backgrounds = next_int(input);
    background_offset = next_int(input);
    num_unkown8 = next_int(input);
    unknown8_offset = next_int(input);
    next_int(input); // Unkown/Null
    if (next_int(input) != 1)
        logger->warn("Header constant not 1, file may be invalid");
    num_reflective = next_int(input);
    reflective_offset = next_int(input);
    next_long(input, BIG, 12); // Unknown/Null
    num_model_instances = next_int(input);
    model_offset = next_int(input);
    num_models_a = next_int(input);
    models_a_offset = next_int(input);
    num_models_b = next_int(input);
    models_b_offset = next_int(input);
    next_long(input, BIG, 12); // Unknown/Null
    num_switches = next_int(input);
    switch_offset = next_int(input);
    fog_anim_offset = next_int(input);
    num_wormholes = next_int(input);
    wormhole_offset = next_int(input);
    fog_offset = next_int(input);
    unknown3_offset = next_int(input);
    // 1988 Unknown/Null
    
    logger->trace("Read collision group headers");
    // TODO
    
    logger->trace("Read start pos");
    input.seekg(start_pos_offset);
    starts = new StartPos[1];
    StartPos start {};
    start.x_pos = next_float(input);
    start.y_pos = next_float(input);
    start.z_pos = next_float(input);
    start.x_rot = next_short(input);
    start.y_rot = next_short(input);
    start.z_rot = next_short(input);
    starts[0] = start;
    
    logger->trace("Read fallout plane");
    input.seekg(fallout_plane_offset);
    FalloutPlane plane {};
    plane.y_pos = next_short(input);
    fallout_plane = plane;
    
    logger->trace("Read goal list");
    input.seekg(goals_offset);
    goals = new Goal[num_goals];
    for (uint i = 0; i < num_goals; ++i) {
        Goal goal {};
        goal.x_pos = next_float(input);
        goal.y_pos = next_float(input);
        goal.z_pos = next_float(input);
        goal.x_rot = next_short(input);
        goal.y_rot = next_short(input);
        goal.z_rot = next_short(input);
        goal.type = next_short(input);
        goals[i] = goal;
    }
    
    logger->trace("Read bumper list");
    input.seekg(bumper_offset);
    bumpers = new Bumper[num_bumpers];
    for (uint i = 0; i < num_bumpers; ++i) {
        Bumper bumper {};
        bumper.x_pos = next_float(input);
        bumper.y_pos = next_float(input);
        bumper.z_pos = next_float(input);
        bumper.x_rot = next_short(input);
        bumper.y_rot = next_short(input);
        bumper.z_rot = next_short(input);
        next_short(input);
        bumper.x_scale = next_float(input);
        bumper.y_scale = next_float(input);
        bumper.z_scale = next_float(input);
        bumpers[i] = bumper;
    }
    
    logger->trace("Read jamabar list");
    input.seekg(jamabar_offset);
    jamabars = new Jamabar[num_jamabars];
    for (uint i = 0; i < num_jamabars; ++i) {
        Jamabar jamabar {};
        jamabar.x_pos = next_float(input);
        jamabar.y_pos = next_float(input);
        jamabar.z_pos = next_float(input);
        jamabar.x_rot = next_short(input);
        jamabar.y_rot = next_short(input);
        jamabar.z_rot = next_short(input);
        next_short(input);
        jamabar.x_scale = next_float(input);
        jamabar.y_scale = next_float(input);
        jamabar.z_scale = next_float(input);
        jamabars[i] = jamabar;
    }
    
    logger->trace("Read banana list");
    input.seekg(banana_offset);
    bananas = new Banana[num_bananas];
    for (uint i = 0; i < num_bananas; ++i) {
        Banana banana {};
        banana.x_pos = next_float(input);
        banana.y_pos = next_float(input);
        banana.z_pos = next_float(input);
        banana.type = next_int(input);
        bananas[i] = banana;
    }
    
    logger->trace("Read cone collision objects");
    input.seekg(cone_offset);
    cones = new ConeCollision[num_cones];
    for (uint i = 0; i < num_cones; ++i) {
        ConeCollision cone {};
        cone.x_pos = next_float(input);
        cone.y_pos = next_float(input);
        cone.z_pos = next_float(input);
        cone.x_rot = next_short(input);
        cone.y_rot = next_short(input);
        cone.z_rot = next_short(input);
        next_short(input);
        cone.radius = next_float(input);
        cone.height = next_float(input);
        next_float(input); // Radius again?
        cones[i] = cone;
    }
    
    logger->trace("Read sphere collision objects");
    input.seekg(sphere_offset);
    spheres = new SphereCollision[num_spheres];
    for (uint i = 0; i < num_cones; ++i) {
        SphereCollision sphere {};
        sphere.x_pos = next_float(input);
        sphere.y_pos = next_float(input);
        sphere.z_pos = next_float(input);
        sphere.radius = next_float(input);
        next_int(input); // Unknown/Null
        spheres[i] = sphere;
    }
    
    logger->trace("Read cylinder collision objects");
    input.seekg(cylinder_offset);
    cylinders = new CylinderCollision[num_cylinders];
    for (uint i = 0; i < num_cylinders; ++i) {
        CylinderCollision cylinder {};
        cylinder.x_pos = next_float(input);
        cylinder.y_pos = next_float(input);
        cylinder.z_pos = next_float(input);
        cylinder.radius = next_float(input);
        cylinder.height = next_float(input);
        cylinder.x_rot = next_short(input);
        cylinder.y_rot = next_short(input);
        cylinder.z_rot = next_short(input);
        next_short(input);
        cylinders[i] = cylinder;
    }
    
    logger->trace("Read fallout volume list");
    input.seekg(fallouts_offset);
    fallout_volumes = new FalloutVolume[num_fallouts];
    for (uint i = 0; i < num_fallouts; ++i) {
        FalloutVolume fallout {};
        fallout.center_x = next_float(input);
        fallout.center_y = next_float(input);
        fallout.center_z = next_float(input);
        fallout.x_size = next_float(input);
        fallout.y_size = next_float(input);
        fallout.z_size = next_float(input);
        fallout.x_rot = next_short(input);
        fallout.y_rot = next_short(input);
        fallout.z_rot = next_short(input);
        next_short(input);
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
        s.x_pos = next_float(input);
        s.y_pos = next_float(input);
        s.z_pos = next_float(input);
        s.x_rot = next_short(input);
        s.y_rot = next_short(input);
        s.z_rot = next_short(input);
        s.type = next_short(input);
        s.anim_ids = next_short(input);
        next_short(input);
        switches[i] = s;
    }
    
    logger->trace("Read fog animation header");
    
    logger->trace("Read wormhole list");
    input.seekg(wormhole_offset);
    wormholes = new Wormhole[num_wormholes];
    for (uint i = 0; i < num_wormholes; ++i) {
        Wormhole wormhole {};
        if (next_int(input) != 1)
            logger->warn("Malformed wormhole entry, file may be invalid");
        wormhole.offset = (uint)input.tellg();
        wormhole.x_pos = next_float(input);
        wormhole.y_pos = next_float(input);
        wormhole.z_pos = next_float(input);
        wormhole.x_rot = next_short(input);
        wormhole.y_rot = next_short(input);
        wormhole.z_rot = next_short(input);
        next_short(input);
        wormhole.destination = (Wormhole*)next_int(input);
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
