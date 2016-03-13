/*
 * In memory database (vars.inc)
 * Lets you save and load your choice of variables (PVars/SVars) at anytime. Powered by SQLite!
 *
 * By Gammix
 *
 * native SavePVar(playerid, varname[]);
 * native LoadPVar(playerid, varname[]);
 * native SaveSVar(varname[]);
 * native LoadSVar(varname[]);
 */

static DB:serverDB;

#if ! defined FILTERSCRIPT
	public OnGameModeInit()
#else
	public OnFilterScriptInit()
#endif
{
	serverDB = db_open("server.db");

	db_free_result(db_query(serverDB, "CREATE TABLE IF NOT EXISTS `PVars` (`Username` TEXT, `Varname` TEXT, `Vartype` INTEGER, `Varvalue` TEXT)"));
	db_free_result(db_query(serverDB, "CREATE TABLE IF NOT EXISTS `SVars` (`Varname` TEXT, `Vartype` INTEGER, `Varvalue` TEXT)"));

	#if ! defined FILTERSCRIPT
		#if defined SS_OnGameModeInit
			return SS_OnGameModeInit();
		#else
			return 1;
		#endif
	#else
		#if defined SS_OnFilterScriptInit
			return SS_OnFilterScriptInit();
		#else
			return 1;
		#endif
	#endif
}
#if ! defined FILTERSCRIPT
	#if defined _ALS_OnGameModeInit
		#undef OnGameModeInit
	#else
		#define _ALS_OnGameModeInit
	#endif
	#define OnGameModeInit SS_OnGameModeInit
	#if defined SS_OnGameModeInit
		forward SS_OnGameModeInit();
	#endif
#else
	#if defined _ALS_OnFilterScriptInit
		#undef OnFilterScriptInit
	#else
		#define _ALS_OnFilterScriptInit
	#endif
	#define OnFilterScriptInit SS_OnFilterScriptInit
	#if defined SS_OnFilterScriptInit
		forward SS_OnFilterScriptInit();
	#endif
#endif

#if ! defined FILTERSCRIPT
	public OnGameModeExit()
#else
	public OnFilterScriptExit()
#endif
{
	db_close(serverDB);

	#if ! defined FILTERSCRIPT
		#if defined SS_OnGameModeExit
			return SS_OnGameModeExit();
		#else
			return 1;
		#endif
	#else
		#if defined SS_OnFilterScriptExit
			return SS_OnFilterScriptExit();
		#else
			return 1;
		#endif
	#endif
}
#if ! defined FILTERSCRIPT
	#if defined _ALS_OnGameModeExit
		#undef OnGameModeExit
	#else
		#define _ALS_OnGameModeExit
	#endif
	#define OnGameModeExit SS_OnGameModeExit
	#if defined SS_OnGameModeExit
		forward SS_OnGameModeExit();
	#endif
#else
	#if defined _ALS_OnFilterScriptExit
		#undef OnFilterScriptExit
	#else
		#define _ALS_OnFilterScriptExit
	#endif
	#define OnFilterScriptExit SS_OnFilterScriptExit
	#if defined SS_OnFilterScriptExit
		forward SS_OnFilterScriptExit();
	#endif
#endif

stock SavePVar(playerid, varname[])
{
	const SIZE = 1024;
	new value[SIZE];
	
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, MAX_PLAYER_NAME);

	new type = GetPVarType(playerid, varname);

    switch (type)
	{
		case PLAYER_VARTYPE_INT:
			valstr(value, GetPVarInt(playerid, varname));

		case PLAYER_VARTYPE_FLOAT:
		    format(value, SIZE, "%f", GetPVarFloat(playerid, varname));

		case PLAYER_VARTYPE_STRING:
			GetPVarString(playerid, varname, value, SIZE);

		default:
		    return false;
    }

	new query[SIZE + 256];
	format(query, sizeof (query), "SELECT * FROM `PVars` WHERE `Varname` = '%s' AND `Username` = '%s'", varname, name);

    new DBResult:result = db_query(serverDB, query);
    if (db_num_rows(result) >= 1)
		format(query, sizeof (query), "UPDATE `PVars` SET `Vartype` = '%i' AND `Varvalue` = '%s' WHERE `Varname` = '%s' AND `Username` = '%s'", type, value, varname, name);
	else
		format(query, sizeof (query), "INSERT INTO `PVars`(`Username`, `Varname`, `Vartype`, `Varvalue`) VALUES('%s', '%s', '%i', '%s')", name, varname, type, value);

	db_free_result(result);

	result = db_query(serverDB, query);
	if (result)
	{
	    db_free_result(result);
		return true;
	}
	else
 		return false;
}

stock LoadPVar(playerid, varname[])
{
	const SIZE = 1024;
	
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, MAX_PLAYER_NAME);

	new query[SIZE + 256];
	format(query, sizeof (query), "SELECT * FROM `PVars` WHERE `Varname` = '%s' AND `Username` = '%s'", varname, name);

    new DBResult:result = db_query(serverDB, query);
    if (db_num_rows(result) >= 1)
	{
		switch (db_get_field_int(result, 2))
		{
			case PLAYER_VARTYPE_INT:
				SetPVarInt(playerid, varname, db_get_field_int(result, 3));

			case PLAYER_VARTYPE_FLOAT:
				SetPVarFloat(playerid, varname, db_get_field_float(result, 3));

			case PLAYER_VARTYPE_STRING:
			{
				new value[SIZE];
				db_get_field(result, 3, value, sizeof(value));
				SetPVarString(playerid, varname, value);
			}
		}

		db_free_result(result);

		return true;
	}

	return false;
}

stock SaveSVar(varname[])
{
	const SIZE = 1024;
	new value[SIZE];

	new type = GetSVarType(varname);

    switch (type)
	{
		case SERVER_VARTYPE_INT:
			valstr(value, GetSVarInt(varname));

		case SERVER_VARTYPE_FLOAT:
		    format(value, SIZE, "%f", GetSVarFloat(varname));

		case SERVER_VARTYPE_STRING:
			GetSVarString(varname, value, SIZE);
    }

	new query[SIZE + 256];
	format(query, SIZE, "SELECT * FROM `SVars` WHERE `Varname` = '%s'", value, varname);

    new DBResult:result = db_query(serverDB, query);
    if (db_num_rows(result) >= 1)
		format(query, SIZE, "UPDATE `SVars` SET `Vartype` = '%i' AND `Varvalue` = '%s' WHERE `Varname` = '%s'", type, value, varname);
	else
		format(query, SIZE, "INSERT INTO `SVars`(`Varname`, `Vartype`, `Varvalue`) VALUES('%s', '%i', '%s')", varname, type, value);

	db_free_result(result);

	result = db_query(serverDB, query);
	if (result)
	{
	    db_free_result(result);
		return true;
	}
	else
 		return false;
}

stock LoadSVar(varname[])
{
	const SIZE = 1024;

	new query[SIZE + 256];
	format(query, SIZE, "SELECT * FROM `SVars` WHERE `Varname` = '%s'", varname);

    new DBResult:result = db_query(serverDB, query);
    if (db_num_rows(result) >= 1)
	{
		switch (db_get_field_int(result, 2))
		{
			case SERVER_VARTYPE_INT:
				SetSVarInt(varname, db_get_field_int(result, 3));

			case SERVER_VARTYPE_FLOAT:
				SetSVarFloat(varname, db_get_field_float(result, 3));

			case SERVER_VARTYPE_STRING:
			{
				new value[SIZE];
 				db_get_field(result, 3, value, SIZE);
				SetSVarString(varname, value);
			}
		}

		db_free_result(result);

		return true;
	}

	return false;
}
