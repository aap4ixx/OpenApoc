#include "framework/framework.h"
#include "game/state/rules/vequipment_type.h"
#include "tools/extractors/common/ufo2p.h"
#include "tools/extractors/extractors.h"

// Doodads used by tactical weaponry, ids matching vanilla
#define UFO_DOODAD_0 0   // bigmissiles	same as 3 ? ? ?
#define UFO_DOODAD_1 1   // autocannon		0, 4
#define UFO_DOODAD_2 2   // airguard		4, 8
#define UFO_DOODAD_3 3   // explosion		8, 28
#define UFO_DOODAD_4 4   // nothing ?		28, 29
#define UFO_DOODAD_5 5   // smokeexpl		29, 42
#define UFO_DOODAD_6 6   // dimensiong		42, 74
#define UFO_DOODAD_7 7   // janitor etc	74, 84
#define UFO_DOODAD_8 8   // laser			84, 90
#define UFO_DOODAD_9 9   // plasma			90, 96
#define UFO_DOODAD_10 10 // disruptor		96, 102
#define UFO_DOODAD_11 11 // subvertbig		102, 108
#define UFO_DOODAD_12 12 // subvertsml		108, 114
#define UFO_DOODAD_13 13 // smokefume		114, 126
#define UFO_DOODAD_14 14 // infilbig		126, 143
#define UFO_DOODAD_15 15 // infilsml		143, 160

namespace OpenApoc
{

void InitialGameStateExtractor::extractVehicleEquipment(GameState &state, Difficulty)
{
	auto &data = this->ufo2p;

	// FIXME: Track these as some things (the weapon icon?) seem to be ordered by when they're
	// defined
	int weapon_count = 0;
	int engine_count = 0;
	int general_count = 0;

	for (unsigned i = 0; i < data.vehicle_equipment->count(); i++)
	{
		auto e = mksp<VEquipmentType>();
		auto edata = data.vehicle_equipment->get(i);

		e->name = data.vehicle_equipment_names->get(i);
		UString id = UString::format("%s%s", VEquipmentType::getPrefix(), canon_string(e->name));

		e->id = id;

		// Some data fields are common for all equipment types
		switch (edata.usable_by)
		{
			case VEHICLE_EQUIPMENT_USABLE_GROUND:
				e->users.insert(VEquipmentType::User::Ground);
				break;
			case VEHICLE_EQUIPMENT_USABLE_AIR:
				e->users.insert(VEquipmentType::User::Air);
				break;
			case VEHICLE_EQUIPMENT_USABLE_GROUND_AIR:
				e->users.insert(VEquipmentType::User::Ground);
				e->users.insert(VEquipmentType::User::Air);
				break;
			case VEHICLE_EQUIPMENT_USABLE_AMMO:
				// FIXME: Not sure what 'AMMO' usable is used for?
				e->users.insert(VEquipmentType::User::Ammo);
				break;
			default:
				LogWarning("Unexpected 'usable_by' %d for ID %s", (int)edata.usable_by, id.cStr());
				continue;
		}
		e->weight = edata.weight;
		// FIXME: max_ammo 0xffff is used for 'no ammo' (IE automatically-recharging stuff)

		e->max_ammo = edata.max_ammo;
		e->ammo_type = UString::format("%d", (int)edata.ammo_type);
		// Force all sprites into the correct palette by using A_RANDOM_VEHICLES_BACKGROUND pcx
		//(I assume the parts of the palette used for this are the same on all?)
		e->equipscreen_sprite = fw().data->loadImage(UString::format(
		    "PCK:xcom3/ufodata/vehequip.pck:xcom3/ufodata/vehequip.tab:%d:xcom3/ufodata/vhawk.pcx",
		    (int)edata.sprite_idx));
		e->equipscreen_size = {edata.size_x, edata.size_y};
		e->manufacturer = {&state, data.getOrgId(edata.manufacturer)};
		e->store_space = edata.store_space;

		switch (edata.type)
		{
			case VEHICLE_EQUIPMENT_TYPE_ENGINE:
			{
				auto engData = data.vehicle_engines->get(edata.data_idx);
				e->type = VEquipmentType::Type::Engine;
				e->power = engData.power;
				e->top_speed = engData.top_speed;
				engine_count++;
				break;
			}
			case VEHICLE_EQUIPMENT_TYPE_WEAPON:
			{
				auto wData = data.vehicle_weapons->get(edata.data_idx);
				e->type = VEquipmentType::Type::Weapon;
				e->speed = wData.speed;
				e->damage = wData.damage;
				e->accuracy = 100 - wData.accuracy;
				e->fire_delay = wData.fire_delay;
				e->tail_size = wData.tail_size;
				e->guided = wData.guided != 0 ? true : false;
				e->turn_rate = wData.turn_rate;
				e->range = wData.range;
				e->firing_arc_1 = wData.firing_arc_1;
				e->firing_arc_2 = wData.firing_arc_2;
				e->point_defence = wData.point_defence != 0 ? true : false;
				UString sfx_path = "";
				switch (wData.fire_sfx)
				{
					case 71:
						sfx_path = "strategc/weapons/airguard";
						break;
					case 72:
						sfx_path = "strategc/weapons/bolter";
						break;
					case 73:
						sfx_path = "strategc/weapons/dinvrsn1";
						break;
					case 74:
						sfx_path = "strategc/weapons/disrupt1";
						break;
					case 75:
						sfx_path = "strategc/weapons/disrupt2";
						break;
					case 76:
						sfx_path = "strategc/weapons/disrupt3";
						break;
					case 77:
						sfx_path = "strategc/weapons/gr_missl";
						break;
					case 78:
						sfx_path = "strategc/weapons/hellfire";
						break;
					case 79:
						sfx_path = "strategc/weapons/janitor";
						break;
					case 80:
						sfx_path = "strategc/weapons/justice";
						break;
					case 81:
						sfx_path = "strategc/weapons/lancer";
						break;
					case 82:
						sfx_path = "strategc/weapons/mars_glm";
						break;
					case 83:
						sfx_path = "strategc/weapons/marsdef";
						break;
					case 84:
						sfx_path = "strategc/weapons/marsplas";
						break;
					case 85:
						sfx_path = "strategc/weapons/mcannon";
						break;
					case 86:
						sfx_path = "strategc/weapons/meglaser";
						break;
					case 87:
						sfx_path = "strategc/weapons/mplasma1";
						break;
					case 88:
						sfx_path = "strategc/weapons/mplasma2";
						break;
					case 89:
						sfx_path = "strategc/weapons/multplas";
						break;
					case 90:
						sfx_path = "strategc/weapons/repeater";
						break;
					case 91:
						sfx_path = "strategc/weapons/retrib";
						break;
					case 92:
						sfx_path = "strategc/weapons/ruptmult";
						break;
					case 93:
						sfx_path = "strategc/weapons/stasis";
						break;
					case 94:
						sfx_path = "strategc/weapons/tnkcanon";
						break;
				}
				if (sfx_path != "")
					e->fire_sfx =
					    fw().data->loadSample("RAWSOUND:xcom3/rawsound/" + sfx_path + ".raw:22050");
				// FIXME: I think this is correct? All non-guided attacks sound like expl1, all
				// missiles as expl2? Confirm!
				if (wData.guided == 0)
				{
					e->impact_sfx = fw().data->loadSample(
					    "RAWSOUND:xcom3/rawsound/strategc/explosns/explosn1.raw:22050");
				}
				else
				{
					e->impact_sfx = fw().data->loadSample(
					    "RAWSOUND:xcom3/rawsound/strategc/explosns/explosn2.raw:22050");
				}

				UString doodad_id = "";
				switch (wData.explosion_graphic)
				{
					case UFO_DOODAD_1:
						doodad_id = "DOODAD_1_AUTOCANNON";
						break;
					case UFO_DOODAD_2:
						doodad_id = "DOODAD_2_AIRGUARD";
						break;
					case UFO_DOODAD_0: // same as 3
					case UFO_DOODAD_3:
						doodad_id = "DOODAD_3_EXPLOSION";
						break;
					case UFO_DOODAD_4:
						doodad_id = "DOODAD_4_BLUEDOT";
						break;
					case UFO_DOODAD_5:
						doodad_id = "DOODAD_5_SMOKE_EXPLOSION";
						break;
					case UFO_DOODAD_6:
						doodad_id = "DOODAD_6_DIMENSION_GATE";
						break;
					case UFO_DOODAD_7:
						doodad_id = "DOODAD_7_JANITOR";
						break;
					case UFO_DOODAD_8:
						doodad_id = "DOODAD_8_LASER";
						break;
					case UFO_DOODAD_9:
						doodad_id = "DOODAD_9_PLASMA";
						break;
					case UFO_DOODAD_10:
						doodad_id = "DOODAD_10_DISRUPTOR";
						break;
					case UFO_DOODAD_11:
						doodad_id = "DOODAD_11_SUBVERSION_BIG";
						break;
					case UFO_DOODAD_12:
						doodad_id = "DOODAD_12_SUBVERSION_SMALL";
						break;
					case UFO_DOODAD_13:
						doodad_id = "DOODAD_13_SMOKE_FUME";
						break;
					case UFO_DOODAD_14:
						doodad_id = "DOODAD_14_INFILTRATION_BIG";
						break;
					case UFO_DOODAD_15:
						doodad_id = "DOODAD_15_INFILTRATION_SMALL";
						break;
				}
				if (doodad_id != "")
				{
					e->explosion_graphic = {&state, doodad_id};
				}

				e->icon = fw().data->loadImage(UString::format(
				    "PCK:xcom3/ufodata/vs_obs.pck:xcom3/ufodata/vs_obs.tab:%d", weapon_count));

				auto projectile_sprites = data.projectile_sprites->get(wData.projectile_image);
				for (int i = 0; i < e->tail_size; i++)
				{
					UString sprite_path = "";
					if (projectile_sprites.sprites[i] != 255)
					{
						sprite_path = UString::format("bulletsprites/city/%02u.png",
						                              (unsigned)projectile_sprites.sprites[i]);
					}
					else
					{
						sprite_path = ""; // a 'gap' in the projectile trail
					}
					e->projectile_sprites.push_back(fw().data->loadImage(sprite_path));
				}
				weapon_count++;
				break;
			}
			case VEHICLE_EQUIPMENT_TYPE_GENERAL:
			{
				auto gData = data.vehicle_general_equipment->get(edata.data_idx);
				e->type = VEquipmentType::Type::General;
				e->accuracy_modifier = gData.accuracy_modifier;
				e->cargo_space = gData.cargo_space;
				e->passengers = gData.passengers;
				e->alien_space = gData.alien_space;
				e->missile_jamming = gData.missile_jamming;
				e->shielding = gData.shielding;
				e->cloaking = gData.cloaking != 0 ? true : false;
				e->teleporting = gData.teleporting != 0 ? true : false;
				general_count++;
				break;
			}
			default:
				// FIXME:
				// We should never reach here because we "continue" before in "usable_by" field
				// If we do reach here, however, should we not just log a warning and go on?
				// Or log an error that we actually got here (which is the actual bug, and
				// not the fact that we encountered an expected and known id for empty item)
				LogError("Unexpected vequipment type %d for ID %s", (int)e->type, id.cStr());
		}

		state.vehicle_equipment[id] = e;
	}

	// DOODADS
	{
		static const int frameTTL = 4;
		static const std::vector<Vec2<int>> doodadTabOffsets = {
		    {0, 4},     {4, 8},     {8, 28},    {28, 29},   {29, 42},
		    {42, 74},   {74, 84},   {84, 90},   {90, 96},   {96, 102},
		    {102, 108}, {108, 114}, {114, 126}, {126, 143}, {143, 160},
		};

		for (int i = 1; i <= 15; i++)
		{
			UString doodad_id;
			switch (i)
			{
				case UFO_DOODAD_1:
					doodad_id = "DOODAD_1_AUTOCANNON";
					break;
				case UFO_DOODAD_2:
					doodad_id = "DOODAD_2_AIRGUARD";
					break;
				case UFO_DOODAD_3:
					doodad_id = "DOODAD_3_EXPLOSION";
					break;
				case UFO_DOODAD_4:
					doodad_id = "DOODAD_4_BLUEDOT";
					break;
				case UFO_DOODAD_5:
					doodad_id = "DOODAD_5_SMOKE_EXPLOSION";
					break;
				case UFO_DOODAD_6:
					doodad_id = "DOODAD_6_DIMENSION_GATE";
					break;
				case UFO_DOODAD_7:
					doodad_id = "DOODAD_7_JANITOR";
					break;
				case UFO_DOODAD_8:
					doodad_id = "DOODAD_8_LASER";
					break;
				case UFO_DOODAD_9:
					doodad_id = "DOODAD_9_PLASMA";
					break;
				case UFO_DOODAD_10:
					doodad_id = "DOODAD_10_DISRUPTOR";
					break;
				case UFO_DOODAD_11:
					doodad_id = "DOODAD_11_SUBVERSION_BIG";
					break;
				case UFO_DOODAD_12:
					doodad_id = "DOODAD_12_SUBVERSION_SMALL";
					break;
				case UFO_DOODAD_13:
					doodad_id = "DOODAD_13_SMOKE_FUME";
					break;
				case UFO_DOODAD_14:
					doodad_id = "DOODAD_14_INFILTRATION_BIG";
					break;
				case UFO_DOODAD_15:
					doodad_id = "DOODAD_15_INFILTRATION_SMALL";
					break;
			}

			auto tabOffsets = doodadTabOffsets[i - 1];
			auto d = mksp<DoodadType>();

			d->imageOffset = CITY_IMAGE_OFFSET;
			d->lifetime = (tabOffsets.y - tabOffsets.x) * frameTTL;
			d->repeatable = i == UFO_DOODAD_6; // dimension gate
			for (int j = tabOffsets.x; j < tabOffsets.y; j++)
			{
				d->frames.push_back({fw().data->loadImage(UString::format(
				                         "PCK:xcom3/ufodata/ptang.pck:xcom3/ufodata/"
				                         "ptang.tab:%d",
				                         j)),
				                     frameTTL});
			}

			state.doodad_types[doodad_id] = d;
		}
	}
}

} // namespace OpenApoc
