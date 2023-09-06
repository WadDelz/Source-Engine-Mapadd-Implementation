#include "cbase.h"

class COriginHelper : public CBaseEntity
{
public:
	DECLARE_CLASS(COriginHelper, CBaseEntity);

	void Activate();
	void Think();
};

LINK_ENTITY_TO_CLASS(origin_helper, COriginHelper);

void COriginHelper::Activate() {
	BaseClass::Activate();
	SetNextThink(gpGlobals->curtime + 0.2f);
}

void COriginHelper::Think() {
	CBasePlayer * pPlayer = UTIL_GetLocalPlayer();
	if (pPlayer)
	{
		Vector vecDir;
		AngleVectors(pPlayer->EyeAngles(), &vecDir);

		Vector vecAbsStart = pPlayer->EyePosition();
		Vector vecAbsEnd = vecAbsStart + (vecDir * MAX_TRACE_LENGTH);

		trace_t tr;
		UTIL_TraceLine(vecAbsStart, vecAbsEnd, MASK_ALL, pPlayer, COLLISION_GROUP_INTERACTIVE, &tr);

		this->SetAbsOrigin(tr.endpos);
	}
	char name[FILENAME_MAX];
	Q_snprintf(name, sizeof(name), "Origin: \"%f %f %f\"", GetAbsOrigin().x, GetAbsOrigin().y, GetAbsOrigin().z);

	NDebugOverlay::Axis(GetAbsOrigin(), GetAbsAngles(), 20, 0, NDEBUG_PERSIST_TILL_NEXT_SERVER);
	NDebugOverlay::EntityTextAtPosition(GetAbsOrigin(), 0, name, NDEBUG_PERSIST_TILL_NEXT_SERVER);

	SetNextThink(gpGlobals->curtime);
}

CON_COMMAND(mapadd_origin_helper, "Creates an origin helper\n")
{
	CBaseEntity *pEnt = gEntList.FindEntityByClassname(NULL, "origin_helper");
	if (!pEnt)
	{
		engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "ent_create origin_helper");
	}
	else
	{
		UTIL_Remove(pEnt);
	}
}

CON_COMMAND(mapadd_origin_helper_print, "Prints the origin of an origin helper\n")
{
	CBaseEntity *pEnt = gEntList.FindEntityByClassname(NULL, "origin_helper");
	if (pEnt)
	{
		ConMsg("\"origin\" \"%f %f %f\"\n", pEnt->GetAbsOrigin().x, pEnt->GetAbsOrigin().y, pEnt->GetAbsOrigin().z);
	}
}