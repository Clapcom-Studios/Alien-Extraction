using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using YmirEngine;

public class Narrative_Item : YmirComponent
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

    public GameObject canvas_Items = null;
    public GameObject name_Npc = null;
    public GameObject dialogue_Npc = null;
    public GameObject Ybutton = null;
    public GameObject Bbutton = null;
    public GameObject Abutton = null;
    public GameObject Xbutton = null;

    //Popup
    private GameObject popup;

    private bool retryDialogue;
    private float retryTimer;
    private const float retryDuration = 0.5f;

    public int itemID;

    //Save & Load
    string saveName;

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

    }

    public Dialogue_id dialogue_;

    public void Start()
    {
        player = InternalCalls.GetGameObjectByName("Player").GetComponent<Player>();
        gameCanvas = InternalCalls.GetGameObjectByName("Game Canvas");

        active_Dialogue = false;
        canvas_Items = InternalCalls.GetGameObjectByName("Npc_Dialogue");
        name_Npc = InternalCalls.GetGameObjectByName("Name_Npc");
        dialogue_Npc = InternalCalls.GetGameObjectByName("dialogue_Npc");
        Ybutton = InternalCalls.GetGameObjectByName("buttonY");
        Bbutton = InternalCalls.GetGameObjectByName("buttonB");
        Abutton = InternalCalls.GetGameObjectByName("buttonA");
        Xbutton = InternalCalls.GetGameObjectByName("buttonX");

        popup = InternalCalls.CS_GetChild(gameObject, 1);

        //En relación al ID cargar un csv o otro
        if (itemID == 0)
        {
            //Holo Screen
            dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/HOLO_SCREEN_ID100.csv");
        }
        else if (itemID == 1)
        {
            //Corpse Lv 1
            dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/CORPSE_ID101.csv");

        }
        else if (itemID == 2)
        {
            //Android head
            dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/ANDROID_HEAD_ID102.csv");

        }
        else if (itemID == 3)
        {
            //Corpse Lv 2
            dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/CORPSE_ID103.csv");

        }
        else if (itemID == 6)
        {
            //Corpse Lv 3
            dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/CORPSE_ID106.csv");

        }
        else if (itemID == 7)
        {
            //C4
            dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/C4_ID107.csv");

        }
        else if (itemID == 8)
        {
            //Clothes
            dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/CLOTHES_ID108.csv");

        }
        else if (itemID == 9)
        {
            //Android body
            dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/ANDROID_BODY_ID109.csv");

        }

        LoadDialogues(dialoguescsv);
        dialogue_ = Dialogue_id.ID_1;


        saveName = SaveLoad.LoadString(Globals.saveGameDir, Globals.saveGamesInfoFile, Globals.saveCurrentGame);
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

            //Interacciones - Respuestas TODO: En relación al ID del item(, cambiar la ruta del "dialoguescsv", vaciar el diccionario y volver a cargar los diálogos) (Tal vez no haga falta). Y hacer los inputs en relación a la ID
            //player.PlayerStopState(true);
            //IFs de todas las interacciones:
            if (itemID == 0)
            {
                //Holo Screen
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_3;
                    return;
                }
                //ID 3
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_3)
                {
                    //Guardar el bool de interacción con la holo screen
                    SaveLoad.SaveBool(Globals.saveGameDir, saveName, "Interacted Holo Screen", true);
                    ExitDialogue();
                    return;
                }
            }
            else if (itemID == 1)
            {
                //Corpse Lv 1
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_3;
                    return;
                }
                //ID 3
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_3)
                {
                    dialogue_ = Dialogue_id.ID_5;
                    return;
                }
                //ID 5
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_5)
                {
                    ExitDialogue();
                    return;
                }
            }
            else if (itemID == 2)
            {
                //Android head
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_3;
                    return;
                }
                //ID 3
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_3)
                {
                    dialogue_ = Dialogue_id.ID_5;
                    return;
                }
                //ID 5
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_5)
                {
                    //Guardar el bool de interacción con la Android Head
                    SaveLoad.SaveBool(Globals.saveGameDir, saveName, "Interacted Android Head", true);
                    ExitDialogue();
                    return;
                }
            }
            else if (itemID == 3)
            {
                //Corpse Lv 2
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_3;
                    return;
                }
                //ID 3
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_3)
                {
                    dialogue_ = Dialogue_id.ID_5;
                    return;
                }
                //ID 5
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_5)
                {
                    ExitDialogue();
                    return;
                }
            }
            else if (itemID == 6)
            {
                //Corpse Lv 3
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_3;
                    return;
                }
                //ID 3
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_3)
                {
                    dialogue_ = Dialogue_id.ID_5;
                    return;
                }
                //ID 5
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_5)
                {
                    //Guardar el bool de interacción con el cadaver
                    SaveLoad.SaveBool(Globals.saveGameDir, saveName, "Interacted Corpse", true);
                    ExitDialogue();
                    return;
                }
            }
            else if (itemID == 7)
            {
                //C4
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_3;
                    return;
                }
                //ID 3
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_3)
                {
                    ExitDialogue();
                    return;
                }
            }
            else if (itemID == 8)
            {
                //Clothes
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_3;
                    return;
                }
                //ID 3
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_3)
                {
                    ExitDialogue();
                    return;
                }
            }
            else if (itemID == 9)
            {
                //Android body
                //ID 1
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
                {
                    dialogue_ = Dialogue_id.ID_3;
                    return;
                }
                //ID 3
                if (Input.GetGamepadButton(GamePadButton.Y) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_3)
                {
                    ExitDialogue();
                    return;
                }
            }

            DialogueManager();

        }
        else
        {
            popup.SetActive(false);
        }

        if (retryDialogue)
        {
            retryTimer -= Time.deltaTime;
            if (retryTimer <= 0)
            {
                retryDialogue = false;
            }
        }
    }

    public void DialogueManager()
    {
        //Visual - Diálogos + Respuestas
        if (itemID == 0)
        {
            //Holo Screen
            
            switch (dialogue_)
            {
                case Dialogue_id.ID_0:
                    dialogue_ = Dialogue_id.ID_1;
                    break;

                case Dialogue_id.ID_1:
                    UI.TextEdit(name_Npc, dialogue[1].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                    UI.TextEdit(Ybutton, dialogue[2].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_3:
                    UI.TextEdit(name_Npc, dialogue[3].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[3].Text);
                    UI.TextEdit(Ybutton, dialogue[4].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;

            }
        }
        else if (itemID == 1)
        {
            //Corpse Lv 1
            switch (dialogue_)
            {
                case Dialogue_id.ID_0:
                    dialogue_ = Dialogue_id.ID_1;
                    break;

                case Dialogue_id.ID_1:
                    UI.TextEdit(name_Npc, dialogue[1].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                    UI.TextEdit(Ybutton, dialogue[2].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_3:
                    UI.TextEdit(name_Npc, dialogue[3].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[3].Text);
                    UI.TextEdit(Ybutton, dialogue[4].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_5:
                    UI.TextEdit(name_Npc, dialogue[5].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[5].Text);
                    UI.TextEdit(Ybutton, dialogue[6].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;

            }

        }
        else if (itemID == 2)
        {
            //Android head
            switch (dialogue_)
            {
                case Dialogue_id.ID_0:
                    dialogue_ = Dialogue_id.ID_1;
                    break;

                case Dialogue_id.ID_1:
                    UI.TextEdit(name_Npc, dialogue[1].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                    UI.TextEdit(Ybutton, dialogue[2].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_3:
                    UI.TextEdit(name_Npc, dialogue[3].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[3].Text);
                    UI.TextEdit(Ybutton, dialogue[4].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_5:
                    UI.TextEdit(name_Npc, dialogue[5].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[5].Text);
                    UI.TextEdit(Ybutton, dialogue[6].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;

            }

        }
        else if (itemID == 3)
        {
            //Corpse Lv 2
            switch (dialogue_)
            {
                case Dialogue_id.ID_0:
                    dialogue_ = Dialogue_id.ID_1;
                    break;

                case Dialogue_id.ID_1:
                    UI.TextEdit(name_Npc, dialogue[1].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                    UI.TextEdit(Ybutton, dialogue[2].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_3:
                    UI.TextEdit(name_Npc, dialogue[3].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[3].Text);
                    UI.TextEdit(Ybutton, dialogue[4].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_5:
                    UI.TextEdit(name_Npc, dialogue[5].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[5].Text);
                    UI.TextEdit(Ybutton, dialogue[6].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;

            }

        }
        else if (itemID == 6)
        {
            //Corpse Lv 3
            switch (dialogue_)
            {
                case Dialogue_id.ID_0:
                    dialogue_ = Dialogue_id.ID_1;
                    break;

                case Dialogue_id.ID_1:
                    UI.TextEdit(name_Npc, dialogue[1].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                    UI.TextEdit(Ybutton, dialogue[2].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_3:
                    UI.TextEdit(name_Npc, dialogue[3].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[3].Text);
                    UI.TextEdit(Ybutton, dialogue[4].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_5:
                    UI.TextEdit(name_Npc, dialogue[5].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[5].Text);
                    UI.TextEdit(Ybutton, dialogue[6].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;

            }

        }
        else if (itemID == 7)
        {
            //C4
            switch (dialogue_)
            {
                case Dialogue_id.ID_0:
                    dialogue_ = Dialogue_id.ID_1;
                    break;

                case Dialogue_id.ID_1:
                    UI.TextEdit(name_Npc, dialogue[1].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                    UI.TextEdit(Ybutton, dialogue[2].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_3:
                    UI.TextEdit(name_Npc, dialogue[3].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[3].Text);
                    UI.TextEdit(Ybutton, dialogue[4].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;

            }
        }
        else if (itemID == 8)
        {
            //Clothes
            switch (dialogue_)
            {
                case Dialogue_id.ID_0:
                    dialogue_ = Dialogue_id.ID_1;
                    break;

                case Dialogue_id.ID_1:
                    UI.TextEdit(name_Npc, dialogue[1].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                    UI.TextEdit(Ybutton, dialogue[2].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_3:
                    UI.TextEdit(name_Npc, dialogue[3].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[3].Text);
                    UI.TextEdit(Ybutton, dialogue[4].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;

            }

        }
        else if (itemID == 9)
        {
            //Android body
            switch (dialogue_)
            {
                case Dialogue_id.ID_0:
                    dialogue_ = Dialogue_id.ID_1;
                    break;

                case Dialogue_id.ID_1:
                    UI.TextEdit(name_Npc, dialogue[1].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[1].Text);
                    UI.TextEdit(Ybutton, dialogue[2].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;
                case Dialogue_id.ID_3:
                    UI.TextEdit(name_Npc, dialogue[3].Name);
                    UI.TextEdit(dialogue_Npc, dialogue[3].Text);
                    UI.TextEdit(Ybutton, dialogue[4].Text);
                    UI.TextEdit(Bbutton, " ");
                    UI.TextEdit(Abutton, " ");
                    UI.TextEdit(Xbutton, " ");
                    break;

            }
        }
    }
    public void OnCollisionStay(GameObject other)
    {
        if (other.Tag == "Player" && !active_Dialogue)
        {
            popup.SetActive(true);
        }

        if (other.Tag == "Player" && (Input.IsGamepadButtonAPressedCS() || Input.GetKey(YmirKeyCode.SPACE) == KeyState.KEY_DOWN) && !active_Dialogue && !retryDialogue && player.currentMenu == "")
        {
            gameCanvas.SetActive(false);

            player.currentMenu = "Item Dialogue";

            canvas_Items.SetActive(true);
            active_Dialogue = true;
            player.PlayerStopState(true);
        }
    }
    public void OnCollisionExit(GameObject other)
    {
        if (other.Tag == "Player")
        {
            canvas_Items.SetActive(false);
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

    private void ExitDialogue()
    {
        dialogue_ = Dialogue_id.ID_1;
        //EXIT
        player.currentMenu = "";

        player.PlayerStopState(false);
        active_Dialogue = false;
        canvas_Items.SetActive(false);

        retryDialogue = true;
        retryTimer = retryDuration;

        gameCanvas.SetActive(true);
    }
}