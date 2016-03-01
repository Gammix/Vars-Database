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

	db_free_result(db_query(serverDB, "CREATE TABLE IF NOT EXISTS `PVars` (`Varname` TEXT, `Vartype` INTEGER, `Varvalue` TEXT)"));
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
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, MAX_PLAYER_NAME);

	new type = GetPVarType(playerid, varname);
	new value[3024];
    switch (type)
	{
		case PLAYER_VARTYPE_INT:
			valstr(value, GetPVarInt(playerid, varname));

		case PLAYER_VARTYPE_FLOAT:
		    format(value, sizeof (value), "%f", GetPVarFloat(playerid, varname));

		case PLAYER_VARTYPE_STRING:
			GetPVarString(playerid, varname, value, sizeof (value));
    }

	new query[300];
	format(query, sizeof (query), "SELECT * FROM `PVars` WHERE `Varname` = '%s' AND `Username` = '%s'", varname, name);

    new DBResult:result = db_query(serverDB, query);
    if (db_num_rows(result))
	{
	    db_free_result(result);
	    
		format(query, sizeof (query), "UPDATE `PVars` SET `Vartype` = '%i' AND `Varvalue` = '%s' WHERE `Varname` = '%s' AND `Username` = '%s'", type, value, varname, name);
	    db_free_result(db_query(serverDB, query));

	    return true;
	}
	else
	{
		format(query, sizeof (query), "INSERT INTO `PVars`(`Username`, `Varname`, `Vartype`, `Varvalue`) VALUES('%s', '%s', '%i', '%s')", name, varname, type, value);

	    result = db_query(serverDB, query);
	    if (result)
		{
		    db_free_result(result);
		    return true;
		}
		else return false;
	}
}

stock LoadPVar(playerid, varname[])
{
	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, MAX_PLAYER_NAME);

	new query[300];
	format(query, sizeof (query), "SELECT `Vartype` AND `Varvalue` FROM `PVars` WHERE `Varname` = '%s' AND `Username` = '%s' LIMIT 1", varname, name);

    new DBResult:result = db_query(serverDB, query);
    if (db_num_rows(result))
	{
	    new value[3024];
	    db_get_field(result, 1, value, sizeof(value));

		switch (db_get_field_int(result, 0))
		{
			case PLAYER_VARTYPE_INT:
				SetPVarInt(playerid, varname, strval(value));

			case PLAYER_VARTYPE_FLOAT:
				SetPVarFloat(playerid, varname, floatstr(value));

			case PLAYER_VARTYPE_STRING:
				SetPVarString(playerid, varname, value);
		}

		db_free_result(result);

		return true;
	}
	else
	{
	    new type = GetPVarType(playerid, varname);
	    new value[3024];

	    switch (type)
		{
			case PLAYER_VARTYPE_INT:
				valstr(value, GetPVarInt(playerid, varname));

			case PLAYER_VARTYPE_FLOAT:
			    format(value, sizeof (value), "%f", GetPVarFloat(playerid, varname));

			case PLAYER_VARTYPE_STRING:
				GetPVarString(playerid, varname, value, sizeof (value));
	    }
		format(query, sizeof (query), "INSERT INTO `PVars`(`Username`, `Varname`, `Vartype`, `Varvalue`) VALUES('%s', '%s', '%i', '%s')", name, varname, type, value);

	    result = db_query(serverDB, query);
	    if (result)
		{
		    db_free_result(result);
		    return true;
		}
		else return false;
	}
}

stock SaveSVar(varname[])
{
	new type = GetSVarType(varname);
	new value[3024];
    switch (type)
	{
		case SERVER_VARTYPE_INT:
			valstr(value, GetSVarInt(varname));

		case SERVER_VARTYPE_FLOAT:
		    format(value, sizeof (value), "%f", GetSVarFloat(varname));

		case SERVER_VARTYPE_STRING:
			GetSVarString(varname, value, sizeof (value));
    }

	new query[300];
	format(query, sizeof (query), "SELECT * FROM `SVars` WHERE `Varname` = '%s'", varname);

    new DBResult:result = db_query(serverDB, query);
    if (db_num_rows(result))
	{
	    db_free_result(result);
	    
		format(query, sizeof (query), "UPDATE `SVars` SET `Vartype` = '%i' AND `Varvalue` = '%s' WHERE `Varname` = '%s'", type, value, varname);
	    db_free_result(db_query(serverDB, query));
	    
	    return true;
	}
	else
	{
		format(query, sizeof (query), "INSERT INTO `SVars`(`Varname`, `Vartype`, `Varvalue`) VALUES('%s', '%i', '%s')", varname, type, value);

	    result = db_query(serverDB, query);
	    if (result)
		{
		    db_free_result(result);
		    return true;
		}
		else return false;
	}
}

stock LoadSVar(varname[])
{
	new query[300];
	format(query, sizeof (query), "SELECT `Vartype` AND `Varvalue` FROM `SVars` WHERE `Varname` = '%s' LIMIT 1", varname);

    new DBResult:result = db_query(serverDB, query);
    if (db_num_rows(result))
	{
	    new value[3024];
	    db_get_field(result, 1, value, sizeof(value));

		switch (db_get_field_int(result, 0))
		{
			case SERVER_VARTYPE_INT:
				SetSVarInt(varname, strval(value));

			case SERVER_VARTYPE_FLOAT:
				SetSVarFloat(varname, floatstr(value));

			case SERVER_VARTYPE_STRING:
				SetSVarString(varname, value);
		}

		db_free_result(result);

		return true;
	}
	else
	{
	    new type = GetSVarType(varname);
	    new value[3024];

	    switch (type)
		{
			case SERVER_VARTYPE_INT:
				valstr(value, GetSVarInt(varname));

			case SERVER_VARTYPE_FLOAT:
			    format(value, sizeof (value), "%f", GetSVarFloat(varname));

			case SERVER_VARTYPE_STRING:
				GetSVarString(varname, value, sizeof (value));
	    }
		format(query, sizeof (query), "INSERT INTO `SVars`(`Varname`, `Vartype`, `Varvalue`) VALUES('%s', '%i', '%s')", varname, type, value);

	    result = db_query(serverDB, query);
	    if (result)
		{
		    db_free_result(result);
		    return true;
		}
		else return false;
	}
}
