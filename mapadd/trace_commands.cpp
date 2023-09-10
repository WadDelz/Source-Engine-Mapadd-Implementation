#include "cbase.h"

CON_COMMAND(mapadd_trace_origin, "Prints out the origin of the end of a trace coming out of the players eyes\n")
{
	CBasePlayer * pPlayer = UTIL_GetLocalPlayer();

	Vector vecDir;
	AngleVectors(pPlayer->EyeAngles(), &vecDir);

	Vector vecAbsStart = pPlayer->EyePosition();
	Vector vecAbsEnd = vecAbsStart + (vecDir * MAX_TRACE_LENGTH);

	trace_t tr;
	UTIL_TraceLine(vecAbsStart, vecAbsEnd, MASK_ALL, pPlayer, COLLISION_GROUP_INTERACTIVE, &tr);

	ConMsg("Trace Hit: \"%f %f %f\"\n", tr.endpos.x, tr.endpos.y, tr.endpos.z);
}

CON_COMMAND(mapadd_entity_origin_print, "Prints The Origin Of The Entity The Player Is Looking At.\n Use This With \"picker\" To See The Entity Your Looking At\n")
{
	CBaseEntity* pEnt = gEntList.FindEntityByName(NULL, "!picker");
	if (pEnt)
	{
		ConMsg("Entity: \"%s\" Found With This Origin And Angles\n \"Origin\" \"%f %f %f\"\n \"Angles\" \"%f %f %f\"\n", pEnt->GetClassname(), pEnt->GetAbsOrigin().x,
			pEnt->GetAbsOrigin().y, pEnt->GetAbsOrigin().z, pEnt->GetAbsAngles().x, pEnt->GetAbsAngles().y, pEnt->GetAbsAngles().z);
	}
	else
	{
		ConMsg("Entity Not Found\n");
	}
}

CON_COMMAND(mapadd_entity_origin_by_sphere, "Prints The Origin Of The Entity The Player Is Looking At.\n Use This With \"picker\" To See The Entity Your Looking At\n")
{
	if (args.ArgC() < 3)
	{
		ConColorMsg(Color(255, 0, 0, 200), "Error Command Usage:  \"mapadd_entity_origin_by_sphere \"x y z\" Radius\"\n");
	}
	
	Vector vec;
	UTIL_StringToVector(vec.Base(), args.Arg(1));
	CBaseEntity* pEnt = gEntList.FindEntityInSphere(NULL, Vector(vec.x, vec.y, vec.z), Q_atof(args.Arg(2)));
	
	if (pEnt)
		ConMsg("%s Found With Origin Being: \"%f %f %f\"\n", pEnt->GetClassname(), pEnt->GetAbsOrigin().x, pEnt->GetAbsOrigin().y, pEnt->GetAbsOrigin().z);
	else {
		ConMsg("Error: \"No Entity Found Near\"");
	}
}
