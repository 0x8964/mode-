#if defined _module_player_cmd_included
    #endinput
#endif

#define _module_player_cmd_included

#include <Pawn.CMD>

#include <config-colors>

#include <utils>

#include <character>
#include <inventory>
#include <vehicle>
#include <portal>

static lastCmdUseTimestamp[MAX_PLAYERS] = {0, ...};

static CmdArray:PC_array;
static PC_cmdNames[1000][32];
static PC_cmdFlags[1000];
CMD:ochat(playerid,params[])
{
    if(isnull(params)) return Player_Msg(playerid,COLOR_WHITE,"SU DUNG:/ochat (chat)");
    new name[15],s[128];
    Character_GetName(playerid,name,sizeof(name));
    format(s,128,"((OOC)%s:%s)",name,params);
    Player_MsgAll(COLOR_WHITE,s);
    return 1;
}
alias:ochat("o");
CMD:inv(playerid, params[])
{
    Inventory_Show(playerid, 0);
    return 1;
}
alias:inv("inventory");

CMD:enter(playerid)
{
    new areas[20],
        count = GetPlayerNumberDynamicAreas(playerid),
        entrance_extra_id = Portal_GetEntranceExtraId(); 
        
    GetPlayerDynamicAreas(playerid, areas, sizeof(areas));

    for(new i = 0, j = sizeof(areas); i != j; i++)
    {
        if(i >= count)
        {
            break;
        }

        if(Streamer_GetIntData(STREAMER_TYPE_AREA, areas[i], E_STREAMER_EXTRA_ID) == entrance_extra_id)
        {
            new portal_index = Portal_GetByEntranceAreaId(areas[i]);
            if(portal_index != -1)
            {
                Portal_StreamPlayerToExit(playerid, portal_index);
                break;
            }
        }
    }
    return 1;
}

CMD:exit(playerid)
{
    new areas[20],
        count = GetPlayerNumberDynamicAreas(playerid),
        exit_extra_id = Portal_GetExitExtraId(); 
        
    GetPlayerDynamicAreas(playerid, areas, sizeof(areas));

    for(new i = 0, j = sizeof(areas); i != j; i++)
    {
        if(i >= count)
        {
            break;
        }

        if(Streamer_GetIntData(STREAMER_TYPE_AREA, areas[i], E_STREAMER_EXTRA_ID) == exit_extra_id)
        {
            new portal_index = Portal_GetByExitAreaId(areas[i]);
            if(portal_index != -1)
            {
                Portal_StreamPlayerToEntrance(playerid, portal_index);
                break;
            }
        }
    }
    return 1;
}

CMD:vehicle(playerid, params[])
{
    if(isnull(params))
    {
        Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /v(ehicle) [choice]");
        Player_Msg(playerid, COLOR_FADE2, "* choice: get, insertkey, takekey, checkkey, engine, lights, lock, trunk, hood, park, buypark.");
        return 1;
    }

    if(!strcmp(params, "get", true))
    {
        task_yield(1);

        new uuid[UUID_LEN];
        Character_GetUUID(playerid, uuid, sizeof(uuid));
        await mysql_aquery_s(MySQL_GetHandle(), str_format("SELECT * FROM personal_vehicles WHERE owner_uuid = '%e'", uuid));
        
        new rows = cache_num_rows();
        if(rows == 0)
        {
            Player_Msg(playerid, COLOR_RED, "> ERROR: Ban khong co so huu bat ki phuong tien nao.");
        }
        else
        {
            new string[21 + (10 + 17 + 37) * MAX_CHARACTER_PERSONAL_VEHICLES],
                model,
                name[17];
            strcat(string, "Ten phuong tien\tUUID");
            for(new i = 0; i < rows; i++)
            {
                cache_get_value_name_int(i, "model", model);
                cache_get_value_name(i, "uuid", uuid, sizeof(uuid));

                Vehicle_GetNameByModel(model, name, sizeof(name));

                format(string, sizeof(string), "%s\n%s\t%s", string, name, uuid);

                format(name, sizeof(name), "veh_UUID_%d", i);
                SetPVarString(playerid, name, uuid);
            }
            
            ShowPlayerDialog(playerid, DIALOG_CHAR_PERSONAL_VEHICLE, DIALOG_STYLE_TABLIST_HEADERS, "Character | Vehicles", string, "Chon", "Tat");
        }
    }
    else if(!strcmp(params, "insertkey", true))
    {
        if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleSeat(playerid) != 0) return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai ngoi o ghe tai xe cua mot phuong tien.");

        new vehicleid = GetPlayerVehicleID(playerid);
        if(Vehicle_IsKeyInserted(vehicleid)) return Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien da co san mot chia khoa.");

        new key[24];
        Vehicle_GetKey(vehicleid, key, sizeof(key));
        
        if(isnull(key))
        {
            Vehicle_SetKeyInserted(vehicleid, true);
            Player_Msg(playerid, COLOR_GREEN, "> Info: Ban da dut chia khoa vao phuong tien, co the bat dau su dung.");
        }
        else
        {
            new index = Inventory_GetByIdAndKey(playerid, 44, key);
            if(index == -1)
            {
                Player_Msg(playerid, COLOR_RED, "> ERROR: Tren nguoi ban khong co bat ki chia khoa nao phu hop voi phuong tien.");
            }
            else
            {
                Vehicle_SetKeyInserted(vehicleid, true);
                Player_Msg(playerid, COLOR_GREEN, "> Info: Ban da dut chia khoa vao phuong tien, co the bat dau su dung.");
                Inventory_RemoveByIndex(playerid, index);
            }
        }
    }
    else if(!strcmp(params, "takekey", true))
    {
        if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleSeat(playerid) != 0) return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai ngoi o ghe tai xe cua mot phuong tien.");

        new vehicleid = GetPlayerVehicleID(playerid);
        if(!Vehicle_IsKeyInserted(vehicleid)) return Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien chua co bat ki chia khoa nao.");
        
        new key[24];
        Vehicle_GetKey(vehicleid, key, sizeof(key));
        
        if(!isnull(key))
        {
            Inventory_Add(playerid, 44, 1, 1, key);
        }
        
        Vehicle_SetParams(vehicleid, false, false, Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_ALARM), Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_DOORS), Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_BONNET), Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_BOOT), Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_OBJECTIVE));
        
        Player_Msg(playerid, COLOR_GREEN, "> Info: Ban da lay chia khoa tu phuong tien.");
        Vehicle_SetKeyInserted(vehicleid, false);
    }
    else if(!strcmp(params, "checkkey", true))
    {
        if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleSeat(playerid) != 0) return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai ngoi o ghe tai xe cua mot phuong tien.");

        new key[24],
            vehicleid = GetPlayerVehicleID(playerid);
        Vehicle_GetKey(vehicleid, key, sizeof(key));
        
        if(!isnull(key))
        {
            new string[24 + 45];
            format(string, sizeof(string), "> Info: Chia khoa cua phuong tien nay la: %s.", key);
            Player_Msg(playerid, COLOR_GREEN, string);
        }
        else
        {
            Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien khong can chia khoa.");
        }
        
    }
    else if(!strcmp(params, "engine", true))
    {
        if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleSeat(playerid) != 0) return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai ngoi o ghe tai xe cua mot phuong tien.");
        
        new vehicleid = GetPlayerVehicleID(playerid),
            model = GetVehicleModel(vehicleid);
        if(model == 510 || model == 509 || model == 481) return Player_Msg(playerid, COLOR_RED, "> ERROR: Xe dap khong phu hop cho lua chon nay.");

        if(!Vehicle_IsKeyInserted(vehicleid)) return Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien chua duoc dut chia khoa vao.");

        if(Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_ENGINE))
        {
            Player_Msg(playerid, COLOR_GREEN, "> Info: Phuong tien da tat dong co.");
        }
        else
        {
            Player_Msg(playerid, COLOR_GREEN, "> Info: Phuong tien da khoi dong.");
        }

        Vehicle_ToggleParam(vehicleid, VEHICLE_PARAMS_ENGINE);
    }
    else if(!strcmp(params, "lights", true))
    {
        if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleSeat(playerid) != 0) return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai ngoi o ghe tai xe cua mot phuong tien.");
        
        new vehicleid = GetPlayerVehicleID(playerid),
            model = GetVehicleModel(vehicleid);
        if(model == 510 || model == 509 || model == 481) return Player_Msg(playerid, COLOR_RED, "> ERROR: Xe dap khong phu hop cho lua chon nay.");

        if(!Vehicle_IsKeyInserted(vehicleid)) return Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien chua duoc dut chia khoa vao.");

        if(Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_LIGHTS))
        {
            Player_Msg(playerid, COLOR_GREEN, "> Info: Phuong tien da tat den.");
        }
        else
        {
            Player_Msg(playerid, COLOR_GREEN, "> Info: Phuong tien da mo den.");
        }

        Vehicle_ToggleParam(vehicleid, VEHICLE_PARAMS_LIGHTS);
    }
    else if(!strcmp(params, "lock", true))
    {
        new vehicleid = (IsPlayerInAnyVehicle(playerid)) ? GetPlayerVehicleID(playerid) : Player_GetClosestVehicleId(playerid, 3.0);
        if(vehicleid == 0) return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban phai ngoi ben trong hoac o gan mot phuong tien.");

        if(Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_DOORS))
        {
            new key[24];
            Vehicle_GetKey(vehicleid, key, sizeof(key));
            
            if(!isnull(key))
            {
                new index = Inventory_GetByIdAndKey(playerid, 44, key);
                if(index == -1)
                {
                    return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban khong co chia khoa cua phuong tien nay.");
                }
            }
        }

        if(Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_DOORS))
        {
            Player_Msg(playerid, COLOR_GREEN, "> Info: Phuong tien da mo khoa.");
        }
        else
        {
            Player_Msg(playerid, COLOR_GREEN, "> Info: Phuong tien da dong khoa.");
        }

        Vehicle_ToggleParam(vehicleid, VEHICLE_PARAMS_DOORS);
    }
    else if(!strcmp(params, "trunk", true))
    {
        new vehicleid = Player_GetClosestVehicleId(playerid, 5.0);

        if(vehicleid == 0)
        {
            return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai o gan mot phuong tien.");
        }

        new Float:model_pos[3],
            Float:pos[3];
        GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_WHEELSREAR, model_pos[0], model_pos[1], model_pos[2]);
        Utils_GetVehicleRelativePos(vehicleid, pos[0], pos[1], pos[2], model_pos[0] - 1.0, model_pos[1] - 0.5, model_pos[2]);

        if(IsPlayerInRangeOfPoint(playerid, 2.0, pos[0], pos[1], pos[2]))
        {
            if(Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_BOOT))
            {
                Vehicle_ToggleParam(vehicleid, VEHICLE_PARAMS_BOOT);

                Player_Msg(playerid, COLOR_GREEN, "> Info: Cop cua phuong tien da duoc dong.");
            }
            else
            {
                if(!Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_DOORS))
                {
                    Vehicle_ToggleParam(vehicleid, VEHICLE_PARAMS_BOOT);

                    Player_Msg(playerid, COLOR_GREEN, "> Info: Cop cua phuong tien da duoc mo.");
                }
                else Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien da bi khoa, khong the mo cop.");
            }
        }
        else Player_Msg(playerid, COLOR_RED, "> ERROR: Ban khong o gan cop cua phuong tien.");
    }
    else if(!strcmp(params, "hood", true))
    {
        new vehicleid = Player_GetClosestVehicleId(playerid, 5.0);

        if(vehicleid == 0)
        {
            return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai o gan mot phuong tien.");
        }
        
        new Float:model_pos[3],
            Float:pos[3];
        GetVehicleModelInfo(GetVehicleModel(vehicleid), VEHICLE_MODEL_INFO_WHEELSFRONT, model_pos[0], model_pos[1], model_pos[2]);
        Utils_GetVehicleRelativePos(vehicleid, pos[0], pos[1], pos[2], model_pos[0] - 1.0, model_pos[1] + 0.5, model_pos[2]);

        if(IsPlayerInRangeOfPoint(playerid, 2.0, pos[0], pos[1], pos[2]))
        {
            if(Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_BONNET))
            {
                Vehicle_ToggleParam(vehicleid, VEHICLE_PARAMS_BONNET);

                Player_Msg(playerid, COLOR_GREEN, "> Info: Nap cua phuong tien da duoc dong.");
            }
            else
            {
                if(!Vehicle_GetParam(vehicleid, VEHICLE_PARAMS_DOORS))
                {
                    Vehicle_ToggleParam(vehicleid, VEHICLE_PARAMS_BONNET);

                    Player_Msg(playerid, COLOR_GREEN, "> Info: Nap cua phuong tien da duoc mo.");
                }
                else Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien da bi khoa, khong the mo nap.");
            }
        }
        else Player_Msg(playerid, COLOR_RED, "> ERROR: Ban khong o gan nap cua phuong tien.");
    }
    else if(!strcmp(params, "park", true))
    {
        if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleSeat(playerid) != 0) return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai ngoi tren ghe tai xe cua mot phuong tien.");

        new vehicleid = GetPlayerVehicleID(playerid);

        if(Vehicle_GetType(vehicleid) != VEHICLE_TYPE_PERSONAL) return Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien nay khong phu hop de dau xe.");

        new key[24];
        Vehicle_GetKey(vehicleid, key, sizeof(key));

        task_yield(1);
        new result = await PersonalVehicle_Park(vehicleid);

        if(result < 0)
        {
            Player_Msg(playerid, COLOR_RED, sprintf("> ERROR: Da co loi xay ra trong qua trinh dau phuong tien (error code: %d).", result));
        }
        else
        {
            //Inventory_Add(playerid, 44, 1, 1, key); // give the key back to player after parking the vehicle.

            Player_Msg(playerid, COLOR_GREEN, "> Info: Phuong tien cua ban da duoc dau thanh cong.");
        }
    }
    else if(!strcmp(params, "buypark", true))
    {
        if(!IsPlayerInAnyVehicle(playerid) || GetPlayerVehicleSeat(playerid) != 0) return Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can phai ngoi tren ghe tai xe cua mot phuong tien.");

        new vehicleid = GetPlayerVehicleID(playerid);

        if(!Vehicle_IsKeyInserted(vehicleid)) return Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien chua duoc cam chia khoa.");

        if(Vehicle_GetType(vehicleid) != VEHICLE_TYPE_PERSONAL) return Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien nay khong phu hop de mua vi tri dau xe.");

        new Float:pos[4],
            uuid[UUID_LEN];
        GetVehiclePos(vehicleid, pos[0], pos[1], pos[2]);
        GetVehicleZAngle(vehicleid, pos[3]);
        PersonalVehicle_GetUUID(vehicleid, uuid, sizeof(uuid));

        task_yield(1);
        if(task_await(PersonalVehicle_SetParkingPos(uuid, pos[0], pos[1], pos[2], pos[3], Vehicle_GetInterior(vehicleid), Vehicle_GetWorld(vehicleid))) > 0)
        {
            Player_Msg(playerid, COLOR_GREEN, "> Info: Ban da mua vi tri dau xe cho phuong tien thanh cong, su dung /v park de dau phuong tien.");
        }
        else
        {
            Player_Msg(playerid, COLOR_RED, "> ERROR: Da co loi xay ra trong qua trinh mua vi tri dau xe.");
        }
    }
    else
    {
        Player_Msg(playerid, COLOR_FADE2, "* choice: get, insertkey, takekey, checkkey, engine, lights, lock, trunk, hood, park, buypark.");
    }
    return 1;
}
alias:vehicle("v", "veh");

public OnPlayerCommandReceived(playerid, cmd[], params[], flags)
{
    if(!Character_IsSpawned(playerid))
    {
        Player_Msg(playerid, COLOR_RED, "> ERROR: Ban can dang nhap vao nhan vat truoc khi su dung lenh.");
        return 0;
    } 
    else if(!(flags & Character_GetCmdFlags(playerid)) && flags != 0)
    {
        Player_Msg(playerid, COLOR_RED, "> ERROR: Ban khong du quyen han de su dung lenh nay.");
        return 0;
    }
    else if((gettime() - lastCmdUseTimestamp[playerid]) < 1)
    {
        Player_Msg(playerid, COLOR_FADE3, "> ERROR: Moi lan su dung lenh phai cach nhau it nhat 1 giay.");
        return 0;
    }
    return 1;
}

public OnPlayerCommandPerformed(playerid, cmd[], params[], result, flags)
{
    if(result == -1)
    {
        Player_Msg(playerid, COLOR_RED, "> ERROR: Lenh ban vua nhap khong ton tai.");
        
        if(strlen(cmd) >= 2)
        {
            new string[200],
                bool:similarCmdsFound = false;
            for (new i, j = PC_GetArraySize(PC_array); i != j; i++)
            {
                if(Utils_CountCommonPrefix(PC_cmdNames[i], cmd) > 2 || strfind(PC_cmdNames[i], cmd, true) != -1)
                {
                    if((PC_cmdFlags[i] != 0 && (PC_cmdFlags[i] & Character_GetCmdFlags(playerid))) || PC_cmdFlags[i] == 0)
                    {
                        if(strlen(string) > 170)
                        {
                            break;
                        }
                        format(string, sizeof(string), "%s/%s, ", string, PC_cmdNames[i]);
                        similarCmdsFound = true;
                    }
                }
            }

            if(similarCmdsFound)
            {
                string[strlen(string)-2] = EOS;
                strcat(string, " "EMBED_COLOR_YELLOW"<");
                Player_Msg(playerid, COLOR_FADE3, string);
            }
        }
        return 0;
    }
    else
    {
        lastCmdUseTimestamp[playerid] = gettime();
    }
    return 1;
}

public PC_OnInit()
{
    PC_array = PC_GetCommandArray();
    for (new i = 0, j = PC_GetArraySize(PC_array); i != j; i++)
    {
        PC_GetCommandName(PC_array, i, PC_cmdNames[i]);
        PC_cmdFlags[i] = PC_GetFlags(PC_cmdNames[i]);
    }
    return 1;
}

#include <YSI_Coding\y_hooks>

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_CHAR_PERSONAL_VEHICLE:
        {
            if(response)
            {
                new var_name[17];
                format(var_name, sizeof(var_name), "veh_UUID_%d", listitem);
                
                if(GetPVarType(playerid, var_name) == PLAYER_VARTYPE_STRING)
                {
                    new uuid[UUID_LEN];
                    GetPVarString(playerid, var_name, uuid, sizeof(uuid));

                    if(PersonalVehicle_GetByUUID(uuid) != 0)
                    {
                        Player_Msg(playerid, COLOR_RED, "> ERROR: Phuong tien da duoc lay ra truoc do.");
                    }
                    else
                    {
                        task_yield(1);

                        new result = await PersonalVehicle_Spawn(uuid);
                        if(result == -1)
                        {
                            Player_Msg(playerid, COLOR_RED, "> ERROR: Da co loi xay ra trong luc lay phuong tien, vui long lay lai hoac bao loi.");
                        }
                        else
                        {
                            Player_Msg(playerid, COLOR_GREEN, "> Info: Phuong tien da xuat hien tai vi tri dau xe.");
                        }
                    }  
                }
                else
                {
                    Player_Msg(playerid, COLOR_RED, "> ERROR: Da co loi xay ra, vui long [/v get] lai.");
                }
            }
            DeletePVar(playerid, "veh_UUID_0");
            DeletePVar(playerid, "veh_UUID_1");
            DeletePVar(playerid, "veh_UUID_2");
            DeletePVar(playerid, "veh_UUID_3");
            DeletePVar(playerid, "veh_UUID_4");
            DeletePVar(playerid, "veh_UUID_5");
        }
    }
    return 1;
}