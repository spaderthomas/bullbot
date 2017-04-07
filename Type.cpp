#include "Type.h"

float effectiveness[20][20] = {
	// Attacking Type
	////NRM  GRS  FIR  WTR  ELE  FLY  GND  RCK  ICE  BUG  PSN  FGT  PSY  GHO  DRK  STL  DRG  FRY  LGT  NTL
	{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f }, // NRM Defending Type
	{ 1.f, .5f, 2.f, .5f, .5f, 2.f, .5f, 1.f, 2.f, 2.f, 2.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f }, // GRS
	{ 1.f, .5f, .5f, 2.f, 1.f, 1.f, 2.f, 2.f, .5f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 1.f, .5f, 1.f, 1.f }, // FIR
	{ 1.f, 2.f, .5f, .5f, 2.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f }, // WTR
	{ 1.f, 1.f, 1.f, 1.f, .5f, .5f, 2.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f }, // ELE

	{ 1.f, .5f, 1.f, 1.f, 2.f, 1.f, 0.f, 2.f, 2.f, .5f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f }, // FLY
	{ 1.f, 2.f, 1.f, 2.f, 0.f, 1.f, 1.f, .5f, 2.f, .5f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f }, // GND
	{ .5f, 2.f, .5f, 2.f, 1.f, .5f, 2.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f }, // RCK
	{ 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 2.f, .5f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f }, // ICE
	{ 1.f, 1.f, 2.f, 1.f, 1.f, 2.f, .5f, 2.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f }, // BUG

	{ 1.f, .5f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, .5f, .5f, .5f, 2.f, 1.f, 1.f, 1.f, 1.f, .5f, 1.f, 1.f }, // PSN
	{ 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, .5f, 1.f, .5f, 1.f, 1.f, 2.f, 1.f, .5f, 1.f, 1.f, 2.f, .5f, 1.f }, // FGT
	{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, .5f, .5f, 2.f, 2.f, 1.f, 1.f, 1.f, 1.f, 1.f }, // PSY
	{ 0.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, .5f, 0.f, 1.f, 2.f, 2.f, 1.f, 1.f, 1.f, 2.f, 1.f }, // GHO
	{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 1.f, 2.f, 0.f, .5f, .5f, 1.f, 1.f, 2.f, 2.f, 1.f }, // DRK

	{ .5f, .5f, 2.f, 1.f, 1.f, .5f, 2.f, .5f, .5f, .5f, 0.f, 2.f, .5f, 1.f, 1.f, .5f, .5f, .5f, 1.f, 1.f }, // STL
	{ 1.f, .5f, .5f, .5f, .5f, 1.f, 1.f, 1.f, 2.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 2.f, 2.f, 1.f, 1.f }, // DRG
	{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 2.f, .5f, 1.f, 1.f, .5f, 2.f, 0.f, 1.f, 1.f, 1.f }, // FRY
	{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, .5f, 2.f, 1.f, 1.f, 1.f, .5f, 1.f }, // LGT
	{ 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f, 1.f }  // NTL
};;