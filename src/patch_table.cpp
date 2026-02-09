#include "patch_table.hpp"

const static uint32_t DLC_mission_size = 0x110;
const static uint32_t DLC_mission_patch_limit = 0x1000;
const static uint32_t matrixPool_size = 0x30d400;
const static uint32_t hiRezPatchArea = 0x1000;

enum es_layout : uint32_t {
   ES_DLC_START = 0,
   ES_DLC_END = ES_DLC_START + (DLC_mission_size * DLC_mission_patch_limit),

   //ES_END = ES_DLC_END,

   ES_MATRIX_START = ES_DLC_END,
   ES_MATRIX_END = ES_MATRIX_START + matrixPool_size,

   ES_HIREZ_START = ES_MATRIX_END,
   ES_HIREZ_END = ES_HIREZ_START + hiRezPatchArea,

   ES_END = ES_HIREZ_END,
};

// Function names matched from BF1 Mac executable. Could be wrong in cases.

// clang-format off

const exe_patch_list patch_lists[EXE_COUNT] = {
   exe_patch_list{
      .name = "Battlefront SWBFspy",
      .id_address = 0x29c978,
      .expected_id = 0x746163696c707041,
      .patches =
         {
            patch_set{
               .name = "RedMemory Heap Extensions",
               .patches =
                  {
                     patch{0x1ec651, 0x4000000, 0x8000000}, // Startup_RedInitHeap (main)
                     patch{0x1ec65c, 0x4000000, 0x8000000}, // Startup_RedInitHeap
                     patch{0x1ec66d, 0x200000, 0x400000}, // Startup_RedInitHeap (debug)
                     patch{0x9dace, 0xf40000, 0x1400000}, // Increase App Heap from 15.5 to 31 MB
                  },
            },

            patch_set{
               .name = "SoundParameterized Layer Limit Extension",
               .patches =
                  {
                     //patch{0x3e170c, 0xa0, 0x2000},
                  },
            },

            patch_set{
               .name = "DLC Mission Limit Extension",
               .patches =
                  {
                     //patch{0x9d52f, 0x32, DLC_mission_patch_limit, true},                    // AddDownloadableContent
                     //disable the limit... wait for Dave to crash game.
                     patch{0x9d52d, 0x0f32f883, 0x90909090},// AddDownloadableContent
                     patch{0x9d531, 0x8c8d, 0x90909090},// AddDownloadableContent
                     patch{0x9d535, 0x4c8d5300, 0x4c8d5390},// AddDownloadableContent

                     //move out to new location in memory
                     patch{0x9d550, 0x734328, ES_DLC_START, true},                           // AddDownloadableContent
                     patch{0x9d573, 0x73432c, (0x73432c - 0x734328) + ES_DLC_START, true},   // AddDownloadableContent
                     patch{0x9d579, 0x734330, (0x734330 - 0x734328) + ES_DLC_START, true}, // AddDownloadableContent
                     patch{0x9d57e, 0x737848, (0x737848 - 0x734328) + ES_DLC_START, true}, // AddDownloadableContent
                     patch{0x9d5a2, 0x734433, (0x734433 - 0x734328) + ES_DLC_START, true}, // AddDownloadableContent
                     patch{0x9d5ae, 0x734434, (0x734434 - 0x734328) + ES_DLC_START, true}, // AddDownloadableContent
                     patch{0x9d40c, 0x734328, ES_DLC_START, true},                           // SetCurrentMap
                     patch{0x9d44c, 0x73432c, (0x73432c - 0x734328) + ES_DLC_START, true}, // SetCurrentMission
                     patch{0x9d490, 0x734330, (0x734330 - 0x734328) + ES_DLC_START, true}, // GetContentDirectory
                     patch{0x9d4d2, 0x73432c, (0x73432c - 0x734328) + ES_DLC_START, true}, // IsMissionDownloaded
                     patch{0x9d37a, 0x737848, (0x737848 - 0x734328) + ES_DLC_START, true}, // AddMissionCommon? 
                  },
            },

            patch_set{
               .name = "Spawn Screen Fix",
               .patches =
                  {
                     //Allow 10 units
                     patch{0x1a94b2, 0x0f05ff83, 0x0f0Aff83}, // SlotWindow Loop (SpawnDisplay::Initialize)
                     patch{0x1a9985, 0x7c05ff83, 0x7c0Aff83}, // Unit Selection Loop (SpawnDisplay::UpdateInput)

                     //resize heap
                     patch{0x1a7e47, 0x2340, 0x2740}, //SpawnDisplay::Create

                     //patch Team Switcher
                     patch{0x1a91c1, 0x0590, 0x2350}, //SpawnDisplay::Initialize
                     patch{0x1a97fb, 0x0580, 0x2340}, //SpawnDisplay::UpdateInput
                     patch{0x1a9856, 0x0590, 0x2350}, //SpawnDisplay::UpdateInput
                     patch{0x1a987b, 0x0594, 0x2354}, //SpawnDisplay::UpdateInput

                     //patch hiding the team switcher when not in IA - 1/8/25
                     patch{0x1a8d7c, 0x0588, 0x2348}, //SpawnDisplay::Show
                     patch{0x1a8d85, 0x0580, 0x2340}, //SpawnDisplay::Show
                     patch{0x1a8d8e, 0x0590, 0x2350}, //SpawnDisplay::Show
                     patch{0x1a8d97, 0x058c, 0x234c}, //SpawnDisplay::Show
                     patch{0x1a8da0, 0x0584, 0x2344}, //SpawnDisplay::Show
                     patch{0x1a8da9, 0x0594, 0x2354}, //SpawnDisplay::Show

                     //patch extra slots not deleting
                     patch(0x1a86bb, 0x7d05f983, 0x7d0af983), //SpawnDisplay::SetupSlots
                     patch(0x1a86c1, 0x05, 0x0a), //SpawnDisplay::SetupSlots
                     patch(0x1a86f3, 0x7c050000, 0x7c0a0000), //SpawnDisplay::SetupSlots

                     //patch extra slots not reapplying if different amounts of units were present - 1/12/2025
                     patch(0x1a8690, 0x0914468b, 0x0928468b), //SpawnDisplay::SetupSlots
                     patch(0x1a8696, 0x093c468b, 0x0950468b), //SpawnDisplay::SetupSlots

                     //patch text not all appearing, move to end of heap
                     patch(0x1a8008, 0x0530, 0x2500), //SpawnDisplay::UpdateObjectText
                     patch(0x1a8646, 0x0530, 0x2500), //SpawnDisplay::SetupSlots 
                     patch(0x1a870B, 0x0530, 0x2500), //SpawnDisplay::SetupSlots
                     patch(0x1a863e, 0xfffffad0, 0xffffdb00), //SpawnDisplay::SetupSlots //woah a negative
                     patch(0x1a9333, 0x0530, 0x2500), //SpawnDisplay::Initialize
                     patch(0x1a9569, 0x0530, 0x2500), //SpawnDisplay::Initialize
                     patch(0x1a98c2, 0x0530, 0x2500), //SpawnDisplay::UpdateInput 
                     patch(0x1a9919, 0x0530, 0x2500), //SpawnDisplay::UpdateInput 
                     patch(0x1a9A3D, 0x0530, 0x2500), //SpawnDisplay::UpdateInput
                     patch(0x1a9A19, 0x0530, 0x2500), //SpawnDisplay::UpdateInput
                     //30 05 00 00 

                     patch(0x1a85a4, 0x0544, 0x2528), //SpawnDisplay::SetSlotInfo //initially 0x2514
                     patch(0x1a85ae, 0x0544, 0x2528), //SpawnDisplay::SetSlotInfo
                     patch(0x1a85c5, 0x0544, 0x2528), //SpawnDisplay::SetSlotInfo
                     patch(0x1a86c8, 0x0544, 0x2528), //SpawnDisplay::SetupSlots
                     patch(0x1a871d, 0x0544, 0x2528), //SpawnDisplay::SetSlotInfo
                     patch(0x1a98d4, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     patch(0x1a9925, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     patch(0x1a9a2b, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     patch(0x1a9a4a, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     patch(0x1a9925, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     //44 05 00 00

                     //patch a sub-pointer that was assigning class names
                     patch(0x1a936b, 0xeb146b89, 0xeb286b89), //SpawnDisplay::Initialize **This is to move the class slot names out of the firing range
                     patch(0x1a86d3, 0x21ec488b, 0x21d8488b), //SpawnDisplay::SetupSlots **This one is for not accidentally wiping out the unit names if less than 10 units

                     //Hotspot Behavior
                     patch(0x1a9499, 0xe83c6b89, 0xe8506b89), //SpawnDisplay::Initialize
                     patch(0x1a9937, 0x056c, 0x2550), //SpawnDisplay::UpdateInput

                  },
            },

            patch_set{
               .name = "Matrix Pool Fix",
               .patches =
                  {
                    //move matrix pool to new location in memory, increase size, should fix sliding
                     patch{0x2c55e + 0x1, 0x01d0d1b0, ES_MATRIX_START, true}, //RedRenderer::AllocMatrixInCache
                     patch{0x2cf23 + 0x1, 0x01d0d1b0, ES_MATRIX_START, true}, //RedRenderer::pcRenderPrimitive
                     patch{0x353f2 + 0x1, 0x01d0d1b0, ES_MATRIX_START, true}, //???
                     patch{0x50f71 + 0x2, 0x01d0d1b0, ES_MATRIX_START, true}, //???
                     patch{0x2c563 + 0x2, 0x0bf6, matrixPool_size}, //RedRenderer::AllocMatrixInCache
                     patch{0x50f77 + 0x2, 0x0bf6, matrixPool_size}, //???
                     patch{0x353f7 + 0x2, 0x0bf6, matrixPool_size}, //???
                     patch{0x5109b + 0x2, 0x0bf6, matrixPool_size}, //???
                     patch{0x356d3 + 0x2, 0x0bf6, matrixPool_size}, //???
                     patch{0x355c9 + 0x1, 0x0bf6, matrixPool_size}, //???
                     patch{0x35536 + 0x2, 0x0bf6, matrixPool_size}, //???
                  },
            },

            patch_set{
               .name = "Hi Rez Unit Fix",
               .patches =
                  {
                     //increase allowable hi-rez units from 10 to 100
                     patch{0x133bf0, 0x014150, 0xc8c90}, //SoldierAnimatorClass::_PostLoad
                     patch{0x133c55, 0x014140, 0xc8c80}, //SoldierAnimatorClass::_PostLoad
                     patch{0x132706, 0x014140, 0xc8c80}, //SoldierAnimatorClass::Destroy
                     patch{0x133c17, 0x10708d0a, 0x10708d64}, //SoldierAnimatorClass::_PostLoad
                     patch{0x133c21 + 0x2, 0x0a, 0x64}, //SoldierAnimatorClass::_PostLoad
                     patch{0x3859b + 0x1, 0x2710, 0x130B0}, //FLRenderer::Init -->>>ISSUE
                     //patch{0x3859b + 0x1, 0x2710, 0x27100}, //FLRenderer::Init -->>>ISSUE
                     patch{0x385cc + 0x1, 0x186a0, 0xF4240}, //FLRenderer::Init
                     patch{0x385e5 + 0x1, 0x3e8, 0x2710}, //FLRenderer::Init
                     patch{0x3862f + 0x1, 0xc350, 0x30D40}, //FLRenderer::Init
                     patch{0x38593 + 0x1, 0x64, 0x3e8}, //FLRenderer::Init
                     patch{0x63bf8, 0x50fb8345, 0x7cfb8345}, //RedLODManager::SetClassMaxCost
                     patch{0x63bfe + 0x1, 0x50, 0x7c}, //RedLODManager::SetClassMaxCost
                     patch{0x3858f, 0x03e8, 0x2710}, //FLRenderer::Init

                     //5b7265 and 8

                  },
            },
         },
   },

   exe_patch_list{
      .name = "Battlefront SPTest",
      .id_address = 0x202164,
      .expected_id = 0x746163696c707041,
      .patches =
         {
            patch_set{
               .name = "RedMemory Heap Extensions",
               .patches =
                  {
                     //patch{0x2165b1, 0x4000000, 0x10000000}, // malloc call arg
                     //patch{0x2165c7, 0x4000000, 0x10000000}, // malloc'd block end pointer
                  },
            },

            patch_set{
               .name = "SoundParameterized Layer Limit Extension",
               .patches =
                  {
                     //patch{0x3e170c, 0xa0, 0x2000},
                  },
            },

            patch_set{
               .name = "DLC Mission Limit Extension",
               .patches =
                  {
                     //patch{0x9d52f, 0x32, DLC_mission_patch_limit, true},                    // AddDownloadableContent
                     //patch{0x9d52f, 0x32, 0x64, true},                    // AddDownloadableContent
                     patch{0xd8a8, 0x67aef8, ES_DLC_START, true},                           // AddDownloadableContent
                     patch{0xd8ca, 0x67aefc, (0x67aefc - 0x67aef8) + ES_DLC_START, true},   // AddDownloadableContent
                     patch{0xd8d0, 0x67af00, (0x67af00 - 0x67aef8) + ES_DLC_START, true}, // AddDownloadableContent
                     patch{0xd8d5, 0x67e418, (0x67e418 - 0x67aef8) + ES_DLC_START, true}, // AddDownloadableContent
                     patch{0xd8f5, 0x67b003, (0x67b003 - 0x67aef8) + ES_DLC_START, true}, // AddDownloadableContent
                    patch{0xd900, 0x67b004, (0x67b004 - 0x67aef8) + ES_DLC_START, true}, // AddDownloadableContent
                     patch{0xd7af, 0x67aef8, ES_DLC_START, true},                           // SetCurrentMap
                     patch{0xd7e4, 0x67aefc, (0x67aefc - 0x67aef8) + ES_DLC_START, true}, // SetCurrentMission
                     patch{0xd822, 0x67af00, (0x67af00 - 0x67aef8) + ES_DLC_START, true}, // GetContentDirectory
                    patch{0xd746, 0x67e418, (0x67e418 - 0x67aef8) + ES_DLC_START, true}, // AddMissionCommon?
                  },
            },

            patch_set{
              /* .name = "Spawn Screen Fix",
               .patches =
                  {
                     patch{0x1a94b4, 0x05, 0x0a}, // SlotWindow Loop
                     patch{0x1a9987, 0x05, 0x0a}, // Unit Selection Loop
                     //patch{0x, 0x734328, ES_DLC_START, true}
                     //patch{0x1a9987, 0x05, 0x0a}, // Unit Selection Loop
                     //patch{0x1a9987, 0x05, 0x0a}, // Unit Selection Loop
                     //patch{0x1a9987, 0x05, 0x0a}, // Unit Selection Loop

                                          //Allow 10 units
                     patch{0x1a94b2, 0x0f05ff83, 0x0f0Aff83}, // SlotWindow Loop (SpawnDisplay::Initialize)
                     patch{0x1a9985, 0x7c05ff83, 0x7c0Aff83}, // Unit Selection Loop (SpawnDisplay::UpdateInput)

                     //resize heap
                     patch{0x1a7e47, 0x2340, 0x2740}, //SpawnDisplay::Create

                     //patch Team Switcher
                     patch{0x1a91c1, 0x0590, 0x2350}, //SpawnDisplay::Initialize
                     patch{0x1a97fb, 0x0580, 0x2340}, //SpawnDisplay::UpdateInput
                     patch{0x1a9856, 0x0590, 0x2350}, //SpawnDisplay::UpdateInput
                     patch{0x1a987b, 0x0594, 0x2354}, //SpawnDisplay::UpdateInput

                     //patch hiding the team switcher when not in IA - 1/8/25
                     patch{0x1a8d7c, 0x0588, 0x2348}, //SpawnDisplay::Show
                     patch{0x1a8d85, 0x0580, 0x2340}, //SpawnDisplay::Show
                     patch{0x1a8d8e, 0x0590, 0x2350}, //SpawnDisplay::Show
                     patch{0x1a8d97, 0x058c, 0x234c}, //SpawnDisplay::Show
                     patch{0x1a8da0, 0x0584, 0x2344}, //SpawnDisplay::Show
                     patch{0x1a8da9, 0x0594, 0x2354}, //SpawnDisplay::Show

                     //patch extra slots not deleting
                     patch(0x1a86bb, 0x7d05f983, 0x7d0af983), //SpawnDisplay::SetupSlots
                     patch(0x1a86c1, 0x05, 0x0a), //SpawnDisplay::SetupSlots
                     patch(0x1a86f3, 0x7c050000, 0x7c0a0000), //SpawnDisplay::SetupSlots

                     //patch extra slots not reapplying if different amounts of units were present - 1/12/2025
                     patch(0x1a8690, 0x0914468b, 0x0928468b), //SpawnDisplay::SetupSlots
                     patch(0x1a8696, 0x093c468b, 0x0950468b), //SpawnDisplay::SetupSlots

                     //patch text not all appearing, move to end of heap
                     patch(0x1a8008, 0x0530, 0x2500), //SpawnDisplay::UpdateObjectText
                     patch(0x1a8646, 0x0530, 0x2500), //SpawnDisplay::SetupSlots 
                     patch(0x1a870B, 0x0530, 0x2500), //SpawnDisplay::SetupSlots
                     patch(0x1a863e, 0xfffffad0, 0xffffdb00), //SpawnDisplay::SetupSlots //woah a negative
                     patch(0x1, 0x0530, 0x2500), //SpawnDisplay::Initialize
                     patch(0x1a9569, 0x0530, 0x2500), //SpawnDisplay::Initialize
                     patch(0xd7df3, 0x0530, 0x2500), //SpawnDisplay::UpdateInput 
                     patch(0xd7e13, 0x0530, 0x2500), //SpawnDisplay::UpdateInput 
                     patch(0xd7ce8, 0x0530, 0x2500), //SpawnDisplay::UpdateInput
                     patch(0xd7d08, 0x0530, 0x2500), //SpawnDisplay::UpdateInput
                     //30 05 00 00 

                     patch(0x1a85a4, 0x0544, 0x2528), //SpawnDisplay::SetSlotInfo //initially 0x2514
                     patch(0x1a85ae, 0x0544, 0x2528), //SpawnDisplay::SetSlotInfo
                     patch(0x1a85c5, 0x0544, 0x2528), //SpawnDisplay::SetSlotInfo
                     patch(0x1a86c8, 0x0544, 0x2528), //SpawnDisplay::SetupSlots
                     patch(0x1a871d, 0x0544, 0x2528), //SpawnDisplay::SetSlotInfo
                     patch(0xd7e00, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     patch(0xd7e1e, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     patch(0xd7cf5, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     patch(0xd7d12, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     patch(0x1a9925, 0x0544, 0x2528), //SpawnDisplay::UpdateInput
                     //44 05 00 00

                     //patch a sub-pointer that was assigning class names
                     patch(0x1a936b, 0xeb146b89, 0xeb286b89), //SpawnDisplay::Initialize **This is to move the class slot names out of the firing range
                     patch(0x1a86d3, 0x21ec488b, 0x21d8488b), //SpawnDisplay::SetupSlots **This one is for not accidentally wiping out the unit names if less than 10 units

                     //Hotspot Behavior
                     patch(0x1a9499, 0xe83c6b89, 0xe8506b89), //SpawnDisplay::Initialize
                     patch(0x1a9937, 0x056c, 0x2550), //SpawnDisplay::UpdateInput

                  },*/
            },
            patch_set{
               .name = "Matrix Pool Fix",
               .patches =
                  {
                    //move matrix pool to new location in memory, increase size, should fix sliding
                    /* patch{0x18251f + 0x2, 0x01c71100, ES_MATRIX_START, true}, //RedRenderer::AllocMatrixInCache
                     patch{0x182593 + 0x2, 0x01c71100, ES_MATRIX_START, true}, //RedRenderer::pcRenderPrimitive
                     patch{0x186b7d + 0x1, 0x01c71100, ES_MATRIX_START, true}, //???
                     patch{0x186b84 + 0x1, 0x01ca0e80, ES_MATRIX_START, true}, //???
                     patch{0x182599 + 0x2, 0x0bf6, matrixPool_size}, //RedRenderer::AllocMatrixInCache
                     patch{0x182525 + 0x2, 0x0bf6, matrixPool_size}, //???*/
                    /* patch{0x353f7 + 0x2, 0x0bf6, matrixPool_size}, //???
                     patch{0x5109b + 0x2, 0x0bf6, matrixPool_size}, //???
                     patch{0x356d3 + 0x2, 0x0bf6, matrixPool_size}, //???
                     patch{0x355c9 + 0x1, 0x0bf6, matrixPool_size}, //???
                     patch{0x35536 + 0x2, 0x0bf6, matrixPool_size}, //???*/
                  },
            },

            patch_set{
               .name = "Hi Rez Unit Fix",
               .patches =
                  {
                     //increase allowable hi-rez units from 10 to 100
                     patch{0x7e12f, 0x014150, 0xc8c90}, //SoldierAnimatorClass::_PostLoad
                     patch{0x7e165, 0x014140, 0xc8c80}, //SoldierAnimatorClass::_PostLoad
                    // patch{0x132706, 0x014140, 0xc8c80}, //SoldierAnimatorClass::~SoldierAnimatorClass
                     patch{0x7e143, 0x5b68590a, 0x5b685964}, //SoldierAnimatorClass::_PostLoad
                     //patch{0x133c21 + 0x2, 0x0a, 0x64}, //SoldierAnimatorClass::_PostLoad
                     patch{0x192d1f + 0x1, 0x2710, 0x271000}, //FLRenderer::Init
                     patch{0x192d24 + 0x1, 0x24e85364, 0x24e8537f}, //FLRenderer::Init
                  },
            },
         },
   },
};

extern const uint32_t ext_section_size = ES_END;