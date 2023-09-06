#include "cbase.h"
#include "mapadd.h"

class CLogicLabel : public CBaseEntity
{
public:
	DECLARE_CLASS(CLogicLabel, CBaseEntity);
	DECLARE_DATADESC();

	CLogicLabel() {
		wait = 0.1f;
	}

	void Activate() {
		BaseClass::Activate();
		SetNextThink(gpGlobals->curtime + wait);
	}

	void Think() {
		CMapaddSystem* mapadd = GetMapaddSystem();
		if (mapadd)
		{ 
			char LabelName[FILENAME_MAX];
			Q_snprintf(LabelName, sizeof(LabelName), "entities:%s", label);
			if (LabelName)
			{
				KeyValues* labelabs = mapadd->mapaddFile->FindKey(LabelName);
				if (labelabs)
				{
					mapadd->ParseEntities(labelabs);
					onExecute.FireOutput(this, 0);
				}
			}
		}
	}

	void ExecuteLabel(inputdata_t& inputdata) {
		CMapaddSystem* mapadd = GetMapaddSystem();
		if (mapadd)
		{
			char LabelName[FILENAME_MAX];
			Q_snprintf(LabelName, sizeof(LabelName), "entities:%s", inputdata.value);
			if (LabelName)
			{
				KeyValues* labelabs = mapadd->mapaddFile->FindKey(LabelName);
				if (labelabs)
				{
					mapadd->ParseEntities(labelabs);
					onExecute.FireOutput(this, 0);
				}
			}
		}
	}

private: 
	string_t label;
	float wait;
	COutputEvent onExecute;
};

LINK_ENTITY_TO_CLASS(logic_label, CLogicLabel);

BEGIN_DATADESC(CLogicLabel)
DEFINE_KEYFIELD(label, FIELD_STRING, "label"),
DEFINE_KEYFIELD(wait, FIELD_FLOAT, "wait"),
DEFINE_OUTPUT(onExecute, "onexecute"),
DEFINE_INPUTFUNC(FIELD_STRING, "execute", ExecuteLabel),
END_DATADESC();

class CLabelLoop : public CBaseEntity
{
public:
	DECLARE_CLASS(CLabelLoop, CBaseEntity);
	DECLARE_DATADESC();

	void Activate() {
		BaseClass::Activate();
		SetNextThink(gpGlobals->curtime + 0.1f);
	}

	void Think() {
		CMapaddSystem* mapadd = GetMapaddSystem();
		if (mapadd)
		{
			if (enabled)
			{
				char LabelName[FILENAME_MAX];
				Q_snprintf(LabelName, sizeof(LabelName), "entities:%s", label);
				if (LabelName)
				{
					KeyValues* labelabs = mapadd->mapaddFile->FindKey(LabelName);
					if (labelabs)
						mapadd->ParseEntities(labelabs);
				}
			}
		}
		SetNextThink(gpGlobals->curtime + delay);
	}

	void inputEnable(inputdata_t& inputdata) {
		enabled = true;
	}

	void inputDisable(inputdata_t& inputdata) {
		enabled = false;
	}

private:
	string_t label;
	bool enabled;
	float delay;
};

LINK_ENTITY_TO_CLASS(label_loop, CLabelLoop);

BEGIN_DATADESC(CLabelLoop)
DEFINE_KEYFIELD(label, FIELD_STRING, "label"),
DEFINE_KEYFIELD(enabled, FIELD_BOOLEAN, "enabled"),
DEFINE_KEYFIELD(delay, FIELD_FLOAT, "delay"),
DEFINE_INPUTFUNC(FIELD_VOID, "enable", inputEnable),
DEFINE_INPUTFUNC(FIELD_VOID, "disable", inputDisable),
END_DATADESC();