using System;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace YmirEngine
{
    /// <summary>
    /// Provides methods for internal engine calls.
    /// </summary>
    class InternalCalls
    {
        /// <summary>
        /// Creates a game object.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateGameObject(object name, object position);

        /// <summary>
        /// Creates a game object with Prefab.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject CreatePrefab(object prefab_path, object position, object rotation, object scale);

        /// <summary>
        /// Creates a bullet.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateBullet(object position, object rotation, object scale);

        /// <summary>
        /// Creates Tail Sensor
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateTailSensor(object position, object rotation);

        /// <summary>
        /// Creates Shotgun Sensor
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateShotgunSensor(object position, object rotation, float lenght, float width, object rightVector);

        /// <summary>
        /// Creates a bullet.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateAcidicSpit(object name, object position);

        /// <summary>
        /// Creates a bullet.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateAcidPuddle(object name, object position);

        /// <summary>
        /// Creates spitter acid spit attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateSpitterAcidSpit(object position, object rotation, float damage);

        /// <summary>
        /// Creates spitter acid explosive attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateSpitterAcidExplosive(object position, object rotation, float damage);

        /// <summary>
        /// Creates spitter acid shrapnel attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateSpitterAcidShrapnel(object position, object rotation, float damage);


        /// <summary>
        /// Creates facehugger tail attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateFaceHuggerTailAttack(object position, object rotation, float damage);

        /// <summary>
        /// Creates drone claw attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateDroneClawAttack(object position, object rotation, float damage);

        /// <summary>
        /// Creates drone tail attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateDroneTailAttack(object position, object rotation, float damage);

        /// <summary>
        /// Creates queen claw attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateQueenClawAttack(object position, object rotation);

        /// <summary>
        /// Creates queen tail attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateQueenTailAttack(object position, object rotation);

        /// <summary>
        /// Creates queen spit attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateQueenSpitAttack(object position, object rotation);

        /// <summary>
        /// Creates queen shrapnel attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateQueenShrapnel(object position, object rotation);

        /// <summary>
        /// Creates queen puddle attack.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateQueenPuddle(object position, object rotation);

        /// <summary>
        /// Destroys a game object.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Destroy(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetColor(object go, object color);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetPlayerHitBoolean(object go, bool value);

        // -----------------------

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CS_SetBothFOV(float value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CS_SetHorizontalFOV(float value);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CS_SetVerticalFOV(float value);

        /// <summary>
        /// Retrieves a game object by its name.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject GetGameObjectByName(string name);

        /// <summary>
        /// Retrieves a game object by its name.
        /// </summary>
        /// <param name="go">Parent from which we search for the game object.</param>
        /// <param name="name">Name of the game object being searched for.</param>
        /// <returns>The children as game object if found, otherwise null.</returns>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject GetChildrenByName(object parentGO, string name);
        /// Retrieves a game object by its UID.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject GetGameObjectByUID(int id);

        /// <summary>
        /// Loads a scene.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject LoadScene(string name);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject GetHealtBarObject(object go, int numChild);

        
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject GetEnemyGameObject(object go);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern Vector3 GetWalkablePointAround(object position, float radius);

        /// <summary>
        /// Exits the game.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject ExitGame();

        /// <summary>
        /// Read all de characters of the csv file
        /// </summary>
        /// <param name="filePath">The path to the CSV file (in this format: "Assets/Loot Tables/loot_table.csv").</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern string CSVToString(string filePath);

        /// <summary>
        /// Retrieves a string with the values of the fields of the CSV, every field end with "," and every item with ";"
        /// </summary>
        /// <param name="filePath">The path to the CSV file (in this format: "Assets/Loot Tables/loot_table.csv").</param>
        /// <param name="field">The CSV field names with their corresponding delimiters (in this format: "Nombre:,Probabilidad:") every field end with a comma ",".</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern string CSVToStringKeys(string filePath, string field);

        /// <summary>
        /// Creates a GameObject in the Engine as the prefab indicated
        /// </summary>
        /// <param name="prefabPath">The path to the prefab folder (in this format: "Assets/Prefabs").</param>
        /// <param name="prefabName">The name of the prefab inside of the prefabPath (in this format: "Player").</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateGOFromPrefab(string prefabPath, string prefabName, Vector3 pos);          
        
        // TODO: PONER EN GAMEOBJECT.CS
        /// <summary>
        /// Get parent from go.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject CS_GetParent(object parentGO);

        // TODO: PONER EN GAMEOBJECT.CS
        /// <summary>
        /// Get child in numberChild position in mchildren.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject CS_GetChild(object go, int numberChild);

        // TODO: PONER EN GAMEOBJECT.CS
        /// <summary>
        /// Get child in numberChild position in mchildren.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int CS_GetChildrenSize(object go);

        /// <summary>
        /// Compare GameObjects by UID.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool CompareGameObjectsByUID(object go1, object go2);          

        /// <summary>
        /// Disable the component with name.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool DisableComponent(object go, string name, bool includeChildren);

        /// <summary>
        /// Spawns an item prefab with name and position.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SpawnItem(string name, Vector3 pos);

        /// <summary>
        /// Returns the ID of the current map (IDs: NO_MAP = -1, LVL_BASE = 0, LVL_1 = 1, LVL_2_PART_1 = 2, LVL_2_PART_2 = 3, LVL_3_PART_1 = 4, LVL_3_PART_2 = 5) 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetCurrentMap();

    }

    /// <summary>
    /// Provides methods for UI operations.
    /// </summary>
    public class UI
    {
        /// <summary>
        /// Get navigation bool.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool GetCanNav();

        /// <summary>
        /// Set navigation bool.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetCanNav(bool isActive);
        
        /// <summary>
        /// Get UI state.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetUIState(object go);

        /// <summary>
        /// Change UI state.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetUIState(object go, int uiState);

        /// <summary>
        /// Creates an image UI element.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject CreateImageUI(object go, string name, int x, int y);

        /// <summary>
        /// Changes an image UI element.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject ChangeImageUI(object go, string name, int state);

        /// <summary>
        /// Get sprite sheet rows.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetImageRows(object go);
        /// <summary>
        /// Get sprite sheet columns.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetImageColumns(object go);

        /// <summary>
        /// Get current sprite X.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetImageCurrentFrameX(object go);
        /// <summary>
        /// Get current sprite Y.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetImageCurrentFrameY(object go);

        /// <summary>
        /// Set image current frame.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetImageCurrentFrame(object go, int x = 0, int y = 0);

        /// <summary>
        /// Edits a text UI element.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject TextEdit(object go, string text);

        /// <summary>
        /// Edits a slider UI element.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject SliderEdit(object go, double value);

        /// <summary>
        /// Set a fillOject to the slider.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject SetSliderFill(object go, object fillObject);

        /// <summary>
        /// Sets the range of a slider UI element.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject SliderSetRange(object go, double min, double max);

        /// <summary>
        /// Sets the minimum value of a slider UI element.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject SliderSetMin(object go, double value);

        /// <summary>
        /// Sets the maximum value of a slider UI element.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject SliderSetMax(object go, double value);

        /// <summary>
        /// Get UI go that is in selected state.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject GetSelected();

        /// <summary>
        /// Get UI go that is in focused state.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject GetFocused();

        /// <summary>
        /// Switch position of two game objects.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject SwitchPosition(object selectedgo, object targetobject);        
        
        /// <summary>
        /// Set active all children of the go's canvas.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetActiveAllUI(object selectedgo, bool isActive);

        /// <summary>
        /// Navigate in x axis with gamepad.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void NavigateGridHorizontal(object go, int rows, int columns, bool isRight, bool navigate, object gridLeft, object gridRight, bool bounce, int childNumber, bool isEmpty);

        /// <summary>
        /// Navigate in y axis with gamepad.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void NavigateGridVertical(object go, int rows, int columns, bool isDown, bool navigate, object gridDown, object gridUp, bool bounce, int childNumber, bool isEmpty);

        /// <summary>
        /// Navigate in x axis with gamepad for crafting.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]

        public static extern void NavigateCraftingHorizontal(object go, int rows, int columns, bool isRight, bool navigateGrids, object gridLeft, object gridRight, bool bounce);
       
        /// <summary>
        /// Navigate in y axis with gamepad for crafting.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void NavigateCraftingVertical(object go, int rows, int columns, bool isDown, bool navigate, object gridDown, object gridUp, bool bounce);
        
        /// <summary>
        /// Unlock upgrades.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern GameObject UpgradeToUnlock(object go);
        
        /// <summary>
        /// Set child of UI go as focused.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetFirstFocused(object go);

        /// <summary>
        /// Get UI text
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern string GetUIText(object go);     
        
        /// <summary>
        /// Get UI text
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SetUIPosWithOther(object goSource, object goDestination);       
        
        /// <summary>
        /// Check UI inside list
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool CheckUI(object goTarget, object goOrigin);

        /// <summary>
        /// Compare string to go name.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool CompareStringToName(object go, string name);
    }

    /// <summary>
    /// Provides methods for Save and Load operations.
    /// </summary>
    public class SaveLoad
    {
        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool GameFileExists(string dir, string name);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void CreateSaveGameFile(string dir, string name);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SaveInt(string dir, string name, string saveAs, int val);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SaveFloat(string dir, string name, string saveAs, double val);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SaveBool(string dir, string name, string saveAs, bool val);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SaveString(string dir, string name, string saveAs, string val);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SaveIntArray(string dir, string name, string saveAs, int[] val);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void SaveFloatArray(string dir, string name, string saveAs, double[] val);

        //

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int LoadInt(string dir, string name, string loadFrom);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern double LoadFloat(string dir, string name, string loadFrom);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool LoadBool(string dir, string name, string loadFrom);

        /// <summary>
        /// 
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern string LoadString(string dir, string name, string loadFrom);

        ///// <summary>
        ///// 
        ///// </summary>
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public static extern void LoadIntArray(string dir, string name, string loadFrom);

        ///// <summary>
        ///// 
        ///// </summary>
        //[MethodImplAttribute(MethodImplOptions.InternalCall)]
        //public static extern void LoadFloatArray(string dir, string name, string loadFrom);
    }

    /// <summary>
    /// Provides methods for handling input.
    /// </summary>
    public class Input
    {
        // Keyboard and mouse input methods

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern KeyState GetKey(object keyPressed);

        /// <summary>
        /// Retrieves the state of the X position of the mouse.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetMouseX();

        /// <summary>
        /// Retrieves the state of the Y position of the mouse.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern int GetMouseY();

        // Gamepad input methods
        /// <summary>
        /// Retrieves the state of a gamepad button.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern KeyState GetGamepadButton(object keyPressed);

        /// <summary>
        /// Retrieves the state of the left X-axis of the gamepad.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float GetLeftAxisX();

        /// <summary>
        /// Retrieves the state of the left Y-axis of the gamepad.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float GetLeftAxisY();

        /// <summary>
        /// Retrieves the state of the right X-axis of the gamepad.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float GetRightAxisX();

        /// <summary>
        /// Retrieves the state of the right Y-axis of the gamepad.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float GetRightAxisY();

        /// <summary>
        /// Retrieves the state of the left trigger of the gamepad.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float GetGamepadLeftTrigger();

        /// <summary>
        /// Retrieves the state of the right trigger of the gamepad.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern float GetGamepadRightTrigger();

        /// <summary>
        /// Checks if the A button of the gamepad is pressed.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool IsGamepadButtonAPressedCS();

        /// <summary>
        /// Checks if the B button of the gamepad is pressed.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern bool IsGamepadButtonBPressedCS();

        /// <summary>
        /// Controls the rumble of the gamepad for a specific time.
        /// </summary>
        /// <param name="time">The duration of the rumble in milliseconds.</param>
        /// /// <param name="intesity">The intensity of the rumble min 1, max 10.</param>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Rumble_Controller(int time, int intesity);
    }

    /// <summary>
    /// Provides methods for debugging and logging.
    /// </summary>
    public partial class Debug
    {
        /// <summary>
        /// Logs a message.
        /// </summary>
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        public static extern void Log(object logText);
    }


    [StructLayout(LayoutKind.Sequential)]
    public partial struct mat4x4 /*: IEquatable<Vector3>*/
    {
        //    |  0   1   2   3
        // ---+----------------
        // 0  | m00 m10 m20 m30
        // 1  | m01 m11 m21 m31
        // 2  | m02 m12 m22 m32
        // 3  | m03 m13 m23 m33

        public float m00;
        public float m10;
        public float m20;
        public float m30;
        public float m01;
        public float m11;
        public float m21;
        public float m31;
        public float m02;
        public float m12;
        public float m22;
        public float m32;
        public float m03;
        public float m13;
        public float m23;
        public float m33;

        public mat4x4(Quaternion column0, Quaternion column1, Quaternion column2, Quaternion column3)
        {
            this.m00 = column0.x; this.m01 = column1.x; this.m02 = column2.x; this.m03 = column3.x;
            this.m10 = column0.y; this.m11 = column1.y; this.m12 = column2.y; this.m13 = column3.y;
            this.m20 = column0.z; this.m21 = column1.z; this.m22 = column2.z; this.m23 = column3.z;
            this.m30 = column0.w; this.m31 = column1.w; this.m32 = column2.w; this.m33 = column3.w;
        }
        public float this[int row, int column]
        {
            get
            {
                return this[row + column * 4];
            }

            set
            {
                this[row + column * 4] = value;
            }
        }
        public float this[int index]
        {
            get
            {
                switch (index)
                {
                    case 0: return m00;
                    case 1: return m10;
                    case 2: return m20;
                    case 3: return m30;
                    case 4: return m01;
                    case 5: return m11;
                    case 6: return m21;
                    case 7: return m31;
                    case 8: return m02;
                    case 9: return m12;
                    case 10: return m22;
                    case 11: return m32;
                    case 12: return m03;
                    case 13: return m13;
                    case 14: return m23;
                    case 15: return m33;
                    default:
                        throw new IndexOutOfRangeException("Invalid matrix index!");
                }
            }

            set
            {
                switch (index)
                {
                    case 0: m00 = value; break;
                    case 1: m10 = value; break;
                    case 2: m20 = value; break;
                    case 3: m30 = value; break;
                    case 4: m01 = value; break;
                    case 5: m11 = value; break;
                    case 6: m21 = value; break;
                    case 7: m31 = value; break;
                    case 8: m02 = value; break;
                    case 9: m12 = value; break;
                    case 10: m22 = value; break;
                    case 11: m32 = value; break;
                    case 12: m03 = value; break;
                    case 13: m13 = value; break;
                    case 14: m23 = value; break;
                    case 15: m33 = value; break;

                    default:
                        throw new IndexOutOfRangeException("Invalid matrix index!");
                }
            }
        }

        public void SetFromTRS(Vector3 pos, Quaternion r, Vector3 s)
        {
            //mat4x4 posMat = SetTranslatePart(pos);
            //mat4x4 rotMat = SetRotatePart(rot);
            //mat4x4 scaleMat = SetScalePart(size);

            //InternalCalls.CSLog(posMat.ToString());
            //InternalCalls.CSLog(rotMat.ToString());
            //InternalCalls.CSLog(scaleMat.ToString());

            //mat4x4 posRotMat = posMat * rotMat;

            this[0] = (1.0f - 2.0f * (r.y * r.y + r.z * r.z)) * s.x;
            this[1] = (r.x * r.y + r.z * r.w) * s.x * 2.0f;
            this[2] = (r.x * r.z - r.y * r.w) * s.x * 2.0f;
            this[3] = 0.0f;
            this[4] = (r.x * r.y - r.z * r.w) * s.y * 2.0f;
            this[5] = (1.0f - 2.0f * (r.x * r.x + r.z * r.z)) * s.y;
            this[6] = (r.y * r.z + r.x * r.w) * s.y * 2.0f;
            this[7] = 0.0f;
            this[8] = (r.x * r.z + r.y * r.w) * s.z * 2.0f;
            this[9] = (r.y * r.z - r.x * r.w) * s.z * 2.0f;
            this[10] = (1.0f - 2.0f * (r.x * r.x + r.y * r.y)) * s.z;
            this[11] = 0.0f;
            this[12] = pos.x;
            this[13] = pos.y;
            this[14] = pos.z;
            this[15] = 1.0f;

            //InternalCalls.CSLog("Pos * Rot * scale: " + this.GetForward().ToString());
        }

        public static mat4x4 SetTranslatePart(Vector3 pos)
        {
            mat4x4 mat = identity;

            mat[0] = 1; mat[1] = 0; mat[2] = 0; mat[3] = pos.x;
            mat[4] = 0; mat[5] = 1; mat[6] = 0; mat[7] = pos.y;
            mat[8] = 0; mat[9] = 0; mat[10] = 1; mat[11] = pos.z;
            mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1;

            Debug.Log("Position: " + mat.ToString());

            return mat;
        }
        public static mat4x4 SetRotatePart(Quaternion rot)
        {
            mat4x4 mat = identity;

            float x = rot.x;
            float y = rot.y;
            float z = rot.z;
            float w = rot.w;

            mat[0] = 1 - 2 * (y * y + z * z); mat[1] = 2 * (x * y - z * w); mat[2] = 2 * (x * z + y * w); mat[3] = 0;
            mat[4] = 2 * (x * y + z * w); mat[5] = 1 - 2 * (x * x + z * z); mat[6] = 2 * (y * z - x * w); mat[7] = 0;
            mat[8] = 2 * (x * z - y * w); mat[9] = 2 * (y * z + x * w); mat[10] = 1 - 2 * (x * x + y * y); mat[11] = 0;
            mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1;

            Debug.Log("Rotation: " + mat.ToString());

            return mat;
        }
        public static mat4x4 SetScalePart(Vector3 scale)
        {
            mat4x4 mat = identity;

            mat[0] = scale.x; mat[1] = 0; mat[2] = 0; mat[3] = 0;
            mat[4] = 0; mat[5] = scale.y; mat[6] = 0; mat[7] = 0;
            mat[8] = 0; mat[9] = 0; mat[10] = scale.z; mat[11] = 0;
            mat[12] = 0; mat[13] = 0; mat[14] = 0; mat[15] = 1;

            Debug.Log("Scale: " + mat.ToString());


            return mat;
        }
        void SetIdentity()
        {
            this.m00 = 1; this.m01 = 0; this.m02 = 0; this.m03 = 0;
            this.m10 = 0; this.m11 = 1; this.m12 = 0; this.m13 = 0;
            this.m20 = 0; this.m21 = 0; this.m22 = 1; this.m23 = 0;
            this.m30 = 0; this.m31 = 0; this.m32 = 0; this.m33 = 1;
        }

        public Vector3 GetRight()
        {
            Vector3 ret = Vector3.zero;
            ret.Set(this[0], this[1], this[2]);
            return ret.normalized;
        }
        public Vector3 GetForward()
        {
            Vector3 ret = Vector3.zero;
            ret.Set(this[8], this[9], this[10]);
            return ret.normalized;
        }

        static readonly mat4x4 zeroMatrix = new mat4x4(new Quaternion(0, 0, 0, 0),
            new Quaternion(0, 0, 0, 0),
            new Quaternion(0, 0, 0, 0),
            new Quaternion(0, 0, 0, 0));

        // Returns a matrix with all elements set to zero (RO).
        public static mat4x4 zero { get { return zeroMatrix; } }

        static readonly mat4x4 identityMatrix = new mat4x4(new Quaternion(1, 0, 0, 0),
            new Quaternion(0, 1, 0, 0),
            new Quaternion(0, 0, 1, 0),
            new Quaternion(0, 0, 0, 1));

        // Returns the identity matrix (RO).
        public static mat4x4 identity { get { return identityMatrix; } }

        public override string ToString()
        {
            string ret = "";
            for (int i = 0; i < 16; i += 4)
            {
                ret += (this[i].ToString() + ", " + this[i + 1].ToString() + ", " + this[i + 2].ToString() + ", " + this[i + 3].ToString()) + Environment.NewLine;
            }

            return ret;
        }

        public static mat4x4 operator *(mat4x4 lhs, mat4x4 rhs)
        {
            mat4x4 res = identity;
            res.m00 = lhs.m00 * rhs.m00 + lhs.m01 * rhs.m10 + lhs.m02 * rhs.m20 + lhs.m03 * rhs.m30;
            res.m01 = lhs.m00 * rhs.m01 + lhs.m01 * rhs.m11 + lhs.m02 * rhs.m21 + lhs.m03 * rhs.m31;
            res.m02 = lhs.m00 * rhs.m02 + lhs.m01 * rhs.m12 + lhs.m02 * rhs.m22 + lhs.m03 * rhs.m32;
            res.m03 = lhs.m00 * rhs.m03 + lhs.m01 * rhs.m13 + lhs.m02 * rhs.m23 + lhs.m03 * rhs.m33;

            res.m10 = lhs.m10 * rhs.m00 + lhs.m11 * rhs.m10 + lhs.m12 * rhs.m20 + lhs.m13 * rhs.m30;
            res.m11 = lhs.m10 * rhs.m01 + lhs.m11 * rhs.m11 + lhs.m12 * rhs.m21 + lhs.m13 * rhs.m31;
            res.m12 = lhs.m10 * rhs.m02 + lhs.m11 * rhs.m12 + lhs.m12 * rhs.m22 + lhs.m13 * rhs.m32;
            res.m13 = lhs.m10 * rhs.m03 + lhs.m11 * rhs.m13 + lhs.m12 * rhs.m23 + lhs.m13 * rhs.m33;

            res.m20 = lhs.m20 * rhs.m00 + lhs.m21 * rhs.m10 + lhs.m22 * rhs.m20 + lhs.m23 * rhs.m30;
            res.m21 = lhs.m20 * rhs.m01 + lhs.m21 * rhs.m11 + lhs.m22 * rhs.m21 + lhs.m23 * rhs.m31;
            res.m22 = lhs.m20 * rhs.m02 + lhs.m21 * rhs.m12 + lhs.m22 * rhs.m22 + lhs.m23 * rhs.m32;
            res.m23 = lhs.m20 * rhs.m03 + lhs.m21 * rhs.m13 + lhs.m22 * rhs.m23 + lhs.m23 * rhs.m33;

            res.m30 = lhs.m30 * rhs.m00 + lhs.m31 * rhs.m10 + lhs.m32 * rhs.m20 + lhs.m33 * rhs.m30;
            res.m31 = lhs.m30 * rhs.m01 + lhs.m31 * rhs.m11 + lhs.m32 * rhs.m21 + lhs.m33 * rhs.m31;
            res.m32 = lhs.m30 * rhs.m02 + lhs.m31 * rhs.m12 + lhs.m32 * rhs.m22 + lhs.m33 * rhs.m32;
            res.m33 = lhs.m30 * rhs.m03 + lhs.m31 * rhs.m13 + lhs.m32 * rhs.m23 + lhs.m33 * rhs.m33;

            return res;
        }

        public Quaternion GetColumn(int index)
        {
            switch (index)
            {
                case 0: return new Quaternion(m00, m10, m20, m30);
                case 1: return new Quaternion(m01, m11, m21, m31);
                case 2: return new Quaternion(m02, m12, m22, m32);
                case 3: return new Quaternion(m03, m13, m23, m33);
                default:
                    throw new IndexOutOfRangeException("Invalid column index!");
            }
        }

        // Returns a row of the matrix.
        public Quaternion GetRow(int index)
        {
            switch (index)
            {
                case 0: return new Quaternion(m00, m01, m02, m03);
                case 1: return new Quaternion(m10, m11, m12, m13);
                case 2: return new Quaternion(m20, m21, m22, m23);
                case 3: return new Quaternion(m30, m31, m32, m33);
                default:
                    throw new IndexOutOfRangeException("Invalid row index!");
            }
        }

        // Sets a column of the matrix.
        public void SetColumn(int index, Quaternion column)
        {
            this[0, index] = column.x;
            this[1, index] = column.y;
            this[2, index] = column.z;
            this[3, index] = column.w;
        }

        // Sets a row of the matrix.
        public void SetRow(int index, Quaternion row)
        {
            this[index, 0] = row.x;
            this[index, 1] = row.y;
            this[index, 2] = row.z;
            this[index, 3] = row.w;
        }

        public float At(int x, int y)
        {
            return this[(y * 4) + x];
        }
        public void Set(int x, int y, float value)
        {
            this[(y * 4) + x] = value;
        }
    }
}
public enum GamePadButton
{
    INVALID = -1,
    A,
    B,
    X,
    Y,
    BACK,
    GUIDE,
    START,
    LEFTSTICK,
    RIGHTSTICK,
    LEFTSHOULDER,
    RIGHTSHOULDER,
    DPAD_UP,
    DPAD_DOWN,
    DPAD_LEFT,
    DPAD_RIGHT,
    MAX
}

public enum YmirKeyCode //This is a mirror from the SDL scancode enum to allow C# to C++ compatibility
{
    SDL_SCANCODE_UNKNOWN = 0,
    A = 4,
    B = 5,
    C = 6,
    D = 7,
    E = 8,
    F = 9,
    G = 10,
    H = 11,
    I = 12,
    J = 13,
    K = 14,
    L = 15,
    M = 16,
    N = 17,
    O = 18,
    P = 19,
    Q = 20,
    R = 21,
    S = 22,
    T = 23,
    U = 24,
    V = 25,
    W = 26,
    X = 27,
    Y = 28,
    Z = 29,
    Alpha1 = 30,
    Alpha2 = 31,
    Alpha3 = 32,
    Alpha4 = 33,
    Alpha5 = 34,
    Alpha6 = 35,
    Alpha7 = 36,
    Alpha8 = 37,
    Alpha9 = 38,
    Alpha0 = 39,
    RETURN = 40,
    ESCAPE = 41,
    BACKSPACE = 42,
    TAB = 43,
    SPACE = 44,
    MINUS = 45,
    EQUALS = 46,
    LEFTBRACKET = 47,
    RIGHTBRACKET = 48,
    BACKSLASH = 49,
    NONUSHASH = 50,
    SEMICOLON = 51,
    APOSTROPHE = 52,
    GRAVE = 53,
    COMMA = 54,
    PERIOD = 55,
    SLASH = 56,
    CAPSLOCK = 57,
    F1 = 58,
    F2 = 59,
    F3 = 60,
    F4 = 61,
    F5 = 62,
    F6 = 63,
    F7 = 64,
    F8 = 65,
    F9 = 66,
    F10 = 67,
    F11 = 68,
    F12 = 69,
    PRINTSCREEN = 70,
    SCROLLLOCK = 71,
    PAUSE = 72,
    INSERT = 73,
    HOME = 74,
    PAGEUP = 75,
    DELETE = 76,
    END = 77,
    PAGEDOWN = 78,
    RIGHT = 79,
    LEFT = 80,
    DOWN = 81,
    UP = 82,
    NUMLOCKCLEAR = 83,
    KP_DIVIDE = 84,
    KP_MULTIPLY = 85,
    KP_MINUS = 86,
    KP_PLUS = 87,
    KP_ENTER = 88,
    KP_1 = 89,
    KP_2 = 90,
    KP_3 = 91,
    KP_4 = 92,
    KP_5 = 93,
    KP_6 = 94,
    KP_7 = 95,
    KP_8 = 96,
    KP_9 = 97,
    KP_0 = 98,
    KP_PERIOD = 99,
    NONUSBACKSLASH = 100,
    APPLICATION = 101,
    POWER = 102,
    KP_EQUALS = 103,
    F13 = 104,
    F14 = 105,
    F15 = 106,
    F16 = 107,
    F17 = 108,
    F18 = 109,
    F19 = 110,
    F20 = 111,
    F21 = 112,
    F22 = 113,
    F23 = 114,
    F24 = 115,
    EXECUTE = 116,
    HELP = 117,
    MENU = 118,
    SELECT = 119,
    STOP = 120,
    AGAIN = 121,
    UNDO = 122,
    CUT = 123,
    COPY = 124,
    PASTE = 125,
    FIND = 126,
    MUTE = 127,
    VOLUMEUP = 128,
    VOLUMEDOWN = 129,
    KP_COMMA = 133,
    KP_EQUALSAS400 = 134,
    INTERNATIONAL1 = 135,
    INTERNATIONAL2 = 136,
    INTERNATIONAL3 = 137,
    INTERNATIONAL4 = 138,
    INTERNATIONAL5 = 139,
    INTERNATIONAL6 = 140,
    INTERNATIONAL7 = 141,
    INTERNATIONAL8 = 142,
    INTERNATIONAL9 = 143,
    LANG1 = 144,
    LANG2 = 145,
    LANG3 = 146,
    LANG4 = 147,
    LANG5 = 148,
    LANG6 = 149,
    LANG7 = 150,
    LANG8 = 151,
    LANG9 = 152,
    ALTERASE = 153,
    SYSREQ = 154,
    CANCEL = 155,
    CLEAR = 156,
    PRIOR = 157,
    RETURN2 = 158,
    SEPARATOR = 159,
    OUT = 160,
    OPER = 161,
    CLEARAGAIN = 162,
    CRSEL = 163,
    EXSEL = 164,
    KP_00 = 176,
    KP_000 = 177,
    THOUSANDSSEPARATOR = 178,
    DECIMALSEPARATOR = 179,
    CURRENCYUNIT = 180,
    CURRENCYSUBUNIT = 181,
    KP_LEFTPAREN = 182,
    KP_RIGHTPAREN = 183,
    KP_LEFTBRACE = 184,
    KP_RIGHTBRACE = 185,
    KP_TAB = 186,
    KP_BACKSPACE = 187,
    KP_A = 188,
    KP_B = 189,
    KP_C = 190,
    KP_D = 191,
    KP_E = 192,
    KP_F = 193,
    KP_XOR = 194,
    KP_POWER = 195,
    KP_PERCENT = 196,
    KP_LESS = 197,
    KP_GREATER = 198,
    KP_AMPERSAND = 199,
    KP_DBLAMPERSAND = 200,
    KP_VERTICALBAR = 201,
    KP_DBLVERTICALBAR = 202,
    KP_COLON = 203,
    KP_HASH = 204,
    KP_SPACE = 205,
    KP_AT = 206,
    KP_EXCLAM = 207,
    KP_MEMSTORE = 208,
    KP_MEMRECALL = 209,
    KP_MEMCLEAR = 210,
    KP_MEMADD = 211,
    KP_MEMSUBTRACT = 212,
    KP_MEMMULTIPLY = 213,
    KP_MEMDIVIDE = 214,
    KP_PLUSMINUS = 215,
    KP_CLEAR = 216,
    KP_CLEARENTRY = 217,
    KP_BINARY = 218,
    KP_OCTAL = 219,
    KP_DECIMAL = 220,
    KP_HEXADECIMAL = 221,
    LCTRL = 224,
    LSHIFT = 225,
    LALT = 226,
    LGUI = 227,
    RCTRL = 228,
    RSHIFT = 229,
    RALT = 230,
    RGUI = 231,
    MODE = 257,
    AUDIONEXT = 258,
    AUDIOPREV = 259,
    AUDIOSTOP = 260,
    AUDIOPLAY = 261,
    AUDIOMUTE = 262,
    MEDIASELECT = 263,
    WWW = 264,
    MAIL = 265,
    CALCULATOR = 266,
    COMPUTER = 267,
    AC_SEARCH = 268,
    AC_HOME = 269,
    AC_BACK = 270,
    AC_FORWARD = 271,
    AC_STOP = 272,
    AC_REFRESH = 273,
    AC_BOOKMARKS = 274,
    BRIGHTNESSDOWN = 275,
    BRIGHTNESSUP = 276,
    DISPLAYSWITCH = 277,
    KBDILLUMTOGGLE = 278,
    KBDILLUMDOWN = 279,
    KBDILLUMUP = 280,
    EJECT = 281,
    SLEEP = 282,
    APP1 = 283,
    APP2 = 284,
    AUDIOREWIND = 285,
    AUDIOFASTFORWARD = 286,
    SDL_NUM_SCANCODES = 512
}
public enum KeyState
{
    KEY_IDLE = 0,
    KEY_DOWN,
    KEY_REPEAT,
    KEY_UP
};

public enum MouseButton
{
    LEFT = 1,
    MIDDLE,
    RIGHT,
}