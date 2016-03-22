/*
 * In memory database (vars.inc)
 * Lets you save and load your choice of variables (PVars/SVars) at anytime. Powered by SQLite and MySQL!
 *
 * By Gammix
 *
 * native SavePVar(playerid, varname[], bool:db_type_sqlite = true);
 * native LoadPVar(playerid, varname[], bool:db_type_sqlite = true);
 * native SaveSVar(varname[], bool:db_type_sqlite = true);
 * native LoadSVar(varname[], bool:db_type_sqlite = true);
 */
 
#if ! defined mysql_included
	#tryinclude <a_mysql>
#endif

#if defined mysql_included
	#if ! defined MYSQL_HOST
		#define MYSQL_HOST "localhost"
	#endif

	#if ! defined MYSQL_USER
		#define MYSQL_USER "admin"
	#endif

	#if ! defined MYSQL_DATABASE
		#define MYSQL_DATABASE "server"
	#endif

	#if ! defined MYSQL_PASSWORD
		#define MYSQL_PASSWORD "admin"
	#endif

	static mysql;
#endif

static DB:sqlite;

#if ! defined FILTERSCRIPT
	public OnGameModeInit()
#else
	public OnFilterScriptInit()
#endif
{
	sqlite = db_open("server.db");

	db_free_result(db_query(sqlite, "CREATE TABLE IF NOT EXISTS `PVars` (`Username` TEXT, `Varname` TEXT, `Vartype` INTEGER, `Varvalue` TEXT)"));
	db_free_result(db_query(sqlite, "CREATE TABLE IF NOT EXISTS `SVars` (`Varname` TEXT, `Vartype` INTEGER, `Varvalue` TEXT)"));

	#if defined mysql_included
		mysql = mysql_connect(MYSQL_HOST, MYSQL_USER, MYSQL_DATABASE, MYSQL_PASSWORD);
	    mysql_query(mysql, "CREATE TABLE IF NOT EXISTS `PVars` (`Username` TEXT, `Varname` TEXT, `Vartype` INTEGER, `Varvalue` TEXT)", false);
	    mysql_query(mysql, "CREATE TABLE IF NOT EXISTS `SVars` (`Varname` TEXT, `Vartype` INTEGER, `Varvalue` TEXT)", false);
	#endif
	
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
	db_close(sqlite);

	#if defined mysql_included
		mysql_close(mysql);
	#endif
	
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

stock SavePVar(playerid, varname[], bool:db_type_sqlite = true)
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

    if (! db_type_sqlite)
    {
    	#if defined mysql_included
			mysql_format(mysql, query, sizeof (query), "SELECT * FROM `PVars` WHERE `Varname` = '%e' AND `Username` = '%e'", varname, name);
	
            new Cache:result = mysql_query(mysql, query);
			if (cache_get_row_count(mysql) >= 1)
				mysql_format(mysql, query, sizeof (query), "UPDATE `PVars` SET `Vartype` = '%i' AND `Varvalue` = '%e' WHERE `Varname` = '%e' AND `Username` = '%e'", type, value, varname, name);
			else
				mysql_format(mysql, query, sizeof (query), "INSERT INTO `PVars`(`Username`, `Varname`, `Vartype`, `Varvalue`) VALUES('%e', '%e', '%i', '%e')", name, varname, type, value);

			cache_delete(result, mysql);
			
			result = mysql_query(mysql, query);
			if (result)
			{
			    cache_delete(result, mysql);
				return true;
			}
  	 	#endif
	}
	else
	{
		format(query, sizeof (query), "SELECT * FROM `PVars` WHERE `Varname` = '%q' AND `Username` = '%q'", varname, name);
	
		new DBResult:result = db_query(sqlite, query);
		if (db_num_rows(result) >= 1)
			format(query, sizeof (query), "UPDATE `PVars` SET `Vartype` = '%i' AND `Varvalue` = '%q' WHERE `Varname` = '%q' AND `Username` = '%q'", type, value, varname, name);
		else
			format(query, sizeof (query), "INSERT INTO `PVars`(`Username`, `Varname`, `Vartype`, `Varvalue`) VALUES('%q', '%q', '%i', '%q')", name, varname, type, value);

		db_free_result(result);

		result = db_query(sqlite, query);
		if (result)
		{
		    db_free_result(result);
			return true;
		}
	}

	return false;
}

stock LoadPVar(playerid, varname[], bool:db_type_sqlite = true)
{
	const SIZE = 1024;

	new name[MAX_PLAYER_NAME];
	GetPlayerName(playerid, name, MAX_PLAYER_NAME);

	new query[SIZE + 256];

    if (! db_type_sqlite)
    {
		#if defined mysql_included
			mysql_format(mysql, query, sizeof (query), "SELECT * FROM `PVars` WHERE `Varname` = '%e' AND `Username` = '%e'", varname, name);
			
            new Cache:result = mysql_query(mysql, query);
            if (result)
			{
				if (cache_get_row_count(mysql) >= 1)
				{
					switch (cache_get_row_int(0, 2, mysql))
					{
						case PLAYER_VARTYPE_INT:
							SetPVarInt(playerid, varname, cache_get_row_int(0, 3, mysql));

						case PLAYER_VARTYPE_FLOAT:
							SetPVarFloat(playerid, varname, cache_get_row_float(0, 3, mysql));

						case PLAYER_VARTYPE_STRING:
						{
							new value[SIZE];
							cache_get_row(0, 3, value, mysql, SIZE);
							SetPVarString(playerid, varname, value);
						}
					}

                    cache_delete(result, mysql);

					return true;
				}

				cache_delete(result, mysql);
			}
		#endif
   	}
   	else
   	{
		format(query, sizeof (query), "SELECT * FROM `PVars` WHERE `Varname` = '%q' AND `Username` = '%q'", varname, name);
		
	    new DBResult:result = db_query(sqlite, query);
     	if (result)
		{
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
						db_get_field(result, 3, value, SIZE);
						SetPVarString(playerid, varname, value);
					}
				}

				db_free_result(result);

				return true;
			}
			
			db_free_result(result);
		}
	}
	
	return false;
}

stock SaveSVar(varname[], bool:db_type_sqlite = true)
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

    if (! db_type_sqlite)
    {
    	#if defined mysql_included
			mysql_format(mysql, query, sizeof (query), "SELECT * FROM `SVars` WHERE `Varname` = '%e'", varname);

            new Cache:result = mysql_query(mysql, query);
			if (cache_get_row_count(mysql) >= 1)
				mysql_format(mysql, query, sizeof (query), "UPDATE `SVars` SET `Vartype` = '%i' AND `Varvalue` = '%e' WHERE `Varname` = '%e'", type, value, varname);
			else
				mysql_format(mysql, query, sizeof (query), "INSERT INTO `SVars`(`Varname`, `Vartype`, `Varvalue`) VALUES('%e', '%i', '%e')", varname, type, value);

			cache_delete(result, mysql);

			result = mysql_query(mysql, query);
			if (result)
			{
			    cache_delete(result, mysql);
				return true;
			}
  	 	#endif
	}
	else
	{
		format(query, sizeof (query), "SELECT * FROM `SVars` WHERE `Varname` = '%q'", varname);

		new DBResult:result = db_query(sqlite, query);
		if (db_num_rows(result) >= 1)
			format(query, sizeof (query), "UPDATE `SVars` SET `Vartype` = '%i' AND `Varvalue` = '%q' WHERE `Varname` = '%q'", type, value, varname);
		else
			format(query, sizeof (query), "INSERT INTO `SVars`(`Varname`, `Vartype`, `Varvalue`) VALUES('%q', '%i', '%q')", varname, type, value);

		db_free_result(result);

		result = db_query(sqlite, query);
		if (result)
		{
		    db_free_result(result);
			return true;
		}
	}

	return false;
}

stock LoadSVar(varname[], bool:db_type_sqlite = true)
{
	const SIZE = 1024;

	new query[SIZE + 256];

    if (! db_type_sqlite)
    {
		#if defined mysql_included
			mysql_format(mysql, query, sizeof (query), "SELECT * FROM `SVars` WHERE `Varname` = '%e'", varname);

            new Cache:result = mysql_query(mysql, query);
            if (result)
			{
				if (cache_get_row_count(mysql) >= 1)
				{
					switch (cache_get_row_int(0, 1, mysql))
					{
						case PLAYER_VARTYPE_INT:
							SetSVarInt(varname, cache_get_row_int(0, 2, mysql));

						case PLAYER_VARTYPE_FLOAT:
							SetSVarFloat(varname, cache_get_row_float(0, 2, mysql));

						case PLAYER_VARTYPE_STRING:
						{
							new value[SIZE];
							cache_get_row(0, 2, value, mysql, SIZE);
							SetSVarString(varname, value);
						}
					}

                    cache_delete(result, mysql);

					return true;
				}

				cache_delete(result, mysql);
			}
		#endif
   	}
   	else
   	{
		format(query, sizeof (query), "SELECT * FROM `SVars` WHERE `Varname` = '%q'", varname);

	    new DBResult:result = db_query(sqlite, query);
     	if (result)
		{
		    if (db_num_rows(result) >= 1)
			{
				switch (db_get_field_int(result, 1))
				{
					case PLAYER_VARTYPE_INT:
						SetSVarInt(varname, db_get_field_int(result, 2));

					case PLAYER_VARTYPE_FLOAT:
						SetSVarFloat(varname, db_get_field_float(result, 2));

					case PLAYER_VARTYPE_STRING:
					{
						new value[SIZE];
						db_get_field(result, 2, value, SIZE);
						SetSVarString(varname, value);
					}
				}

				db_free_result(result);

				return true;
			}

			db_free_result(result);
		}
	}

	return false;
}
