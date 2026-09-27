/*
 * T249 - original object ScnRegister.cpp (guessed name), one translation unit: Scenaric_RegisterAllClasses 0x513560
 * alone, with the class-factory declarations.
 * .text 0x513560-0x5145bc, .data 0x57b98c-0x57b998 (the cinematic header's stride-table copy + 3 pad; the map notes
 * its owner among the objects 402..407 is uncertain, it is assigned here). No .rdata/.bss.
 */
/* BYTES: layout. */
/* BYTES(layout): the Cine.h header static: every object including the cinematic header carries this copy in its .data, referenced or not */
#include "sdw_types.h"
#include "sdw_enums.h"
#include "scenaric_props.h"
#include "sdw_classes.h"

void Scenaric_RegisterClass_2(u16 classId, ScnObject *(*factory)(void *), u32 classFlags, u16 iconIdA, u16 iconIdB);

/* 0x57b98c - the cinematic header's static copy of the 9-byte opcode stride table (src/engine/cine1.cpp,
 * g_cineOpStride 0x5816fc): unreferenced here; an object including that header carries its own copy in .data. */
static u8 s_cineOpStride[9] = {0, 8, 8, 4, 2, 2, 4, 2, 2};

/* The class factories (one per scenaric class, each in its class's file). */
#include "../objects/bipbiplevel14.h"
#include "../objects/bull.h"
#include "../objects/crocodilelevel09.h"
#include "../objects/crocodilelevel11.h"
#include "../objects/crowd.h"
#include "../objects/daffyelf.h"
#include "../objects/daffylevel01.h"
#include "../objects/daffylevel02.h"
#include "../objects/daffylevel09.h"
#include "../objects/daffymilitary.h"
#include "../objects/daffyscene.h"
#include "../objects/daffytraininglevel.h"
#include "../objects/daffywheel.h"
#include "../objects/dancingghost.h"
#include "../objects/dragon.h"
#include "../objects/elmer.h"
#include "../objects/ghost.h"
#include "../objects/gossamer_boss.h"
#include "../objects/gossamer_lev08.h"
#include "../objects/instantmartian.h"
#include "../objects/marvin.h"
#include "../objects/porkylevel01.h"
#include "../objects/prayingghost.h"
#include "../objects/robot.h"
#include "../objects/sam_api.h"
#include "../objects/sam_pirate.h"
#include "../objects/shark.h"
#include "../objects/sheep.h"
#include "../game/wolf_api.h"
#include "../objects/anvil.h"
#include "../objects/automaticdoor.h"
#include "../objects/balance.h"
#include "../objects/bat.h"
#include "../objects/battery.h"
#include "../objects/bell.h"
#include "../objects/bees.h"
#include "../objects/bird.h"
#include "../objects/bipbip.h"
#include "../objects/blackhole.h"
#include "../objects/bullet.h"
#include "../objects/box.h"
#include "../objects/bridge.h"
#include "../objects/bush.h"
#include "../objects/butterfly.h"
#include "../objects/cactus.h"
#include "../objects/cannonball.h"
#include "../objects/cannonball2.h"
#include "../objects/canondummy.h"
#include "../objects/canonsheep.h"
#include "../objects/canonsimple.h"
#include "../objects/case.h"
#include "../objects/catapult.h"
#include "../objects/chronometer.h"
#include "../objects/crane.h"
#include "../objects/crumblyground.h"
#include "../objects/crumblyplat.h"
#include "../objects/mine.h"
#include "../objects/diamond.h"
#include "../objects/doorlevel.h"
#include "../objects/doormechanism.h"
#include "../objects/doorworld.h"
#include "../objects/dynamite.h"
#include "../objects/elastic.h"
#include "../objects/elastictree.h"
#include "../objects/facingcamera.h"
#include "../objects/fallinggate.h"
#include "../objects/fallinggate2.h"
#include "../objects/fallingrock.h"
#include "../objects/fan.h"
#include "../objects/fireball.h"
#include "../objects/firefly.h"
#include "../objects/fish.h"
#include "../objects/fishingrod.h"
#include "../objects/floatingbox.h"
#include "../objects/flute.h"
#include "../objects/frozenriver.h"
#include "../objects/geyserin.h"
#include "../objects/geyserout.h"
#include "../objects/ghostcostume.h"
#include "../objects/ghosthalo.h"
#include "../objects/goal.h"
#include "../objects/goldencoins.h"
#include "../objects/gossameronde.h"
#include "../objects/hairdryer.h"
#include "../objects/heapofleaf.h"
#include "../objects/hiddenrocks.h"
#include "../objects/hitswitch.h"
#include "../objects/hive.h"
#include "../objects/honeypot.h"
#include "../objects/hoover.h"
#include "../objects/icecube.h"
#include "../objects/iceground.h"
#include "../objects/inflatablesheep.h"
#include "../objects/instanthoover.h"
#include "../objects/instantsocket.h"
#include "../objects/jail.h"
#include "../objects/key.h"
#include "../objects/laser.h"
#include "../objects/lava.h"
#include "../objects/leaf.h"
#include "../objects/lazerrobot.h"
#include "../objects/lightspot.h"
#include "../objects/magnet.h"
#include "../objects/mailbox.h"
#include "../objects/minedetector.h"
#include "../objects/monolithe.h"
#include "../objects/perfume.h"
#include "../objects/piranhas.h"
#include "../objects/pipe.h"
#include "../objects/pipe2.h"
#include "../objects/rabbitcostume.h"
#include "../objects/raft.h"
#include "../objects/remotecontrol.h"
#include "../objects/rock.h"
#include "../objects/rook.h"
#include "../objects/rocket.h"
#include "../objects/rocks.h"
#include "../objects/rollingcarpet.h"
#include "../objects/rcarpetmobile.h"
#include "../objects/resizer.h"
#include "../objects/salad.h"
#include "../objects/sail.h"
#include "../objects/scenesheeppanel.h"
#include "../objects/scene_wheel.h"
#include "../objects/seaweed.h"
#include "../objects/seed.h"
#include "../objects/seesaw.h"
#include "../objects/sensiblebutton.h"
#include "../objects/sheepcostume.h"
#include "../objects/signpost.h"
#include "../objects/signtips.h"
#include "../objects/slidingicecube.h"
#include "../objects/smallrock.h"
#include "../objects/snowball.h"
#include "../objects/snowyground.h"
#include "../objects/superbutton.h"
#include "../objects/swirlsign.h"
#include "../objects/telescope.h"
#include "../objects/timekeeper.h"
#include "../objects/timemachinechrono.h"
#include "../objects/timemachine.h"
#include "../objects/torch.h"
#include "../objects/train.h"
#include "../objects/trainstation.h"
#include "../objects/trafficjams.h"
#include "../objects/treesection.h"
#include "../objects/triggedstone.h"
#include "../objects/twig.h"
#include "../objects/umbrella.h"
#include "../objects/volcano.h"
#include "../objects/watch.h"
#include "../objects/watergeyser.h"
#include "../objects/watermine.h"
#include "../objects/wheel.h"
#include "../objects/wheeldummy.h"
#include "../objects/wolftrap.h"
#include "../objects/woodenlift.h"
#include "../objects/woodenplatform.h"
#include "../objects/ambient_sound.h"
#include "../objects/bonusmanager.h"
#include "../objects/cameramanager.h"
#include "../objects/cameramanager2.h"
#include "../objects/camerarestriction.h"
#include "../objects/checkpointmanager.h"
#include "../objects/cinematicsmanager.h"
#include "../objects/creditsmanager.h"
#include "../objects/dancingghost_manager.h"
#include "../objects/fogmanager.h"
#include "../objects/geysermanger.h"
#include "../objects/maplocation.h"
#include "../objects/mcardmanager.h"
#include "../objects/mirrormanager.h"
#include "../objects/sfxcinemanager.h"
#include "../objects/visibilitymanager.h"
#include "../objects/objectmanager.h"

/* 0x513560  Registers every scenaric class: first clears all 200 registry slots, then one call per class with its
 * factory, class flags and the resource ids of its two inventory icons. */
void Scenaric_RegisterAllClasses()
{
    u16 id;

    for (id = 0; id < 200; id++)
        Scenaric_RegisterClass_2(id, 0, 0, 0, 0);
    Scenaric_RegisterClass_2(CLASSID_BIPBIPLEVEL14, BipbipLevel14_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BULL, bull_Create, SCN_CF_CARRIER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CROCODILELEVEL09, CrocodileLevel09_Create, SCN_CF_CARRIER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CROCODILELEVEL11, CrocodileLevel11_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CROWD, Crowd_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DAFFYELF, DaffyElf_Create, SCN_CF_INTERACTABLE | SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DAFFYLEVEL01, DaffyLevel01_Create, SCN_CF_INTERACTABLE | SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DAFFYLEVEL02, DaffyLevel02_Create, SCN_CF_INTERACTABLE | SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DAFFYLEVEL09, DaffyLevel09_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DAFFYMILITARY, DaffyMilitary_Create, SCN_CF_INTERACTABLE | SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DAFFYSCENE, DaffyScene_Create, SCN_CF_INTERACTABLE | SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DAFFYTRAININGLEVEL, DaffyTrainingLevel_Create, SCN_CF_INTERACTABLE | SCN_CF_08,
                             0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DAFFYWHEEL, DaffyWheel_Create, SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DANCINGGHOST, DancingGhost_Create, SCN_CF_IGNORED_BY_BUTTONS, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DRAGON, Dragon_Create, SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_ELMER, Elmer_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GHOST, Ghost_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GOSSAMER_BOSS, Gossamer_Boss_Create, SCN_CF_CARRIER | SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GOSSAMER_LEV08, Gossamer_Lev08_Create, SCN_CF_CARRIER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_INSTANTMARTIAN, InstantMartian_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_MARVIN, Marvin_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_PORKYLEVEL01, PorkyLevel01_Create, SCN_CF_INTERACTABLE | SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_PRAYINGGHOST, PrayingGhost_Create, SCN_CF_IGNORED_BY_BUTTONS, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_ROBOT, Robot_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_CARRIER | SCN_CF_08 | SCN_CF_SEESAW_SNAP, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SAM, Sam_Create, SCN_CF_CARRIER | SCN_CF_08, DAV_IDI_IFTTSAM_, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SAM_PIRATE, Sam_Pirate_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SHARK, Shark_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SHEEP, Sheep_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_CARRIER | SCN_CF_08 | SCN_CF_SEESAW_SNAP |
                                 SCN_CF_SHEEP_ANCHORABLE,
                             DAV_IDI_IMOICONB, DAV_IDI_IMOICONA);
    Scenaric_RegisterClass_2(CLASSID_WOLF, Wolf_Create,
                             SCN_CF_SHEEP_ATTRACTOR | SCN_CF_CARRIER | SCN_CF_08 | SCN_CF_SEESAW_SNAP, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_ANVIL, Anvil_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_AUTOMATICDOOR, AutomaticDoor_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BALANCE, balance_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BAT, Bat_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BATTERY, Battery_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BELL, Bell_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BEES, Bees_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BIRD, Bird_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BIPBIP, bipbip_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BLACKHOLE, BlackHole_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BULLET, Bullet_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BOX, box_Create, SCN_CF_CARRIER | SCN_CF_08 | SCN_CF_SEESAW_SNAP, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BRIDGE, bridge_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BUSH, Bush_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SHEEP_ATTRACTOR | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT,
                             0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BUTTERFLY, Butterfly_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CACTUS, Cactus_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CANNONBALL, CannonBall_Create, SCN_CF_IGNORED_BY_BUTTONS, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CANNONBALL2, CannonBall2_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_CARRIER | SCN_CF_08 | SCN_CF_SEESAW_SNAP, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CANONDUMMY, CanonDummy_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CANONSHEEP, CanonSheep_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CANONSIMPLE, CanonSimple_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CASE, Case_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    /* cast kept: this factory takes its record as u16 *; the registry stores every factory as ScnObject *(*)(void *) */
    Scenaric_RegisterClass_2(CLASSID_CATAPULT, (ScnObject * (*)(void *)) Catapult_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    /* cast kept: this factory takes its record as u16 *; the registry stores every factory as ScnObject *(*)(void *) */
    Scenaric_RegisterClass_2(CLASSID_CHRONOMETER, (ScnObject * (*)(void *)) Chronometer_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CRANE, Crane_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CRUMBLYGROUND, CrumblyGround_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CRUMBLYPLAT, CrumblyPlat_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DEFUSABLEMINE, DefusableMine_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IMNICONB, DAV_IDI_IMNICONA);
    Scenaric_RegisterClass_2(CLASSID_DIAMOND, Diamond_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DOORLEVEL, DoorLevel_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DOORMECHANISM, DoorMechanism_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DOORWORLD, DoorWorld_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DYNAMITE, Dynamite_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IDYICONB, DAV_IDI_IDYICONA);
    Scenaric_RegisterClass_2(CLASSID_ELASTIC, elastic_Create, SCN_CF_INTERACTABLE | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IEAICONB, DAV_IDI_IEAICONA);
    Scenaric_RegisterClass_2(CLASSID_ELASTICTREE, ElasticTree_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FACINGCAMERA, FacingCamera_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FALLINGGATE, FallingGate_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FALLINGGATE2, FallingGate2_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FALLINGROCK, FallingRock_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FAN, Fan_Create, SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IVEICONB, DAV_IDI_IVEICONA);
    Scenaric_RegisterClass_2(CLASSID_FIREBALL, FireBall_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FIREFLY, Firefly_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FISH, Fish_Create, 0x0, DAV_IDI_ICPICONB, DAV_IDI_ICPICONA);
    Scenaric_RegisterClass_2(CLASSID_FISHINGROD, FishingRod_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_COMPONENT_ITEM | SCN_CF_INVENTORY_ITEM, DAV_IDI_ICPICONB,
                             DAV_IDI_ICPICONA);
    Scenaric_RegisterClass_2(
        CLASSID_FLOATINGBOX, FloatingBox_Create,
        SCN_CF_INTERACTABLE | SCN_CF_CARRIER | SCN_CF_08 | SCN_CF_SEESAW_SNAP | SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FLUTE, Flute_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_INVENTORY_ITEM, DAV_IDI_IFTICONB,
                             DAV_IDI_IFTICONA);
    Scenaric_RegisterClass_2(CLASSID_FROZENRIVER, FrozenRiver_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GEYSERIN, GeyserIn_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GEYSEROUT, GeyserOut_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GHOSTCOSTUME, GhostCostume_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_ICFICONB, DAV_IDI_ICFICONA);
    Scenaric_RegisterClass_2(CLASSID_GHOSTHALO, GhostHalo_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GOAL, Goal_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GOLDENCOINS, GoldenCoins_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IORICONB, DAV_IDI_IORICONA);
    Scenaric_RegisterClass_2(CLASSID_GOSSAMERONDE, GossamerOnde_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GROUNDMINE, GroundMine_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_HAIRDRYER, HairDryer_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_INVENTORY_ITEM, DAV_IDI_ICHICONB,
                             DAV_IDI_ICHICONA);
    Scenaric_RegisterClass_2(CLASSID_HEAPOFLEAF, HeapOfLeaf_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_HIDDENROCKS, HiddenRocks_Create, SCN_CF_INTERACTABLE | SCN_CF_INTERACT_ANY_FACING,
                             0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_HITSWITCH, HitSwitch_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_HIVE, Hive_Create, SCN_CF_08, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_HONEYPOT, HoneyPot_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IPMICONB, DAV_IDI_IPMICONA);
    Scenaric_RegisterClass_2(CLASSID_HOOVER, Hoover_Create, SCN_CF_INTERACTABLE | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IASICONB, DAV_IDI_IASICONA);
    Scenaric_RegisterClass_2(CLASSID_ICECUBE, IceCube_Create, SCN_CF_CARRIER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_ICEGROUND, IceGround_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_INFLATABLESHEEP, InflatableSheep_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IMGICONB, DAV_IDI_IMGICONA);
    Scenaric_RegisterClass_2(CLASSID_INSTANTHOOVER, InstantHoover_Create, SCN_CF_INTERACTABLE | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IGHICONB, DAV_IDI_IGHICONA);
    Scenaric_RegisterClass_2(CLASSID_INSTANTSOCKET, InstantSocket_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_JAIL, Jail_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_KEY, Key_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IFFICONB, DAV_IDI_IFFICONA);
    Scenaric_RegisterClass_2(CLASSID_LASER, Laser_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_LAVA, Lava_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_LEAF, Leaf_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_LAZERROBOT, LazerRobot_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_LIGHTSPOT, LightSpot_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_MAGNET, Magnet_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_COMPONENT_ITEM | SCN_CF_INVENTORY_ITEM, DAV_IDI_IAIICONB,
                             DAV_IDI_IAIICONA);
    Scenaric_RegisterClass_2(CLASSID_MAGNETROD, MagnetRod_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_COMPOSITE_ITEM | SCN_CF_INVENTORY_ITEM, DAV_IDI_ICPICONC,
                             DAV_IDI_ICPICONA);
    Scenaric_RegisterClass_2(CLASSID_MAILBOX, Mailbox_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_MINEDETECTOR, MineDetector_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_INVENTORY_ITEM, DAV_IDI_IDTICONB,
                             DAV_IDI_IDTICONA);
    Scenaric_RegisterClass_2(CLASSID_MONOLITHE, Monolithe_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_PERFUME, Perfume_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IFLICONB, DAV_IDI_IFLICONA);
    Scenaric_RegisterClass_2(CLASSID_PIRANHAS, Piranhas_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_PIPE, Pipe_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_PIPE2, Pipe2_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_RABBITCOSTUME, RabbitCostume_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_ICLICONB, DAV_IDI_ICLICONA);
    Scenaric_RegisterClass_2(CLASSID_RAFT, Raft_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_REMOTECONTROL, RemoteControl_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_INVENTORY_ITEM, DAV_IDI_ITRICONB,
                             DAV_IDI_ITRICONA);
    Scenaric_RegisterClass_2(CLASSID_ROCK, Rock_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_CARRIER | SCN_CF_SEESAW_SNAP | SCN_CF_GROUND_PROVIDER, 0x0,
                             0x0);
    Scenaric_RegisterClass_2(CLASSID_ROOK, Rook_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_ROCKET, Rocket_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_INVENTORY_ITEM, DAV_IDI_IFUICONB,
                             DAV_IDI_IFUICONA);
    Scenaric_RegisterClass_2(CLASSID_ROCKS, Rocks_Create, SCN_CF_CARRIER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_ROLLINGCARPET, RollingCarpet_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_RCARPETMOBILE, RCarpetMobile_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_RESIZER, Resizer_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SALAD, Salad_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SHEEP_ATTRACTOR | SCN_CF_SEESAW_SNAP |
                                 SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_ILAICONB, DAV_IDI_ILAICONA);
    Scenaric_RegisterClass_2(CLASSID_SAIL, Sail_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SALADROD, SaladRod_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_COMPOSITE_ITEM | SCN_CF_INVENTORY_ITEM, DAV_IDI_ICPICOND,
                             DAV_IDI_ICPICONA);
    Scenaric_RegisterClass_2(CLASSID_SCENESHEEPPANEL, SceneSheepPanel_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SCENE_WHEEL, Scene_Wheel_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SEAWEED, Seaweed_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SEED, Seed_Create, SCN_CF_INTERACTABLE | SCN_CF_INVENTORY_ITEM, DAV_IDI_IGAICONB,
                             DAV_IDI_IGAICONA);
    Scenaric_RegisterClass_2(CLASSID_SEESAW, seesaw_Create, SCN_CF_08 | SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SENSIBLEBUTTON, SensibleButton_Create,
                             SCN_CF_SHEEP_ATTRACTOR | SCN_CF_IGNORED_BY_BUTTONS, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SHEEPCOSTUME, SheepCostume_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_SNAP_TO_SUPPORT | SCN_CF_INVENTORY_ITEM,
                             DAV_IDI_IDUICONB, DAV_IDI_IDUICONA);
    Scenaric_RegisterClass_2(CLASSID_SIGNPOST, SignPost_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SIGNPOSTSIMPLE, SignPostSimple_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SIGNPOSTANIMATED, SignPostAnimated_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SIGNTIPS, SignTips_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SLIDINGICECUBE, SlidingIceCube_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SMALLROCK, SmallRock_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_CARRIER | SCN_CF_SEESAW_SNAP, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SNOWBALL, Snowball_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_CARRIER | SCN_CF_08 | SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SNOWYGROUND, SnowyGround_Create, SCN_CF_CARRIER | SCN_CF_GROUND_PROVIDER, 0x0,
                             0x0);
    Scenaric_RegisterClass_2(CLASSID_SUPERBUTTON, SuperButton_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SWIRLSIGN, SwirlSign_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_TELESCOPE, Telescope_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    /* cast kept: this factory takes its record as u16 *; the registry stores every factory as ScnObject *(*)(void *) */
    Scenaric_RegisterClass_2(CLASSID_TIMEKEEPER, (ScnObject * (*)(void *)) TimeKeeper_Create, SCN_CF_INTERACTABLE, 0x0,
                             0x0);
    Scenaric_RegisterClass_2(CLASSID_TIMEMACHINECHRONO, TimeMachineChrono_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_INVENTORY_ITEM, DAV_IDI_IC4ICONB, DAV_IDI_IC4ICONA);
    Scenaric_RegisterClass_2(CLASSID_TIMEMACHINESPHERE, TimeMachineSphere_Create, 0x0, 0x0, 0x0);
    /* cast kept: this factory takes its record as u16 *; the registry stores every factory as ScnObject *(*)(void *) */
    Scenaric_RegisterClass_2(CLASSID_TORCH, (ScnObject * (*)(void *)) Torch_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_TRAIN, Train_Create, SCN_CF_GROUND_PROVIDER | SCN_CF_IGNORED_BY_BUTTONS, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_TRAINSTATION, TrainStation_Create, 0x0, 0x0, 0x0);
    /* cast kept: this factory takes its record as u16 *; the registry stores every factory as ScnObject *(*)(void *) */
    Scenaric_RegisterClass_2(CLASSID_TRAFFICJAMS, (ScnObject * (*)(void *)) TrafficJams_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_TREE, Tree_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_TREESECTION, TreeSection_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_TRIGGEDSTONE, TriggedStone_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_TWIG, Twig_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_UMBRELLA, Umbrella_Create,
                             SCN_CF_INTERACTABLE | SCN_CF_SEESAW_SNAP | SCN_CF_INVENTORY_ITEM, DAV_IDI_IPRICONB,
                             DAV_IDI_IPRICONA);
    Scenaric_RegisterClass_2(CLASSID_VOLCANO, Volcano_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_WATCH, Watch_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_WATERGEYSER, WaterGeyser_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_WATERMINE, WaterMine_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_WHEEL, Wheel_Create, SCN_CF_CARRIER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_WHEELDUMMY, WheelDummy_Create, SCN_CF_CARRIER | SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_WOLFTRAP, WolfTrap_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_WOODEN_LIFT, WoodenLift_Create, SCN_CF_INTERACTABLE | SCN_CF_GROUND_PROVIDER, 0x0,
                             0x0);
    Scenaric_RegisterClass_2(CLASSID_WOODENPLATFORM, WoodenPlatForm_Create, SCN_CF_GROUND_PROVIDER, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_AMBIENTSOUNDMANAGER, AmbientSoundManager_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_BONUSMANAGER, BonusManager_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CAMERAMANAGER, CameraManager_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CAMERAMANAGER2, CameraManager2_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_CAMERARESTRICTION, CameraRestriction_Create, 0x0, 0x0, 0x0);
    /* cast kept: this factory takes its record as u16 *; the registry stores every factory as ScnObject *(*)(void *) */
    Scenaric_RegisterClass_2(CLASSID_CHECKPOINTMANAGER, (ScnObject * (*)(void *)) CheckpointManager_Create, 0x0, 0x0,
                             0x0);
    /* cast kept: this factory takes its record as u16 *; the registry stores every factory as ScnObject *(*)(void *) */
    Scenaric_RegisterClass_2(CLASSID_CINEMATICSMANAGER, (ScnObject * (*)(void *)) CinematicsManager_Create, 0x0, 0x0,
                             0x0);
    Scenaric_RegisterClass_2(CLASSID_CREDITSMANAGER, CreditsManager_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_DANCINGGHOSTMANAGER, DancingGhostManager_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_FOGMANAGER, FogManager_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_GEYSERMANGER, GeyserManger_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_MAPLOCATION, MapLocation_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_MCARDMANAGER, MCardManager_Create, SCN_CF_INTERACTABLE, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_MIRRORMANAGER, MirrorManager_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_SFXCINEMANAGER, SfxCineManager_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_VISIBILITYMANAGER, VisibilityManager_Create, 0x0, 0x0, 0x0);
    Scenaric_RegisterClass_2(CLASSID_OBJECTMANAGER, ObjectManager_Create, 0x0, 0x0, 0x0);
}
