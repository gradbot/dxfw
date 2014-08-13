#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 9.30.9200.16384
//
//
///
// Buffer Definitions: 
//
// cbuffer LogicControl
// {
//
//   float index;                       // Offset:    0 Size:     4 [unused]
//   float scale;                       // Offset:    4 Size:     4
//   float bias;                        // Offset:    8 Size:     4
//   bool depthMode;                    // Offset:   12 Size:     4 [unused]
//   int renderMode;                    // Offset:   16 Size:     4
//   bool normalized;                   // Offset:   20 Size:     4 [unused]
//   float width;                       // Offset:   24 Size:     4 [unused]
//   float height;                      // Offset:   28 Size:     4 [unused]
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim Slot Elements
// ------------------------------ ---------- ------- ----------- ---- --------
// samLinear                         sampler      NA          NA    0        1
// tx1D                              texture  float4          1d    0        1
// LogicControl                      cbuffer      NA          NA    0        1
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// TEXCOORD                 0   xyzw        1     NONE   float   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Target                0   xyzw        0   TARGET   float   xyzw
//
ps_4_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer cb0[2], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture1d (float,float,float,float) t0
dcl_input_ps linear v1.x
dcl_output o0.xyzw
dcl_temps 2
ieq r0.xyz, cb0[1].xxxx, l(1, 2, 3, 0)
sample r1.xyzw, v1.xxxx, t0.xyzw, s0
movc r0.z, r0.z, r1.z, r1.w
movc r0.y, r0.y, r1.y, r0.z
movc r0.xyz, r0.xxxx, r1.xxxx, r0.yyyy
mul r1.xyzw, r1.xyzw, cb0[0].yyyy
mov r0.w, l(1.000000)
mul r0.xyzw, r0.xyzw, cb0[0].yyyy
movc r0.xyzw, cb0[1].xxxx, r0.xyzw, r1.xyzw
add o0.xyzw, r0.xyzw, cb0[0].zzzz
ret 
// Approximately 11 instruction slots used
#endif

const BYTE g_PS_1D[] =
{
     68,  88,  66,  67, 187,  85, 
     54, 211,  67,  96, 205, 100, 
     24, 105, 202, 232, 142, 103, 
    242, 115,   1,   0,   0,   0, 
     24,   5,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
     84,   2,   0,   0, 172,   2, 
      0,   0, 224,   2,   0,   0, 
    156,   4,   0,   0,  82,  68, 
     69,  70,  24,   2,   0,   0, 
      1,   0,   0,   0, 152,   0, 
      0,   0,   3,   0,   0,   0, 
     28,   0,   0,   0,   1,   4, 
    255, 255,   0,   1,   0,   0, 
    228,   1,   0,   0, 124,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    134,   0,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      2,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 139,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 115,  97, 
    109,  76, 105, 110, 101,  97, 
    114,   0, 116, 120,  49,  68, 
      0,  76, 111, 103, 105,  99, 
     67, 111, 110, 116, 114, 111, 
    108,   0, 139,   0,   0,   0, 
      8,   0,   0,   0, 176,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 112,   1,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    120,   1,   0,   0,   0,   0, 
      0,   0, 136,   1,   0,   0, 
      4,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    120,   1,   0,   0,   0,   0, 
      0,   0, 142,   1,   0,   0, 
      8,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    120,   1,   0,   0,   0,   0, 
      0,   0, 147,   1,   0,   0, 
     12,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    160,   1,   0,   0,   0,   0, 
      0,   0, 176,   1,   0,   0, 
     16,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    188,   1,   0,   0,   0,   0, 
      0,   0, 204,   1,   0,   0, 
     20,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    160,   1,   0,   0,   0,   0, 
      0,   0, 215,   1,   0,   0, 
     24,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    120,   1,   0,   0,   0,   0, 
      0,   0, 221,   1,   0,   0, 
     28,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    120,   1,   0,   0,   0,   0, 
      0,   0, 105, 110, 100, 101, 
    120,   0, 171, 171,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 115,  99,  97, 108, 
    101,   0,  98, 105,  97, 115, 
      0, 100, 101, 112, 116, 104, 
     77, 111, 100, 101,   0, 171, 
    171, 171,   0,   0,   1,   0, 
      1,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    114, 101, 110, 100, 101, 114, 
     77, 111, 100, 101,   0, 171, 
      0,   0,   2,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 110, 111, 
    114, 109,  97, 108, 105, 122, 
    101, 100,   0, 119, 105, 100, 
    116, 104,   0, 104, 101, 105, 
    103, 104, 116,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  57,  46,  51, 
     48,  46,  57,  50,  48,  48, 
     46,  49,  54,  51,  56,  52, 
      0, 171,  73,  83,  71,  78, 
     80,   0,   0,   0,   2,   0, 
      0,   0,   8,   0,   0,   0, 
     56,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     68,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  15,   1,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0, 171, 171, 171, 
     79,  83,  71,  78,  44,   0, 
      0,   0,   1,   0,   0,   0, 
      8,   0,   0,   0,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
     95,  84,  97, 114, 103, 101, 
    116,   0, 171, 171,  83,  72, 
     68,  82, 180,   1,   0,   0, 
     65,   0,   0,   0, 109,   0, 
      0,   0, 106,   8,   0,   1, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  90,   0, 
      0,   3,   0,  96,  16,   0, 
      0,   0,   0,   0,  88,  16, 
      0,   4,   0, 112,  16,   0, 
      0,   0,   0,   0,  85,  85, 
      0,   0,  98,  16,   0,   3, 
     18,  16,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 104,   0,   0,   2, 
      2,   0,   0,   0,  32,   0, 
      0,  11, 114,   0,  16,   0, 
      0,   0,   0,   0,   6, 128, 
     32,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     69,   0,   0,   9, 242,   0, 
     16,   0,   1,   0,   0,   0, 
      6,  16,  16,   0,   1,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     55,   0,   0,   9,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      1,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     55,   0,   0,   9,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
     55,   0,   0,   9, 114,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,   6,   0,  16,   0, 
      1,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   8, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  86, 133,  32,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  56,   0, 
      0,   8, 242,   0,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     86, 133,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     55,   0,   0,  10, 242,   0, 
     16,   0,   0,   0,   0,   0, 
      6, 128,  32,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   8, 242,  32,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
     11,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
