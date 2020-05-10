#if defined _module_account_auth_included
    #endinput
#endif

#define _module_account_auth_included

#include <PawnPlus>
#include <pp-mysql>
#include <strlib>
#include <bcrypt>
#include <samp-precise-timers>

#include <config-msg-prefixes>
#include <config-dialog-ids>

#include <mysql>
#include <character>

static const BCRYPT_COST = 12;
static const ACCOUNT_REGISTER = 0;
static const ACCOUNT_LOGIN = 1; 

forward bool:Character_IsSpawned(const playerid);

stock Task:Account_IsRegistered(const name[])
{
    new Task:my_task = task_new();
    task_detach();
    task_yield(my_task);
    
    await mysql_aquery_s(MySQL_GetHandle(), str_format("SELECT COUNT(*) FROM accounts WHERE name = '%e' AND password = 'chuaco' LIMIT 1", name)); // Nếu tìm thấy row chứa name và password = 'chuaco' sẽ trả về true

    new counts = 0;
    cache_get_value_name_int(0, "COUNT(*)", counts);
    task_set_result(my_task, (counts > 0) ? ACCOUNT_LOGIN : ACCOUNT_REGISTER); // Counts >1 (true) trả về ACCOUNT_LOGIN, counts = 0 (false) trả về ACCOUNT_REGISTER
    return Task:0;
}

forward OnAccountPasswordHashed(const playerid);
public OnAccountPasswordHashed(const playerid)
{
    new hash[BCRYPT_HASH_LENGTH],
        uuid[UUID_LEN],
        name[MAX_PLAYER_NAME + 1];
    bcrypt_get_hash(hash);
    UUID(uuid, sizeof(uuid));
    GetPlayerName(playerid, name, sizeof(name));

    task_yield(1);
    await mysql_aquery_s(MySQL_GetHandle(), str_format("UPDATE accounts SET uuid = '%e', password = '%e' WHERE name = '%e'", uuid, hash, name)); // set thành UPDATE vì đã đăng ký ở launcher rồi

    new auth = await Account_IsRegistered(name);
    if(auth == ACCOUNT_LOGIN)
    {
        if(GetPVarType(playerid, "accountAuth_Timer"))
        {
            DeletePreciseTimer(GetPVarInt(playerid, "accountAuth_Timer"));
        }

        SetPVarInt(playerid, "accountAuth_Timer", SetPreciseTimer("OnAuthenticationExpired", 60000, false, "i", playerid));

        //SendClientMessage(playerid, -1, MSG_PREFIX_ALT"Ban co 60 giay de hoan tat qua trinh nhan dien tai khoan va nhan vat.");
        SendClientMessage(playerid, -1, MSG_PREFIX_ALT"B¥n cÅ 60s ³¼ ho¢n tét qu¡ trÀnh nh±n di¾n t¢i kho£n v¢ nh¬n v±t.");
        //ShowPlayerDialog(playerid, DIALOG_ACCOUNT_LOGIN, DIALOG_STYLE_PASSWORD, "Account | Login", sprintf("{FFFFFF}Tai khoan '%s' da duoc dang ky, nhap ma pin de dang nhap.", name), "Dang nhap", "Thoat");
        ShowPlayerDialog(playerid, DIALOG_ACCOUNT_LOGIN, DIALOG_STYLE_PASSWORD, "Account | Login", sprintf("{FFFFFF}T¢i kho£n '%s' ³¤ ³ÚÔc ³¦ng kà, nh±p m¤ pin ³¼ ³¦ng nh±p.", name), "²¦ng nh±p", "Tho¡t");
    }
    else
    {
        if(GetPVarType(playerid, "accountAuth_Timer"))
        {
            DeletePreciseTimer(GetPVarInt(playerid, "accountAuth_Timer"));
        }

        SetPVarInt(playerid, "accountAuth_Timer", SetPreciseTimer("OnAuthenticationExpired", 180000, false, "i", playerid));

        //SendClientMessage(playerid, -1, MSG_PREFIX_ALT"Ban co 180 giay de hoan tat qua trinh dang ky tai khoan va nhan vat.");
        SendClientMessage(playerid, -1, MSG_PREFIX_ALT"B¥n cÅ 180 gi¬y ³¼ ho¢n tét qu¡ trÀnh ³¦ng kà m¤ pin v¢ nh¬n v±t.");
        ShowPlayerDialog(playerid, DIALOG_ACCOUNT_REGISTER, DIALOG_STYLE_PASSWORD, "Account | Enter Pin", sprintf("{FFFFFF}T¢i kho£n '%s' chÚa ³¦ng kà m¤ pin, vui lÆng ³¦ng kà m¤ pin.", name), "²¦ng kà", "Tho¡t");
    }
	return 1;
}

forward OnAccountPasswordChecked(const playerid, const uuid[]);
public OnAccountPasswordChecked(const playerid, const uuid[])
{
    if(bcrypt_is_equal())
    {   
        CallLocalFunction("Account_OnLogin", "is", playerid, uuid);
    }
    else
    {
        new attempts = GetPVarInt(playerid, "accountAuth_Attempts");
        SetPVarInt(playerid, "accountAuth_Attempts", --attempts);
        if(attempts <= 0)
        {
            //Player_Kick(playerid, -1, MSG_PREFIX_ALT"Ban da vuot qua so lan nhap sai mat khau.");
            Player_Kick(playerid, -1, MSG_PREFIX_ALT"B¥n ³¤ vÚÔt qu¡ sÊ l®n ³¦ng nh±p sai m±t kh¯u.");
        }
        else
        {
            //Player_Msg(playerid, -1, sprintf(MSG_PREFIX_ALT"Ban con lai %d lan nhap sai mat khau.", attempts));
            Player_Msg(playerid, -1, sprintf(MSG_PREFIX_ALT"B¥n cÆn l¥i %d l®n nh±p sai m±t kh¯u.", attempts));
        }

        new name[MAX_PLAYER_NAME+1];
        GetPlayerName(playerid, name, sizeof(name));
        ShowPlayerDialog(playerid, DIALOG_ACCOUNT_LOGIN, DIALOG_STYLE_PASSWORD, "Account | Login", sprintf("{FFFFFF}T¢i kho£n '%s' ³¤ ³ÚÔc ³¦ng kà, nh±p m¤ pin ³¼ ³¦ng nh±p.", name), "²¦ng nh±p", "Tho¡t");
    }
    return 1;
}

forward OnAuthenticationExpired(const playerid);
public OnAuthenticationExpired(const playerid)
{
    if(!Character_IsSpawned(playerid))
    {
        //Player_Kick(playerid, -1, MSG_PREFIX_ALT"Thoi gian dang nhap/dang ky da het.");
        Player_Kick(playerid, -1, MSG_PREFIX_ALT"ThÑi gian ³¦ng nh±p/³¦ng kà ³¤ hºt.");
    }
    return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    print("INFO: ./modules/account-auth loaded");
    return 1;
}

hook OnPlayerConnect(playerid)
{
    TogglePlayerSpectating(playerid, true);
    
    new name[MAX_PLAYER_NAME+1];
    GetPlayerName(playerid, name, sizeof(name));

    new auth = await Account_IsRegistered(name);
    SetPVarInt(playerid, "accountAuth_Timestamp", gettime());
    SetPVarInt(playerid, "accountAuth_Attempts", 3);
    if(auth == 1)
    {
        if(GetPVarType(playerid, "accountAuth_Timer"))
        {
            DeletePreciseTimer(GetPVarInt(playerid, "accountAuth_Timer"));
        }

        SetPVarInt(playerid, "accountAuth_Timer", SetPreciseTimer("OnAuthenticationExpired", 60000, false, "i", playerid));
        //SendClientMessage(playerid, -1, MSG_PREFIX_ALT"Ban co 60 giay de hoan tat qua trinh nhan dien tai khoan va nhan vat.");
        SendClientMessage(playerid, -1, MSG_PREFIX_ALT"B¥n cÅ 60s ³¼ ho¢n tét qu¡ trÀnh nh±n di¾n t¢i kho£n v¢ nh¬n v±t.");
        //ShowPlayerDialog(playerid, DIALOG_ACCOUNT_LOGIN, DIALOG_STYLE_PASSWORD, "Account | Login", sprintf("{FFFFFF}Tai khoan '%s' da duoc dang ky, nhap ma pin de dang nhap.", name), "Dang nhap", "Thoat");
        ShowPlayerDialog(playerid, DIALOG_ACCOUNT_LOGIN, DIALOG_STYLE_PASSWORD, "Account | Login", sprintf("{FFFFFF}T¢i kho£n '%s' ³¤ ³ÚÔc ³¦ng kà, nh±p m¤ pin ³¼ ³¦ng nh±p.", name), "²¦ng nh±p", "Tho¡t");
    }
    else
    {
        if(GetPVarType(playerid, "accountAuth_Timer"))
        {
            DeletePreciseTimer(GetPVarInt(playerid, "accountAuth_Timer"));
        }

        SetPVarInt(playerid, "accountAuth_Timer", SetPreciseTimer("OnAuthenticationExpired", 180000, false, "i", playerid));
        SendClientMessage(playerid, -1, MSG_PREFIX_ALT"B¥n cÅ 180 gi¬y ³¼ ho¢n tét qu¡ trÀnh ³¦ng kà m¤ pin v¢ nh¬n v±t.");
        ShowPlayerDialog(playerid, DIALOG_ACCOUNT_REGISTER, DIALOG_STYLE_PASSWORD, "Account | Enter Pin", sprintf("{FFFFFF}T¢i kho£n '%s' chÚa ³¦ng kà m¤ pin, vui lÆng ³¦ng kà m¤ pin.", name), "²¦ng kà", "Tho¡t");
    }
    return 1;
}

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_ACCOUNT_REGISTER:
        {
            if(response)
            {
                if(isnull(inputtext) || strlen(inputtext) > 127)
                {
                    new name[MAX_PLAYER_NAME+1];
                    GetPlayerName(playerid, name, sizeof(name));
                    ShowPlayerDialog(playerid, DIALOG_ACCOUNT_REGISTER, DIALOG_STYLE_PASSWORD, "Account | Enter Pin", sprintf("{FFFFFF}T¢i kho£n '%s' chÚa ³¦ng kà m¤ pin, vui lÆng ³¦ng kà m¤ pin.", name), "²¦ng kà", "Tho¡t");
                    return 1;
                }

                bcrypt_hash(inputtext, BCRYPT_COST, "OnAccountPasswordHashed", "i", playerid);
            }
            else
            {
                //Player_Kick(playerid, -1, MSG_PREFIX_ALT"Ban da ngat ket noi voi ALT:Roleplay.");
                Player_Kick(playerid, -1, MSG_PREFIX_ALT"B¥n ³¤ ng§t kºt nÊi vÐi SvR:RPG.");
                
            }
        }
        case DIALOG_ACCOUNT_LOGIN:
        {
            if(response)
            {
                new name[MAX_PLAYER_NAME+1];
                GetPlayerName(playerid, name, sizeof(name));
                if(isnull(inputtext) || strlen(inputtext) > 127)
                {
                    
                    ShowPlayerDialog(playerid, DIALOG_ACCOUNT_LOGIN, DIALOG_STYLE_PASSWORD, "Account | Login", sprintf("{FFFFFF}T¢i kho£n '%s' ³¤ ³ÚÔc ³¦ng kà, nh±p m¤ pin ³¼ ³¦ng nh±p.", name), "²¦ng nh±p", "Tho¡t");
                    return 1;
                }
                
                task_yield(1);
                await mysql_aquery_s(MySQL_GetHandle(), str_format("SELECT uuid, password FROM accounts WHERE name = '%e' LIMIT 1", name));

                if(cache_num_rows() > 0)
                {
                    new hash[BCRYPT_HASH_LENGTH],
                        uuid[UUID_LEN];
                    
                    cache_get_value_name(0, "uuid", uuid, sizeof(uuid));
                    cache_get_value_name(0, "password", hash, sizeof(hash));

                    bcrypt_check(inputtext, hash, "OnAccountPasswordChecked", "is", playerid, uuid);
                }
                else
                {
                    Player_Kick(playerid, -1, MSG_PREFIX_ALT"H¾ thÊng khÄng tÀm théy t¹n t¢i kho£n tÚÏng Ûng vÐi t¹n c×a b¥n.");
                }
            }
            else
            {
                Player_Kick(playerid, -1, MSG_PREFIX_ALT"B¥n ³¤ ng§t kºt nÊi vÐi "EMBED_COLOR_ALT"SvR:RPG.");
            }
        }
    }
    return 1;
}