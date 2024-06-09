using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

using YmirEngine;

public class Iscariot : YmirComponent
{
    public struct Dialogue
    {
        public int ID;
        public string Name;
        public string Text;
        public string Code;
    }
    Dictionary<int, Dialogue> dialogue = new Dictionary<int, Dialogue>();
    string dialoguescsv;
    bool active_Dialogue;

    public Player player;
    public GameObject gameCanvas = null;

    public GameObject canvas_Iscariot = null;
    public GameObject name_Npc = null;
    public GameObject dialogue_Npc = null;
    public GameObject Ybutton = null;
    public GameObject Bbutton = null;
    public GameObject Abutton = null;
    public GameObject Xbutton = null;

    //Popup
    private GameObject popup;

    bool talked;

    public enum Dialogue_id
    {
        ID_0,
        ID_1,
        ID_2,
        ID_3,
        ID_4,
        ID_5,
        ID_6,
        ID_7,
        ID_8,
        ID_9,
        ID_10,
        ID_11,
        ID_12,
        ID_13,
        ID_14,
        ID_15,
        ID_16,
        ID_17,
        ID_18,
        ID_19,
        ID_20,
        ID_21,
        ID_22,
        ID_23,
        ID_24,
        ID_25,
        ID_26,
        ID_27,
        ID_28,
        ID_29,
        ID_30,
        ID_31,
        ID_32,
        ID_33,
        ID_34,
        ID_35,
        ID_36,
        ID_37,
        ID_38,
        ID_39,
        ID_40,
        ID_41,
        ID_42,
        ID_43,
        ID_44,
        ID_45,
        ID_46,
        ID_47,
        ID_48,
        ID_49,
        ID_50,
        ID_51,
        ID_52,
        ID_53,
        ID_54,
        ID_55,
        ID_56,
        ID_57,
        ID_58,
        ID_59,
        ID_60,
        ID_61,
        ID_62,
        ID_63,
        ID_64,
        ID_65,
        ID_66,
        ID_67,
        ID_68,
        ID_69,
        ID_70,
        ID_71,
        ID_72,
        ID_73,
        ID_74,
        ID_75,
        ID_76,
        ID_77,
        ID_78,
        ID_79,
        ID_80,

    }

    public Dialogue_id dialogue_;
    public void Start()
    {
        player = InternalCalls.GetGameObjectByName("Player").GetComponent<Player>();
        gameCanvas = InternalCalls.GetGameObjectByName("Game Canvas");

        active_Dialogue = false;
        canvas_Iscariot = InternalCalls.GetGameObjectByName("Npc_Dialogue");
        name_Npc = InternalCalls.GetGameObjectByName("Name_Npc");
        dialogue_Npc = InternalCalls.GetGameObjectByName("dialogue_Npc");
        dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/Iscariot_Dialogue.csv");
        Ybutton = InternalCalls.GetGameObjectByName("buttonY");
        Bbutton = InternalCalls.GetGameObjectByName("buttonB");
        Abutton = InternalCalls.GetGameObjectByName("buttonA");
        Xbutton = InternalCalls.GetGameObjectByName("buttonX");

        popup = InternalCalls.CS_GetChild(gameObject, 1);

        //Animation - WIP
        Animation.SetLoop(InternalCalls.CS_GetChild(gameObject, 0), "Iscariot_Idle", true);
        Animation.SetSpeed(InternalCalls.CS_GetChild(gameObject, 0), "Iscariot_Idle", 0.2f);
        Animation.PlayAnimation(InternalCalls.CS_GetChild(gameObject, 0), "Iscariot_Idle");

        LoadDialogues(dialoguescsv);
        dialogue_ = Dialogue_id.ID_1;

        talked = false;
    }
    public void Update()
    {
        popup.SetAsBillboard();

        if (active_Dialogue)
        {
            if (popup.IsActive())
            {
                popup.SetActive(false);
            }

            

            //Interacciones - Respuestas
            //player.PlayerStopState(true);
            //IFs de todas las interacciones:
            if (!talked){
                
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_6;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_6;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_53;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.X) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    gameCanvas.SetActive(true);

                    SaveLoad.SaveBool(Globals.saveGameDir, SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame), "True Ending", true);

                    dialogue_ = Dialogue_id.ID_1;
                    //EXIT
                    player.PlayerStopState(false);
                    active_Dialogue = false;
                    canvas_Iscariot.SetActive(false);
                    talked = true;

                    return;
                }
                //ID 6
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_6)
                {
                    dialogue_ = Dialogue_id.ID_10;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_6)
                {
                    dialogue_ = Dialogue_id.ID_35;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_6)
                {
                    gameCanvas.SetActive(true);

                    SaveLoad.SaveBool(Globals.saveGameDir, SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame), "True Ending", true);

                    dialogue_ = Dialogue_id.ID_1;
                    //EXIT
                    player.PlayerStopState(false);
                    active_Dialogue = false;
                    canvas_Iscariot.SetActive(false);
                    talked = true;
                    return;
                }
                //ID 10
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_10)
                {
                    dialogue_ = Dialogue_id.ID_14;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_10)
                {
                    dialogue_ = Dialogue_id.ID_73;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_10)
                {
                    gameCanvas.SetActive(true);

                    SaveLoad.SaveBool(Globals.saveGameDir, SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame), "True Ending", true);

                    dialogue_ = Dialogue_id.ID_1;
                    //EXIT
                    player.PlayerStopState(false);
                    active_Dialogue = false;
                    canvas_Iscariot.SetActive(false);
                    talked = true;
                    return;
                }
                //ID 14
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_14)
                {
                    dialogue_ = Dialogue_id.ID_16;
                    return;
                }
                //ID 16
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_16)
                {
                    dialogue_ = Dialogue_id.ID_18;
                    return;
                }
                //ID 18
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_18)
                {
                    dialogue_ = Dialogue_id.ID_20;
                    return;
                }
                //ID 20
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_20)
                {
                    dialogue_ = Dialogue_id.ID_23;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_20)
                {
                    dialogue_ = Dialogue_id.ID_29;
                    return;
                }
                //ID 23
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_23)
                {
                    dialogue_ = Dialogue_id.ID_25;
                    return;
                }
                //ID 25
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_25)
                {
                    dialogue_ = Dialogue_id.ID_27;
                    return;
                }
                //ID 27
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN ||
                    Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN ||
                    Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN ||
                    Input.GetGamepadButton(GamePadButton.X) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_27)
                {

                    gameCanvas.SetActive(true);

                    SaveLoad.SaveBool(Globals.saveGameDir, SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame), "True Ending", true);

                    dialogue_ = Dialogue_id.ID_1;
                    //EXIT
                    player.PlayerStopState(false);
                    active_Dialogue = false;
                    canvas_Iscariot.SetActive(false);
                    talked = true;
                    return;
                }
                //ID 29
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_29)
                {
                    dialogue_ = Dialogue_id.ID_31;
                    return;
                }
                //ID 31
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_31)
                {
                    dialogue_ = Dialogue_id.ID_33;
                    return;
                }
                //ID 33
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_33)
                {
                    dialogue_ = Dialogue_id.ID_23;
                    return;
                }
                //ID 73
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_73)
                {
                    dialogue_ = Dialogue_id.ID_44;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_73)
                {
                    gameCanvas.SetActive(true);

                    SaveLoad.SaveBool(Globals.saveGameDir, SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame), "True Ending", true);

                    dialogue_ = Dialogue_id.ID_1;
                    //EXIT
                    player.PlayerStopState(false);
                    active_Dialogue = false;
                    canvas_Iscariot.SetActive(false);
                    talked = true;
                    return;
                }
                //ID 44
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_44)
                {
                    dialogue_ = Dialogue_id.ID_46;
                    return;
                }
                //ID 46
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_46)
                {
                    dialogue_ = Dialogue_id.ID_48;
                    return;
                }
                //ID 48
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_48)
                {
                    dialogue_ = Dialogue_id.ID_50;
                    return;
                }
                //ID 50
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN ||
                    Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN ||
                    Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN ||
                    Input.GetGamepadButton(GamePadButton.X) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_50)
                {

                    gameCanvas.SetActive(true);

                    SaveLoad.SaveBool(Globals.saveGameDir, SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame), "True Ending", true);

                    dialogue_ = Dialogue_id.ID_1;
                    //EXIT
                    player.PlayerStopState(false);
                    active_Dialogue = false;
                    canvas_Iscariot.SetActive(false);
                    talked = true;
                    return;
                }
                //ID 35
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_35)
                {
                    dialogue_ = Dialogue_id.ID_37;
                    return;
                }
                //ID 37
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_37)
                {
                    dialogue_ = Dialogue_id.ID_39;
                    return;
                }
                //ID 39
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_39)
                {
                    dialogue_ = Dialogue_id.ID_42;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_39)
                {
                    dialogue_ = Dialogue_id.ID_51;
                    return;
                }
                //ID 42
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_42)
                {
                    dialogue_ = Dialogue_id.ID_44;
                    return;
                }
                //ID 51
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_51)
                {
                    dialogue_ = Dialogue_id.ID_23;
                    return;
                }
                //ID 53
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_53)
                {
                    dialogue_ = Dialogue_id.ID_57;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_53)
                {
                    dialogue_ = Dialogue_id.ID_68;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_53)
                {
                    dialogue_ = Dialogue_id.ID_57;
                    return;
                }
                //ID 57
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_57)
                {
                    dialogue_ = Dialogue_id.ID_59;
                    return;
                }
                //ID 59
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_59)
                {
                    dialogue_ = Dialogue_id.ID_71;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_59)
                {
                    dialogue_ = Dialogue_id.ID_64;
                    return;
                }
                //ID 64
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_64)
                {
                    dialogue_ = Dialogue_id.ID_68;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_64)
                {
                    dialogue_ = Dialogue_id.ID_42;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.A) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_64)
                {
                    gameCanvas.SetActive(true);

                    SaveLoad.SaveBool(Globals.saveGameDir, SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame), "True Ending", true);

                    dialogue_ = Dialogue_id.ID_1;
                    //EXIT
                    player.PlayerStopState(false);
                    active_Dialogue = false;
                    canvas_Iscariot.SetActive(false);
                    talked = true;
                    return;
                }
                //ID 68
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_68)
                {
                    dialogue_ = Dialogue_id.ID_23;
                    return;
                }
                //ID 71
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_71)
                {
                    dialogue_ = Dialogue_id.ID_29;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.B) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_71)
                {
                    dialogue_ = Dialogue_id.ID_64;
                    return;
                }
            }

            DialogueManager();

        }
        else
        {
            popup.SetActive(false);
        }
        return;
    }
    public void DialogueManager()
    {

        //Visual - Diálogos + Respuestas
        switch (dialogue_)
        {

            case Dialogue_id.ID_0:
                dialogue_ = Dialogue_id.ID_1;
                break;

            case Dialogue_id.ID_1:
                UI.TextEdit(name_Npc, dialogue[1].Name);
                UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                UI.TextEdit(Ybutton, dialogue[2].Text);
                UI.TextEdit(Bbutton, dialogue[3].Text);
                UI.TextEdit(Abutton, dialogue[4].Text);
                UI.TextEdit(Xbutton, dialogue[5].Text);
                break;
            case Dialogue_id.ID_6:
                UI.TextEdit(name_Npc, dialogue[6].Name);
                UI.TextEdit(dialogue_Npc, dialogue[6].Text);
                UI.TextEdit(Ybutton, dialogue[7].Text);
                UI.TextEdit(Bbutton, dialogue[8].Text);
                UI.TextEdit(Abutton, dialogue[9].Text);
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_10:
                UI.TextEdit(name_Npc, dialogue[10].Name);
                UI.TextEdit(dialogue_Npc, dialogue[10].Text);
                UI.TextEdit(Ybutton, dialogue[11].Text);
                UI.TextEdit(Bbutton, dialogue[12].Text);
                UI.TextEdit(Abutton, dialogue[13].Text);
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_14:
                UI.TextEdit(name_Npc, dialogue[14].Name);
                UI.TextEdit(dialogue_Npc, dialogue[14].Text);
                UI.TextEdit(Ybutton, dialogue[15].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_16:
                UI.TextEdit(name_Npc, dialogue[16].Name);
                UI.TextEdit(dialogue_Npc, dialogue[16].Text);
                UI.TextEdit(Ybutton, dialogue[17].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_18:
                UI.TextEdit(name_Npc, dialogue[18].Name);
                UI.TextEdit(dialogue_Npc, dialogue[18].Text);
                UI.TextEdit(Ybutton, dialogue[19].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_20:
                UI.TextEdit(name_Npc, dialogue[20].Name);
                UI.TextEdit(dialogue_Npc, dialogue[20].Text);
                UI.TextEdit(Ybutton, dialogue[21].Text);
                UI.TextEdit(Bbutton, dialogue[22].Text);
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_23:
                UI.TextEdit(name_Npc, dialogue[23].Name);
                UI.TextEdit(dialogue_Npc, dialogue[23].Text);
                UI.TextEdit(Ybutton, dialogue[24].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_25:
                UI.TextEdit(name_Npc, dialogue[25].Name);
                UI.TextEdit(dialogue_Npc, dialogue[25].Text);
                UI.TextEdit(Ybutton, dialogue[26].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_27:
                UI.TextEdit(name_Npc, dialogue[27].Name);
                UI.TextEdit(dialogue_Npc, dialogue[27].Text);
                UI.TextEdit(Ybutton, dialogue[28].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_29:
                UI.TextEdit(name_Npc, dialogue[29].Name);
                UI.TextEdit(dialogue_Npc, dialogue[29].Text);
                UI.TextEdit(Ybutton, dialogue[30].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_31:
                UI.TextEdit(name_Npc, dialogue[31].Name);
                UI.TextEdit(dialogue_Npc, dialogue[31].Text);
                UI.TextEdit(Ybutton, dialogue[32].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_33:
                UI.TextEdit(name_Npc, dialogue[33].Name);
                UI.TextEdit(dialogue_Npc, dialogue[33].Text);
                UI.TextEdit(Ybutton, dialogue[34].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_73:
                UI.TextEdit(name_Npc, dialogue[73].Name);
                UI.TextEdit(dialogue_Npc, dialogue[73].Text);
                UI.TextEdit(Ybutton, dialogue[43].Text);
                UI.TextEdit(Bbutton, dialogue[74].Text);
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_44:
                UI.TextEdit(name_Npc, dialogue[44].Name);
                UI.TextEdit(dialogue_Npc, dialogue[44].Text);
                UI.TextEdit(Ybutton, dialogue[45].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_46:
                UI.TextEdit(name_Npc, dialogue[46].Name);
                UI.TextEdit(dialogue_Npc, dialogue[46].Text);
                UI.TextEdit(Ybutton, dialogue[47].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_48:
                UI.TextEdit(name_Npc, dialogue[48].Name);
                UI.TextEdit(dialogue_Npc, dialogue[48].Text);
                UI.TextEdit(Ybutton, dialogue[49].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_50:
                UI.TextEdit(name_Npc, dialogue[50].Name);
                UI.TextEdit(dialogue_Npc, dialogue[50].Text);
                UI.TextEdit(Ybutton, dialogue[51].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_35:
                UI.TextEdit(name_Npc, dialogue[35].Name);
                UI.TextEdit(dialogue_Npc, dialogue[35].Text);
                UI.TextEdit(Ybutton, dialogue[36].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_37:
                UI.TextEdit(name_Npc, dialogue[37].Name);
                UI.TextEdit(dialogue_Npc, dialogue[37].Text);
                UI.TextEdit(Ybutton, dialogue[38].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_39:
                UI.TextEdit(name_Npc, dialogue[39].Name);
                UI.TextEdit(dialogue_Npc, dialogue[39].Text);
                UI.TextEdit(Ybutton, dialogue[40].Text);
                UI.TextEdit(Bbutton, dialogue[41].Text);
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_42:
                UI.TextEdit(name_Npc, dialogue[42].Name);
                UI.TextEdit(dialogue_Npc, dialogue[42].Text);
                UI.TextEdit(Ybutton, dialogue[43].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_51:
                UI.TextEdit(name_Npc, dialogue[51].Name);
                UI.TextEdit(dialogue_Npc, dialogue[51].Text);
                UI.TextEdit(Ybutton, dialogue[21].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_53:
                UI.TextEdit(name_Npc, dialogue[53].Name);
                UI.TextEdit(dialogue_Npc, dialogue[53].Text);
                UI.TextEdit(Ybutton, dialogue[54].Text);
                UI.TextEdit(Bbutton, dialogue[55].Text);
                UI.TextEdit(Abutton, dialogue[56].Text);
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_57:
                UI.TextEdit(name_Npc, dialogue[57].Name);
                UI.TextEdit(dialogue_Npc, dialogue[57].Text);
                UI.TextEdit(Ybutton, dialogue[58].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_59:
                UI.TextEdit(name_Npc, dialogue[59].Name);
                UI.TextEdit(dialogue_Npc, dialogue[59].Text);
                UI.TextEdit(Ybutton, dialogue[60].Text);
                UI.TextEdit(Bbutton, dialogue[63].Text);
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_64:
                UI.TextEdit(name_Npc, dialogue[64].Name);
                UI.TextEdit(dialogue_Npc, dialogue[64].Text);
                UI.TextEdit(Ybutton, dialogue[2].Text);
                UI.TextEdit(Bbutton, dialogue[66].Text);
                UI.TextEdit(Abutton, dialogue[67].Text);
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_68:
                UI.TextEdit(name_Npc, dialogue[68].Name);
                UI.TextEdit(dialogue_Npc, dialogue[68].Text);
                UI.TextEdit(Ybutton, dialogue[69].Text);
                UI.TextEdit(Bbutton, " ");
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;
            case Dialogue_id.ID_71:
                UI.TextEdit(name_Npc, dialogue[71].Name);
                UI.TextEdit(dialogue_Npc, dialogue[71].Text);
                UI.TextEdit(Ybutton, dialogue[62].Text);
                UI.TextEdit(Bbutton, dialogue[63].Text);
                UI.TextEdit(Abutton, " ");
                UI.TextEdit(Xbutton, " ");
                break;

        }
    }
    public void OnCollisionStay(GameObject other)
    {
        if (other.Tag == "Player" && !active_Dialogue)
        {
            popup.SetActive(true);
        }

        if (other.Tag == "Player" && (Input.IsGamepadButtonAPressedCS() || Input.GetKey(YmirKeyCode.SPACE) == KeyState.KEY_DOWN) && !active_Dialogue)
        {
            gameCanvas.SetActive(false);

            canvas_Iscariot.SetActive(true);
            active_Dialogue = true;
            player.PlayerStopState(true);
        }
    }
    public void OnCollisionExit(GameObject other)
    {
        if (other.Tag == "Player")
        {
            canvas_Iscariot.SetActive(false);
            active_Dialogue = false;
            dialogue_ = Dialogue_id.ID_1;
        }
    }
    public void LoadDialogues(string dialogueData)
    {
        string[] lines = dialogueData.Split(new string[] { "<end>" }, System.StringSplitOptions.RemoveEmptyEntries);

        foreach (string line in lines)
        {
            string[] dialogueParts = line.Split(';');

            if (dialogueParts.Length >= 4)
            {
                Dialogue _dialogue = new Dialogue();
                Debug.Log("[WARNING] 1");
                _dialogue.ID = int.Parse(dialogueParts[0]);
                Debug.Log("[WARNING] 1");
                _dialogue.Name = dialogueParts[1];
                Debug.Log("[WARNING] 4");
                _dialogue.Text = dialogueParts[2];
                Debug.Log("[WARNING] 5" + _dialogue.Text);
                _dialogue.Code = dialogueParts[3];
                Debug.Log("[WARNING] 6");

                dialogue.Add(_dialogue.ID, _dialogue);
                Debug.Log("[WARNING] Ended");
            }
        }

        //Debug.Log("[WARNING] GG Loading dialogue data" + lines[0]);
    }
}