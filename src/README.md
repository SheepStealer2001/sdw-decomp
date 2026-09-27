# src/ — the decompiled source

What is here and what is not:

| Layer | Where | State |
|---|---|---|
| Ghidra's automatic pseudo-C for every function | `work/decomp/` (regenerated, not committed) | all game functions |
| Names / structs / enums that make it readable | `data/symbols*.csv`, `data/structs/`, `data/enums/` → `src/include/sdw_*.h` | every matched function is named (the matcher finds functions by name) |
| Source **proven** by recompiling with the original compiler and byte-matching the exe | `src/**/*.c, *.cpp` | **all 3,305 game functions**, organised as the original 316 object files (one source file per object, in link order: `data/tu_map.json`), and nothing written here is unmatched (`data/match_results.json`) |

The proof: `python3 tools/vc6.py --all` compiles every file here with VC6 SP5 + Processor Pack (the game's compiler, installed in `work/vc6/`, see BUILDING.md) and `tools/match.py` compares each function with the exe after resolving every relocation against the symbol tables. MATCH = every byte equal and every address agreed; MATCH~ = every byte equal but some target address could only be read from the original (CRT helpers such as `__ftol`, or a name the tables spell differently); DIFF = not yet. A match proves the machine code, not the original spelling of the source.

| File | Object | Original .text | Functions |
|---|---|---|---|
| `app/app_main.cpp` | T001 | 0x401000–0x403f29 | 22 |
| `app/input_device.cpp` | T002 | 0x403f30–0x40448c | 9 |
| `app/d3dapp.cpp` | T003 | 0x4044c0–0x40656f | 39 |
| `engine/sound_device.cpp` | T004 | 0x4065a0–0x406d7a | 17 |
| `app/joystick.cpp` | T005 | 0x406db0–0x40717c | 6 |
| `app/keyboard.cpp` | T006 | 0x4071b0–0x4077b4 | 7 |
| `engine/mat44.cpp` | T007 | 0x4077f0–0x409e88 | 31 |
| `engine/mouse.cpp` | T008 | 0x409e90–0x40a10f | 5 |
| `engine/texture.cpp` | T009 | 0x40a140–0x40b028 | 15 |
| `engine/timer.cpp` | T010 | 0x40b060–0x40b5c8 | 12 |
| `engine/mesh_anim_seq.cpp` | T011 | 0x40b600–0x40b62b | 2 |
| `engine/anim_mesh.cpp` | T012 | 0x40b630–0x40bfe2 | 17 |
| `engine/mesh.cpp` | T013 | 0x40c1b0–0x4150e6 | 35 |
| `engine/polybatcher.cpp` | T014 | 0x416640–0x418a9f | 23 |
| `engine/frustrum.cpp` | T015 | 0x418ad0–0x41a034 | 20 |
| `engine/mesh_part.cpp` | T016 | 0x41a070–0x41a822 | 8 |
| `engine/mesh_anim_frame.cpp` | T017 | 0x41a830–0x41a85b | 2 |
| `engine/mesh_part_pose.cpp` | T018 | 0x41a860–0x41a8e5 | 2 |
| `engine/poly_bf.cpp` | T019 | 0x41a8f0–0x41a92b | 2 |
| `engine/poly_bg.cpp` | T020 | 0x41a930–0x41a96b | 2 |
| `engine/poly_f.cpp` | T021 | 0x41a970–0x41a9ab | 2 |
| `engine/poly_ft.cpp` | T022 | 0x41a9b0–0x41a9eb | 2 |
| `engine/poly_g.cpp` | T023 | 0x41a9f0–0x41aa2b | 2 |
| `engine/poly_gt.cpp` | T024 | 0x41aa30–0x41aa6b | 2 |
| `engine/poly_tri.cpp` | T025 | 0x41aa70–0x41aa9b | 3 |
| `engine/render_poly.cpp` | T026 | 0x41aad0–0x41b197 | 10 |
| `engine/bs_stream.cpp` | T027 | 0x41b1a0–0x41b3dd | 8 |
| `engine/vdx7.cpp` | T028 | 0x41b410–0x41b6a2 | 3 |
| `engine/bs_io.cpp` | T029 | 0x41b6e0–0x41b7eb | 3 |
| `engine/bs_file.cpp` | T030 | 0x41b7f0–0x42041c | 46 |
| `engine/sound.cpp` | T031 | 0x4205b0-0x4205e0 | 3 |
| `engine/static_sound.cpp` | T032 | 0x420680-0x420bd0 | 17 |
| `engine/stream_sound.cpp` | T033 | 0x420bd0-0x421740 | 21 |
| `engine/wave_file.cpp` | T034 | 0x421740-0x421ba0 | 9 |
| `objects/video_player.cpp` | T035 | 0x421ba0-0x421db0 | 6 |
| `objects/video.cpp` | T036 | 0x421db0-0x4222c0 | 10 |
| `jpeg/jcomapi.c` | T037 | 0x4222c0-0x422380 | 4 |
| `jpeg/jdapimin.c` | T038 | 0x422380-0x422810 | 5 |
| `jpeg/jdapistd.c` | T039 | 0x422810-0x422a70 | 3 |
| `jpeg/jdatasrc.c` | T040 | 0x422a70-0x422c40 | 5 |
| `jpeg/jdcoefct.c` | T041 | 0x422c40-0x423b30 | 8 |
| `jpeg/jdcolor.c` | T042 | 0x423b30-0x4240d0 | 7 |
| `jpeg/jddctmgr.c` | T043 | 0x4240d0-0x424390 | 2 |
| `jpeg/jdhuff.c` | T044 | 0x424390-0x424d50 | 6 |
| `jpeg/jdinput.c` | T045 | 0x424d50-0x4255d0 | 6 |
| `jpeg/jdmainct.c` | T046 | 0x4255d0-0x425c70 | 5 |
| `jpeg/jdmarker.c` | T047 | 0x425c70-0x427760 | 14 |
| `jpeg/jdmaster.c` | T048 | 0x427760-0x427e30 | 5 |
| `jpeg/jdmerge.c` | T049 | 0x427e30-0x4283b0 | 6 |
| `jpeg/jdphuff.c` | T050 | 0x4283b0-0x4291b0 | 6 |
| `jpeg/jdpostct.c` | T051 | 0x4291b0-0x429510 | 5 |
| `jpeg/jdsample.c` | T052 | 0x429510-0x429bf0 | 10 |
| `jpeg/jerror.c` | T053 | 0x429bf0-0x429db0 | 6 |
| `jpeg/jidctflt.c` | T054 | 0x429db0-0x42a1a0 | 1 |
| `jpeg/jidctfst.c` | T055 | 0x42a1a0-0x42a590 | 1 |
| `jpeg/jidctint.c` | T056 | 0x42a590-0x42aa80 | 1 |
| `jpeg/jidctred.c` | T057 | 0x42aa80-0x42af90 | 3 |
| `jpeg/jmemmgr.c` | T058 | 0x42af90-0x42bc10 | 14 |
| `jpeg/jmemnobs.c` | T059 | 0x42bc10-0x42bc90 | 8 |
| `jpeg/jquant1.c` | T060 | 0x42bc90-0x42c800 | 10 |
| `jpeg/jquant2.c` | T061 | 0x42c800-0x42da00 | 13 |
| `jpeg/jutils.c` | T062 | 0x42da00-0x42dae0 | 5 |
| `objects/bipbiplevel14.cpp` | T063 | 0x42dae0–0x42ec11 | 12 |
| `objects/bull.cpp` | T064 | 0x42ec60–0x431d1c | 9 |
| `objects/crocodilelevel09.cpp` | T065 | 0x431d20–0x4342a6 | 8 |
| `objects/crocodilelevel11.cpp` | T066 | 0x4342b0–0x435f24 | 13 |
| `objects/crowd.cpp` | T067 | 0x435f30–0x4360d5 | 5 |
| `objects/daffyelf.cpp` | T068 | 0x4360e0–0x436902 | 10 |
| `objects/daffylevel01.cpp` | T069 | 0x436910–0x437b0b | 13 |
| `objects/daffylevel02.cpp` | T070 | 0x437b10–0x43a09a | 17 |
| `objects/daffylevel09.cpp` | T071 | 0x43a0a0–0x43b2d3 | 8 |
| `objects/daffymilitary.cpp` | T072 | 0x43b2e0–0x43bf14 | 12 |
| `objects/daffyscene.cpp` | T073 | 0x43bf20–0x43d5b2 | 10 |
| `objects/daffytraininglevel.cpp` | T074 | 0x43d5c0–0x440f30 | 13 |
| `objects/daffywheel.cpp` | T075 | 0x440f30–0x441303 | 7 |
| `objects/dancingghost.cpp` | T076 | 0x441310–0x44299b | 15 |
| `objects/dragon.cpp` | T077 | 0x444310–0x4486bd | 34 |
| `objects/elmer.cpp` | T078 | 0x448aa0–0x44b12d | 8 |
| `objects/ghost.cpp` | T079 | 0x44b130–0x44ebdf | 18 |
| `objects/gossamer_boss.cpp` | T080 | 0x44ebe0–0x45220a | 17 |
| `objects/gossamer_lev08.cpp` | T081 | 0x452210–0x455c24 | 13 |
| `objects/instantmartian.cpp` | T082 | 0x455c30–0x458bb5 | 14 |
| `objects/marvin.cpp` | T083 | 0x458bc0–0x459394 | 7 |
| `objects/patrol.cpp` | T084 | 0x4593a0–0x459a8c | 4 |
| `objects/porkylevel01.cpp` | T085 | 0x459a90–0x45ac1d | 13 |
| `objects/prayingghost.cpp` | T086 | 0x45ac20–0x45e56d | 12 |
| `objects/robot.cpp` | T087 | 0x45e570–0x461f19 | 36 |
| `objects/sam.cpp` | T088 | 0x461f20–0x46ed5c | 69 |
| `objects/sam_pirate.cpp` | T089 | 0x46ed60–0x47237a | 13 |
| `engine/shadow.cpp` | T090 | 0x472380–0x474d8e | 18 |
| `objects/shark.cpp` | T091 | 0x474d90–0x476e4a | 22 |
| `objects/sheep.cpp` | T092 | 0x476e50–0x47d30e | 57 |
| `game/wolf.cpp` | T093 | 0x47d310–0x488bda | 20 |
| `game/wolf_move.cpp` | T094 | 0x488be0–0x48d3b1 | 52 |
| `game/wolf_misc.cpp` | T095 | 0x48d3c0–0x491e03 | 63 |
| `game/scn_controllable.cpp` | T096 | 0x491e10–0x493e75 | 18 |
| `objects/wallavoid.cpp` | T097 | 0x493e80–0x494169 | 3 |
| `objects/ambient_sound.cpp` | T098 | 0x494170–0x494de5 | 8 |
| `objects/anvil.cpp` | T099 | 0x494df0–0x495152 | 5 |
| `objects/automaticdoor.cpp` | T100 | 0x495160–0x495541 | 8 |
| `objects/balance.cpp` | T101 | 0x495550–0x4958f2 | 5 |
| `objects/bat.cpp` | T102 | 0x495900–0x496188 | 7 |
| `objects/battery.cpp` | T103 | 0x496190–0x4971ed | 9 |
| `objects/bees.cpp` | T104 | 0x4971f0–0x499022 | 11 |
| `objects/bell.cpp` | T105 | 0x499030–0x4992f9 | 7 |
| `objects/bipbip.cpp` | T106 | 0x499300–0x499e23 | 7 |
| `objects/bird.cpp` | T107 | 0x499e30–0x49ad4d | 8 |
| `objects/blackhole.cpp` | T108 | 0x49ad50–0x49b61d | 8 |
| `objects/bonusmanager.cpp` | T109 | 0x49b620–0x49ce2d | 10 |
| `objects/box.cpp` | T110 | 0x49ce30–0x49d301 | 7 |
| `objects/bridge.cpp` | T111 | 0x49d310–0x49e020 | 10 |
| `objects/bullet.cpp` | T112 | 0x49e020–0x49e9c8 | 8 |
| `objects/bush.cpp` | T113 | 0x49e9d0–0x49f25b | 8 |
| `objects/butterfly.cpp` | T114 | 0x49f260–0x49ff30 | 7 |
| `objects/cactus.cpp` | T115 | 0x49ff30–0x4a00b1 | 5 |
| `objects/cameramanager.cpp` | T116 | 0x4a00c0–0x4a07a5 | 8 |
| `objects/cameramanager2.cpp` | T117 | 0x4a07b0–0x4a0db5 | 7 |
| `objects/camerarestriction.cpp` | T118 | 0x4a0dc0–0x4a1782 | 8 |
| `objects/cannonball.cpp` | T119 | 0x4a1790–0x4a4bb7 | 12 |
| `objects/cannonball2.cpp` | T120 | 0x4a4bc0–0x4a5710 | 9 |
| `objects/canondummy.cpp` | T121 | 0x4a5710–0x4a5b19 | 8 |
| `objects/canonsheep.cpp` | T122 | 0x4a5b20–0x4a713f | 10 |
| `objects/canonsimple.cpp` | T123 | 0x4a7140–0x4a88c0 | 12 |
| `objects/case.cpp` | T124 | 0x4a88c0–0x4a8a34 | 4 |
| `objects/catapult.cpp` | T125 | 0x4a8a40–0x4ab38a | 20 |
| `objects/checkpointmanager.cpp` | T126 | 0x4ab390–0x4abcbc | 9 |
| `objects/chronometer.cpp` | T127 | 0x4abcc0–0x4ac5ab | 6 |
| `objects/cinematicsmanager.cpp` | T128 | 0x4ac5b0–0x4acd80 | 8 |
| `objects/crane.cpp` | T129 | 0x4acd80–0x4ad97e | 9 |
| `objects/creditsmanager.cpp` | T130 | 0x4ad980–0x4ae754 | 9 |
| `objects/crumblyground.cpp` | T131 | 0x4ae760–0x4aef2c | 10 |
| `objects/crumblyplat.cpp` | T132 | 0x4aef30–0x4af49b | 10 |
| `objects/dancingghost_manager.cpp` | T133 | 0x4af4a0–0x4b275f | 19 |
| `objects/dancingghost_track.cpp` | T133b | 0x4b2840–0x4b3265 | 1 |
| `objects/diamond.cpp` | T134 | 0x4b3270–0x4b3ad3 | 8 |
| `objects/doorlevel.cpp` | T135 | 0x4b3ae0–0x4b4270 | 8 |
| `objects/doormechanism.cpp` | T136 | 0x4b4270–0x4b4392 | 5 |
| `objects/doorworld.cpp` | T137 | 0x4b43a0–0x4b4e65 | 7 |
| `objects/dynamite.cpp` | T138 | 0x4b4e70–0x4b594f | 8 |
| `objects/elastic.cpp` | T139 | 0x4b5950–0x4b8f12 | 29 |
| `objects/elastictree.cpp` | T140 | 0x4b8f20–0x4b910d | 8 |
| `objects/facingcamera.cpp` | T141 | 0x4b9110–0x4b9259 | 6 |
| `objects/fallinggate.cpp` | T142 | 0x4b9260–0x4ba2ab | 6 |
| `objects/fallinggate2.cpp` | T143 | 0x4ba2b0–0x4bb06b | 7 |
| `objects/fallingrock.cpp` | T144 | 0x4bb070–0x4bb545 | 7 |
| `objects/fan.cpp` | T145 | 0x4bb550–0x4bc142 | 10 |
| `objects/fireball.cpp` | T146 | 0x4bc150–0x4bd400 | 13 |
| `objects/firefly.cpp` | T147 | 0x4bd400–0x4bd757 | 5 |
| `objects/fish.cpp` | T148 | 0x4bd760–0x4bdc2e | 7 |
| `objects/fishingrod.cpp` | T149 | 0x4bdc30–0x4bf6a2 | 26 |
| `objects/floatingbox.cpp` | T150 | 0x4bf6b0–0x4c1eb1 | 11 |
| `objects/flute.cpp` | T151 | 0x4c1ef0–0x4c2752 | 8 |
| `objects/fogmanager.cpp` | T152 | 0x4c2760–0x4c2978 | 6 |
| `objects/frozenriver.cpp` | T153 | 0x4c2980–0x4c3ea4 | 15 |
| `objects/geyserin.cpp` | T154 | 0x4c3eb0–0x4c5913 | 8 |
| `objects/geysermanger.cpp` | T155 | 0x4c5920–0x4c5c8d | 6 |
| `objects/geyserout.cpp` | T156 | 0x4c5c90–0x4c7069 | 8 |
| `objects/ghostcostume.cpp` | T157 | 0x4c7070–0x4c7261 | 4 |
| `objects/ghosthalo.cpp` | T158 | 0x4c7270–0x4c7498 | 4 |
| `objects/goal.cpp` | T159 | 0x4c74a0–0x4c7922 | 7 |
| `objects/goldencoins.cpp` | T160 | 0x4c7930–0x4c8102 | 9 |
| `objects/gossameronde.cpp` | T161 | 0x4c8110–0x4c8871 | 7 |
| `objects/hairdryer.cpp` | T162 | 0x4c8880–0x4c913e | 8 |
| `objects/heapofleaf.cpp` | T163 | 0x4c9140–0x4c992c | 10 |
| `objects/hiddenrocks.cpp` | T164 | 0x4c9930–0x4c9aef | 7 |
| `objects/hitswitch.cpp` | T165 | 0x4c9af0–0x4c9d3e | 7 |
| `objects/hive.cpp` | T166 | 0x4c9d40–0x4ca657 | 8 |
| `objects/honeypot.cpp` | T167 | 0x4ca660–0x4cb126 | 7 |
| `objects/hoover.cpp` | T168 | 0x4cb130–0x4cbb11 | 7 |
| `objects/icecube.cpp` | T169 | 0x4cbb20–0x4cc143 | 8 |
| `objects/iceground.cpp` | T170 | 0x4cc150–0x4cc5a7 | 6 |
| `objects/inflatablesheep.cpp` | T171 | 0x4cc5b0–0x4cdcf3 | 16 |
| `objects/instanthoover.cpp` | T172 | 0x4cdd00–0x4cf0cc | 11 |
| `objects/instantsocket.cpp` | T173 | 0x4cf0d0–0x4cf2ad | 6 |
| `objects/jail.cpp` | T174 | 0x4cf2b0–0x4cff6f | 8 |
| `objects/key.cpp` | T175 | 0x4cff70–0x4d04e3 | 7 |
| `objects/laser.cpp` | T176 | 0x4d04f0–0x4d100e | 8 |
| `objects/lava.cpp` | T177 | 0x4d1010–0x4d11db | 4 |
| `objects/lazerrobot.cpp` | T178 | 0x4d11e0–0x4d1af6 | 9 |
| `objects/leaf.cpp` | T179 | 0x4d1b00–0x4d1d55 | 4 |
| `objects/magnet.cpp` | T180 | 0x4d1d60–0x4d2cdf | 11 |
| `objects/mailbox.cpp` | T181 | 0x4d2ce0–0x4d3e37 | 8 |
| `objects/maplocation.cpp` | T182 | 0x4d3e40–0x4d42bb | 8 |
| `objects/mcardmanager.cpp` | T183 | 0x4d42d0–0x4d4cd1 | 6 |
| `objects/mine.cpp` | T184 | 0x4d4ce0–0x4d7cf6 | 29 |
| `objects/minedetector.cpp` | T185 | 0x4d7d00–0x4d8a9d | 11 |
| `objects/mirrormanager.cpp` | T186 | 0x4d8aa0–0x4d9005 | 8 |
| `objects/monolithe.cpp` | T187 | 0x4d9010–0x4d9e27 | 7 |
| `objects/objectmanager.cpp` | T188 | 0x4d9e30–0x4da35f | 9 |
| `objects/perfume.cpp` | T189 | 0x4da360–0x4dac7a | 7 |
| `objects/pipe.cpp` | T190 | 0x4dac80–0x4dc4d5 | 7 |
| `objects/pipe2.cpp` | T191 | 0x4dc4e0–0x4de03a | 8 |
| `objects/piranhas.cpp` | T192 | 0x4de040–0x4def82 | 14 |
| `objects/rabbitcostume.cpp` | T193 | 0x4def90–0x4df181 | 4 |
| `objects/raft.cpp` | T194 | 0x4df190–0x4dfe7c | 7 |
| `objects/rcarpetmobile.cpp` | T195 | 0x4dfe80–0x4e1234 | 12 |
| `objects/remotecontrol.cpp` | T196 | 0x4e1240–0x4e1811 | 6 |
| `objects/resizer.cpp` | T197 | 0x4e1820–0x4e25a4 | 9 |
| `objects/rock.cpp` | T198 | 0x4e25b0–0x4e4803 | 20 |
| `objects/rocket.cpp` | T199 | 0x4e4810–0x4e554a | 9 |
| `objects/rocks.cpp` | T200 | 0x4e5550–0x4e58dc | 5 |
| `objects/rollingcarpet.cpp` | T201 | 0x4e58e0–0x4e7317 | 12 |
| `objects/rook.cpp` | T202 | 0x4e7320–0x4e7cb7 | 8 |
| `objects/sail.cpp` | T203 | 0x4e7cc0–0x4e7daa | 5 |
| `objects/salad.cpp` | T204 | 0x4e7db0–0x4e8ac9 | 10 |
| `objects/scenesheeppanel.cpp` | T205 | 0x4e8ad0–0x4e947e | 6 |
| `objects/scene_wheel.cpp` | T206 | 0x4e9480–0x4e9e6d | 7 |
| `objects/seaweed.cpp` | T207 | 0x4e9e70–0x4e9fd2 | 6 |
| `objects/secretdoor.cpp` | T208 | 0x4e9fe0–0x4ea072 | 7 |
| `objects/seed.cpp` | T209 | 0x4ea080–0x4eb0aa | 16 |
| `objects/seesaw.cpp` | T210 | 0x4eb0b0–0x4efaed | 20 |
| `objects/sensiblebutton.cpp` | T211 | 0x4efaf0–0x4f06b0 | 6 |
| `objects/sfxcinemanager.cpp` | T212 | 0x4f06b0–0x4f0d97 | 7 |
| `objects/sheepcostume.cpp` | T213 | 0x4f0da0–0x4f0f91 | 4 |
| `objects/signpost.cpp` | T214 | 0x4f0fa0–0x4f1eae | 18 |
| `objects/signtips.cpp` | T215 | 0x4f1eb0–0x4f2912 | 7 |
| `objects/slidingicecube.cpp` | T216 | 0x4f2920–0x4f3bdf | 17 |
| `objects/smallrock.cpp` | T217 | 0x4f3be0–0x4f5767 | 13 |
| `objects/snowball.cpp` | T218 | 0x4f5770–0x4f615f | 12 |
| `objects/snowyground.cpp` | T219 | 0x4f6160–0x4f64b3 | 5 |
| `objects/lightspot.cpp` | T220 | 0x4f64c0–0x4f8891 | 16 |
| `objects/superbutton.cpp` | T221 | 0x4f88a0–0x4f8cc3 | 13 |
| `objects/swirlsign.cpp` | T222 | 0x4f8cd0–0x4f9023 | 7 |
| `objects/telescope.cpp` | T223 | 0x4f9030–0x4f9b8a | 9 |
| `objects/timekeeper.cpp` | T224 | 0x4f9b90–0x4f9fd6 | 7 |
| `objects/timemachine.cpp` | T225 | 0x4f9fe0–0x4fc075 | 11 |
| `objects/timemachinechrono.cpp` | T226 | 0x4fc080–0x4fc5d7 | 7 |
| `objects/torch.cpp` | T227 | 0x4fc5e0–0x4fc836 | 5 |
| `objects/trafficjams.cpp` | T228 | 0x4fc840–0x4fd5ac | 8 |
| `objects/train.cpp` | T229 | 0x4fd5b0–0x50110e | 10 |
| `objects/trainstation.cpp` | T230 | 0x501150–0x501faf | 9 |
| `objects/treesection.cpp` | T231 | 0x501fb0–0x5022c1 | 6 |
| `objects/triggedstone.cpp` | T232 | 0x5022d0–0x5028ed | 7 |
| `objects/twig.cpp` | T233 | 0x5028f0–0x502c97 | 7 |
| `objects/umbrella.cpp` | T234 | 0x502ca0–0x503523 | 6 |
| `objects/visibilitymanager.cpp` | T235 | 0x503530–0x503ab1 | 9 |
| `objects/volcano.cpp` | T236 | 0x503ac0–0x5061a4 | 7 |
| `objects/watch.cpp` | T237 | 0x5061b0–0x506833 | 7 |
| `objects/watergeyser.cpp` | T238 | 0x506840–0x50713b | 7 |
| `objects/watermine.cpp` | T239 | 0x507140–0x5078e3 | 7 |
| `objects/wheel.cpp` | T240 | 0x5078f0–0x509679 | 14 |
| `objects/wheeldummy.cpp` | T241 | 0x509680–0x5098b4 | 8 |
| `objects/wolftrap.cpp` | T242 | 0x5098c0–0x509fbc | 8 |
| `objects/woodenlift.cpp` | T243 | 0x509fc0–0x50b216 | 12 |
| `objects/woodenplatform.cpp` | T244 | 0x50b220–0x50b91f | 10 |
| `engine/progress.cpp` | T245 | 0x50b920–0x50c372 | 12 |
| `engine/progress_inventory.cpp` | T246 | 0x50c380–0x50d533 | 24 |
| `engine/scenaric.cpp` | T247 | 0x50d540–0x511f4e | 85 |
| `engine/navigation.cpp` | T248 | 0x511f50–0x51355a | 17 |
| `engine/scn_register.cpp` | T249 | 0x513560–0x5145bc | 1 |
| `engine/scn_tools.cpp` | T250 | 0x5145c0–0x515ef4 | 37 |
| `engine/transition.cpp` | T251 | 0x515f00–0x516736 | 17 |
| `engine/obj_grid.cpp` | T252 | 0x516740–0x5169be | 5 |
| `engine/coll_clip.cpp` | T253 | 0x5169c0–0x5196b9 | 14 |
| `engine/collide.cpp` | T254 | 0x5196c0–0x51e1eb | 19 |
| `engine/debug_draw.cpp` | T255 | 0x51e1f0–0x5228e3 | 8 |
| `engine/draw2d.cpp` | T256 | 0x5228f0–0x5265f7 | 32 |
| `engine/fixed_math.cpp` | T257 | 0x526600–0x5276a4 | 28 |
| `engine/lerp.cpp` | T258 | 0x5276b0–0x5279fe | 9 |
| `engine/input_mgr.cpp` | T259 | 0x527a00–0x529f07 | 33 |
| `engine/game_level.cpp` | T260 | 0x529f40–0x52a295 | 2 |
| `engine/card.cpp` | T261 | 0x52a2a0–0x52a4b4 | 11 |
| `engine/screen.cpp` | T262 | 0x52a4c0–0x52a7da | 15 |
| `engine/tex_table.cpp` | T263 | 0x52a810–0x52a851 | 1 |
| `engine/emitter.cpp` | T264 | 0x52a860–0x52dd3d | 23 |
| `engine/emitter_stubs.cpp` | T265 | 0x52dd40–0x52dd65 | 7 |
| `engine/cheat.cpp` | T266 | 0x52dd70–0x52df77 | 5 |
| `engine/weather.cpp` | T267 | 0x52df80–0x52e1d3 | 11 |
| `engine/weather_fx.cpp` | T268 | 0x52e1e0–0x531eca | 12 |
| `engine/file.cpp` | T269 | 0x531f00–0x5321d0 | 10 |
| `engine/text.cpp` | T270 | 0x5321d0–0x5362d1 | 66 |
| `engine/game_state.cpp` | T271 | 0x5362e0–0x536395 | 3 |
| `engine/debug.cpp` | T272 | 0x5363a0–0x5363be | 6 |
| `fx/holefx.cpp` | T273 | 0x5363c0–0x538db4 | 14 |
| `fx/hole_fx_stub.cpp` | T274 | 0x538df0–0x538dfa | 2 |
| `engine/interface.cpp` | T275 | 0x538e00–0x53e6fe | 53 |
| `engine/fade.cpp` | T276 | 0x53e700–0x53ea45 | 4 |
| `engine/prompt.cpp` | T277 | 0x53ea50–0x53f423 | 25 |
| `engine/map.cpp` | T278 | 0x53f430–0x5428bf | 37 |
| `engine/pause_menu.cpp` | T279 | 0x5428c0–0x546679 | 69 |
| `engine/approach.cpp` | T280 | 0x546680–0x547237 | 11 |
| `engine/list.cpp` | T281 | 0x547240–0x548423 | 19 |
| `engine/load_dav.cpp` | T282 | 0x548430–0x5486a7 | 3 |
| `engine/jpeg_mlt.cpp` | T283 | 0x5486b0–0x548dcd | 8 |
| `engine/sound_mgr.cpp` | T284 | 0x548dd0–0x549d1d | 28 |
| `engine/load_war.cpp` | T285 | 0x549d20–0x54a222 | 3 |
| `engine/load_warmeshes.cpp` | T286 | 0x54a230–0x54a5ea | 7 |
| `objects/mcard.cpp` | T287 | 0x54a5f0–0x54d4fa | 37 |
| `engine/heap_stub.cpp` | T288 | 0x54d500–0x54d507 | 1 |
| `engine/heap.cpp` | T289 | 0x54d510–0x54e7f4 | 23 |
| `objects/menu.cpp` | T290 | 0x54e800–0x54f8e5 | 19 |
| `objects/textresbank.cpp` | T291 | 0x54f8f0–0x54fe13 | 5 |
| `objects/animation.cpp` | T292 | 0x54fe50–0x55244c | 18 |
| `objects/camera.cpp` | T293 | 0x552450–0x55b19e | 52 |
| `objects/instance.cpp` | T294 | 0x55b1a0–0x55c0d8 | 16 |
| `objects/bounds.cpp` | T295 | 0x55c0e0–0x55ca2f | 3 |
| `objects/world_draw.cpp` | T296 | 0x55ca30–0x55daaf | 17 |
| `objects/pack_jpeg.cpp` | T297 | 0x55dab0–0x55e480 | 11 |
| `engine/input.cpp` | T298 | 0x55e480–0x55f6a1 | 38 |
| `engine/crc32.cpp` | T299 | 0x55f6b0–0x55f6fe | 1 |
| `engine/registry.cpp` | T300 | 0x55f700–0x55fbf6 | 10 |
| `engine/scenaric_loop.cpp` | T301 | 0x55fc00–0x5606d2 | 7 |
| `engine/id_list.cpp` | T302 | 0x5606e0–0x5609b2 | 8 |
| `engine/tex_scroll.cpp` | T303 | 0x5609c0–0x560f7b | 6 |
| `engine/time.cpp` | T304 | 0x560f80–0x561200 | 5 |
| `engine/maths.cpp` | T305 | 0x561200–0x5615cd | 12 |
| `objects/video_sequence.cpp` | T306 | 0x5615d0–0x5617bf | 5 |
| `engine/cine1.cpp` | T307 | 0x5617c0-0x561800 | 14 |
| `engine/cine2.cpp` | T308 | 0x561be0-0x561c50 | 21 |
| `engine/cine3.cpp` | T309 | (data | 0 |
| `engine/cine4.cpp` | T310 | (data | 0 |
| `engine/cine5.cpp` | T311 | (data | 0 |
| `engine/cine_update.cpp` | T312 | 0x5626d0-0x562e00 | 12 |
| `engine/cine.cpp` | T313 | 0x5631d0-0x563350 | 5 |
| `engine/sfx_volume.cpp` | T314 | 0x5634c0-0x5634d0 | 3 |
| `engine/stream_player.cpp` | T315 | 0x5634d0-0x563500 | 18 |

Rules for files in this directory
- Every function carries its address. Only write a function as source once it has been read end to end and the things it calls are named.
- Say at the top of a file if any of it is only *reconstructed by reading* rather than match-compiled.
- Use the names in the symbol tables: the matcher looks functions up by name, and flags a relocation whose target the tables call something else.
- **A table row may not contradict matched source; check it with `python3 tools/check_symbol_prototypes.py`.** It compares every declaration in `src/` against the prototype written in that function's symbol row and exits 1 on a disagreement. A matched declaration outranks a row: the function it describes compiles to the original's exact bytes, while the row is prose. Examples of what it catches: a row giving `Box_GroundQueryFlatTop` 0x515934 three arguments where the matched `train.cpp` declares four, and a row giving NavNode's stride as 0x70 where the struct CSV generates 0x68. Rows with no parseable prototype are listed as UNCHECKED under `--all` rather than guessed at.
- **Declaration drift** (`python3 tools/check_decl_drift.py`: one function declared two ways in two files) has two known cases. `Stub_Ret` 0x42bc80 is a bare `ret` that the original calls both with and without an argument, so no single prototype fits both callers. `Box_ContainsPointXZ` 0x4486c0 is an inline helper that `rook.cpp` defines on a `CollBox` and `engine/coll_box_inlines.h` on a `Box`; each file's functions match with its own version. Neither affects a byte match.
- C++ methods drop the class prefix of the table name when it has one (`ScnControllable::SteerRun` = ScnControllable_SteerRun; a virtual override must share the base's name) and keep the full table name otherwise (`Mobile_Steer`, `Frame_LimitFps`); the matcher tries `Class_Method`, then `Method`, then the class's vtable slot of that name.
- **Where /Od puts locals is decided by their names**: the compiler hashes each name into one of 16 buckets and hands out stack slots bucket by bucket from EBP-4 downward; inside a bucket the last-declared local comes first. So to move a local, rename it. `python3 tools/vc6_locals.py frame TYPE:NAME ...` predicts the offsets and `pick` finds names that give a wanted order. Grouping locals in a struct (one local) also works; say so in a comment where it is done.
- **Byte-matching tags.** Code that is shaped only to reproduce the original bytes carries a tag, so it is not mistaken for a mistake or "cleaned up": `/* BYTES(kind): reason */`, or `/* BYTES(kind, inferred): reason */` when the reason is read from the code but not yet proven by a test build. A tag sits above the function it concerns, or in the tag block after the file header (which opens with `/* BYTES: <kinds used in this file>. */`) when it concerns the file, a macro or an inline helper. `grep -rn "BYTES(" src/` lists them all. The kinds: `slot-name` (a local's name picks its stack slot), `slot-group` (locals grouped in one struct or widened for the frame), `slot-scope` (a nested block that ends a local's lifetime), `dead-code` (a statement with no effect that the original has), `temp` (an extra local or copy that forces a store and reload), `cast` (a type or constant spelling chosen for the instruction it selects), `flow` (control-flow or expression shape chosen for block order, including statements written twice), `inline` (a helper or device that exists for its /Ob1 expansion), `layout` (placement of data and code: `= 0` initialisers, placeholders, definition order), `bss-name` (a global's name chosen for its .bss hash), `view` (a deliberate reinterpretation) and `switches` (per-file or per-function compiler switches).
- Bitfields: a struct CSV row with ctype `u8:1` / `u16:3` is a bitfield; consecutive rows at the same offset share one storage unit, first row = lowest bits (e.g. data/structs/CamFlagBits.csv). No local bitfield types in source.
- Globals with constructors: define them naturally (`WaveFile g_sndBankWaves[256];`) and list the file's static-initialiser roots, in definition order, with `match-init: NAME ...` in the first 40 lines; the matcher places the ctor / atexit / dtor thunks by following the roots' calls (src/engine/sound.cpp).
- A shared member header (src/game/wolf.h) defines SDW_MEMBERS_<Class>; a new file adds members on top with SDW_EXTRA_<Class>.
- **When every instruction is already the original's and only the ARRANGEMENT differs** (block order, which register holds what, which stack slot a value gets), the lever is the shape of the source, not a compiler switch: /O1, /O2 /Os, /Oa, /Ow and /GB, measured against /O2 on three such functions, did not help any of them. Three shapes did, each proven by a byte match (now in src/engine/stream_sound.cpp and src/engine/wave_file.cpp):
    - **A wrong stack slot is a LIFETIME question.** The original reuses a dead variable's slot for a later value. Declare that variable in a nested block that CLOSES at its last use, and the compiler reuses the slot by itself (StreamSound_GetVoiceAmplitude 0x421570: the play cursor's slot becomes the window count's).
    - **A shared address computation that should be two** comes from writing one body where the original had two: split the function into `__forceinline` halves, and each half computes its own (WaveFile_Open 0x4217d0, whose two halves each keep their own `lea &ckRiff`, leaving EBX free for the result code).
    - **A block in the wrong order** can be moved by DEFERRING it behind a flag that the optimiser then removes: set `bool f = 1` in the branch that needs the work, and do the work afterwards under `if (f)`. The flag costs nothing at runtime and the blocks come out in the original's order (StreamSound_ReadOrPadSilence 0x421490). A shape that reproduces the bytes is a representation, not proof that the original source read that way; say so in the file.
- A name the tables cannot place (two constructor overloads share one undecorated name) is pinned in the file with `match-addr: <decorated COFF name>=0xVA` in the first 40 lines, so `--all` places it the same way (src/app/d3dapp.cpp).
- A vtable slot that is `_purecall` in the class that introduces it is marked by ending its `data/vtable_slots.csv` signature with ` = 0` (the Sound base of StaticSound / StreamSound); the overrides inherit the signature, not the marker.
- Not every file was built /Od: `engine/cine.cpp` needs /O2 /Oy- and part of `engine/scenaric_loop.cpp` `#pragma optimize("g")` with /Ob2. A file sets its extra switches with a `match-flags:` line in its first 40 lines.
- Windows, DirectX and CRT declarations come from the SDK stand-ins in `src/sdk/` (only what `src/` uses, in the SDK's spelling): add to them, not to a file. `src/include/*.h` are GENERATED (`python3 tools/structs_to_c.py`) — edit the CSVs, not the headers. **Use the generated types, never a local copy of a class**: C++ files include `sdw_classes.h` (every recovered class with its base, vtable slots and fields; `tools/cpp_classes.py`), C files `sdw_structs.h`. Both compile under VC6 with a check per field offset, and `vc6.py --all` recompiles them first.
- Methods: a class's non-virtual methods are declared in its generated body, from `data/class_methods.csv` (class, declaration, address comment). Before including `sdw_classes.h`, a file defines `SDW_MEMBERS_<Class>` only for what stays per file: the constructors (a constructor made visible everywhere would be called where the original constructs nothing) and members spelled differently in different files (see `src/game/scn_controllable.cpp`). Inline helpers that several files use have one body, in the owner's `<file>_inlines.h`: a file defines the `SDW_INLINE_<HELPER>` selectors of the helpers it uses, includes that header and undefines them again, so each file keeps its own selection and definition order (VC6 expands an inline where it was defined). The few helpers with more than one byte-proven spelling take the variant's number as the selector's value. A generated type with member functions (in the table or a file's member hook), a vtable or a base is a `class`; plain data is a `struct`. Spell it the same way everywhere, forward declarations included: VC6 names a type's symbols by the keyword it sees first. Virtual methods are already declared, from the vtables in the exe; slot names and signatures that the implementations' names cannot give are in `data/vtable_slots.csv`. A shared member list lives in one header that only defines the macro (`src/engine/timer.h`), included before `sdw_classes.h`.
- Include the generated headers by name (`#include "sdw_classes.h"`), not by relative path, so another header directory (`vc6.py --include`) can take their place.
- Check: `python3 tools/vc6.py <file>` (one file) or `--all`; `--diff NAME` shows the instruction diff for a function that does not match yet. `--all` rewrites `data/match_results.json` (one row per function: file, address, verdict) — commit it with the source change.

Building the exe: `python3 tools/build_exe.py` compiles every file (vc6.py --all -j 6, about a minute), links the 316 objects in the original order with LINK 6.00.8447 on Microsoft's MSVCRT 6.0.8168 (its qsort decides /OPT:ICF survivors), the DirectX 8.0 SDK and VC6 SP5 libraries and the resources taken from your own SheepD3D.exe (tools/exe_res.py), stamps the original timestamp and compares the result with the original byte for byte (work/link/compare.json). Microsoft's files are not in the repository.

Compiler rules that decide an object's layout (each measured by compiling test files)
- **Uninitialised globals are ordered by their NAMES as well** (.bss, like the /Od locals above). Key = `(h ^ (h >> 16)) & 1023` with `h = (h << 2) + (h >> 4) + c` over the plain undecorated name; keys ascend with the address; inside one key the later definition gets the lower address. Constructed objects count as uninitialised; `static` changes nothing; globals written `= 0` come after all of them, in definition order (confirmed on a test file with 70 names). So an object's .bss order is reproduced by choosing the names: the most-used names are kept, and each other global gets a descriptive name in the right key window (engine/draw2d.cpp).
- **/Ob1 expands `inline` functions within a per-caller budget.** Bodies defined AFTER the caller are expanded too, and where they are defined (in the class or outside, before or after) and `const` make no difference. The caller's own call sites are decided first, greedily in source order, skipping a site that does not fit and trying the later ones; sites inside an expanded body are decided afterwards, by a rule that is measured but not fully modelled. The budget grows with the caller's size (a brace pair counts). A function left unexpanded at any site is emitted as a COMDAT after the object's main .text, in definition order. `__forceinline` draws on the same budget; `#pragma inline_depth`, `auto_inline`, `optimize`, an empty `__asm{}` and c2's `/d2inlt` `/d2inls` `/d2isize` change nothing under /Od. What does change a callee's cost without changing its bytes: extra brace nesting in its body, a trailing `return;`, an unused defaulted parameter. `objects/dragon.cpp` reproduces the original's exact expansions that way. It is a representation of the bytes, not the developers' spelling, and the file says so.
- **String literals are emitted in source order per function.** Where the original's .data has them in another order, named static arrays in the exe's order reproduce it (engine/navigation.cpp, whose four literals are reversed).

