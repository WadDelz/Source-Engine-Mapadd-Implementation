#include "cbase.h"

//entity origin checker
class CEntityOriginChecker : public CBaseEntity
{
public:
	DECLARE_CLASS(CEntityOriginChecker, CBaseEntity);
	DECLARE_DATADESC();

	CEntityOriginChecker();

	void Activate();
	void Think();
	void InputEnable(inputdata_t& input);
	void InputDisable(inputdata_t& input);

private:
	//functions
	bool m_bEnabled;
	string_t m_sEntity;
	int m_vecX;
	int m_vecY;
	int m_vecZ;
	COutputEvent OnOriginMatch;
	COutputEvent OnOriginLessThan;
	COutputEvent OnOriginMoreThan;
};

LINK_ENTITY_TO_CLASS(entity_origin_checker, CEntityOriginChecker);

BEGIN_DATADESC(CEntityOriginChecker)
DEFINE_KEYFIELD(m_bEnabled, FIELD_BOOLEAN, "enabled"),
DEFINE_KEYFIELD(m_sEntity, FIELD_STRING, "entity"),
DEFINE_KEYFIELD(m_vecX, FIELD_INTEGER, "OriginX"),
DEFINE_KEYFIELD(m_vecY, FIELD_INTEGER, "OriginY"),
DEFINE_KEYFIELD(m_vecZ, FIELD_INTEGER, "OriginZ"),
DEFINE_OUTPUT(OnOriginMatch, "OnOriginMatch"),
DEFINE_OUTPUT(OnOriginLessThan, "OnOriginLessThan"),
DEFINE_OUTPUT(OnOriginMoreThan, "OnOriginMoreThan"),
DEFINE_INPUTFUNC(FIELD_VOID, "enable", InputEnable),
DEFINE_INPUTFUNC(FIELD_VOID, "disable", InputDisable),
END_DATADESC()

CEntityOriginChecker::CEntityOriginChecker()
{
	m_bEnabled = true; 
}

void CEntityOriginChecker::Activate()
{
	BaseClass::Activate();
	SetNextThink(gpGlobals->curtime);
}

void CEntityOriginChecker::Think()
{
	if (m_bEnabled)
	{
		CBaseEntity* pEnt = gEntList.FindEntityByName(NULL, m_sEntity);
		if (!Q_strcmp(m_sEntity.ToCStr(), "player"))
			pEnt = dynamic_cast<CBaseEntity*>(UTIL_GetLocalPlayer());

		if (pEnt)
		{
			if (m_vecX != 0)
			{
				if (pEnt->GetAbsOrigin().x == m_vecX)
				{
					OnOriginMatch.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsOrigin().x < m_vecX)
				{
					OnOriginLessThan.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsOrigin().x > m_vecX)
				{
					OnOriginMoreThan.FireOutput(this, 0);
				}
			}
			else if (m_vecY != 0)
			{
				if (pEnt->GetAbsOrigin().y == m_vecY)
				{
					OnOriginMatch.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsOrigin().y < m_vecY)
				{
					OnOriginLessThan.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsOrigin().y > m_vecY)
				{
					OnOriginMoreThan.FireOutput(this, 0);
				}
			}
			else if (m_vecZ != 0)
			{
				if (pEnt->GetAbsOrigin().z == m_vecZ)
				{
					OnOriginMatch.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsOrigin().z < m_vecZ)
				{
					OnOriginLessThan.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsOrigin().z > m_vecZ)
				{
					OnOriginMoreThan.FireOutput(this, 0);
				}
			}
		}
	}
	SetNextThink(gpGlobals->curtime);
}

void CEntityOriginChecker::InputDisable(inputdata_t& input)
{
	m_bEnabled = false;
}

void CEntityOriginChecker::InputEnable(inputdata_t& input)
{
	m_bEnabled = true;
}



class CEntityVelocityChecker : public CBaseEntity
{
public:
	DECLARE_CLASS(CEntityVelocityChecker, CBaseEntity);
	DECLARE_DATADESC();

	CEntityVelocityChecker();

	void Activate();
	void Think();
	void InputEnable(inputdata_t& input);
	void InputDisable(inputdata_t& input);

private:
	//functions
	bool m_bEnabled;
	string_t m_sEntity;
	int m_vecX;
	int m_vecY;
	int m_vecZ;
	COutputEvent OnVelocityMatch;
	COutputEvent OnVelocityLessThan;
	COutputEvent OnVelocityMoreThan;
};

LINK_ENTITY_TO_CLASS(entity_velocity_checker, CEntityVelocityChecker);

BEGIN_DATADESC(CEntityVelocityChecker)
DEFINE_KEYFIELD(m_bEnabled, FIELD_BOOLEAN, "enabled"),
DEFINE_KEYFIELD(m_sEntity, FIELD_STRING, "entity"),
DEFINE_KEYFIELD(m_vecX, FIELD_INTEGER, "VelocityX"),
DEFINE_KEYFIELD(m_vecY, FIELD_INTEGER, "VelocityY"),
DEFINE_KEYFIELD(m_vecZ, FIELD_INTEGER, "VelocityZ"),
DEFINE_OUTPUT(OnVelocityMatch, "OnVelocityMatch"),
DEFINE_OUTPUT(OnVelocityLessThan, "OnVelocityLessThan"),
DEFINE_OUTPUT(OnVelocityMoreThan, "OnVelocityMoreThan"),
DEFINE_INPUTFUNC(FIELD_VOID, "enable", InputEnable),
DEFINE_INPUTFUNC(FIELD_VOID, "disable", InputDisable),
END_DATADESC()

CEntityVelocityChecker::CEntityVelocityChecker()
{
	m_bEnabled = true;

}

void CEntityVelocityChecker::Activate()
{
	BaseClass::Activate();
	SetNextThink(gpGlobals->curtime);
}

void CEntityVelocityChecker::Think()
{
	if (m_bEnabled)
	{
		CBaseEntity* pEnt = gEntList.FindEntityByName(NULL, m_sEntity);
		if (!Q_strcmp(m_sEntity.ToCStr(), "player"))
			pEnt = dynamic_cast<CBaseEntity*>(UTIL_GetLocalPlayer());

		if (pEnt)
		{
			if (m_vecX != 0)
			{
				if (pEnt->GetAbsVelocity().x == m_vecX)
				{
					OnVelocityMatch.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsVelocity().x < m_vecX)
				{
					OnVelocityLessThan.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsVelocity().x > m_vecX)
				{
					OnVelocityMoreThan.FireOutput(this, 0);
				}
			}
			else if (m_vecY != 0)
			{
				if (pEnt->GetAbsVelocity().y == m_vecY)
				{
					OnVelocityMatch.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsVelocity().y < m_vecY)
				{
					OnVelocityLessThan.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsVelocity().y > m_vecY)
				{
					OnVelocityMoreThan.FireOutput(this, 0);
				}
			}
			else if (m_vecZ != 0)
			{
				if (pEnt->GetAbsVelocity().z == m_vecZ)
				{
					OnVelocityMatch.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsVelocity().z < m_vecZ)
				{
					OnVelocityLessThan.FireOutput(this, 0);
				}
				else if (pEnt->GetAbsVelocity().z > m_vecZ)
				{
					OnVelocityMoreThan.FireOutput(this, 0);
				}
			}
		}
	}
	SetNextThink(gpGlobals->curtime);
}

void CEntityVelocityChecker::InputDisable(inputdata_t& input)
{
	m_bEnabled = false;
}

void CEntityVelocityChecker::InputEnable(inputdata_t& input)
{
	m_bEnabled = true;
}