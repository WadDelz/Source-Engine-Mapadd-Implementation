#include "igamesystem.h"
#include "KeyValues.h"
#include "dynamic_array.h"

extern ConVar mapadd_disableload;
extern ConVar mapadd_secondary_script;
extern ConVar mapadd_debug;

class CMapaddSystem : public CAutoGameSystem
{
public:
	CMapaddSystem(const char* name);

	//startup functions
	virtual void LevelInitPostEntity(void);
	virtual void LevelShutdownPostEntity(void);
	virtual void OnRestore(void);

	//entity and section functions
	virtual void ParseMapaddFile(const char* filename);
	virtual void PrecacheSection(KeyValues* keyvalues);
	virtual void ParseEntities(KeyValues* keyvalues);

	KeyValues *mapaddFile; 

private:
	DynamicArray<const char*> intNames;
	DynamicArray<int> intValues;

	DynamicArray<const char*> floatNames;
	DynamicArray<float> floatValues;

	DynamicArray<const char*> boolNames;
	DynamicArray<bool> boolValues;

	DynamicArray<const char*> stringNames;
	DynamicArray<const char*> stringValues;
};
extern CMapaddSystem* GetMapaddSystem();

CBaseEntity* FindEntityByOrigin(const Vector& origin);
CBaseEntity *FindEntityByHammerId(int id);

void MapaddDevMsg(const tchar* msg, ...);
