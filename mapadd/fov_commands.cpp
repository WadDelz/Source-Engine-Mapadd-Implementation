#include "cbase.h"
#include "portal_player.h"

CON_COMMAND(fov_zoom, "Zooms The Players Fov\n")
{
	CPortal_Player* pPlayer = dynamic_cast<CPortal_Player*>(UTIL_GetLocalPlayer());
	if (!pPlayer)
		return;

	if (args.ArgC() < 3)
	{
		ConMsg("command usage:\n fov_zoom <zoom_ammount> <zoom_speed>\n");
		return;
	}

	int fov = Q_atoi(args.Arg(1));
	float speed = Q_atof(args.Arg(2));

	pPlayer->SetFOV(pPlayer, fov, speed);
}