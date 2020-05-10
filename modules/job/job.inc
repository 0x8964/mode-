#if defined _module_job_included
    #endinput
#endif

#define _module_job_included

#include <Pawn.CMD>

#include <config-colors>
#include <config-dialog-ids>
#include <utils>

#include <character>

CMD:job(playerid,params[])
{

    if(!IsPlayerInRangeOfPoint(playerid,3.0,1,1,1)) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong dung tai Job Center"); // Toa do xin viec x,y,z = 1,1,1 tu thay doi nhe!
    ShowPlayerDialog(playerid,DIALOG_JOB,DIALOG_STYLE_LIST,"Job Center","Job 1\nJob 2\nJob 3\nJob4","Accept","Cancel");
    return 1;
}
alias:job("xinviec");
CMD:quitjob(playerid,params[])
{
    Player_Msg(playerid,COLOR_WHITE,"[SERVER]Ban da thoat cong viec cua ban!");
    characterData[playerid][char_Job]=0;
    return 1;
}
alias:quitjob("thoatviec");
#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    
    print("INFO: ./modules/job loaded");
    return 1;
}
hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_JOB:
        {
            if(response)
            {
                switch(listitem)
                {
                    case 0://Job 1
                    {
                        Player_Msg(playerid,COLOR_WHITE,"[JOB]Chuc mung ban da nhan duoc Job trucker");
                        
                    }
                    case 1://Job 2
                    {
                        Player_Msg(playerid,COLOR_WHITE,"[JOB]Chuc mung ban da nhan duoc Job 2");
                        
                    }
                    case 2://Job 31
                    {
                        Player_Msg(playerid,COLOR_WHITE,"[JOB]Chuc mung ban da nhan duoc Job 3");
                        
                    }
                    case 3://Job 14
                    {
                        Player_Msg(playerid,COLOR_WHITE,"[JOB]Chuc mung ban da nhan duoc Job 4");
                        
                    }
                }
                characterData[playerid][char_Job] = listitem;
                
            }
        }
        
    }
    return 1;
}


