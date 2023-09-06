#include "cbase.h"
#include "filesystem.h"
#include "mapadd.h"
#include "includes.h"

ConVar mapadd_disableload("mapadd_disableload", "0");
ConVar mapadd_secondary_script("mapadd_secondary_script", "mapadd/mods/NULL.txt");

CMapaddSystem::CMapaddSystem(const char* name) : CAutoGameSystem(name)
{
}

CMapaddSystem g_mapaddSystem("mapadd");

CMapaddSystem *GetMapaddSystem()
{
	return &g_mapaddSystem;
}

void CMapaddSystem::LevelInitPostEntity()
{
	if (mapadd_disableload.GetBool())
		return;

	char MainFile[FILENAME_MAX];
	Q_snprintf(MainFile, sizeof(MainFile), "mapadd/%s.txt", STRING(gpGlobals->mapname));
	ParseMapaddFile(MainFile);

	char SecondFile[FILENAME_MAX];
	Q_snprintf(SecondFile, sizeof(SecondFile), "mapadd/main.txt");
	ParseMapaddFile(SecondFile);

	char ModFile[FILENAME_MAX];
	Q_snprintf(ModFile, sizeof(ModFile), "%s", mapadd_secondary_script.GetString());
	ParseMapaddFile(ModFile);
}

void CMapaddSystem::ParseMapaddFile(const char* name)
{
	if (!mapaddFile)
		mapaddFile = new KeyValues("mapadd");

	if (!mapaddFile->LoadFromFile(filesystem, name, "MOD"))
		return;
		
	KeyValues* Precache = mapaddFile->FindKey("precache");
	KeyValues* Entities = mapaddFile->FindKey("entities");
	KeyValues* map = mapaddFile->FindKey(gpGlobals->mapname.ToCStr());

	if (Precache)
		PrecacheSection(Precache);

	if (Entities)
		ParseEntities(Entities);

	if (map)
		ParseEntities(map);
}

void CMapaddSystem::PrecacheSection(KeyValues* keyvalues)
{
	FOR_EACH_VALUE(keyvalues, value)
	{
		if (!Q_strcmp(value->GetName(), "targetname"))
		{
			UTIL_PrecacheOther(value->GetString());
		}
		else if (!Q_strcmp(value->GetName(), "model"))
		{
			CBaseEntity::PrecacheModel(value->GetString());
		}
	}
}

void CMapaddSystem::ParseEntities(KeyValues* keyvalues)
{
	FOR_EACH_SUBKEY(keyvalues, classname)
	{
		if (!Q_strcmp(classname->GetName(), "int"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				if (hasTwoWords(value->GetString()))
				{
					intNames.addElement(value->GetName());
					intValues.addElement(random->RandomInt(Q_atoi(getFirstWord(value->GetString()).c_str()), Q_atoi(getSecondWord(value->GetString()).c_str())));
				}
				else
				{
					if (IsNumber(value->GetString()))
					{
						intNames.addElement(value->GetName());
						intValues.addElement(Q_atoi(value->GetString()));
					}
					else
					{
						ConMsg("Error: Int %s Value Is Not Int\n", value->GetName());
					}
				}
			}
			KeyValues* change = classname->FindKey("change_value");
			if (change)
			{
				FOR_EACH_VALUE(change, value)
				{
					for (int i = 0; i < intNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), intNames[i]))
						{
							if (!Q_strcmp(getFirstWord(value->GetString()).c_str(), "+"))
							{
								if (IsNumber(getSecondWord(value->GetString()).c_str()))
								{
									intValues[i] += Q_atoi(getSecondWord(value->GetString()).c_str());
									break;
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), intNames[j]))
										{
											intValues[i] += intValues[j];
										}
									}
								}
							}
							else if (!Q_strcmp(getFirstWord(value->GetString()).c_str(), "-"))
							{
								if (IsNumber(getSecondWord(value->GetString()).c_str()))
								{
									intValues[i] -= Q_atoi(getSecondWord(value->GetString()).c_str());
									break;
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), intNames[j]))
										{
											intValues[i] -= intValues[j];
										}
									}
								}
							}
							else if (!Q_strcmp(getFirstWord(value->GetString()).c_str(), "*"))
							{
								if (IsNumber(getSecondWord(value->GetString()).c_str()))
								{
									intValues[i] *= Q_atoi(getSecondWord(value->GetString()).c_str());
									break;
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), intNames[j]))
										{
											intValues[i] *= intValues[j];
										}
									}
								}
							}
							else if (!Q_strcmp(getFirstWord(value->GetString()).c_str(), "/"))
							{
								if (IsNumber(getSecondWord(value->GetString()).c_str()))
								{
									intValues[i] /= Q_atoi(getSecondWord(value->GetString()).c_str());
									break;
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), intNames[j]))
										{
											intValues[i] /= intValues[j];
										}
									}
								}
							}
							else
							{
								for (int j = 0; j < intNames.getSize(); j++)
								{
									if (!Q_strcmp(value->GetName(), intNames[i]))
									{
										if (!Q_strcmp(value->GetString(), intNames[j]))
										{
											intValues[i] = intValues[j];
											break;
										}
										else if (IsNumber(value->GetString()))
										{
											intValues[i] = Q_atoi(value->GetString());
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			KeyValues *OnHit = classname->FindKey("if");
			if (OnHit)
			{
				FOR_EACH_VALUE(OnHit, value)
				{
					for (int i = 0; i < intNames.getSize(); i++)
					{
						if (!Q_strcmp(getFirstWord(value->GetName()).c_str(), intNames[i]))
						{
							if (StartsWith(getSecondWord(value->GetName()).c_str(), "<"))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()) < intValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[j]))
										{
											if (intValues[i] < intValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), "<="))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()) <= intValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[j]))
										{
											if (intValues[i] <= intValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), ">"))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()) > intValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[j]))
										{
											if (intValues[i] >= intValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), ">="))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()) >= intValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[j]))
										{
											if (intValues[i] >= intValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), "=="))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()) == intValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[j]))
										{
											if (intValues[i] == intValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), "!="))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()) != intValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < intNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[j]))
										{
											if (intValues[i] != intValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
						}
					}
				}
			}
			KeyValues* deletevalue = classname->FindKey("delete");
			if (deletevalue)
			{
				FOR_EACH_VALUE(deletevalue, value)
				{
					for (int i = 0; i < intNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), "ByName"))
						{
							if (!Q_strcmp(value->GetString(), intNames[i]))
							{
								intNames.deleteElement(i);
								intValues.deleteElement(i);
								break;
							}
						}
						else if (!Q_strcmp(value->GetName(), "ByValue"))
						{
							if (Q_atoi(value->GetString()) == intValues[i])
							{
								intNames.deleteElement(i);
								intValues.deleteElement(i);
								break;
							}
						}
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "float"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				if (hasTwoWords(value->GetString()))
				{
					floatNames.addElement(value->GetName());
					floatValues.addElement(random->RandomFloat(Q_atof(getFirstWord(value->GetString()).c_str()), Q_atof(getSecondWord(value->GetString()).c_str())));
				}
				else
				{
					floatNames.addElement(value->GetName());
					floatValues.addElement(Q_atof(value->GetString()));
				}
			}
			KeyValues* change = classname->FindKey("change_value");
			if (change)
			{
				FOR_EACH_VALUE(change, value)
				{
					for (int i = 0; i < floatNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), floatNames[i]))
						{
							if (!Q_strcmp(getFirstWord(value->GetString()).c_str(), "+"))
							{
								if (IsNumber(getSecondWord(value->GetString()).c_str()))
								{
									floatValues[i] = floatValues[i] + Q_atof(getSecondWord(value->GetString()).c_str());
									break;
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), floatNames[j]))
										{
											floatValues[i] += floatValues[j];
										}
									}
								}
							}
							else if (!Q_strcmp(getFirstWord(value->GetString()).c_str(), "-"))
							{
								if (IsNumber(getSecondWord(value->GetString()).c_str()))
								{
									floatValues[i] -= Q_atof(getSecondWord(value->GetString()).c_str());
									break;
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), floatNames[j]))
										{
											floatValues[i] -= floatValues[j];
										}
									}
								}
							}
							else if (!Q_strcmp(getFirstWord(value->GetString()).c_str(), "*"))
							{
								if (IsNumber(getSecondWord(value->GetString()).c_str()))
								{
									floatValues[i] *= Q_atof(getSecondWord(value->GetString()).c_str());
									break;
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), floatNames[j]))
										{
											floatValues[i] *= floatValues[j];
										}
									}
								}
							}
							else if (!Q_strcmp(getFirstWord(value->GetString()).c_str(), "/"))
							{
								if (IsNumber(getSecondWord(value->GetString()).c_str()))
								{
									floatValues[i] /= Q_atof(getSecondWord(value->GetString()).c_str());
									break;
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), floatNames[j]))
										{
											floatValues[i] /= floatValues[j];
										}
									}
								}
							}
							else
							{
								for (int j = 0; j < floatNames.getSize(); j++)
								{
									if (!Q_strcmp(value->GetName(), floatNames[i]))
									{
										if (!Q_strcmp(value->GetString(), floatNames[j]))
										{
											floatValues[i] = floatValues[j];
											break;
										}
										else if (IsNumber(value->GetString()))
										{
											floatValues[i] = Q_atof(value->GetString());
											break;
										}
									}
								}
							}
						}
					}
				}
			}
			KeyValues *OnHit = classname->FindKey("if");
			if (OnHit)
			{
				FOR_EACH_VALUE(OnHit, value)
				{
					for (int i = 0; i < floatNames.getSize(); i++)
					{
						if (!Q_strcmp(getFirstWord(value->GetName()).c_str(), floatNames[i]))
						{
							if (StartsWith(getSecondWord(value->GetName()).c_str(), "<"))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()) < floatValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[j]))
										{
											if (floatValues[i] < floatValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), "<="))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()) <= floatValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[j]))
										{
											if (floatValues[i] <= floatValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), ">"))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()) > floatValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[j]))
										{
											if (floatValues[i] >= floatValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), ">="))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()) >= floatValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[j]))
										{
											if (floatValues[i] >= floatValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), "=="))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()) == floatValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[j]))
										{
											if (floatValues[i] == floatValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
							else if (StartsWith(getSecondWord(value->GetName()).c_str(), "!="))
							{
								if (IsNumber(getSecondWord(value->GetName()).c_str()))
								{
									if (Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()) != floatValues[i])
									{
										PARSELABEL
									}
								}
								else
								{
									for (int j = 0; j < floatNames.getSize(); j++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[j]))
										{
											if (floatValues[i] != floatValues[j])
											{
												PARSELABEL
											}
										}
									}
								}
							}
						}
					}
				}
			}
			KeyValues* deletevalue = classname->FindKey("delete");
			if (deletevalue)
			{
				FOR_EACH_VALUE(deletevalue, value)
				{
					for (int i = 0; i < floatNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), "ByName"))
						{
							if (!Q_strcmp(value->GetString(), floatNames[i]))
							{
								floatNames.deleteElement(i);
								floatValues.deleteElement(i);
								break;
							}
						}
						else if (!Q_strcmp(value->GetName(), "ByValue"))
						{
							if (Q_atof(value->GetString()) == floatValues[i])
							{
								floatNames.deleteElement(i);
								floatValues.deleteElement(i);
								break;
							}
						}
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "bool"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				if (!Q_strcmp(value->GetString(), "true"))
				{
					boolNames.addElement(value->GetName());
					boolValues.addElement(true);
				}
				else if (!Q_strcmp(value->GetString(), "false"))
				{
					boolNames.addElement(value->GetName());
					boolValues.addElement(false);
				}
			}
			KeyValues* change = classname->FindKey("change_value");
			if (change)
			{
				FOR_EACH_VALUE(change, value)
				{
					for (int i = 0; i < boolNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), boolNames[i]))
						{
							for (int j = 0; j < boolNames.getSize(); j++)
							{
								if (IsBool(value->GetString()))
								{
									if (!Q_strcmp(value->GetString(), "true"))
									{
										boolValues.changeElement(i, true);
										break;
									}
									else if (!Q_strcmp(value->GetString(), "false"))
									{
										boolValues.changeElement(i, false);
										break;
									}
								}
								else if (!Q_strcmp(value->GetString(), boolNames[j]))
								{
									boolValues[i] = boolValues[j];
								}
							}
						}
					}
				}
			}
			KeyValues* ontrue = classname->FindKey("ontrue");
			if (ontrue)
			{
				FOR_EACH_VALUE(ontrue, value)
				{
					for (int i = 0; i < boolNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), boolNames[i]))
						{
							if (boolValues[i] == true)
							{
								PARSELABEL
							}
						}
					}
				}
			}
			KeyValues* onfalse = classname->FindKey("onfalse");
			if (onfalse)
			{
				FOR_EACH_VALUE(onfalse, value)
				{
					for (int i = 0; i < boolNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), boolNames[i]))
						{
							if (boolValues[i] == false)
							{
								PARSELABEL
							}
						}
					}
				}
			}
			KeyValues* deletevalue = classname->FindKey("delete");
			if (deletevalue)
			{
				FOR_EACH_VALUE(deletevalue, value)
				{
					for (int i = 0; i < boolNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), "ByName"))
						{
							if (!Q_strcmp(value->GetString(), boolNames[i]))
							{
								boolNames.deleteElement(i);
								boolValues.deleteElement(i);
								break;
							}
						}
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "removeentity"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				CBaseEntity* ent = nullptr;
				if (!Q_strcmp(value->GetName(), "targetname"))
				{
					ent = gEntList.FindEntityByName(NULL, value->GetString());
				}
				else if (!Q_strcmp(value->GetName(), "model"))
				{
					ent = gEntList.FindEntityByModel(NULL, value->GetString());
				}
				else if (!Q_strcmp(value->GetName(), "origin"))
				{
					Vector ori;
					UTIL_StringToVector(ori.Base(), value->GetString());

					ent = FindEntityByOrigin(Vector(ori.x, ori.y, ori.z));
				}
				else if (!Q_strcmp(value->GetName(), "hammerid"))
				{
					ent = FindEntityByHammerId(value->GetInt());
				}
				if (ent)
					UTIL_Remove(ent);
			}
		}
		else if (!Q_strcmp(classname->GetName(), "player"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				CBasePlayer* pPlayer = UTIL_GetLocalPlayer();
				if (!Q_strcmp(value->GetName(), "origin"))
				{
					Vector origin;
					UTIL_StringToVector(origin.Base(), value->GetString());

					pPlayer->SetAbsOrigin(Vector(origin.x, origin.y, origin.z));
				}
				else if (!Q_strcmp(value->GetName(), "velocity"))
				{
					Vector velocity;
					UTIL_StringToVector(velocity.Base(), value->GetString());

					pPlayer->VelocityPunch(Vector(velocity.x, velocity.y, velocity.z));
				}
				else if (!Q_strcmp(value->GetName(), "absvelocity"))
				{
					Vector velocity;
					UTIL_StringToVector(velocity.Base(), value->GetString());

					pPlayer->SetAbsVelocity(Vector(velocity.x, velocity.y, velocity.z));
				}
				else if (!Q_strcmp(value->GetName(), "viewpunch"))
				{
					Vector punch;
					UTIL_StringToVector(punch.Base(), value->GetString());

					pPlayer->ViewPunch(QAngle(punch.x, punch.y, punch.z));
				}
				else if (!Q_strcmp(value->GetName(), "music"))
				{
					char play[FILENAME_MAX];
					Q_snprintf(play, sizeof(play), "playgamesound %s", value->GetString());
					engine->ClientCommand(pPlayer->edict(), play);
				}
				else if (!Q_strcmp(value->GetName(), "give"))
				{
					pPlayer->GiveNamedItem(value->GetString());
				}
				else if (!Q_strcmp(value->GetName(), "size"))
				{
					DYNAMICINT(engine->ClientCommand(pPlayer->edict(), "ent_fire !player setmodelscale %s", intValChar))
					else
					{
						engine->ClientCommand(pPlayer->edict(), "ent_fire !player setmodelscale %s", value->GetString());
					}
				}
				else if (!Q_strcmp(value->GetName(), "health"))
				{
					DYNAMICINT(engine->ClientCommand(pPlayer->edict(), "ent_fire !player sethealth %s", intValChar))
					else
					{
						engine->ClientCommand(pPlayer->edict(), "ent_fire !player sethealth %s", value->GetString());
					}
				}
				else if (!Q_strcmp(value->GetName(), "velocity_x"))
				{
					DYNAMICINT(pPlayer->SetAbsVelocity(Vector(Q_atof(intValChar), pPlayer->GetAbsVelocity().y, pPlayer->GetAbsVelocity().z)))
					else
					{
						pPlayer->SetAbsVelocity(Vector(value->GetFloat(), pPlayer->GetAbsVelocity().y, pPlayer->GetAbsVelocity().z));
					}
				}
				else if (!Q_strcmp(value->GetName(), "velocity_y"))
				{
					DYNAMICINT(pPlayer->SetAbsVelocity(Vector(pPlayer->GetAbsVelocity().x, Q_atof(intValChar), pPlayer->GetAbsVelocity().z)))
					else
					{
						pPlayer->SetAbsVelocity(Vector(pPlayer->GetAbsVelocity().x, value->GetFloat(), pPlayer->GetAbsVelocity().z));
					}
				}
				else if (!Q_strcmp(value->GetName(), "velocity_z"))
				{
					DYNAMICINT(pPlayer->SetAbsVelocity(Vector(UTIL_GetLocalPlayer()->GetAbsVelocity().x, pPlayer->GetAbsVelocity().y, Q_atof(intValChar))))
					else
					{
						pPlayer->SetAbsVelocity(Vector(pPlayer->GetAbsVelocity().x, pPlayer->GetAbsVelocity().y, value->GetFloat()));
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "bind"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "bind %s \"%s\"", value->GetName(), value->GetString());
			}
		}
		else if (!Q_strcmp(classname->GetName(), "alias"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "alias %s \"%s\"", value->GetName(), value->GetString());
			}
		}
		else if (!Q_strcmp(classname->GetName(), "solid"))
		{
			CBaseEntity* pEnt = CreateEntityByName("func_brush");
			if (pEnt)
			{
				FOR_EACH_VALUE(classname, value)
				{
					if (!Q_strcmp(value->GetName(), "targetname"))
					{
						pEnt->KeyValue("targetname", value->GetString());
					}
					else if (!Q_strcmp(value->GetName(), "origin"))
					{
						pEnt->KeyValue("origin", value->GetString());
					}
					else if (!Q_strcmp(value->GetName(), "angles"))
					{
						pEnt->KeyValue("angles", value->GetString());
					}
					else if (!Q_strcmp(value->GetName(), "mins"))
					{
						pEnt->KeyValue("mins", value->GetString());
					}
					else if (!Q_strcmp(value->GetName(), "maxs"))
					{
						pEnt->KeyValue("maxs", value->GetString());
					}
				}
			}
			pEnt->KeyValue("solid", "3");
		}
		else if (!Q_strcmp(classname->GetName(), "hud"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				if (!Q_strcmp(value->GetName(), "message"))
				{
					DYNAMICINT(ClientPrint(UTIL_GetLocalPlayer(), HUD_PRINTCENTER, intValChar))
					else
					{
						ClientPrint(UTIL_GetLocalPlayer(), HUD_PRINTCENTER, value->GetString());
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "console"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				if (!Q_strcmp(value->GetName(), "command"))
				{
					engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), value->GetString());
				}
				else if (!Q_strcmp(value->GetName(), "message"))
				{
					DYNAMICINT(ConMsg("%s\n", intValChar))
					else
					{
						ConMsg("%s\n", value->GetString());
					}
				}
				else if (!Q_strcmp(value->GetName(), "DevMsg"))
				{
					DYNAMICINT(DevMsg("%s\n", intValChar))
					else
					{
						DevMsg("%s\n", value->GetString());
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "console_command"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				DYNAMICINT(engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "%s %s", value->GetName(), intValChar))
				else
				{
					engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "%s %s", value->GetName(), value->GetString());
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "ent_fire"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				DYNAMICINT(engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "ent_fire %s %s", value->GetName(), intValChar))
				engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "ent_fire %s %s", value->GetName(), value->GetString());
			}
			KeyValues* addoutput = classname->FindKey("addoutput");
			if (addoutput)
			{
				FOR_EACH_VALUE(addoutput, value)
				{
					engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "ent_fire %s addoutput \"%s\"", value->GetName(), value->GetString());
				}
			}
		}
		else
		{
			CBaseEntity* ent = CreateEntityByName(classname->GetName());
			if (ent)
			{
				FOR_EACH_VALUE(classname, value)
				{
					if (!Q_strcmp(value->GetName(), "targetname"))
					{
						ent->KeyValue("targetname", value->GetString());
					}
					else if (!Q_strcmp(value->GetName(), "origin"))
					{
						CBaseEntity* pEnt2 = gEntList.FindEntityByName(NULL, value->GetString());
						if (pEnt2)
						{
							ent->SetAbsOrigin(pEnt2->GetAbsOrigin());
						}
						else
						{
							ent->KeyValue("origin", value->GetString());
						}
					}
					else  if (!Q_strcmp(value->GetName(), "angles"))
					{
						ent->KeyValue("angles", value->GetString());
					}
					else if (!Q_strcmp(value->GetName(), "velocity"))
					{
						Vector velocity;
						UTIL_StringToVector(velocity.Base(), value->GetString());

						ent->SetAbsVelocity(Vector(velocity.x, velocity.y, velocity.z));
					}
				}
				KeyValues* kvsec = classname->FindKey("keyvalues");
				if (kvsec)
				{
					FOR_EACH_VALUE(kvsec, value)
					{
						DYNAMICINT(ent->KeyValue(value->GetName(), intValChar))
						else
						{
							ent->KeyValue(value->GetName(), value->GetString());
						}
					}
				}
			}
			else
			{
				ConMsg("Mapadd: Error spawning entity: \"%s\"\n", classname->GetName());
				return;
			}
			ent->Precache();
			DispatchSpawn(ent);
			ent->Activate();
		}
	}
}

void CMapaddSystem::OnRestore()
{
	return;
}

void CMapaddSystem::LevelShutdownPostEntity()
{
	if (mapaddFile)
	{
		mapaddFile->deleteThis();
		mapaddFile = nullptr;
	}
}

CBaseEntity *FindEntityByOrigin(const Vector& origin)
{
	CBaseEntity* pEnt = gEntList.FirstEnt();
	while (pEnt)
	{
		if (pEnt->GetAbsOrigin() == origin)
		{
			return pEnt;
		}
		pEnt = gEntList.NextEnt(pEnt);
	}
	return nullptr;
}

CBaseEntity *FindEntityByHammerId(int id)
{
	CBaseEntity* pEnt = gEntList.FirstEnt();
	while (pEnt)
	{
		if (pEnt->m_iHammerID == id)
		{
			return pEnt;
		}

		pEnt = gEntList.NextEnt(pEnt);
	}
	return nullptr;
}