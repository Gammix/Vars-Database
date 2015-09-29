/*
    PVars Database - SQLITE (savepvar.inc)
	* This include allows you to save and load PVars into database which will have a table for saving/loading specified PVars.
	* This is the best way and the fastest way to save and load a user database.

	Author: (creator)
 	* Gammix

	(c) Copyright 2015
 	* This file is provided as is (no warranties).
*/
/*
STOCKS:
native LoadPVar(playerid, DB:db, table[], varname[]);
native SavePVar(playerid, DB:db, table[], varname[]);
*/

stock bool:LoadPVar(playerid, DB:db, table[], varname[]) {
	#if ! defined SKIP_VERIFICATION
		// Column verification
		// If not exist, this will automatically create one in the table
		new
			iType = GetPVarType(playerid, varname),
			sQuery[450]
		;
		switch (iType) {
			case PLAYER_VARTYPE_INT: {
				format(sQuery, sizeof(sQuery), "ALTER TABLE `%s` ADD COLUMN `%s` INTEGER DEFAULT('%d')", table, varname, 0);
			}

			case PLAYER_VARTYPE_FLOAT: {
				format(sQuery, sizeof(sQuery), "ALTER TABLE `%s` ADD COLUMN `%s` REAL DEFAULT('%f')", table, varname, 0.0);
			}

			case PLAYER_VARTYPE_STRING: {
				format(sQuery, sizeof(sQuery), "ALTER TABLE `%s` ADD COLUMN `%s` TEXT DEFAULT(`%s`)", table, varname, "");
			}

			default: {
				return false;
			}
		}
		db_free_result(db_query(db, sQuery));
	#endif
	
	// Username or Account verification in the table
	// If not exist, this stock will return false
	new
	    sPlayerName[MAX_PLAYER_NAME]
	;
	GetPlayerName(playerid, sPlayerName, MAX_PLAYER_NAME);
	format(sQuery, sizeof(sQuery), "SELECT `ID`, `%s` FROM `%s` WHERE `username` = '%s'", varname, table, sPlayerName);

    new
		DBResult:iDBResult = db_query(db, sQuery)
	;
    if (iDBResult) {// If the user is found in the table
		if (db_num_rows(iDBResult) >= 1) {
			switch (iType) {//store values into the PVars
				case PLAYER_VARTYPE_INT: {
					SetPVarInt(playerid, varname, db_get_field_assoc_int(iDBResult, varname));
				}

				case PLAYER_VARTYPE_FLOAT: {
					SetPVarFloat(playerid, varname, db_get_field_assoc_float(iDBResult, varname));
	        	}

				case PLAYER_VARTYPE_STRING: {
				    new
				        sValue[3024]
					;
					db_get_field_assoc(iDBResult, varname, sValue, sizeof(sValue));
					SetPVarString(playerid, varname, sValue);
	        	}
	        }

      		iDBResult = db_query(db, sQuery);
       		if (iDBResult) {//if the query was successfull, return true else false
				db_free_result(iDBResult);
				
				return true;
			}
			else {
				return false;
			}
		}
	}

	return false;
}

stock bool:SavePVar(playerid, DB:db, table[], varname[]) {
	#if ! defined SKIP_VERIFICATION
		// Column verification
		// If not exist, this will automatically create one in the table
		new
			iType = GetPVarType(playerid, varname),
			sQuery[450]
		;
		switch (iType) {
			case PLAYER_VARTYPE_INT: {
				format(sQuery, sizeof(sQuery), "ALTER TABLE `%s` ADD COLUMN `%s` INTEGER DEFAULT('%d')", table, varname, 0);
			}

			case PLAYER_VARTYPE_FLOAT: {
				format(sQuery, sizeof(sQuery), "ALTER TABLE `%s` ADD COLUMN `%s` REAL DEFAULT('%f')", table, varname, 0.0);
			}

			case PLAYER_VARTYPE_STRING: {
				format(sQuery, sizeof(sQuery), "ALTER TABLE `%s` ADD COLUMN `%s` TEXT DEFAULT('%s')", table, varname, "");
			}

			default: {
				return false;
			}
		}
		db_free_result(db_query(db, sQuery));
	#endif
	
	// Username or Account verification in the table
	// If not exist, this stock will return false
	new
	    sPlayerName[MAX_PLAYER_NAME],
	    iKey
	;
	GetPlayerName(playerid, sPlayerName, MAX_PLAYER_NAME);
	format(sQuery, sizeof(sQuery), "SELECT `ID` FROM `%s` WHERE `username` = '%s'", table, sPlayerName);

    new
		DBResult:iDBResult = db_query(db, sQuery)
	;
 	if (iDBResult) {// If the user is found in the table
    	if (db_num_rows(iDBResult) >= 1) {
			db_get_field(iDBResult, 0, sQuery, sizeof(sQuery));
			iKey = strval(sQuery);// Store the key value (ID)

			db_free_result(iDBResult);

			switch (iType) {//Read values from the PVars
				case PLAYER_VARTYPE_INT: {
	        		format(sQuery, sizeof(sQuery), "UPDATE `%s` SET `%s`='%d' WHERE `ID`='%d'", table, varname, GetPVarInt(playerid, varname), iKey);
	        	}

				case PLAYER_VARTYPE_FLOAT: {
	        		format(sQuery, sizeof(sQuery), "UPDATE `%s` SET `%s`='%f' WHERE `ID`='%d'", table, varname, GetPVarFloat(playerid, varname), iKey);
	        	}

				case PLAYER_VARTYPE_STRING: {
				    new
				        sValue[3024]
					;
					GetPVarString(playerid, varname, sValue, sizeof(sValue));
	        		format(sQuery, sizeof(sQuery), "UPDATE `%s` SET `%s`='%s' WHERE `ID`='%d'", table, varname, sValue, iKey);
	        	}
	        }

       		iDBResult = db_query(db, sQuery);
       		if (iDBResult) {//if the query was successfull, return true else false
				db_free_result(iDBResult);
				
				return true;
			}
			else {
				return false;
			}
		}
	}

	// Secondary option
	// If the above one wasn't executed, then we will create the account/row into the table
    switch (iType) {
		case PLAYER_VARTYPE_INT: {
       		format(sQuery, sizeof(sQuery), "INSERT INTO `%s`(`username`,`%s`) VALUES('%s','%i')", table, varname, sPlayerName, GetPVarInt(playerid, varname));
       	}

		case PLAYER_VARTYPE_FLOAT: {
       		format(sQuery, sizeof(sQuery), "INSERT INTO `%s`(`username`,`%s`) VALUES('%s','%f')", table, varname, sPlayerName, GetPVarFloat(playerid, varname));
       	}

		case PLAYER_VARTYPE_STRING: {
		    new
		        sValue[3024]
			;
			GetPVarString(playerid, varname, sValue, sizeof(sValue));
       		format(sQuery, sizeof(sQuery), "INSERT INTO `%s`(`username`,`%s`) VALUES('%s','%s')", table, varname, sPlayerName, sValue);
       	}
   	}

	iDBResult = db_query(db, sQuery);
    if (iDBResult) {//if the query was successfull, return true else false
		db_free_result(iDBResult);
		
		return true;
	}
	else {
		return false;
	}
}
