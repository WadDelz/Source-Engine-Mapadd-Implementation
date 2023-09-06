#include "igamesystem.h"
#include "KeyValues.h"
#include "dynamic_array.h"

class CMapaddSystem : public CAutoGameSystem
{
public:
	CMapaddSystem(const char* name);

	//startup functions
	virtual void LevelInitPostEntity();
	virtual void LevelShutdownPostEntity();
	virtual void OnRestore();

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
};
extern CMapaddSystem* GetMapaddSystem();

CBaseEntity* FindEntityByOrigin(const Vector& origin);
CBaseEntity *FindEntityByHammerId(int id);