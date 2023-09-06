#include "cbase.h"

CON_COMMAND(trace_origin, "Prints out the origin of the end of a trace coming out of the players eyes\n")
{
	CBasePlayer * pPlayer = UTIL_GetLocalPlayer();

	Vector vecDir;
	AngleVectors(pPlayer->EyeAngles(), &vecDir);

	Vector vecAbsStart = pPlayer->EyePosition();
	Vector vecAbsEnd = vecAbsStart + (vecDir * MAX_TRACE_LENGTH);

	trace_t tr;
	UTIL_TraceLine(vecAbsStart, vecAbsEnd, MASK_ALL, pPlayer, COLLISION_GROUP_INTERACTIVE, &tr);

	ConMsg("Trace Hit \"%f %f %f\"\n", tr.endpos.x, tr.endpos.y, tr.endpos.z);
}