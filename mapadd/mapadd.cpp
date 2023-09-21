#include "cbase.h"
#include "filesystem.h"
#include "mapadd.h"
#include "includes.h"

ConVar mapadd_disableload("mapadd_disableload", "0");
ConVar mapadd_secondary_script("mapadd_secondary_script", "mapadd/mods/NULL.txt");
ConVar mapadd_debug("mapadd_debug", "0");

CMapaddSystem::CMapaddSystem(const char* name) : CAutoGameSystem(name)
{
}

CMapaddSystem g_mapaddSystem("mapadd");

CMapaddSystem *GetMapaddSystem()
{
	return &g_mapaddSystem;
}

//more messages coming soon 
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
		MapaddDevMsg("Subkey Found: %s\n", classname->GetName());
		if (!Q_strcmp(classname->GetName(), "int"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				if (hasTwoWords(value->GetString()) && IsInt(getFirstWord(value->GetString()).c_str()) && IsInt(getSecondWord(value->GetString()).c_str()))
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
					else if (StartsWith(value->GetString(), "b"))
					{
						intNames.addElement(value->GetName());
						intValues.addElement(binaryToInt(Q_atoi(removeNonIntCharacters(value->GetString()).c_str())));
					}
					else if (hasTwoWords(value->GetString()) && !IsInt(getFirstWord(value->GetString()).c_str()) && !IsInt(getSecondWord(value->GetString()).c_str()))
					{
						CBaseEntity* pEnt = gEntList.FindEntityByName(NULL, getFirstWord(value->GetString()).c_str());
						if (pEnt)
						{
							if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), "health"))
							{
								intNames.addElement(value->GetName());
								intValues.addElement(pEnt->GetHealth());
							}
						}
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
											break;
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
											break;
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
											break;
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
											break;
										}
									}
								}
							}
							else if (hasTwoWords(value->GetString()) && !IsInt(getFirstWord(value->GetString()).c_str()) && !IsInt(getSecondWord(value->GetString()).c_str()))
							{
								CBaseEntity* pEnt = gEntList.FindEntityByName(NULL, getFirstWord(value->GetString()).c_str());
								if (pEnt)
								{
									if (!Q_strcmp(getSecondWord(value->GetString()).c_str(), "health"))
									{
										intValues[i] = pEnt->GetHealth();
										break;
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
				if (hasTwoWords(value->GetString()) && IsNumber(getFirstWord(value->GetString()).c_str()) && IsNumber(getSecondWord(value->GetString()).c_str()))
				{
					floatNames.addElement(value->GetName());
					floatValues.addElement(random->RandomFloat(Q_atof(getFirstWord(value->GetString()).c_str()), Q_atof(getSecondWord(value->GetString()).c_str())));
				}
				else
				{
					if (IsNumber(value->GetString()))
					{
						floatNames.addElement(value->GetName());
						floatValues.addElement(Q_atof(value->GetString()));
					}
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
											break;
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
											break;
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
											break;
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
											break;
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
									break;
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
		else if (!Q_strcmp(classname->GetName(), "string"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				stringNames.addElement(value->GetName());
				stringValues.addElement(value->GetString());
			}
			KeyValues* change = classname->FindKey("change_value");
			if (change)
			{
				FOR_EACH_VALUE(change, value)
				{
					for (int i = 0; i < stringNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), stringNames[i]))
						{
							stringValues[i] = value->GetString();
							break;				
						}
					}
				}
			}
			KeyValues* deletevalue = classname->FindKey("delete");
			if (deletevalue)
			{
				FOR_EACH_VALUE(deletevalue, value)
				{
					for (int i = 0; i < stringNames.getSize(); i++)
					{
						if (!Q_strcmp(value->GetName(), "ByName"))
						{
							if (!Q_strcmp(value->GetString(), stringNames[i]))
							{
								stringNames.deleteElement(i);
								stringValues.deleteElement(i);
								break;
							}
						}
						else if (!Q_strcmp(value->GetName(), "ByValue"))
						{
							if (!Q_strcmp(value->GetString(), stringValues[i]))
							{
								stringNames.deleteElement(i);
								stringValues.deleteElement(i);
								break;
							}
						}
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "convar"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				ConVar *var = cvar->FindVar(value->GetName());
				if (!var)
				{
					var = new ConVar(value->GetName(), value->GetString());
				}
			}
			KeyValues *Ifcvar = classname->FindKey("if");
			if (Ifcvar)
			{
				FOR_EACH_VALUE(Ifcvar, value)
				{
					ConVar *var = cvar->FindVar(getFirstWord(value->GetName()).c_str());
					if (var)
					{
						if (StartsWith(getSecondWord(value->GetName()).c_str(), "<"))
						{
							if (IsInt(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetInt() < Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else if (IsNumber(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetFloat() < Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else
							{
								bool IsFloat = true;
								for (int i = 0; i < intNames.getSize(); i++)
								{
									if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[i]))
									{
										if (var->GetInt() < intValues[i])
										{
											PARSELABEL
												IsFloat = false;
										}
									}
								}
								if (IsFloat)
								{
									for (int i = 0; i < floatNames.getSize(); i++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[i]))
										{
											if (var->GetFloat() < floatValues[i])
											{
												PARSELABEL
											}
										}
									}
								}
							}
						}
						else if (StartsWith(getSecondWord(value->GetName()).c_str(), "<="))
						{
							if (IsInt(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetInt() <= Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else if (IsNumber(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetFloat() <= Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else
							{
								bool IsFloat = true;
								for (int i = 0; i < intNames.getSize(); i++)
								{
									if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[i]))
									{
										if (var->GetInt() <= intValues[i])
										{
											PARSELABEL
												IsFloat = false;
										}
									}
								}
								if (IsFloat)
								{
									for (int i = 0; i < floatNames.getSize(); i++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[i]))
										{
											if (var->GetFloat() <= floatValues[i])
											{
												PARSELABEL
											}
										}
									}
								}
							}
						}
						else if (StartsWith(getSecondWord(value->GetName()).c_str(), ">"))
						{
							if (IsInt(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetInt() > Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else if (IsNumber(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetFloat() > Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else
							{
								bool IsFloat = true;
								for (int i = 0; i < intNames.getSize(); i++)
								{
									if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[i]))
									{
										if (var->GetInt() > intValues[i])
										{
											PARSELABEL
												IsFloat = false;
										}
									}
								}
								if (IsFloat)
								{
									for (int i = 0; i < floatNames.getSize(); i++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[i]))
										{
											if (var->GetFloat() > floatValues[i])
											{
												PARSELABEL
											}
										}
									}
								}
							}
						}
						else if (StartsWith(getSecondWord(value->GetName()).c_str(), ">="))
						{
							if (IsInt(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetInt() >= Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else if (IsNumber(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetFloat() >= Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else
							{
								bool IsFloat = true;
								for (int i = 0; i < intNames.getSize(); i++)
								{
									if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[i]))
									{
										if (var->GetInt() >= intValues[i])
										{
											PARSELABEL
												IsFloat = false;
										}
									}
								}
								if (IsFloat)
								{
									for (int i = 0; i < floatNames.getSize(); i++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[i]))
										{
											if (var->GetFloat() >= floatValues[i])
											{
												PARSELABEL
											}
										}
									}
								}
							}
						}
						else if (StartsWith(getSecondWord(value->GetName()).c_str(), "=="))
						{
							if (IsInt(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetInt() == Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else if (IsNumber(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetFloat() == Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else
							{
								bool IsFloat = true;
								for (int i = 0; i < intNames.getSize(); i++)
								{
									if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[i]))
									{
										if (var->GetInt() == intValues[i])
										{
											PARSELABEL
												IsFloat = false;
										}
									}
								}
								if (IsFloat)
								{
									for (int i = 0; i < floatNames.getSize(); i++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[i]))
										{
											if (var->GetFloat() == floatValues[i])
											{
												PARSELABEL
											}
										}
									}
								}
							}
						}
						else if (StartsWith(getSecondWord(value->GetName()).c_str(), "!="))
						{
							if (IsInt(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetInt() != Q_atoi(removeNonIntCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else if (IsNumber(getSecondWord(value->GetName()).c_str()))
							{
								if (var->GetFloat() != Q_atof(removeNonFloatCharacters(getSecondWord(value->GetName()).c_str()).c_str()))
								{
									PARSELABEL
								}
							}
							else
							{
								bool IsFloat = true;
								for (int i = 0; i < intNames.getSize(); i++)
								{
									if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), intNames[i]))
									{
										if (var->GetInt() != intValues[i])
										{
											PARSELABEL
												IsFloat = false;
										}
									}
								}
								if (IsFloat)
								{
									for (int i = 0; i < floatNames.getSize(); i++)
									{
										if (!Q_strcmp(removeFunctionCharacters(getSecondWord(value->GetName()).c_str()).c_str(), floatNames[i]))
										{
											if (var->GetFloat() != floatValues[i])
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
			KeyValues* toInt = classname->FindKey("toint");
			if (toInt)
			{
				FOR_EACH_VALUE(toInt, value)
				{
					ConVar *find = cvar->FindVar(value->GetName());
					if (find)
					{
						for (int i = 0; i < intNames.getSize(); i++)
						{
							if (!Q_strcmp(value->GetString(), intNames[i]))
							{
								intValues[i] = find->GetInt();
								break;
							}
						}
					}
				}
			}
			KeyValues* tofloat = classname->FindKey("tofloat");
			if (tofloat)
			{
				FOR_EACH_VALUE(tofloat, value)
				{
					ConVar *find = cvar->FindVar(value->GetName());
					if (find)
					{
						for (int i = 0; i < floatNames.getSize(); i++)
						{
							if (!Q_strcmp(value->GetString(), floatNames[i]))
							{
								floatValues[i] = find->GetFloat();
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
					DYNAMICINT(ent = gEntList.FindEntityByName(NULL, absstr))
					{
						ent = gEntList.FindEntityByName(NULL, str_nonfunc);
					}
				}
				else if (!Q_strcmp(value->GetName(), "model"))
				{
					DYNAMICINT(ent = gEntList.FindEntityByModel(NULL, absstr))
					{
						ent = gEntList.FindEntityByModel(NULL, str_nonfunc);
					}
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
					DYNAMICINT(Vector origin;
					UTIL_StringToVector(origin.Base(), absstr);

					pPlayer->SetAbsOrigin(Vector(origin.x, origin.y, origin.z)))
					{
						Vector origin;
						UTIL_StringToVector(origin.Base(), str_nonfunc);

						pPlayer->SetAbsOrigin(Vector(origin.x, origin.y, origin.z));
					}
				}
				else if (!Q_strcmp(value->GetName(), "velocity"))
				{
					DYNAMICINT(Vector velocity;
					UTIL_StringToVector(velocity.Base(), absstr);

					pPlayer->VelocityPunch(Vector(velocity.x, velocity.y, velocity.z)))
					{
						Vector velocity;
						UTIL_StringToVector(velocity.Base(), str_nonfunc);

						pPlayer->VelocityPunch(Vector(velocity.x, velocity.y, velocity.z));
					}
				}
				else if (!Q_strcmp(value->GetName(), "absvelocity"))
				{
					DYNAMICINT(Vector velocity;
					UTIL_StringToVector(velocity.Base(), absstr);

					pPlayer->SetAbsVelocity(Vector(velocity.x, velocity.y, velocity.z)))
					{
						Vector velocity;
						UTIL_StringToVector(velocity.Base(), str_nonfunc);

						pPlayer->SetAbsVelocity(Vector(velocity.x, velocity.y, velocity.z));
					}
				}
				else if (!Q_strcmp(value->GetName(), "viewpunch"))
				{
					DYNAMICINT(Vector punch;
					UTIL_StringToVector(punch.Base(), absstr);

					pPlayer->ViewPunch(QAngle(punch.x, punch.y, punch.z)))
					{
						Vector punch;
						UTIL_StringToVector(punch.Base(), str_nonfunc);

						pPlayer->ViewPunch(QAngle(punch.x, punch.y, punch.z));
					}
				}
				else if (!Q_strcmp(value->GetName(), "music"))
				{
					DYNAMICINT(char play[FILENAME_MAX];
					Q_snprintf(play, sizeof(play), "playgamesound %s", absstr);
					engine->ClientCommand(pPlayer->edict(), play);)
					{
						char play[FILENAME_MAX];
						Q_snprintf(play, sizeof(play), "playgamesound %s", str_nonfunc);
						engine->ClientCommand(pPlayer->edict(), play);
					}
				}
				else if (!Q_strcmp(value->GetName(), "give"))
				{
					DYNAMICINT(pPlayer->GiveNamedItem(absstr))
					pPlayer->GiveNamedItem(str_nonfunc);
				}
				else if (!Q_strcmp(value->GetName(), "size"))
				{
					DYNAMICINT(engine->ClientCommand(pPlayer->edict(), "ent_fire !player setmodelscale %s", absstr))
					{
						engine->ClientCommand(pPlayer->edict(), "ent_fire !player setmodelscale %s", str_nonfunc);
					}
				}
				else if (!Q_strcmp(value->GetName(), "health"))
				{
					DYNAMICINT(engine->ClientCommand(pPlayer->edict(), "ent_fire !player sethealth %s", absstr))
					{
						engine->ClientCommand(pPlayer->edict(), "ent_fire !player sethealth %s", str_nonfunc);
					}
				}
				else if (!Q_strcmp(value->GetName(), "velocity_x"))
				{
					DYNAMICINT(pPlayer->SetAbsVelocity(Vector(Q_atof(absstr), pPlayer->GetAbsVelocity().y, pPlayer->GetAbsVelocity().z)))
					{
						pPlayer->SetAbsVelocity(Vector(Q_atof(str_nonfunc), pPlayer->GetAbsVelocity().y, pPlayer->GetAbsVelocity().z));
					}
				}
				else if (!Q_strcmp(value->GetName(), "velocity_y"))
				{
					DYNAMICINT(pPlayer->SetAbsVelocity(Vector(pPlayer->GetAbsVelocity().x, Q_atof(absstr), pPlayer->GetAbsVelocity().z)))
					{
						pPlayer->SetAbsVelocity(Vector(pPlayer->GetAbsVelocity().x, Q_atof(str_nonfunc), pPlayer->GetAbsVelocity().z));
					}
				}
				else if (!Q_strcmp(value->GetName(), "velocity_z"))
				{
					DYNAMICINT(pPlayer->SetAbsVelocity(Vector(UTIL_GetLocalPlayer()->GetAbsVelocity().x, pPlayer->GetAbsVelocity().y, Q_atof(absstr))))
					{
						pPlayer->SetAbsVelocity(Vector(pPlayer->GetAbsVelocity().x, pPlayer->GetAbsVelocity().y, Q_atof(str_nonfunc)));
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "bind"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				DYNAMICINT(engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "bind %s \"%s\"", value->GetName(), absstr))
				{
					engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "bind %s \"%s\"", value->GetName(), str_nonfunc);
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "alias"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				DYNAMICINT(engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "alias %s \"%s\"", value->GetName(), absstr))
				{
					engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "alias %s \"%s\"", value->GetName(), str_nonfunc);
				}
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
						DYNAMICINT(pEnt->KeyValue("targetname", absstr))
						{
							pEnt->KeyValue("targetname", str_nonfunc);
						}
					}
					else if (!Q_strcmp(value->GetName(), "origin"))
					{
						DYNAMICINT(pEnt->KeyValue("origin", absstr))
						{
							pEnt->KeyValue("origin", str_nonfunc);
						}
					}
					else if (!Q_strcmp(value->GetName(), "angles"))
					{
						DYNAMICINT(pEnt->KeyValue("angles", absstr))
						{
							pEnt->KeyValue("angles", str_nonfunc);
						}
					}
					else if (!Q_strcmp(value->GetName(), "mins"))
					{
						DYNAMICINT(pEnt->KeyValue("mins", absstr))
						{
							pEnt->KeyValue("mins", str_nonfunc);
						}
					}
					else if (!Q_strcmp(value->GetName(), "maxs"))
					{
						DYNAMICINT(pEnt->KeyValue("maxs", absstr))
						{
							pEnt->KeyValue("maxs", str_nonfunc);
						}
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
					DYNAMICINT(ClientPrint(UTIL_GetLocalPlayer(), HUD_PRINTCENTER, absstr))
					{
						ClientPrint(UTIL_GetLocalPlayer(), HUD_PRINTCENTER, str_nonfunc);
					}
				}
				else if (!Q_strcmp(value->GetName(), "notify"))
				{
					DYNAMICINT(ClientPrint(UTIL_GetLocalPlayer(), HUD_PRINTTALK, absstr))
					{
						ClientPrint(UTIL_GetLocalPlayer(), HUD_PRINTTALK, str_nonfunc);
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
					DYNAMICINT(engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), absstr))
					{
						engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), str_nonfunc);
					}
				}
				else if (!Q_strcmp(value->GetName(), "message"))
				{
					DYNAMICINT(ConMsg("%s\n", absstr))
					{
						ConMsg("%s\n", str_nonfunc);
					}
				}
				else if (!Q_strcmp(value->GetName(), "DevMsg"))
				{
					DYNAMICINT(DevMsg("%s\n", absstr))
					{
						DevMsg("%s\n", str_nonfunc);
					}
				}
			}
		}
		else if (!Q_strcmp(classname->GetName(), "ent_fire"))
		{
			FOR_EACH_VALUE(classname, value)
			{
				DYNAMICINT(engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "ent_fire %s %s", value->GetName(), absstr))
				{
					engine->ClientCommand(UTIL_GetLocalPlayer()->edict(), "ent_fire %s %s", value->GetName(), str_nonfunc);
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
						DYNAMICINT(ent->KeyValue("targetname", absstr))
						{
							ent->KeyValue("targetname", str_nonfunc);
						}
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
							DYNAMICINT(ent->KeyValue("origin", absstr))
							{
								ent->KeyValue("origin", str_nonfunc);
							}
						}
					}
					else  if (!Q_strcmp(value->GetName(), "angles"))
					{
						DYNAMICINT(ent->KeyValue("angles", absstr))
						{
							ent->KeyValue("angles", str_nonfunc);
						}
					}
					else if (!Q_strcmp(value->GetName(), "velocity"))
					{
						DYNAMICINT(Vector velocity;
						UTIL_StringToVector(velocity.Base(), absstr);
						ent->SetAbsVelocity(Vector(velocity.x, velocity.y, velocity.z))
						)
						{
							Vector velocity;
							UTIL_StringToVector(velocity.Base(), str_nonfunc);

							ent->SetAbsVelocity(Vector(velocity.x, velocity.y, velocity.z));
						}
					}
				}
				KeyValues* kvsec = classname->FindKey("keyvalues");
				if (kvsec)
				{
					FOR_EACH_VALUE(kvsec, value)
					{
						DYNAMICINT(ent->KeyValue(value->GetName(), absstr))
						{
							ent->KeyValue(value->GetName(), str_nonfunc);
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

void MapaddDevMsg(const tchar* msg, ...)
{
	if (mapadd_debug.GetBool())
	{
		ConColorMsg(Color(255, 100, 0, 125), msg);
	}
}
