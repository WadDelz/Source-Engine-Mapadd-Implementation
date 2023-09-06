#include "cbase.h"
#include "mapadd.h"

ConVar instant_trigger_debug("instant_trigger_debug", "0");

class CInstantTrigger : public CBaseEntity
{
public:
	DECLARE_CLASS(CInstantTrigger, CBaseEntity);
	DECLARE_DATADESC();

	CInstantTrigger() {
		enabled = true;
		radius = 128.0f;
		DisableOnFire = true;
	}

	void Spawn() {
		BaseClass::Spawn();
		SetSolid(SOLID_BSP);
		SetSolidFlags(FSOLID_TRIGGER);
	}

	void Activate() {
		BaseClass::Activate();
		SetNextThink(gpGlobals->curtime);
	}

	void Think() {
		if (enabled)
		{
			CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
			if (pPlayer && pPlayer->GetAbsOrigin().DistTo(GetAbsOrigin()) <= radius)
			{
				ontrigger.FireOutput(this, 0);
				CMapaddSystem* mapadd = GetMapaddSystem();
				if (mapadd)
				{
					char entities[FILENAME_MAX];
					Q_snprintf(entities, sizeof(entities), "entities:%s", label);

					KeyValues* laabel = mapadd->mapaddFile->FindKey(entities);
					if (laabel)
						mapadd->ParseEntities(laabel);
				}
				if (DisableOnFire)
					enabled = false;
			}
			if (instant_trigger_debug.GetBool())
			{
				NDebugOverlay::Box(GetAbsOrigin(), Vector(-10, -10, -10), Vector(10, 10, 10), 255, 100, 0, 125, NDEBUG_PERSIST_TILL_NEXT_SERVER);
				NDebugOverlay::Sphere(GetAbsOrigin(), radius, 255, 100, 0, 1, NDEBUG_PERSIST_TILL_NEXT_SERVER);
			}
			SetNextThink(gpGlobals->curtime);
		}
	}

private:
	bool enabled;
	bool DisableOnFire;
	float radius;
	string_t label;
	COutputEvent ontrigger;
};

LINK_ENTITY_TO_CLASS(instant_trigger, CInstantTrigger);

BEGIN_DATADESC(CInstantTrigger)
DEFINE_KEYFIELD(enabled, FIELD_BOOLEAN, "enabled"),
DEFINE_KEYFIELD(DisableOnFire, FIELD_BOOLEAN, "autodisable"),
DEFINE_KEYFIELD(radius, FIELD_FLOAT, "radius"),
DEFINE_KEYFIELD(label, FIELD_STRING, "label"),
DEFINE_OUTPUT(ontrigger, "ontrigger"),
END_DATADESC();