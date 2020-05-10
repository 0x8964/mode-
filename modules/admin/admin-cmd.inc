#if defined _module_admin_cmd_included
    #endinput
#endif

#define _module_admin_cmd_included

#include <Pawn.CMD>

#include <config-colors>

#include <utils>

#include <character>
#include <inventory>
#include <vehicle>
#include <portal>
flags:amake(CMD_FLAG_ADMIN_5);
CMD:amake(playerid,params[])
{
    new id,level;
    if(sscanf(params,"di",id,level)) return Player_Msg(playerid,COLOR_WHITE,"SU DUNG:/amake [playerid][level admin]");
    if(level <0 || level >5) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Level Admin 1-5");
    if(!IsPlayerConnected(id)) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Nguoi choi khong ton tai!");
    Character_SetAdminLevel(id,level);
    new name[15],s[35],nameid[15];
    Character_GetName(playerid,name,15);
    Character_GetName(playerid,nameid,15);
    format(s,35,"[ADMIN]%s da tro thanh Admin %d boi %s",nameid,level,name);
    return 1;
}
flags:admin(CMD_FLAG_ADMIN_1 | CMD_FLAG_ADMIN_2 | CMD_FLAG_ADMIN_3 | CMD_FLAG_ADMIN_4 | CMD_FLAG_ADMIN_5);

CMD:admin(playerid, params[])
{
    if(isnull(params)) return Player_Msg(playerid,COLOR_WHITE,"SU DUNG:/a (message)");
    SendAdMess(COLOR_WHITE,params);
    return 1;
}
alias:admin("a");

flags:aooc(CMD_FLAG_ADMIN_1 | CMD_FLAG_ADMIN_2 | CMD_FLAG_ADMIN_3 | CMD_FLAG_ADMIN_4 | CMD_FLAG_ADMIN_5);
CMD:aooc(playerid,params[])
{
    if(isnull(params)) return Player_Msg(playerid,COLOR_WHITE,"SU DUNG:/nooc (chat)");
    new name[14],s[128];
    Character_GetName(playerid,name,sizeof(name));
    format(s,128,"((OOC)Admin %s:%s)",name,params);
    Player_MsgAll(COLOR_WHITE,s);
    return 1;
}
alias:aooc("nooc");
flags:togooc(CMD_FLAG_ADMIN_3 | CMD_FLAG_ADMIN_4 | CMD_FLAG_ADMIN_5);
CMD:togooc(playerid,params[])
{
    if(isnull(params)) return Player_Msg(playerid,COLOR_WHITE,"SU DUNG:/togooc");
    if(!TogOChat)
    {
        Player_MsgAll(COLOR_WHITE,"((OOC)Kenh chat da duoc mo boi Admin)");
        TogOChat=true;
    }
    else
    {
        Player_MsgAll(COLOR_WHITE,"((OOC)Kenh chat da duoc dong boi Admin)");
        TogOChat=false; 
    }
    return 1;
}
alias:togooc("togo");
flags:avehicle(CMD_FLAG_ADMIN_1 | CMD_FLAG_ADMIN_2 | CMD_FLAG_ADMIN_3 | CMD_FLAG_ADMIN_4 | CMD_FLAG_ADMIN_5);
CMD:avehicle(playerid, params[])
{
    new model,
        colors[2],
        siren;
    if(sscanf(params, "iI(0)I(1)I(0)", model, colors[0], colors[1], siren)) return Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /avehicle [model] (primary color) (secondary color) (siren)");

    new Float:pos[3],
        Float:angle;
    GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
    GetPlayerFacingAngle(playerid, angle);
    Utils_GetPlayerForwardPos(playerid, pos[0], pos[1], 3.0);

    CreateVehicle(model, pos[0], pos[1], pos[2], angle, colors[0], colors[1], -1, siren);
    return 1;
}
alias:avehicle("aveh");

flags:aweapon(CMD_FLAG_ADMIN_1 | CMD_FLAG_ADMIN_2 | CMD_FLAG_ADMIN_3 | CMD_FLAG_ADMIN_4 | CMD_FLAG_ADMIN_5);
CMD:aweapon(playerid, params[])
{
    new weapon_id,
        ammo;
    if(sscanf(params, "iI(100)", weapon_id, ammo)) return Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /aweapon [weapon id] (ammo)");

    GivePlayerWeapon(playerid, weapon_id, ammo);
    return 1;
}
alias:aweapon("aweap");

flags:agiveitem(CMD_FLAG_ADMIN_3 | CMD_FLAG_ADMIN_4 | CMD_FLAG_ADMIN_5);
CMD:agiveitem(playerid, params[])
{
    new targetid,
        item_id,
        amount,
        number_of_uses,
        key[24];
    if(sscanf(params, "iiI(1)I(1)S()[24]", targetid, item_id, amount, number_of_uses, key)) return Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /agiveitem [playerid] [item id] (amount) (number of uses) (key)");

    if(!Character_IsSpawned(targetid)) return Player_Msg(playerid, -1, "> ERROR: Nguoi choi chua dang nhap vao nhan vat.");

    task_yield(1);
    new result = await Inventory_Add(targetid, item_id, amount, number_of_uses, key);

    if(result == 0)
    {
        Player_Msg(playerid, COLOR_ERROR, "> ERROR: Item ID khong ton tai trong co so du lieu.");
    }
    else
    {
        new string[82 + 32 + 64 + 24],
            name[MAX_PLAYER_NAME + 1],
            item_name[64];
        Character_GetName(targetid, name, sizeof(name));

        task_yield(1);
        await_arr(item_name) Item_RetrieveName(item_id);

        format(string, sizeof(string), "> Info: Ban da cho '%s' vat pham '%s' (so luong: %d | so lan su dung: %d | key: %s)", name, item_name, amount, number_of_uses, (isnull(key)) ? "empty" : key);
        Player_Msg(playerid, COLOR_GREEN, string);
    }
    return 1;
}

flags:acreatepvehicle(CMD_FLAG_ADMIN_4 | CMD_FLAG_ADMIN_5);
CMD:acreatepvehicle(playerid, params[])
{
    new targetid,
        model,
        primary_color,
        secondary_color,
        siren;
    if(sscanf(params, "iiI(0)I(1)I(0)", targetid, model, primary_color, secondary_color, siren)) return Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /acreatepveh(icle) [playerid] [model] (primary color) (secondary color) (siren)");

    if(!Character_IsSpawned(targetid)) return Player_Msg(playerid, COLOR_RED, "> ERROR: Nguoi choi chua dang nhap vao nhan vat.");

    if(model < 400 || model > 611) return Player_Msg(playerid, COLOR_RED, "> ERROR: Model phuong tien khong hop le.");

    new target_uuid[UUID_LEN];
    Character_GetUUID(targetid, target_uuid, sizeof(target_uuid));

    task_yield(1);
    new count = await PersonalVehicle_CountByOwner(target_uuid);

    if(count >= MAX_CHARACTER_PERSONAL_VEHICLES) return Player_Msg(playerid, COLOR_RED, "> ERROR: Nguoi choi da so huu toi da phuong tien.");
    
    new Float:pos[3],
        Float:angle;
    GetPlayerPos(targetid, pos[0], pos[1], pos[2]);
    Utils_GetPlayerForwardPos(targetid, pos[0], pos[1], 3.5);
    GetPlayerFacingAngle(targetid, angle);
    
    task_yield(1);
    new index = await PersonalVehicle_Create(target_uuid, model, pos[0], pos[1], pos[2], angle, primary_color, secondary_color, GetPlayerInterior(targetid), GetPlayerVirtualWorld(targetid), -1, siren);
    if(index != -1)
    {
        new vehicle_name[17],
            char_name[MAX_PLAYER_NAME + 1];
        
        Character_GetName(playerid, char_name, sizeof(char_name));
        Vehicle_GetNameByModel(model, vehicle_name, sizeof(vehicle_name));
        Player_Msg(playerid, COLOR_GREEN, sprintf("> Info: Ban da tao phuong tien '%s' cho '%s'.", vehicle_name, char_name));
    }
    else
    {
        Player_Msg(playerid, COLOR_RED, "> ERROR: Da co loi xay ra trong luc tao phuong tien, vui long bao loi.");
    }
    return 1;
}
alias:acreatepvehicle("acreatepveh");

flags:apotl(CMD_FLAG_ADMIN_3 | CMD_FLAG_ADMIN_4 | CMD_FLAG_ADMIN_5);
CMD:apotl(playerid, params[])
{
    new choice[16],
        portal_index;
    if(sscanf(params, "s[16]I(-1)", choice, portal_index))
    {
        Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /apotl [choice] (portal index)");
        Player_Msg(playerid, COLOR_FADE2, "* choice: create, entrance, exit, togglelock, destroy");
        return 1;
    }

    if(!strcmp(choice, "create", true))
    {
        #pragma unused portal_index
        
        Player_Msg(playerid, COLOR_GREEN, sprintf("> Info: Ban da tao ra portal index %d, hay su dung portal index cho lua chon khac nhu 'entrance', 'exit',...", Portal_Create()));
    }
    else if(!strcmp(choice, "entrance", true))
    {
        if(portal_index < 0 || portal_index >= Portal_GetSize())
        {
            Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /apotl entrance (portal index)");
            Player_Msg(playerid, COLOR_FADE2, "* portal index khong hop le.");
            return 1;
        }

        new Float:pos[3],
            Float:angle;
        
        GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
        GetPlayerFacingAngle(playerid, angle);
        Portal_SetEntrance(portal_index, pos[0], pos[1], pos[2], angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));

        Player_Msg(playerid, COLOR_GREEN, sprintf("> Info: Ban da dieu chinh loi vao cua portal %d ve vi tri hien tai cua ban.", portal_index));
    }
    else if(!strcmp(choice, "exit", true))
    {
        if(portal_index < 0 || portal_index >= Portal_GetSize())
        {
            Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /apotl exit (portal index)");
            Player_Msg(playerid, COLOR_FADE2, "* portal index khong hop le.");
            return 1;
        }

        new Float:pos[3],
            Float:angle;
        
        GetPlayerPos(playerid, pos[0], pos[1], pos[2]);
        GetPlayerFacingAngle(playerid, angle);
        Portal_SetExit(portal_index, pos[0], pos[1], pos[2], angle, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));

        Player_Msg(playerid, COLOR_GREEN, sprintf("> Info: Ban da dieu chinh loi ra cua portal %d ve vi tri hien tai cua ban.", portal_index));
    }
    else if(!strcmp(choice, "togglelock", true))
    {
        if(portal_index < 0 || portal_index >= Portal_GetSize())
        {
            Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /apotl exit (portal index)");
            Player_Msg(playerid, COLOR_FADE2, "* portal index khong hop le.");
            return 1;
        }

        if(Portal_GetLock(portal_index))
        {
            Player_Msg(playerid, COLOR_GREEN, sprintf("> Info: Ban da khoa portal index %d.", portal_index));
        }
        else
        {
            Player_Msg(playerid, COLOR_GREEN, sprintf("> Info: Ban da mo khoa portal index %d.", portal_index));
        }
        Portal_ToggleLock(portal_index);
    }
    else if(!strcmp(choice, "destroy", true))
    {
        if(portal_index < 0 || portal_index >= Portal_GetSize())
        {
            Player_Msg(playerid, COLOR_YELLOW, "> Syntax: /apotl exit (portal index)");
            Player_Msg(playerid, COLOR_FADE2, "* portal index khong hop le.");
            return 1;
        }

        if(Portal_DestroyByIndex(portal_index) == 1)
        {
            Player_Msg(playerid, COLOR_GREEN, sprintf("> Info: Ban da xoa portal index %d thanh cong.", portal_index));
        }
        else
        {
            Player_Msg(playerid, COLOR_RED, sprintf("> ERROR: Xoa portal index %d that bai.", portal_index));
        }
    }
    else 
    {
        Player_Msg(playerid, COLOR_FADE2, "* choice: create, entrance, exit, togglelock, destroy");
    }
    return 1;
}
