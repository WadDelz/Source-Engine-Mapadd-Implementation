#include "cbase.h"
#include "filesystem.h"
#include "mapadd.h"
#include "includes.h"

extern ConVar mapadd_secondary_script;

static int MapaddLabelCompletion(const char *partial, char commands[COMMAND_COMPLETION_MAXITEMS][COMMAND_COMPLETION_ITEM_LENGTH])
{
	int current = 0;

	const char *cmdname = "mapadd_execute_label";
	char *substring = NULL;
	int substringLen = 0;
	if (Q_strstr(partial, cmdname) && strlen(partial) > strlen(cmdname) + 1)
	{
		substring = (char *)partial + strlen(cmdname) + 1;
		substringLen = strlen(substring);
	}

	for (int i = 1 - 1; i >= 0 && current < COMMAND_COMPLETION_MAXITEMS; i--)
	{	
		char filename[FILENAME_MAX];
		Q_snprintf(filename, sizeof(filename), "mapadd/main.txt");
		char filename_map[FILENAME_MAX];
		Q_snprintf(filename_map, sizeof(filename_map), "mapadd/%s.txt", STRING(gpGlobals->mapname));
		char filename_second[FILENAME_MAX];
		Q_snprintf(filename_second, sizeof(filename_second), "%s", mapadd_secondary_script.GetString());
		
		KeyValues* kv = new KeyValues("CCC");
		kv->LoadFromFile(filesystem, filename, "MOD");
		kv->LoadFromFile(filesystem, filename_map, "MOD");
		kv->LoadFromFile(filesystem, filename_second, "MOD");
			FOR_EACH_SUBKEY(kv, classname)
			{
				if (StartsWith(classname->GetName(), "entities:"))
				{
					const char* labelname = classname->GetName();
					const char* labelname_abs = labelname + 9;
					if (labelname_abs)
					{
						if (!substring || !Q_strncasecmp(labelname_abs, substring, substringLen))
						{
							Q_snprintf(commands[current], sizeof(commands[current]), "%s %s", cmdname, labelname_abs);
							current++;
						}
					}
				}
		}
	}

	return current;
}

CON_COMMAND_F_COMPLETION(mapadd_execute_label, "executes a mapadd label from mapadd file", FCVAR_NONE, MapaddLabelCompletion)
{
	if (args.ArgC() < 2)
	{
		ConMsg("command usage: \"mapadd_execute_label <labelname>\"\n");
		return;
	}
	else
	{
		CMapaddSystem* mapadd = GetMapaddSystem();
		if (mapadd)
		{
			char entities[FILENAME_MAX];
			Q_snprintf(entities, sizeof(entities), "entities:%s", args.Arg(1));

			KeyValues* label = mapadd->mapaddFile->FindKey(entities);
			if (label)
				mapadd->ParseEntities(label);
		}
	}
}

void RestartMapadd()
{
	CMapaddSystem* mapadd = GetMapaddSystem();
	KeyValues* restart = mapadd->mapaddFile->FindKey("restart");
	if (restart)
	{
		mapadd->ParseEntities(restart);
	}
	char buf[FILENAME_MAX];
	Q_snprintf(buf, sizeof(buf), "restart:%s", gpGlobals->mapname);
	KeyValues* map_restart = mapadd->mapaddFile->FindKey(buf);
	if (map_restart)
	{
		mapadd->ParseEntities(map_restart);
	}

	engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "restart");
}

static ConCommand restart_mapadd("restart_mapadd", RestartMapadd, "this is the same as the console \"command\" restart");