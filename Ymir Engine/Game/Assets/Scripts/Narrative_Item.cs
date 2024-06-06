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

        active_Dialogue = false;
        canvas_Items = InternalCalls.GetGameObjectByName("Npc_Dialogue");
        name_Npc = InternalCalls.GetGameObjectByName("Name_Npc");
        dialogue_Npc = InternalCalls.GetGameObjectByName("dialogue_Npc");
        dialoguescsv = InternalCalls.CSVToString("Assets/Dialogue/Iscariot_Dialogue.csv");
        Ybutton = InternalCalls.GetGameObjectByName("buttonY");
        Bbutton = InternalCalls.GetGameObjectByName("buttonB");
        Abutton = InternalCalls.GetGameObjectByName("buttonA");
        Xbutton = InternalCalls.GetGameObjectByName("buttonX");

        popup = InternalCalls.CS_GetChild(gameObject, 1);

        LoadDialogues(dialoguescsv);
        dialogue_ = Dialogue_id.ID_1;
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

            //Interacciones - Respuestas TODO: En relaci�n al ID del item(, cambiar la ruta del "dialoguescsv", vaciar el diccionario y volver a cargar los di�logos) (Tal vez no haga falta). Y hacer los inputs en relaci�n a la ID
            //player.PlayerStopState(true);
            //IFs de todas las interacciones:
            {

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
                    dialogue_ = Dialogue_id.ID_6;
                    return;
                }
                if (Input.GetGamepadButton(GamePadButton.X) == KeyState.KEY_DOWN && dialogue_ == Dialogue_id.ID_1)
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

        //Visual - Di�logos + Respuestas
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

        }
    }
    public void OnCollisionStay(GameObject other)
    {
        if (other.Tag == "Player" && !active_Dialogue)
        {
            popup.SetActive(true);
        }

        if (other.Tag == "Player" && (Input.IsGamepadButtonAPressedCS() || Input.GetKey(YmirKeyCode.SPACE) == KeyState.KEY_DOWN) && !active_Dialogue && !retryDialogue)
        {
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
        player.PlayerStopState(false);
        active_Dialogue = false;
        canvas_Items.SetActive(false);

        retryDialogue = true;
        retryTimer = retryDuration;
    }
}