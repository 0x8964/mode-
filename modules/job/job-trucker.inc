#if defined _module_job_trucker_included
    #endinput
#endif

#define _module_job_trucker_included
#define MAX_SLOT_TRUCK 5

#define PRESSED(%0) (((newkeys & (%0)) == (%0)) && ((oldkeys & (%0)) != (%0)))

#include <Pawn.CMD>
#include <config-colors>
#include <config-dialog-ids>
#include<character>
//Truck
new gCheckPoint[MAX_PLAYERS]=0;
new TruckerVeh[3];
new bool:TruckLock[MAX_VEHICLES]=false;
new TruckUsedVeh[MAX_PLAYERS] = INVALID_VEHICLE_ID;
new TruckType[MAX_VEHICLES char] = 0;//type = 0 none,Type = 1 ->hop phap, type = 2 bat hop phap
new TruckSlot[MAX_VEHICLES] = 0; // So luong hang hoa tren xe truck
new LoadTruck[MAX_PLAYERS] = 0;//Loadtruck = 1 -> chathang, =2 -> Giao hang, =3 nhan tien
new Points[8][MAX_PLAYERS];
new Float:TruckCord[][] = 
{
    {1249.7258, -2047.9263, 59.9209, 90.2055},  
    {1241.2084, -2057.6521, 60.0190, 94.9352},  
    {1241.0105, -2052.6873, 59.9975, 2.8144},  
    {718.4906, -1477.3024, 5.4688, 357.9947}, 
    {722.3772, -1477.2856, 5.4688, 272.3814}, 
    {1249.7258, -2047.9263, 59.9209, 90.2055},  
    {1241.2084, -2057.6521, 60.0190, 94.9352},  
    {1241.0105, -2052.6873, 59.9975, 2.8144}, 
    {718.4906, -1477.3024, 5.4688, 357.9947}, 
    {722.3772, -1477.2856, 5.4688, 272.3814}  
};
stock veh_lock_doors(vehicle)
 {

	new vParamArr[7];
	GetVehicleParamsEx(vehicle, vParamArr[0], vParamArr[1], vParamArr[2], vParamArr[3], vParamArr[4], vParamArr[5], vParamArr[6]);
	return SetVehicleParamsEx(vehicle, vParamArr[0], vParamArr[1], vParamArr[2], VEHICLE_PARAMS_ON, vParamArr[4], vParamArr[5], vParamArr[6]);
}

stock veh_unlock_doors(vehicle) {

	 
    new vParamArr[7];

	GetVehicleParamsEx(vehicle, vParamArr[0], vParamArr[1], vParamArr[2], vParamArr[3], vParamArr[4], vParamArr[5], vParamArr[6]);
	return SetVehicleParamsEx(vehicle, vParamArr[0], vParamArr[1], vParamArr[2], VEHICLE_PARAMS_OFF, vParamArr[4], vParamArr[5], vParamArr[6]);
}


stock IsInTruckVeh(veh)
{
    for(new v=0;v<=sizeof(TruckerVeh);v++)
    {
        if(veh == TruckerVeh[v])
        {
            return 1;
        }
    }
    return 0;
}
stock GetCenterOfTrunk(vehicleid, &Float:x, &Float:y, &Float:z)//Forum samp
{
	new model = GetVehicleModel(vehicleid);

	if (!model)
	    return 0;

	new Float:fPosition[5];

	GetVehicleZAngle(vehicleid, fPosition[3]);
	GetVehiclePos(vehicleid, fPosition[0], fPosition[1], fPosition[2]);
	GetVehicleModelInfo(model, VEHICLE_MODEL_INFO_REAR_BUMPER_Z, x, y, z);

	fPosition[4] = z;

	x += fPosition[0];
	y += fPosition[1];

	x -= (2.35 * (floatsin(-fPosition[3], degrees)));
	y -= (2.35 * (floatcos(fPosition[3], degrees)));
 	return 1;
}
CMD:canceltruck(playerid,params[])
{
    if(characterData[playerid][char_Job]!=1) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong phai trucker");
    
    if(TruckUsedVeh[playerid] == GetPlayerVehicleID(playerid) && LoadTruck[playerid]>=2)
    {
        new vehicle = TruckUsedVeh[playerid];
        TruckType{vehicle}  = 0; //Set hang hoa "None"
        TruckSlot[vehicle] = 0;//Set hang hoa tren xe = 0
        LoadTruck[playerid] = 0;
        SetVehicleToRespawn(vehicle);
        Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Ban da huy chuyen hang");
        
    }
    return 1;
}
alias:canceltruck("huyhang");
CMD:trunkopen(playerid,params[])
{
    if(characterData[playerid][char_Job]!=1) return 1;
    new veh = GetPlayerVehicleID(playerid);
    if(TruckUsedVeh[playerid] == veh && IsPlayerInAnyVehicle(playerid))
    {
        new engine,lights,alarm,doors,bonnet,boot,objective;
        GetVehicleParamsEx(veh,engine,lights,alarm,doors,bonnet,boot,objective);
        if(boot == VEHICLE_PARAMS_ON)
        {
            SetVehicleParamsEx(veh,engine,lights,alarm,doors,bonnet,VEHICLE_PARAMS_OFF,objective);
            Player_Msg(playerid, COLOR_WHITE, "Thung xe dong thanh cong.");
        }
        else if(boot == VEHICLE_PARAMS_OFF || boot == VEHICLE_PARAMS_UNSET)
        {
            SetVehicleParamsEx(veh,engine,lights,alarm,doors,bonnet,VEHICLE_PARAMS_ON,objective);
            Player_Msg(playerid, COLOR_WHITE, "Thung xe mo thanh cong.");
        }
    }
    return 1;
}
alias:trunkopen("copxe");
CMD:truck(playerid,params[])
{
    if(characterData[playerid][char_Job])
    {
        new veh = GetPlayerVehicleID(playerid);
        if(IsInTruckVeh(veh) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER && LoadTruck[playerid]==0)
        {
            if(gCheckPoint[playerid]!=0) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban dang co checkpoint tren ban do");
            if(TruckSlot[veh]>0 && TruckUsedVeh[playerid] == veh) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban dang co 1 chuyen hang khac");
            Player_Msg(playerid,COLOR_WHITE,"[JOB]Hay di toi diem do tren ban do de lay hang");
            SetPlayerCheckpoint(playerid,1,1,1,4);
            gCheckPoint[playerid] = 1234;//code checkpoint truck lay hang = 1234
        }
    }
    return 1;
}
alias:truck("layhang");
CMD:tlock(playerid,params[])
{
    if(characterData[playerid][char_Job]!=1) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong phai trucker!");
    if(!IsPlayerInAnyVehicle(playerid)) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong o tren phuong tien nao");
    new veh = GetPlayerVehicleID(playerid);
    if(!IsInTruckVeh(veh)) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Day khong phai xe trucker");
    if(TruckUsedVeh[playerid] != veh) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Day khong phai la phuong tien ban dang ky de giao hang");
    if(TruckLock[veh] == false)
    {
        TruckLock[veh] = true;
        veh_lock_doors(veh);
        Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Ban da khoa cua xe truck");
    }
    else
    {
        TruckLock[veh] = false;
        veh_unlock_doors(veh);
        Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Ban da mo khoa xe truck,hay can than bi cuop hang");
    }
    return 1;
}
CMD:chathang(playerid,params[])
{
    if(characterData[playerid][char_Job]!=1) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong phai trucker!");
    if(TruckSlot[TruckUsedVeh[playerid]] >MAX_SLOT_TRUCK) return Player_Msg(playerid,COLOR_WHITE,"[TRUCKBan khong the chat them hang hoa len xe");
    if(TruckUsedVeh[playerid] == INVALID_VEHICLE_ID ) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban chua lay hang nen khong the chat hang len xe");
    if(IsPlayerInAnyVehicle(playerid)) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban phai xuong xe de co the chat hang len xe!");
    if(!IsPlayerInRangeOfPoint(playerid,30,40,50,1)) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong o gan dia diem chat hang!");//Kiem tra dia diem chat hang 
    if(IsPlayerAttachedObjectSlotUsed(playerid, 9)) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban dang co 1 thung hang tren tay, hay chuyen no toi cop xe"); 
    if(LoadTruck[playerid]==2) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban dang giao hang, khong the chat hang len xe duoc!");
    ApplyAnimation(playerid,"CARRY","crry_prtial",4.1,1,1,1,1,1,1);
    SetPlayerAttachedObject( playerid, 9, 1271, 1, 0.002953, 0.469660, -0.009797, 269.851104, 88.443557, 0.000000, 0.804894, 1.000000, 0.822361 );        Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Hay dua thung hang toi cop xe roi an nut H de chat hang len cop xe!");
    Player_Msg(playerid,COLOR_WHITE,"[LUU Y]Khi ban len xe thi hang hoa se duoc bat dau van chuyen!");
    LoadTruck[playerid] = 1;
    return 1;
}

#include <YSI_Coding\y_hooks>

hook OnGameModeInit()
{
    //Load Truck Vehicles
    TruckerVeh[0] = AddStaticVehicle(414,0,0,0,0,6,0); // 
    TruckerVeh[1] = AddStaticVehicle(414,0,0,0,0,6,0); // 
    TruckerVeh[2] = AddStaticVehicle(414,0,0,0,0,6,0); // 
    //Dia diem chat hang 
    Create3DTextLabel("Dia diem chat hang", 0x008080FF, 30.0, 40.0, 50.0, 40.0, 0, 0);
    return 1;
}
hook OnPlayerConnect(playerid)
{
    //Detlete truck
    gCheckPoint[playerid] = 0;
    RemovePlayerAttachedObject(playerid,9);//Remove slot box truck;
    LoadTruck[playerid] = 0;
    for(new i=0;i<=sizeof(Points);i++) Points[i][playerid]=0;
    return 1;

}
hook OnPlayerDisconnect(playerid)
{
    //Detlete hang hoa truck
    if(TruckUsedVeh[playerid]!=INVALID_VEHICLE_ID)
    {
        new vehicle = TruckUsedVeh[playerid];
        TruckType{vehicle}  = 0; //Set hang hoa "None"
        TruckSlot[vehicle] = 0;//Set hang hoa tren xe = 0
        TruckLock[vehicle] = false;
        SetVehicleToRespawn(vehicle);
        TruckUsedVeh[playerid] = INVALID_VEHICLE_ID;
    }
    return 1;
}

hook OnPlayerStateChange(playerid, newstate, oldstate)
{
    if(newstate == PLAYER_STATE_DRIVER) 
    {
        new veh = GetPlayerVehicleID(playerid);
        if(IsInTruckVeh(veh)) 
        {
            if(!characterData[playerid][char_Job]) //Khong phai job truck
            {
                Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong len chiec xe nay");
                RemovePlayerFromVehicle(playerid);
            }
            else if (characterData[playerid][char_Job])//La job truck
            {
                new string[70],trucktypes[10];
                switch(TruckType[veh])
                {
                    case 0:format(trucktypes,10,"Empty");
                    case 1:format(trucktypes,10,"Hop phap");
                    case 2:format(trucktypes,10,"Bat hop phap");
                
                }
                format(string, sizeof(string), "TRUCK: (Dang ky xe:%d) - (Hang hoa: %s{FFFF00})",veh, trucktypes);
				Player_Msg(playerid, COLOR_YELLOW, string);
                if(veh == TruckUsedVeh[playerid] && TruckSlot[veh]>0)
                {
                    if(LoadTruck[playerid] == 1)
                    {
                        RemovePlayerAttachedObject(playerid,9);
                        ClearAnimations(playerid);
                        Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Hay di den checkpoint de tiep tuc giao hang");
                        LoadTruck[playerid]=2;
                        new rand;
                        rand = random(sizeof(TruckCord));
                        SetPlayerCheckpoint(playerid,TruckCord[rand][0],TruckCord[rand][1],TruckCord[rand][2],4);
                        gCheckPoint[playerid] = 1235;//Checkpoint giao hang
                        Points[rand][playerid]=true;
                    }
                    Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Day la phuong tien cua ban, hay tiep tuc giao hang");
                    return 1;
                }
                if(TruckType[veh]>0 && TruckSlot[veh]>0) return Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Day khong phai phuong tien cua ban, co the cuop chuyen hang nay");
            }
        }
    }
    return 1;
}
hook OnPlayerKeyStateChange(playerid, newkeys, oldkeys)
{
	if (PRESSED(KEY_CTRL_BACK))
	{
		if(LoadTruck[playerid])
        {
            new Float:x,Float:y,Float:z;
            if(TruckUsedVeh[playerid]!=INVALID_VEHICLE_ID)
            {  
                new veh = TruckUsedVeh[playerid];
                GetVehiclePos(veh,x,y,z);
                if(IsPlayerInRangeOfPoint(playerid,x,y,z,1.5))
                {
                    GetCenterOfTrunk(veh,x,y,z);
                    if(!IsPlayerInRangeOfPoint(playerid,x,y,z,1)) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong o gan cop xe de chat hang");
                    new engine,lights,alarm,doors,bonnet,boot,objective;
                    GetVehicleParamsEx(veh,engine,lights,alarm,doors,bonnet,boot,objective);
                    if(boot == VEHICLE_PARAMS_OFF) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban chua mo thung xe nen khong the chat hang");
                    if(TruckSlot[veh]>MAX_SLOT_TRUCK) return Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong the chat hang hoa len xe");
                    RemovePlayerAttachedObject(playerid,9);
                    TruckSlot[veh]+=1;
                    new s[128];
                    format(s,20,"Ban da dua 1 thung hang len xe [so luong hang hoa %d/5]",TruckSlot[veh]);
                    ClearAnimations(playerid);
                }
                else Player_Msg(playerid,COLOR_WHITE,"[ERROR]Ban khong dung gan xe truck cua ban");
            }
        }
	}
	return 1;
}

hook OnDialogResponse(playerid, dialogid, response, listitem, inputtext[])
{
    switch(dialogid)
    {
        case DIALOG_TRUCK_GIAOHANG:
        {
            if(response)//OK BUTTON
            {
                new vehicleid = GetPlayerVehicleID(playerid);
	   	        if(IsInTruckVeh(vehicleid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
                {
                    TruckSlot[vehicleid] = 0;
                    TruckType[vehicleid]=listitem+1;//Set type truck hop phap,bat hop phap
                    TruckUsedVeh[playerid] = vehicleid;
                    Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Ban hay mo cop xe[/copxe] va xuong chat hang len xe");
                    Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Su dung /chathang, an nut H de dua hang hoa len cop xe");
                    Player_Msg(playerid,COLOR_WHITE,"[LUU Y]Hay su dung /tlock de tranh bi cuop hang");
                }
            }                
        }
    } 
    return 1;
}

hook OnPlayerEnterCheckpoint(playerid)
{
    if(gCheckPoint[playerid]==1234)//Checkpoint lay hang
    {
		new vehicleid = GetPlayerVehicleID(playerid);
	   	if(IsInTruckVeh(vehicleid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER)
		{
            DisablePlayerCheckpoint(playerid);
            gCheckPoint[playerid] = 0;
            ShowPlayerDialog(playerid,DIALOG_TRUCK_GIAOHANG,DIALOG_STYLE_LIST,"Hang hoa","Hop phap\nBat hop phap","OK","Cancel");
        }
		else Player_Msg(playerid, COLOR_WHITE, "[JOB]Ban khong o tren xe truck!");
    }
    if(gCheckPoint[playerid]==1235)//Checkpoint giao hang
    {
        new vehicleid = GetPlayerVehicleID(playerid);
	   	if(IsInTruckVeh(vehicleid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER && TruckUsedVeh[playerid]==vehicleid)
		{
            DisablePlayerCheckpoint(playerid);
            if(TruckSlot[vehicleid]>0 && LoadTruck[playerid]==2)
            {
                new s[100];format(s,100,"[TRUCK]Ban da giao hang, hay toi dia diem tiep theo (hang hoa %d/5)",TruckSlot[vehicleid]);
                Player_Msg(playerid,COLOR_WHITE,s);
                TruckSlot[vehicleid] -=1;
                new rand;
                for(new i=0;i<=8;i++)
                {
                    rand = random(sizeof(TruckCord));
                    if(Points[rand][playerid]!=1) break;
                }     
                SetPlayerCheckpoint(playerid,TruckCord[rand][0],TruckCord[rand][1],TruckCord[rand][2],4);
            }
            else //Tro lai ben cang nhan tien
            {
                LoadTruck[playerid] = 3;
                Player_Msg(playerid,COLOR_WHITE,"[TRUCK]Hay quay tro lai ben cang de nhan tien!");
                gCheckPoint[playerid]=1236;//Nhan tien
                SetPlayerCheckpoint(playerid,1234,1235,1236,4);
            
            }
        }
		else Player_Msg(playerid, COLOR_WHITE, "[JOB]Ban khong o tren xe truck!");
    
    }
    if(gCheckPoint[playerid]==1236)//Nhan tien truck
    {
        new vehicleid = GetPlayerVehicleID(playerid);
	   	if(IsInTruckVeh(vehicleid) && GetPlayerState(playerid) == PLAYER_STATE_DRIVER && TruckUsedVeh[playerid]==vehicleid && LoadTruck[playerid]==3)
		{
            DisablePlayerCheckpoint(playerid);
            GivePlayerMoney(playerid,1000);
            Player_Msg(playerid,COLOR_WHITE,"[JOB]Ban nhan duoc 1000$");
            TruckType{vehicleid}  = 0; //Set hang hoa "None"
            TruckSlot[vehicleid] = 0;//Set hang hoa tren xe = 0
            TruckLock[vehicleid] = false;
            SetVehicleToRespawn(vehicleid);
            TruckUsedVeh[playerid] = INVALID_VEHICLE_ID;
            LoadTruck[playerid] = 0;
        }
		else Player_Msg(playerid, COLOR_WHITE, "[JOB]Ban khong o tren xe truck!");
    
    }
    return 1;
}
